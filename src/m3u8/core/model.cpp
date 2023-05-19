#include "model.h"
#include "parser.h"
#include "protocol.h"
#include <cmath>
#include <QDebug>

Key *find_key(QMap<QString, QVariant> keydata, QList<Key *> keylist) {
    // qDebug() << "Key *find_key start";
    for (int i = 0; i < keylist.size(); i++) {
        Key *key = keylist[i];
        // Check the intersection of keys and values
        if (
            keydata.find("uri") != keydata.end() && keydata["uri"] == key->uri &&
            keydata.find("method") != keydata.end() && keydata["method"] == key->method &&
            keydata.find("iv") != keydata.end() && keydata["iv"] == key->iv
            ) {
            // qDebug() << "Key *find_key end";
            return key;
        }
    }

    // qDebug() << "Key *find_key not found:" << keydata;

    // throw "No key found for key data";
    return NULL;
}

QString denormalize_attribute(QString attribute) {
    return attribute.replace("_", "-").toUpper();
}


QString quoted(QString str) {
    return QString(R"("%1")").arg(str);
}

QString number_to_string(double number) {
    //    with decimal.localcontext() as ctx:
    //        ctx.prec = 20  # set floating point precision
    //        d = decimal.Decimal(str(number))
    //        return str(d.quantize(decimal.Decimal(1)) if d == d.to_integral_value() else d.normalize())
    if (fabs(number-((int)number)) > 0.00000001) {
        return QString::number(number, 'g', 20);
    }
    else {
        return QString::number((int)number);
    }
}


const QList<QPair<QString, QString> > M3U8::simple_attributes = {
    QPair<QString, QString>("is_variant",    "is_variant"),
    QPair<QString, QString>("is_endlist",       "is_endlist"),
    QPair<QString, QString>("is_i_frames_only", "is_i_frames_only"),
    QPair<QString, QString>("target_duration",  "targetduration"),
    QPair<QString, QString>("media_sequence",   "media_sequence"),
    QPair<QString, QString>("program_date_time",   "program_date_time"),
    QPair<QString, QString>("is_independent_segments", "is_independent_segments"),
    QPair<QString, QString>("version",          "version"),
    QPair<QString, QString>("allow_cache",      "allow_cache"),
    QPair<QString, QString>("playlist_type",    "playlist_type"),
    QPair<QString, QString>("discontinuity_sequence", "discontinuity_sequence")
};

M3U8::M3U8(QString content, QString base_path, QString base_uri, bool strict, QString custom_tags_parser) {
    if (content != "") {
        this->data = Parser::parse(content, strict, custom_tags_parser);
    }
    else {
        this->data = QMap<QString, QVariant>();
    }
    // qDebug() << "M3U8::M3U8:this.data:" << this->data;
    this->_base_uri = base_uri;
    if (this->_base_uri != "") {
        if (!this->_base_uri.endsWith("/")) {
            this->_base_uri += "/";
        }
    }

    // qDebug() << "M3U8::M3U8:before _initialize_attributes";
    this->_initialize_attributes();
    this->base_path = base_path;
    qDebug() << "M3U8::M3U8:end";
}

M3U8::~M3U8() {
    for (int i = 0; i < this->keys.size(); i++) {
        Key *the_key = this->keys[i];
        if (the_key) {
            delete the_key;
        }
    }

    if (this->start) {
        delete this->start;
    }
    if (this->server_control) {
        delete this->server_control;
    }
    if (this->part_inf) {
        delete this->part_inf;
    }
    if (this->skip) {
        delete this->skip;
    }

    for (int i = 0; i < this->session_keys.size(); i++) {
        SessionKey *the_key = this->session_keys[i];
        if (the_key) {
            delete the_key;
        }
    }

    if (this->preload_hint) {
        delete this->preload_hint;
    }
    if (this->content_steering) {
        delete this->content_steering;
    }
}

void M3U8::_initialize_attributes() {
    qDebug() << "M3U8::_initialize_attributes:start";
    if (this->data.find("keys") != this->data.end()) {
        QList<QVariant> keys = this->data["keys"].toList();
        for (int i = 0; i < keys.size(); i++) {
            QVariant key_variant = keys[i];
            if (key_variant.type() == QVariant::Map) {
                QMap<QString, QVariant> key_map_v = key_variant.toMap();
                QMap<QString, QString> key_map;
                for (QMap<QString, QVariant>::iterator it=key_map_v.begin(); it != key_map_v.end(); it++) {
                    key_map[it.key()] = it.value().toString();
                }
                Key *item = new Key(this->base_uri(), key_map);
                this->keys.append(item);
            }
            else {
                this->keys.append(NULL);
            }
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init segment_map";

    if (this->data.find("segment_map") != this->data.end()) {
        QList<QVariant> segment_map = this->data["segment_map"].toList();
        for (int i = 0; i < segment_map.size(); i++) {
            QVariant segment = segment_map[i];
            QMap<QString, QVariant> segment_v = segment.toMap();
            InitializationSection item(this->base_uri(), segment_v);
            this->segment_map.append(item);
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init segments";

    if (this->data.find("segments") != this->data.end()) {
        QList<QVariant> segment_list = this->data["segments"].toList();
        for (int i = 0; i < segment_list.size(); i++) {
            QVariant segment = segment_list[i];
            QMap<QString, QVariant> segment_v = segment.toMap();
            // qDebug() << "M3U8::_initialize_attributes:create segment";
            // qDebug() << "segment_v:" << segment_v;
            Segment item(
                this->base_uri(),
                find_key(segment_v.find("key") != segment_v.end() ? segment_v["key"].toMap() : QMap<QString, QVariant>(), this->keys),
                segment_v
            );
            // qDebug() << "M3U8::_initialize_attributes:create segment end";
            this->segments.list.append(item);
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init simple memeber";

    if (this->data.find("is_variant") != this->data.end()) {
        this->is_variant = this->data["is_variant"].toBool();
    }
    if (this->data.find("is_endlist") != this->data.end()) {
        this->is_endlist = this->data["is_endlist"].toBool();
    }
    if (this->data.find("is_i_frames_only") != this->data.end()) {
        this->is_i_frames_only = this->data["is_i_frames_only"].toBool();
    }
    if (this->data.find("target_duration") != this->data.end()) {
        this->target_duration = this->data["target_duration"].toInt();
    }
    if (this->data.find("media_sequence") != this->data.end()) {
        this->media_sequence = this->data["media_sequence"].toInt();
    }
    if (this->data.find("program_date_time") != this->data.end()) {
        this->program_date_time = this->data["program_date_time"].toDateTime();
    }
    if (this->data.find("is_independent_segments") != this->data.end()) {
        this->is_independent_segments = this->data["is_independent_segments"].toBool();
    }
    if (this->data.find("version") != this->data.end()) {
        this->version = this->data["version"].toInt();
    }
    if (this->data.find("allow_cache") != this->data.end()) {
        this->allow_cache = this->data["allow_cache"].toString();
    }
    if (this->data.find("playlist_type") != this->data.end()) {
        this->playlist_type = this->data["playlist_type"].toString();
    }
    if (this->data.find("discontinuity_sequence") != this->data.end()) {
        this->discontinuity_sequence = this->data["discontinuity_sequence"].toInt();
    }

    for (int i = (this->media_sequence == -1 ? 0 : this->media_sequence); i < this->segments.list.size(); i++) {
        this->segments.list[i].media_sequence = i;
    }

    for (int i = 0; i < this->keys.size(); i++) {
        Key *the_key = this->keys[i];
        // # Avoid None key, it could be the first one, don't repeat them
        if (the_key && !this->files.contains(the_key->uri)) {
            this->files.append(the_key->uri);
        }
    }
    this->files = this->files+this->segments.uri();

    qDebug() << "M3U8::_initialize_attributes:init media";

    if (this->data.find("media") != this->data.end()) {
        QList<QVariant> media_list = this->data["media"].toList();
        for (int i = 0; i < media_list.size(); i++) {
            this->media.list.append(Media(this->base_uri(), media_list[i].toMap()));
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init playlists";

    if (this->data.find("playlists") != this->data.end()) {
        QList<QVariant> playlists = this->data["playlists"].toList();
        for (int i = 0; i < playlists.size(); i++) {
            this->playlists.list.append(Playlist(this->base_uri(), this->media, playlists[i].toMap()));
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init iframe_playlists";

    if (this->data.find("iframe_playlists") != this->data.end()) {
        QList<QVariant> playlists = this->data["iframe_playlists"].toList();
        for (int i = 0; i < playlists.size(); i++) {
            QMap<QString, QVariant> kwargs = playlists[i].toMap();
            this->iframe_playlists.list.append(
                IFramePlaylist(
                    this->base_uri(),
                    kwargs.find("uri") != kwargs.end() ? kwargs["uri"].toString() : "",
                    kwargs.find("iframe_stream_info") != kwargs.end() ? kwargs["iframe_stream_info"].toMap() : QMap<QString, QVariant>()
                )
            );
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init start";

    if (this->data.find("start") != this->data.end()) {
        auto temp = this->data["start"].toMap();
        this->start = new Start(temp.value("time_offset", QVariant(0.0)).toDouble(), temp.value("precise", QVariant("")).toString());
    }

    qDebug() << "M3U8::_initialize_attributes:init server_control";

    if (this->data.find("server_control") != this->data.end()) {
        auto temp = this->data["server_control"].toMap();
        this->server_control = new ServerControl(
            temp.value("can_skip_until", QVariant(-1)).toDouble(),
            temp.value("can_block_reload", QVariant("")).toString(),
            temp.value("hold_back", QVariant(-1)).toDouble(),
            temp.value("part_hold_back", QVariant(-1)).toDouble(),
            temp.value("can_skip_dateranges", QVariant("")).toString()
        );
    }

    qDebug() << "M3U8::_initialize_attributes:init part_inf";

    if (this->data.find("part_inf") != this->data.end()) {
        auto temp = this->data["part_inf"].toMap();
        this->part_inf = new PartInformation(
            temp.value("part_target", QVariant(-1)).toDouble()
        );
    }

    qDebug() << "M3U8::_initialize_attributes:init skip";

    if (this->data.find("skip") != this->data.end()) {
        auto temp = this->data["skip"].toMap();
        this->skip = new Skip(
            temp.value("skipped_segments", QVariant(-1)).toDouble(),
            temp.value("recently_removed_dateranges", QVariant("")).toString()
        );
    }

    qDebug() << "M3U8::_initialize_attributes:init rendition_reports";

    if (this->data.find("rendition_reports") != this->data.end()) {
        QList<QVariant> temp = this->data["rendition_reports"].toList();
        for (int i = 0; i < temp.size(); i++) {
            QMap<QString, QVariant> kwargs = temp[i].toMap();
            this->rendition_reports.list.append(
                RenditionReport(
                    this->base_uri(),
                    kwargs.value("uri", QVariant("")).toString(),
                    kwargs.value("last_msn", QVariant(-1)).toDouble(),
                    kwargs.value("last_part", QVariant(-1)).toDouble()
                )
            );
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init session_data";

    if (this->data.find("session_data") != this->data.end()) {
        QList<QVariant> temp = this->data["session_data"].toList();
        for (int i = 0; i < temp.size(); i++) {
            QMap<QString, QVariant> kwargs = temp[i].toMap();
            if (kwargs.find("data_id") != kwargs.end()) {
                this->session_data.list.append(
                    SessionData(
                        kwargs["data_id"].toString(),
                        kwargs.value("value", QVariant("")).toString(),
                        kwargs.value("uri", QVariant("")).toString(),
                        kwargs.value("language", QVariant("")).toString()
                    )
                );
            }
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init session_keys";

    if (this->data.find("session_keys") != this->data.end()) {
        QList<QVariant> temp = this->data["session_keys"].toList();
        for (int i = 0; i < temp.size(); i++) {
            QMap<QString, QVariant> kwargs = temp[i].toMap();
            QMap<QString, QString> key_map;
            for (QMap<QString, QVariant>::iterator it=kwargs.begin(); it != kwargs.end(); it++) {
                key_map[it.key()] = it.value().toString();
            }
            this->session_keys.append(
                new SessionKey(
                    this->base_uri(),
                    key_map
                )
            );
        }
    }

    qDebug() << "M3U8::_initialize_attributes:init preload_hint";

    if (this->data.find("preload_hint") != this->data.end()) {
        auto temp = this->data["preload_hint"].toMap();
        this->preload_hint = new PreloadHint(
            temp.value("type", QVariant("")).toString(),
            this->base_uri(),
            temp.value("uri", QVariant("")).toString(),
            temp.value("byterange_start", QVariant(-1)).toDouble(),
            temp.value("byterange_length", QVariant(-1)).toDouble()
        );
    }

    qDebug() << "M3U8::_initialize_attributes:init content_steering";

    if (this->data.find("content_steering") != this->data.end()) {
        auto temp = this->data["content_steering"].toMap();
        this->content_steering = new ContentSteering(
            this->base_uri(),
            temp.value("server_uri", QVariant("")).toString(),
            temp.value("pathway_id", QVariant("")).toString()
        );
    }
}

QString M3U8::base_uri() {
    return this->_base_uri;
}

QString M3U8::toString() {
    // QMap<QString, QVariant> data;
    QStringList output;
    output.append(QString("_base_uri:%1").arg(this->_base_uri));
    output.append(QString("base_path:%1").arg(this->base_path));

//    QList<Key *> keys;
//    QList<InitializationSection> segment_map;
//    SegmentList segments;
    // output.append(QString("files:%1").arg(this->files.join(";")));
    output.append(QString("media:%1").arg(this->media.toString()));
//    MediaList media;
//    PlaylistList playlists;
//    IFramePlaylistList iframe_playlists;
//    Start *start = NULL;
//    ServerControl *server_control = NULL;
//    PartInformation *part_inf = NULL;
//    Skip *skip = NULL;
//    RenditionReportList rendition_reports;
//    SessionDataList session_data;
//    QList<SessionKey *> session_keys;
//    PreloadHint *preload_hint = NULL;
//    ContentSteering *content_steering = NULL;

//    bool is_variant = false;
//    bool is_endlist = false;
//    bool is_i_frames_only = false;
//    int target_duration = -1;
//    int media_sequence = -1;
//    QDateTime program_date_time;
//    bool is_independent_segments = false;
    output.append(QString("version:%1").arg(this->version));
    output.append(QString("allow_cache:%1").arg(this->allow_cache));
    output.append(QString("playlist_type:%1").arg(this->playlist_type));
    output.append(QString("discontinuity_sequence:%1").arg(this->discontinuity_sequence));

    return output.join(";");
}

QString Key::tag() {
    return Protocol::ext_x_key;
}

Key::Key(QString method, QString base_uri, QString uri, QString iv, QString keyformat, QString keyformatversions, QMap<QString, QString> kwargs) {
    this->method = method;
    this->uri = uri;
    this->iv = iv;
    this->keyformat = keyformat;
    this->keyformatversions = keyformatversions;
    this->base_uri = base_uri;
    this->_extra_params = kwargs;
}

Key::Key(QString base_uri, QMap<QString, QString> kwargs) {
    this->base_uri = base_uri;
    if (kwargs.find("method") != kwargs.end()) {
        this->method = kwargs["method"];
        kwargs.remove("method");
    }
    if (kwargs.find("uri") != kwargs.end()) {
        this->uri = kwargs["uri"];
        kwargs.remove("uri");
    }
    if (kwargs.find("iv") != kwargs.end()) {
        this->iv = kwargs["iv"];
        kwargs.remove("iv");
    }
    if (kwargs.find("keyformat") != kwargs.end()) {
        this->keyformat = kwargs["keyformat"];
        kwargs.remove("keyformat");
    }
    if (kwargs.find("keyformatversions") != kwargs.end()) {
        this->keyformat = kwargs["keyformatversions"];
        kwargs.remove("keyformatversions");
    }
    this->_extra_params = kwargs;
}

QString Key::toString() {
    QStringList output;
    output.append(QString("METHOD=%1").arg(this->method));
    if (this->uri != "") {
        output.append(QString(R"(URI="%1")").arg(this->uri));
    }
    if (this->iv != "") {
        output.append(QString("IV=%1").arg(this->iv));
    }
    if (this->keyformat != "") {
        output.append(QString(R"(KEYFORMAT="%1")").arg(this->keyformat));
    }
    if (this->keyformatversions != "") {
        output.append(QString(R"(KEYFORMATVERSIONS="%1")").arg(this->keyformatversions));
    }
    return this->tag() + ":" + output.join(",");
}

bool Key::operator==(const Key &other) const {
    return this->method == other.method &&
           this->uri == other.uri &&
           this->iv == other.iv &&
           this->base_uri == other.base_uri &&
           this->keyformat == other.keyformat &&
           this->keyformatversions == other.keyformatversions;
}

bool Key::operator!=(const Key &other) const {
    return !(*this == other);
}

Segment::Segment(QString uri, QString base_uri, QDateTime program_date_time, QDateTime current_program_date_time,
        double duration, QString title, QString bitrate, QString byterange, bool cue_out,
        bool cue_out_start, bool cue_in, bool discontinuity, QString key, QString scte35,
        QString oatcls_scte35, QString scte35_duration, QString scte35_elapsedtime, QMap<QString, QVariant> asset_metadata,
        Key *keyobject, QList<QVariant> parts, QMap<QString, QVariant> init_section, QList<QVariant> dateranges, bool gap_tag,
        int media_sequence, QMap<QString, QVariant> custom_parser_values) {
    this->media_sequence = media_sequence;
    this->uri = uri;
    this->duration = duration;
    this->title = title;
    this->_base_uri = base_uri;
    this->bitrate = bitrate;
    this->byterange = byterange;
    this->program_date_time = program_date_time;
    this->current_program_date_time = current_program_date_time;
    this->discontinuity = discontinuity;
    this->cue_out_start = cue_out_start;
    this->cue_out = cue_out;
    this->cue_in = cue_in;
    this->scte35 = scte35;
    this->oatcls_scte35 = oatcls_scte35;
    this->scte35_duration = scte35_duration;
    this->scte35_elapsedtime = scte35_elapsedtime;
    this->asset_metadata = asset_metadata;
    this->key = keyobject;
    for (int i = 0; i < parts.size(); i++) {
        QMap<QString, QVariant> partial = parts[i].toMap();
        this->parts.list.append(PartialSegment(this->_base_uri, partial));
    }
    if (init_section.isEmpty()) {
        this->init_section = NULL;
    }
    else {
        this->init_section = new InitializationSection(this->_base_uri, init_section);
    }
    for (int i = 0; i < dateranges.size(); i++) {
        QMap<QString, QVariant> daterange = dateranges[i].toMap();
        this->dateranges.list.append(DateRange(daterange));
    }
    this->gap_tag = gap_tag;
    this->custom_parser_values = custom_parser_values;
}

Segment::Segment(QString base_uri, Key *keyobject, QMap<QString, QVariant> kwargs) {
    // qDebug() << "Segment::Segment start";
    this->_base_uri = base_uri;
    this->key = keyobject;

    if (kwargs.find("media_sequence") != kwargs.end()) {
        this->media_sequence = kwargs["media_sequence"].toInt();
        kwargs.remove("media_sequence");
    }
    if (kwargs.find("uri") != kwargs.end()) {
        this->uri = kwargs["uri"].toString();
        kwargs.remove("uri");
    }
    if (kwargs.find("duration") != kwargs.end()) {
        this->duration = kwargs["duration"].toDouble();
        kwargs.remove("duration");
    }
    if (kwargs.find("title") != kwargs.end()) {
        this->title = kwargs["title"].toString();
        kwargs.remove("title");
    }
    if (kwargs.find("bitrate") != kwargs.end()) {
        this->bitrate = kwargs["bitrate"].toString();
        kwargs.remove("bitrate");
    }
    if (kwargs.find("byterange") != kwargs.end()) {
        this->byterange = kwargs["byterange"].toString();
        kwargs.remove("byterange");
    }
    if (kwargs.find("program_date_time") != kwargs.end()) {
        this->program_date_time = kwargs["program_date_time"].toDateTime();
        kwargs.remove("program_date_time");
    }
    if (kwargs.find("current_program_date_time") != kwargs.end()) {
        this->current_program_date_time = kwargs["current_program_date_time"].toDateTime();
        kwargs.remove("current_program_date_time");
    }
    if (kwargs.find("discontinuity") != kwargs.end()) {
        this->discontinuity = kwargs["discontinuity"].toBool();
        kwargs.remove("discontinuity");
    }
    if (kwargs.find("cue_out_start") != kwargs.end()) {
        this->cue_out_start = kwargs["cue_out_start"].toBool();
        kwargs.remove("cue_out_start");
    }
    if (kwargs.find("cue_out") != kwargs.end()) {
        this->cue_out = kwargs["cue_out"].toBool();
        kwargs.remove("cue_out");
    }
    if (kwargs.find("cue_in") != kwargs.end()) {
        this->cue_in = kwargs["cue_in"].toBool();
        kwargs.remove("cue_in");
    }
    if (kwargs.find("scte35") != kwargs.end()) {
        this->scte35 = kwargs["scte35"].toString();
        kwargs.remove("scte35");
    }
    if (kwargs.find("oatcls_scte35") != kwargs.end()) {
        this->oatcls_scte35 = kwargs["oatcls_scte35"].toString();
        kwargs.remove("oatcls_scte35");
    }
    if (kwargs.find("scte35_duration") != kwargs.end()) {
        this->scte35_duration = kwargs["scte35_duration"].toString();
        kwargs.remove("scte35_duration");
    }
    if (kwargs.find("scte35_elapsedtime") != kwargs.end()) {
        this->scte35_elapsedtime = kwargs["scte35_elapsedtime"].toString();
        kwargs.remove("scte35_elapsedtime");
    }
    if (kwargs.find("asset_metadata") != kwargs.end()) {
        this->asset_metadata = kwargs["asset_metadata"].toMap();
        kwargs.remove("asset_metadata");
    }
    if (kwargs.find("parts") != kwargs.end()) {
        QList<QVariant> the_parts = kwargs["parts"].toList();
        kwargs.remove("parts");
        for (int i = 0; i < the_parts.size(); i++) {
            QMap<QString, QVariant> partial = the_parts[i].toMap();
            this->parts.list.append(PartialSegment(this->_base_uri, partial));
        }
    }

    if (kwargs.find("init_section") != kwargs.end()) {
        QMap<QString, QVariant> the_init_section = kwargs["init_section"].toMap();
        kwargs.remove("init_section");
        if (the_init_section.isEmpty()) {
            this->init_section = NULL;
        }
        else {
            this->init_section = new InitializationSection(this->_base_uri, the_init_section);
        }
    }

    if (kwargs.find("dateranges") != kwargs.end()) {
        QList<QVariant> the_dateranges = kwargs["dateranges"].toList();
        kwargs.remove("dateranges");
        for (int i = 0; i < the_dateranges.size(); i++) {
            QMap<QString, QVariant> daterange = the_dateranges[i].toMap();
            this->dateranges.list.append(DateRange(daterange));
        }
    }

    if (kwargs.find("gap_tag") != kwargs.end()) {
        this->gap_tag = kwargs["gap_tag"].toBool();
        kwargs.remove("gap_tag");
    }
    this->custom_parser_values = kwargs;
}

Segment::~Segment() {
    if (this->init_section) {
        delete this->init_section;
    }
}

void Segment::add_part(PartialSegment part) {
    this->parts.list.append(part);
}

QString Segment::dumps(Segment *last_segment, QString timespec) {
    QStringList output;

    if (last_segment && this->key != last_segment->key) {
        output.append(this->key->toString());
        output.append("\n");
    }
    else {
        // # The key must be checked anyway now for the first segment
        if (this->key && !last_segment) {
            output.append(this->key->toString());
            output.append("\n");
        }
    }

    if (last_segment && this->init_section != last_segment->init_section) {
        if (!this->init_section) {
            throw "init section can't be None if previous is not None";
        }
        output.append(this->init_section->toString());
        output.append("\n");
    }
    else {
        if (this->init_section && !last_segment) {
            output.append(this->init_section->toString());
            output.append("\n");
        }
    }

    if (this->discontinuity) {
        output.append("#EXT-X-DISCONTINUITY\n");
    }
    if (this->program_date_time.isValid()) {
        output.append(QString("#EXT-X-PROGRAM-DATE-TIME:%1\n").arg(Parser::format_date_time(this->program_date_time, timespec=timespec)));
    }

    if (this->dateranges.list.size() > 0) {
        output.append(this->dateranges.toString());
        output.append("\n");
    }

    if (this->cue_out_start) {
        if (this->oatcls_scte35 != "") {
            output.append(QString("%1:%2\n").arg(Protocol::ext_oatcls_scte35).arg(this->oatcls_scte35));
        }

        if (!this->asset_metadata.isEmpty()) {
            QStringList asset_suffix;
            for (QMap<QString, QVariant>::iterator it = this->asset_metadata.begin(); it != this->asset_metadata.end(); it++) {
                QString metadata_key = it.key();
                QString metadata_value = it.value().toString();
                asset_suffix.append(QString("%1=%2").arg(metadata_key.toUpper()).arg(metadata_value));
            }
            output.append(QString("%1:%2\n").arg(Protocol::ext_x_asset).arg(asset_suffix.join(",")));
        }

        output.append(QString("#EXT-X-CUE-OUT%1\n").arg(this->scte35_duration != "" ? ":"+this->scte35_duration : ""));
    }
    else if (this->cue_out) {
        QStringList cue_out_cont_suffix;
        if (this->scte35_elapsedtime != "") {
            cue_out_cont_suffix.append(QString("ElapsedTime=%1").arg(this->scte35_elapsedtime));
        }
        if (this->scte35_duration != "") {
            cue_out_cont_suffix.append(QString("Duration=%1").arg(this->scte35_duration));
        }
        if (this->scte35 != "") {
            cue_out_cont_suffix.append(QString("SCTE35=%1").arg(this->scte35));
        }

        QString cue_out_cont_suffix_str = "";

        if (cue_out_cont_suffix.size()) {
            cue_out_cont_suffix_str = ":" + cue_out_cont_suffix.join(",");
        }
        output.append(QString("#EXT-X-CUE-OUT-CONT%1\n").arg(cue_out_cont_suffix_str));
    }
    if (this->cue_in) {
        output.append("#EXT-X-CUE-IN\n");
    }

    if (this->parts.list.size()) {
        output.append(this->parts.toString());
        output.append("\n");
    }

    if (this->uri != "") {
        if (this->duration > 0) {
            output.append(QString("#EXTINF:%1").arg(number_to_string(this->duration)));
            if (this->title != "") {
                output.append(this->title);
            }
            output.append("\n");
        }

        if (this->byterange != "") {
            output.append(QString("#EXT-X-BYTERANGE:%1\n").arg(this->byterange));
        }

        if (this->bitrate != "") {
            output.append(QString("#EXT-X-BITRATE:%1\n").arg(this->bitrate));
        }

        if (this->gap_tag) {
            output.append("#EXT-X-GAP\n");
        }

        output.append(this->uri);
    }

    return output.join("");
}

QString Segment::toString() {
    return this->dumps(NULL);
}

void Segment::set_base_path(QString newbase_path) {
    this->BasePathMixin::set_base_path(newbase_path);
    this->parts.set_base_path(newbase_path);
    if (this->init_section) {
        this->init_section->set_base_path(newbase_path);
    }
}

QString Segment::base_uri() {
    return this->_base_uri;
}

void Segment::set_base_uri(QString newbase_uri) {
    this->_base_uri = newbase_uri;
    this->parts.set_base_uri(newbase_uri);
    if (this->init_section) {
        this->init_section->base_uri = newbase_uri;
    }
}

QString SegmentList::toString() {
    QStringList output;
    Segment *last_segment = NULL;
    for (int i = 0; i < this->list.size(); i++) {
        Segment *segment = &(this->list[i]);
        output.append(segment->dumps(last_segment));
        last_segment = segment;
    }
    return output.join("\n");
}

QStringList SegmentList::uri() {
    QStringList ans;
    for (int i = 0; i < this->list.size(); i++) {
        ans.append(this->list[i].uri);
    }
    return ans;
}

QList<Segment> SegmentList::by_key(Key *key) {
    QList<Segment> ans;
    for (int i = 0; i < this->list.size(); i++) {
        Segment *segment = &(this->list[i]);
        if (segment->key == key) {
            ans.append(*segment);
        }
    }
    return ans;
}

PartialSegment::PartialSegment(QString base_uri, QString uri, double duration, QDateTime program_date_time, QDateTime current_program_date_time, QString byterange, QString independent, QString gap, QList<QVariant> dateranges, QString gap_tag) {
//   self.base_uri = base_uri
//        self.uri = uri
//        self.duration = duration
//        self.program_date_time = program_date_time
//        self.current_program_date_time = current_program_date_time
//        self.byterange = byterange
//        self.independent = independent
//        self.gap = gap
//        self.dateranges = DateRangeList( [ DateRange(**daterange) for daterange in dateranges ] if dateranges else [] )
//        self.gap_tag = gap_tag
}

PartialSegment::PartialSegment(QString base_uri, QMap<QString, QVariant> kwargs) {

}

QString PartialSegment::toString() {
    return "";
}

QString PartialSegmentList::toString() {
    QStringList output;
    for (int i = 0;  i < this->list.size(); i++) {
        output.append(this->list[i].toString());
    }
    return output.join("\n");
}

QString InitializationSection::tag() {
    return Protocol::ext_x_map;
}

InitializationSection::InitializationSection(QString base_uri, QString uri, QString byterange) {
    this->base_uri = base_uri;
    this->uri = uri;
    this->byterange = byterange;
}

InitializationSection::InitializationSection(QString base_uri, QMap<QString, QVariant> kwargs) {
    this->base_uri = base_uri;
    this->uri = kwargs["uri"].toString();
    this->byterange = kwargs["byterange"].toString();
}

QString InitializationSection::toString() {
    QStringList output;
    if (this->uri != "") {
        output.append(QString("URI=") + quoted(this->uri));
    }
    if (this->byterange != "") {
        output.append(QString("BYTERANGE=") + quoted(this->byterange));
        return QString("%1:%2").arg(this->tag()).arg(output.join(","));
    }
}

bool InitializationSection::operator==(InitializationSection &other) const {
    return this->uri == other.uri &&
           this->byterange == other.byterange &&
           this->base_uri == other.base_uri;
}

bool InitializationSection::operator!=(InitializationSection &other) const {
    return !(*this == other);
}

QString SessionKey::tag() {
    return Protocol::ext_x_session_key;
}

StreamInfo::StreamInfo(QMap<QString, QVariant> kwargs) {
    this->bandwidth = kwargs["bandwidth"].toInt();
    this->closed_captions = kwargs["closed_captions"].toString();
    this->average_bandwidth = kwargs["average_bandwidth"].toInt();
    this->program_id = kwargs["program_id"].toInt();
    QList<QVariant> resolution = kwargs["resolution"].toList();
    this->resolution = QPair<int, int>(resolution[0].toInt(), resolution[1].toInt());
    this->codecs = kwargs["codecs"].toString();
    this->audio = kwargs["audio"].toString();
    this->video = kwargs["video"].toString();
    this->subtitles = kwargs["subtitles"].toString();
    this->frame_rate = kwargs["frame_rate"].toDouble();
    this->video_range = kwargs["video_range"].toString();
    this->hdcp_level = kwargs["hdcp_level"].toString();
    this->pathway_id = kwargs["pathway_id"].toString();
    this->stable_variant_id = kwargs["stable_variant_id"].toString();
}

QString StreamInfo::toString() {
    QStringList stream_inf;
    if (this->program_id != -1) {
        stream_inf.append(QString("PROGRAM-ID=%1").arg(this->program_id));
    }
    if (this->closed_captions != "") {
        stream_inf.append(QString("CLOSED-CAPTIONS=%1").arg(this->closed_captions));
    }
    if (this->bandwidth != -1) {
        stream_inf.append(QString("BANDWIDTH=%1").arg(this->bandwidth));
    }
    if (this->average_bandwidth != -1) {
        stream_inf.append(QString("AVERAGE-BANDWIDTH=%1").arg(this->average_bandwidth));
    }
    if (this->resolution.first != -1 && this->resolution.second != -1) {
        QString res = QString("%1x%2").arg(this->resolution.first).arg(this->resolution.second);
        stream_inf.append(QString("RESOLUTION=").arg(res));
    }
    if (this->frame_rate != -1) {
        // stream_inf.append('FRAME-RATE=%g' % decimal.Decimal(self.frame_rate).quantize(decimal.Decimal('1.000')));
        stream_inf.append(QString("FRAME-RATE=%1").arg(QString::number(this->frame_rate, 'g', 3)));
    }
    if (this->codecs != "") {
        stream_inf.append(QString("CODECS=%1").arg(quoted(this->codecs)));
    }
    if (this->video_range != "") {
        stream_inf.append(QString("VIDEO-RANGE=%1").arg(this->video_range));
    }
    if (this->hdcp_level != "") {
        stream_inf.append(QString("HDCP-LEVEL=%1").arg(this->hdcp_level));
    }
    if (this->pathway_id != "") {
        stream_inf.append(QString("PATHWAY-ID=%1").arg(quoted(this->pathway_id)));
    }
    if (this->stable_variant_id != "") {
        stream_inf.append(QString("STABLE-VARIANT-ID=%1").arg(quoted(this->stable_variant_id)));
    }
    return stream_inf.join(",");
}

Media::Media(QString uri, QString type, QString group_id, QString language,
      QString name, QString arg_default, QString autoselect, QString forced,
      QString characteristics, QString channels, QString stable_rendition_id,
      QString assoc_language, QString instream_id, QString base_uri, QMap<QString, QVariant> extras) {
    this->base_uri = base_uri;
    this->uri = uri;
    this->type = type;
    this->group_id = group_id;
    this->language = language;
    this->name = name;
    this->arg_default = arg_default;
    this->autoselect = autoselect;
    this->forced = forced;
    this->assoc_language = assoc_language;
    this->instream_id = instream_id;
    this->characteristics = characteristics;
    this->channels = channels;
    this->stable_rendition_id = stable_rendition_id;
    this->extras = extras;
}

Media::Media(QString base_uri, QMap<QString, QVariant> kwargs) {
    this->base_uri = base_uri;
    if (kwargs.find("uri") != kwargs.end()) {
        this->uri = kwargs["uri"].toString();
        kwargs.remove("uri");
    }
    if (kwargs.find("type") != kwargs.end()) {
        this->type = kwargs["type"].toString();
        kwargs.remove("type");
    }
    if (kwargs.find("group_id") != kwargs.end()) {
        this->group_id = kwargs["group_id"].toString();
        kwargs.remove("group_id");
    }
    if (kwargs.find("language") != kwargs.end()) {
        this->language = kwargs["language"].toString();
        kwargs.remove("language");
    }
    if (kwargs.find("name") != kwargs.end()) {
        this->name = kwargs["name"].toString();
        kwargs.remove("name");
    }
    if (kwargs.find("default") != kwargs.end()) {
        this->arg_default = kwargs["default"].toString();
        kwargs.remove("default");
    }
    if (kwargs.find("autoselect") != kwargs.end()) {
        this->autoselect = kwargs["autoselect"].toString();
        kwargs.remove("autoselect");
    }
    if (kwargs.find("forced") != kwargs.end()) {
        this->forced = kwargs["forced"].toString();
        kwargs.remove("forced");
    }
    if (kwargs.find("assoc_language") != kwargs.end()) {
        this->assoc_language = kwargs["assoc_language"].toString();
        kwargs.remove("assoc_language");
    }
    if (kwargs.find("instream_id") != kwargs.end()) {
        this->instream_id = kwargs["instream_id"].toString();
        kwargs.remove("instream_id");
    }
    if (kwargs.find("characteristics") != kwargs.end()) {
        this->characteristics = kwargs["characteristics"].toString();
        kwargs.remove("characteristics");
    }
    if (kwargs.find("channels") != kwargs.end()) {
        this->channels = kwargs["channels"].toString();
        kwargs.remove("channels");
    }
    if (kwargs.find("stable_rendition_id") != kwargs.end()) {
        this->stable_rendition_id = kwargs["stable_rendition_id"].toString();
        kwargs.remove("stable_rendition_id");
    }
    this->extras = kwargs;
}

QString Media::dumps() {
    QStringList media_out;

    if (this->uri != "") {
        media_out.append(QString("URI=%1").arg(quoted(this->uri)));
    }
    if (this->type != "") {
        media_out.append(QString("TYPE=%1").arg(this->type));
    }
    if (this->group_id != "") {
        media_out.append(QString("GROUP-ID=%1").arg(quoted(this->group_id)));
    }
    if (this->language != "") {
        media_out.append(QString("LANGUAGE=%1").arg(quoted(this->language)));
    }
    if (this->assoc_language != "") {
        media_out.append(QString("ASSOC-LANGUAGE=%1").arg(quoted(this->assoc_language)));
    }
    if (this->name != "") {
        media_out.append(QString("NAME=%1").arg(quoted(this->name)));
    }
    if (this->arg_default != "") {
        media_out.append(QString("DEFAULT=%1").arg(this->arg_default));
    }
    if (this->autoselect != "") {
        media_out.append(QString("AUTOSELECT=%1").arg(this->autoselect));
    }
    if (this->forced != "") {
        media_out.append(QString("FORCED=%1").arg(this->forced));
    }
    if (this->instream_id != "") {
        media_out.append(QString("INSTREAM-ID=%1").arg(quoted(this->instream_id)));
    }
    if (this->characteristics != "") {
        media_out.append(QString("CHARACTERISTICS=%1").arg(quoted(this->characteristics)));
    }
    if (this->channels != "") {
        media_out.append(QString("CHANNELS=%1").arg(quoted(this->channels)));
    }
    if (this->stable_rendition_id != "") {
        media_out.append(QString("STABLE-RENDITION-ID=%1").arg(quoted(this->stable_rendition_id)));
    }

    return QString("#EXT-X-MEDIA:%1").arg(media_out.join(","));
}

QString Media::toString() {
    return this->dumps();
}

QStringList MediaList::uri() {
    QStringList ans;
    for (int i = 0; i < this->list.size(); i++) {
        ans.append(this->list[i].uri);
    }
    return ans;
}

Playlist::Playlist(QString uri, QMap<QString, QVariant> stream_info, MediaList media, QString base_uri) {
    this->uri = uri;
    this->base_uri = base_uri;

    QString resolution = stream_info["resolution"].toString();
    QList<QVariant> resolution_pair;
    if (resolution != "") {
        resolution = Parser::strip(resolution);
        QStringList values = resolution.split("x");
        resolution_pair.append(QVariant(values[0].toInt()));
        resolution_pair.append(QVariant(values[1].toInt()));
    }
    stream_info["resolution"] = resolution_pair;

    this->stream_info = StreamInfo(stream_info);
    QList<QString> group_list = {"audio", "video", "subtitles"};
    QString group_id = "";
    for (int i = 0; i < group_list.size(); i++) {
        if (stream_info.find(group_list[i]) != stream_info.end()) {
            for (int j = 0; j < media.list.size(); j++) {
                Media *m = &(media.list[j]);
                if (m->group_id == group_id) {
                    this->media.append(*m);
                }
            }
        }
    }
}

Playlist::Playlist(QString base_uri, MediaList media, QMap<QString, QVariant> kwargs) {
    new (this) Playlist(
        kwargs.find("uri") != kwargs.end() ? kwargs["uri"].toString() : "",
        kwargs.find("stream_info") != kwargs.end() ? kwargs["stream_info"].toMap() : QMap<QString, QVariant>(),
        media, base_uri
    );
}

QString Playlist::toString() {
    QStringList media_types;
    QStringList stream_inf;
    stream_inf.append(this->stream_info.toString());
    for (int i = 0; i < this->media.size(); i++) {
        Media *media = &(this->media[i]);
        if (media_types.contains(media->type)) {
            continue;
        }
        else {
            media_types.append(media->type);
            QString media_type = media->type.toUpper();
            stream_inf.append(QString(R"(%1="%1")").arg(media_type).arg(media->group_id));
        }
    }

    return QString("#EXT-X-STREAM-INF:") + stream_inf.join(",") + "\n" + this->uri;
}

IFramePlaylist::IFramePlaylist(QString base_uri, QString uri, QMap<QString, QVariant> iframe_stream_info) {
    this->uri = uri;
    this->base_uri = base_uri;

    QString resolution = iframe_stream_info["resolution"].toString();
    QList<QVariant> resolution_pair;
    if (resolution != "") {
        resolution = Parser::strip(resolution);
        QStringList values = resolution.split("x");
        resolution_pair.append(QVariant(values[0].toInt()));
        resolution_pair.append(QVariant(values[1].toInt()));
    }
    iframe_stream_info["resolution"] = resolution_pair;
    // # Audio, subtitles, and closed captions should not exist in
    // # EXT-X-I-FRAME-STREAM-INF, so just hardcode them to None.
    iframe_stream_info.remove("audio");
    iframe_stream_info.remove("subtitles");
    iframe_stream_info.remove("closed_captions");
    iframe_stream_info.remove("frame_rate");

    this->iframe_stream_info = StreamInfo(iframe_stream_info);
}

QString IFramePlaylist::toString() {
    QStringList iframe_stream_inf;
    if (this->iframe_stream_info.program_id != -1) {
        iframe_stream_inf.append(QString("PROGRAM-ID=%1").arg(this->iframe_stream_info.program_id));
    }
    if (this->iframe_stream_info.bandwidth != -1) {
        iframe_stream_inf.append(QString("BANDWIDTH=%1").arg(this->iframe_stream_info.bandwidth));
    }
    if (this->iframe_stream_info.average_bandwidth != -1) {
        iframe_stream_inf.append(QString("AVERAGE-BANDWIDTH=%1").arg(this->iframe_stream_info.average_bandwidth));
    }
    if (this->iframe_stream_info.resolution.first != -1 && this->iframe_stream_info.resolution.second != -1) {
        QString res = QString::number(this->iframe_stream_info.resolution.first) + 'x' + QString::number(this->iframe_stream_info.resolution.second);
        iframe_stream_inf.append(QString("RESOLUTION=%1").arg(res));
    }
    if (this->iframe_stream_info.codecs != "") {
        iframe_stream_inf.append(QString("CODECS=%1").arg(quoted(this->iframe_stream_info.codecs)));
    }
    if (this->iframe_stream_info.video_range != "") {
        iframe_stream_inf.append(QString("VIDEO-RANGE=%1").arg(this->iframe_stream_info.video_range));
    }
    if (this->iframe_stream_info.hdcp_level != "") {
        iframe_stream_inf.append(QString("HDCP-LEVEL=%1").arg(this->iframe_stream_info.hdcp_level));
    }
    if (this->uri != "") {
        iframe_stream_inf.append(QString("URI=%1").arg(quoted(this->uri)));
    }
    if (this->iframe_stream_info.pathway_id != "") {
        iframe_stream_inf.append(QString("PATHWAY-ID=%1").arg(quoted(this->iframe_stream_info.pathway_id)));
    }
    if (this->iframe_stream_info.stable_variant_id != "") {
        iframe_stream_inf.append(QString("STABLE-VARIANT-ID=%1").arg(quoted(this->iframe_stream_info.stable_variant_id)));
    }

    return QString("#EXT-X-I-FRAME-STREAM-INF:%1").arg(iframe_stream_inf.join(","));
}

Start::Start(double time_offset, QString precise) {
    this->time_offset = (double)time_offset;
    this->precise = precise;
}

QString Start::toString() {
    QStringList output = {
        QString("TIME-OFFSET=%1").arg(QString::number(this->time_offset))
    };
    if (this->precise == "YES" || this->precise == "NO") {
        output.append(QString("PRECISE=%1").arg(this->precise));
    }

    return Protocol::ext_x_start + ":" + output.join(",");
}

RenditionReport::RenditionReport(QString base_uri, QString uri, double last_msn, double last_part) {
    this->base_uri = base_uri;
    this->uri = uri;
    this->last_msn = last_msn;
    this->last_part = last_part;
}

QString RenditionReport::dumps() {
    QStringList report;
    report.append(QString("URI=%1").arg(quoted(this->uri)));
    report.append(QString("LAST-MSN=%1").arg(number_to_string(this->last_msn)));
    if (this->last_part > 0) {
        report.append(QString("LAST-PART=%1").arg(number_to_string(this->last_part)));
    }

    return QString("#EXT-X-RENDITION-REPORT:%1").arg(report.join(","));
}

QString RenditionReport::toString() {
    return this->dumps();
}

QString RenditionReportList::toString() {
    QStringList output;
    for (int i = 0; i < this->list.size(); i++) {
        output.append(this->list[i].toString());
    }
    return output.join("\n");
}

ServerControl::ServerControl(double can_skip_until, QString can_block_reload, double hold_back, double part_hold_back, QString can_skip_dateranges) {
    this->can_skip_until = can_skip_until;
    this->can_block_reload = can_block_reload;
    this->hold_back = hold_back;
    this->part_hold_back = part_hold_back;
    this->can_skip_dateranges = can_skip_dateranges;
}

QString ServerControl::dumps() {
    QStringList ctrl;
    if (this->can_block_reload != "") {
        ctrl.append(QString("CAN-BLOCK-RELOAD=%1").arg(this->can_block_reload));
    }

    ctrl.append(QString("%1=%2").arg(denormalize_attribute("hold_back")).arg(number_to_string(this->hold_back)));
    ctrl.append(QString("%1=%2").arg(denormalize_attribute("part_hold_back")).arg(number_to_string(this->part_hold_back)));

    if (this->can_skip_until > 0) {
        ctrl.append(QString("CAN-SKIP-UNTIL=%1").arg(number_to_string(this->can_skip_until)));

        if (this->can_skip_dateranges != "") {
            ctrl.append(QString("CAN-SKIP-DATERANGES=%1").arg(this->can_skip_dateranges));
        }
    }

    return QString("#EXT-X-SERVER-CONTROL:%1").arg(ctrl.join(","));
}

QString ServerControl::toString() {
    return this->dumps();
}

Skip::Skip(double skipped_segments, QString recently_removed_dateranges) {
    this->skipped_segments = skipped_segments;
    this->recently_removed_dateranges = recently_removed_dateranges;
}

QString Skip::dumps() {
    QStringList skip;
    skip.append(QString("SKIPPED-SEGMENTS=%1").arg(number_to_string(this->skipped_segments)));
    if (this->recently_removed_dateranges != "") {
        skip.append(QString("RECENTLY-REMOVED-DATERANGES=%1").arg(quoted(this->recently_removed_dateranges)));
    }

    return QString("#EXT-X-SKIP:%1").arg(skip.join(","));
}

QString Skip::toString() {
    return this->dumps();
}

PartInformation::PartInformation(double part_target) {
    this->part_target = part_target;
}

QString PartInformation::dumps() {
    return QString("#EXT-X-PART-INF:PART-TARGET=%1").arg(number_to_string(this->part_target));
}

QString PartInformation::toString() {
    return this->dumps();
}

PreloadHint::PreloadHint(QString type, QString base_uri, QString uri, double byterange_start, double byterange_length) {
    this->hint_type = type;
    this->base_uri = base_uri;
    this->uri = uri;
    this->byterange_start = byterange_start;
    this->byterange_length = byterange_length;
}

QString PreloadHint::dumps() {
    QStringList hint;
    hint.append(QString("TYPE=%1").arg(this->hint_type));
    hint.append(QString("URI=%1").arg(quoted(this->uri)));

    hint.append(QString("%1=%2").arg(denormalize_attribute("byterange_start")).arg(number_to_string(this->byterange_start)));
    hint.append(QString("%1=%2").arg(denormalize_attribute("byterange_length")).arg(number_to_string(this->byterange_length)));

    return QString("#EXT-X-PRELOAD-HINT:%1").arg(hint.join(","));
}

QString PreloadHint::toString() {
    return this->dumps();
}

SessionData::SessionData(QString data_id, QString value, QString uri, QString language) {
    this->data_id = data_id;
    this->value = value;
    this->uri = uri;
    this->language = language;
}

QString SessionData::dumps() {
    QStringList session_data_out = {
        QString("DATA-ID=%1").arg(quoted(this->data_id))
    };

    if (this->value != "") {
        session_data_out.append(QString("VALUE=%1").arg(quoted(this->value)));
    }
    else if (this->uri != "") {
        session_data_out.append(QString("URI=%1").arg(quoted(this->uri)));
    }
    if (this->language != "") {
        session_data_out.append(QString("LANGUAGE=%1").arg(quoted(this->language)));
    }

    return QString("#EXT-X-SESSION-DATA:%1").arg(session_data_out.join(","));
}

QString SessionData::toString() {
    return this->dumps();
}

DateRange::DateRange(QMap<QString, QVariant> kwargs) {
    this->id = kwargs["id"].toString();
    this->start_date = kwargs["start_date"].toString();
    this->class_ = kwargs["class"].toString();
    this->end_date = kwargs["end_date"].toString();
    this->duration = kwargs["duration"].toDouble();
    this->planned_duration = kwargs["planned_duration"].toDouble();
    this->scte35_cmd = kwargs["scte35_cmd"].toString();
    this->scte35_out = kwargs["scte35_out"].toString();
    this->scte35_in = kwargs["scte35_in"].toString();
    this->end_on_next = kwargs["end_on_next"].toString();
    QMap<QString, QVariant>::iterator it;
    for (it = kwargs.begin(); it != kwargs.end(); it++) {
        if (it.key().startsWith("x_")) {
            this->x_client_attrs.append(QPair<QString, QString>(it.key(), it.value().toString()));
        }
    }
}

QString DateRange::dumps() {
    QStringList daterange;
    daterange.append(QString("ID=%1").arg(quoted(this->id)));

    // whilst START-DATE is technically REQUIRED by the spec, this is
    // contradicted by an example in the same document (see
    // https://tools.ietf.org/html/rfc8216#section-8.10), and also by
    // real-world implementations, so we make it optional here
    if (this->start_date != "") {
        daterange.append(QString("START-DATE=%1").arg(quoted(this->start_date)));
    }
    if (this->class_ != "") {
        daterange.append(QString("CLASS=%1").arg(quoted(this->class_)));
    }
    if (this->end_date != "") {
        daterange.append(QString("END-DATE=%1").arg(quoted(this->end_date)));
    }
    if (this->duration > 0) {
        daterange.append(QString("DURATION=%1").arg(number_to_string(this->duration)));
    }
    if (this->planned_duration > 0) {
        daterange.append(QString("PLANNED-DURATION=%1").arg(number_to_string(this->planned_duration)));
    }
    if (this->scte35_cmd != "") {
        daterange.append(QString("SCTE35-CMD=%1").arg(this->scte35_cmd));
    }
    if (this->scte35_out != "") {
        daterange.append(QString("SCTE35-OUT=%1").arg(this->scte35_out));
    }
    if (this->scte35_in != "") {
        daterange.append(QString("SCTE35-IN=%1").arg(this->scte35_in));
    }
    if (this->end_on_next != "") {
        daterange.append(QString("END-ON-NEXT=%1").arg(this->end_on_next));
    }

    // client attributes sorted alphabetically output order is predictable
    for (int i = 0; i < this->x_client_attrs.size(); i++) {
        auto item = this->x_client_attrs[i];
        daterange.append(QString("%1=%2").arg(denormalize_attribute(item.first)).arg(item.second));
    }

    return QString("#EXT-X-DATERANGE:%1").arg(daterange.join(","));
}

QString DateRange::toString() {
    return this->dumps();
}

ContentSteering::ContentSteering(QString base_uri, QString server_uri, QString pathway_id) {
    this->base_uri = base_uri;
    this->uri = server_uri;
    this->pathway_id = pathway_id;
}

QString ContentSteering::dumps() {
    QStringList steering;
    steering.append(QString("SERVER-URI=%1").arg(quoted(this->uri)));

    if (this->pathway_id != "") {
        steering.append(QString("PATHWAY-ID=%1").arg(quoted(this->pathway_id)));
    }

    return QString("#EXT-X-CONTENT-STEERING:%1").arg(steering.join(","));
}

QString ContentSteering::toString() {
    return this->dumps();
}
