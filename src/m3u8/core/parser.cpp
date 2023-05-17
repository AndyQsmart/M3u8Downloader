#include "parser.h"
#include "protocol.h"
#include "url_lib.h"
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

const QStringList Parser::URI_PREFIXES = {"https://", "http://", "s3://", "s3a://", "s3n://"};
const QRegExp Parser::ATTRIBUTELISTPATTERN = QRegExp(R"(''((?:[^,"']|"[^"]*"|'[^']*')+)'')");

QDateTime Parser::cast_data_time(QString value) {
    return QDateTime::fromString(value, Qt::ISODate);
}

QString Parser::format_date_time(QDateTime value, QString timespec) {
    if (timespec == "milliseconds") {
        return value.toString(Qt::ISODateWithMs);
    }
    else {
        return value.toString(Qt::ISODate);
    }
}

QMap<QString, QVariant> Parser::parse(QString content, bool strict, QString custom_tags_parser) {
    /*
        Given a M3U8 playlist content returns a dictionary with all data found
    */
    QMap<QString, QVariant> data;
    data["media_sequence"] = QVariant(0);
    data["is_variant"] = QVariant(false);
    data["is_endlist"] = QVariant(false);
    data["is_i_frames_only"] = QVariant(false);
    data["is_independent_segments"] = QVariant(false);
    data["playlist_type"] = QVariant(QString(""));
    data["playlists"] = QVariant(QList<QVariant>());
    data["segments"] = QVariant(QList<QVariant>());
    data["iframe_playlists"] = QVariant(QList<QVariant>());
    data["media"] = QVariant(QList<QVariant>());
    data["keys"] = QVariant(QList<QVariant>());
    data["rendition_reports"] = QVariant(QList<QVariant>());
    data["skip"] = QVariant(QMap<QString, QVariant>());
    data["part_inf"] = QVariant(QMap<QString, QVariant>());
    data["session_data"] = QVariant(QList<QVariant>());
    data["session_keys"] = QVariant(QList<QVariant>());
    data["segment_map"] = QVariant(QList<QVariant>());

    QMap<QString, QVariant> state;
    state["expect_segment"] = QVariant(false);
    state["expect_playlist"] = QVariant(false);
    state["current_key"] = QVariant(QString(""));
    state["current_segment_map"] = QVariant(QMap<QString, QVariant>());

    int lineno = 0;
    QStringList content_list = Parser::string_to_lines(content);
    for (int i = 0; i < content_list.size(); i++) {
        lineno += 1;
        // qDebug() << "Parser::parse:" << "i:" << i;
        // qDebug() << "content_list[i]:" << content_list[i];
        QString line = Parser::strip(content_list[i]);
        // qDebug() << "line:" << line;
        // # Call custom parser if needed
        // if line.startswith('#') and callable(custom_tags_parser):
            // go_to_next_line = custom_tags_parser(line, lineno, data, state)

            // # Do not try to parse other standard tags on this line if custom_tags_parser function returns 'True'
            // if go_to_next_line:
                // continue

        if (line.startsWith(Protocol::ext_x_byterange)) {
            Parser::_parse_byterange(line, state);
            state["expect_segment"] = QVariant(true);
            continue;
        }

        if (line.startsWith(Protocol::ext_x_bitrate)) {
            Parser::_parse_bitrate(line, state);
        }
        else if (line.startsWith(Protocol::ext_x_targetduration)) {
            // qDebug() << "parser ext_x_targetduration";
            Parser::_parse_simple_parameter(line, data, "float");
        }
        else if (line.startsWith(Protocol::ext_x_media_sequence)) {
            Parser::_parse_simple_parameter(line, data, "int");
        }
        else if (line.startsWith(Protocol::ext_x_discontinuity_sequence)) {
            Parser::_parse_simple_parameter(line, data, "int");
        }
        else if (line.startsWith(Protocol::ext_x_program_date_time)) {
            QPair<QString, QVariant> temp = Parser::_parse_simple_parameter_raw_value(line, "date_time");
            QVariant program_date_time = temp.second;
            if (data.find("program_date_time") == data.end()) {
                data["program_date_time"] = program_date_time;
            }
            state["current_program_date_time"] = program_date_time;
            state["program_date_time"] = program_date_time;
        }
        else if (line.startsWith(Protocol::ext_x_discontinuity)) {
            state["discontinuity"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_cue_out_cont)) {
            Parser::_parse_cueout_cont(line, state);
            state["cue_out"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_cue_out)) {
            Parser::_parse_cueout(line, state);
            state["cue_out_start"] = true;
            state["cue_out"] = true;
        }
        else if (line.startsWith(Protocol::ext_oatcls_scte35+":")) {
            Parser::_parse_oatcls_scte35(line, state);
        }
        else if (line.startsWith(Protocol::ext_x_asset+":")) {
            Parser::_parse_asset(line, state);
        }
        else if (line.startsWith(Protocol::ext_x_cue_in)) {
            state["cue_in"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_cue_span)) {
            state["cue_out"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_version)) {
            Parser::_parse_simple_parameter(line, data, "int");
        }
        else if (line.startsWith(Protocol::ext_x_allow_cache)) {
            Parser::_parse_simple_parameter(line, data);
        }
        else if (line.startsWith(Protocol::ext_x_key)) {
            QMap<QString, QVariant> key = Parser::_parse_key(line);
            state["current_key"] = key;
            QList<QVariant> keys = data["keys"].toList();
            if (!keys.contains(key)) {
                keys.append(key);
                data["keys"] = keys;
            }
        }
        else if (line.startsWith(Protocol::extinf)) {
            Parser::_parse_extinf(line, data, state, lineno, strict);
            state["expect_segment"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_stream_inf)) {
            state["expect_playlist"] = true;
            Parser::_parse_stream_inf(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_i_frame_stream_inf)) {
            Parser::_parse_i_frame_stream_inf(line, data);
        }
        else if (line.startsWith(Protocol::ext_x_media)) {
            Parser::_parse_media(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_playlist_type)) {
            Parser::_parse_simple_parameter(line, data);
        }
        else if (line.startsWith(Protocol::ext_i_frames_only)) {
            data["is_i_frames_only"] = true;
        }
        else if (line.startsWith(Protocol::ext_is_independent_segments)) {
            data["is_independent_segments"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_endlist)) {
            // qDebug() << "parse ext_x_endlist";
            data["is_endlist"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_map)) {
            QMap<QString, QString> quoted_parser;
            quoted_parser["uri"] = "remove_quotes";
            quoted_parser["byterange"] = "remove_quotes";

            QMap<QString, QVariant> segment_map_info = Parser::_parse_attribute_list(Protocol::ext_x_map, line, quoted_parser);
            state["current_segment_map"] = segment_map_info;
            QList<QVariant> segment_map = data["segment_map"].toList();
            segment_map.append(segment_map_info);
            data["segment_map"] = segment_map;
        }
        else if (line.startsWith(Protocol::ext_x_start)) {
            QMap<QString, QString> attribute_parser;
            attribute_parser["time_offset"] = "float";
            QMap<QString, QVariant> start_info = Parser::_parse_attribute_list(Protocol::ext_x_start, line, attribute_parser);
            data["start"] = start_info;
        }
        else if (line.startsWith(Protocol::ext_x_server_control)) {
            Parser::_parse_server_control(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_part_inf)) {
            Parser::_parse_part_inf(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_rendition_report)) {
            Parser::_parse_rendition_report(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_part)) {
            Parser::_parse_part(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_skip)) {
            Parser::_parse_skip(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_session_data)) {
            Parser::_parse_session_data(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_session_key)) {
            Parser::_parse_session_key(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_preload_hint)) {
            Parser::_parse_preload_hint(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_daterange)) {
            Parser::_parse_daterange(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_x_gap)) {
            state["gap"] = true;
        }
        else if (line.startsWith(Protocol::ext_x_content_steering)) {
            Parser::_parse_content_steering(line, data, state);
        }
        else if (line.startsWith(Protocol::ext_m3u)) {
            //# We don't parse #EXTM3U, it just should to be present;
        }
        else if (Parser::strip(line) == "") {
            //# blank lines are legal
        }
        else if (state["expect_segment"].toBool()) {
            Parser::_parse_ts_chunk(line, data, state);
            state["expect_segment"] = false;
        }
        else if (state["expect_playlist"].toBool()) {
            Parser::_parse_variant_playlist(line, data, state);
            state["expect_playlist"] = false;
        }
        else if (strict) {
            throw QString("lineno:")+QString::number(lineno)+QString(",line:")+line;
        }
    }

    // qDebug() << "Parser::parse:parse line end";

    if (state.find("segment") != state.end()) {
        qDebug() << "state.find(segment) != state.end()";
        QList<QVariant> segments_list = data["segments"].toList();
        segments_list.append(state["segment"]);
        data["segments"] = segments_list;
        state.remove("segment");
    }

    return data;
}

void Parser::_parse_byterange(QString line, QMap<QString, QVariant> &state) {
    if (state.find("segment") == state.end()) {
        state["segment"] = QVariant(QMap<QString, QVariant>());
    }
    QMap<QString, QVariant> temp = state["segment"].toMap();
    temp["byterange"] = QVariant(line.replace(Protocol::ext_x_byterange + ":", ""));
    state["segment"] = QVariant(temp);
}

void Parser::_parse_bitrate(QString line, QMap<QString, QVariant> &state) {
    if (state.find("segment") == state.end()) {
        state["segment"] = QVariant(QMap<QString, QVariant>());
    }
    QMap<QString, QVariant> temp = state["segment"].toMap();
    temp["bitrate"] = QVariant(line.replace(Protocol::ext_x_byterange + ":", ""));
    state["segment"] = QVariant(temp);
}

QVariant Parser::_parse_simple_parameter(QString line, QMap<QString, QVariant> &data, QString cast_to) {
    return Parser::_parse_and_set_simple_parameter_raw_value(line, data, cast_to, true);
}

QVariant Parser::_parse_and_set_simple_parameter_raw_value(QString line, QMap<QString, QVariant> &data, QString cast_to, bool normalize) {
    QPair<QString, QVariant> temp = Parser::_parse_simple_parameter_raw_value(line, cast_to, normalize);
    // qDebug() << "Parser::_parse_and_set_simple_parameter_raw_value" << temp;
    QString param = temp.first;
    QVariant value = temp.second;
    data[param] = value;
    return data[param];
}

QMap<QString, QVariant> Parser::_parse_attribute_list(QString prefix, QString line, QMap<QString, QString> atribute_parser, QString default_parser) {
    QStringList temp = line.replace(prefix+":", "").split(Parser::ATTRIBUTELISTPATTERN);
    QList<QString> params; // params = temp[1::2]
    for (int i = 1; i < temp.size(); i += 2) {
        params.append(temp[i]);
    }

    QMap<QString, QVariant> attributes;
    for (int i = 0; i < params.size(); i++) {
        QString param = params[i];
        QPair<QString, QString> t = Parser::split(param, "=");
        QString name = t.first;
        QString value = t.second;
        QVariant ans_value = value;
        name = Parser::normalize_attribute(name);
        if (atribute_parser.find(name) != atribute_parser.end()) {
            QString parser = atribute_parser[name];
            if (parser == "remove_quotes") {
                ans_value = Parser::remove_quotes(value);
            }
            else if (parser == "int") {
                ans_value = value.toInt();
            }
            else if (parser == "float") {
                ans_value = value.toDouble();
            }
            else if (parser == "int_float") {
                ans_value = (int)value.toDouble();
            }
            else if (parser == "str") {
                ans_value = value;
            }
        }
        else if (default_parser.compare("") != 0) {
            if (default_parser == "str") {
                ans_value = value;
            }
        }

        attributes[name] = ans_value;
    }

    return attributes;
}

QString Parser::remove_quotes(QString str) {
    /*
        Remove quotes from string.

        Ex.:
              "foo" -> foo
              'foo' -> foo
              'foo  -> 'foo
    */
    int from_index = 0;
    int end_index = str.length();
    if (str[0] == "\"" || str[0] == "'") {
        from_index = 1;
    }
    if (str[str.length()-1] == "\"" || str[str.length()-1] == "'") {
        end_index = str.length()-1;
    }
    return str.mid(from_index, end_index-from_index);
}

void Parser::_parse_cueout_cont(QString line, QMap<QString, QVariant> &state) {
    QStringList elements = line.split(":");
    if (elements.size() < 2) {
        return;
    }

    QMap<QString, QString> atribute_parser;
    atribute_parser["duration"] = "remove_quotes";
    atribute_parser["elapsedtime"] = "remove_quotes";
    atribute_parser["scte35"] = "remove_quotes";
    QMap<QString, QVariant> cue_info = Parser::_parse_attribute_list(
        Protocol::ext_x_cue_out_cont,
        line,
        atribute_parser
    );

    bool has_duration = cue_info.find("duration") != cue_info.end();

    if (has_duration) {
        QVariant duration = cue_info["duration"];
        state["current_cue_out_duration"] = duration;
    }

    if (has_duration) {
        QVariant scte35 = cue_info["scte35"];
        state["current_cue_out_scte35"] = scte35;
    }

    if (cue_info.find("elapsedtime") != cue_info.end()) {
        QVariant elapsedtime = cue_info["elapsedtime"];
        state["current_cue_out_elapsedtime"] = elapsedtime;
    }
}

QPair<bool, QPair<QString, QString> > Parser::_cueout_no_duration(QString line) {
    // # this needs to be called first since line.split in all other
    // # parsers will throw a ValueError if passed just this tag
    if (line == Protocol::ext_x_cue_out) {
        return QPair<bool, QPair<QString, QString> >(true, QPair<QString, QString>("", ""));
    }
    else {
        return QPair<bool, QPair<QString, QString> >(false, QPair<QString, QString>());
    }
}

QPair<bool, QPair<QString, QString> > Parser::_cueout_envivio(QString line, QMap<QString, QVariant> &state) {
    QPair<QString, QString> temp = Parser::split(line, ":");
    QString param = temp.first;
    QString value = temp.second;
    QRegularExpression regex(".*DURATION=(.*),.*,CUE=\"(.*)\"");
    QRegularExpressionMatch match = regex.match(value);
    if (match.hasMatch()) {
        return QPair<bool, QPair<QString, QString> >(true, QPair<QString, QString>(match.captured(2), match.captured(1)));
    }
    else {
        return QPair<bool, QPair<QString, QString> >(false, QPair<QString, QString>());
    }
}

QPair<bool, QPair<QString, QString> > Parser::_cueout_duration(QString line) {
    // # This was added separately rather than modifying "simple"
    QPair<QString, QString> temp = Parser::split(line, ":");
    QString param = temp.first;
    QString value = temp.second;
    QRegularExpression regex("DURATION=(.*)");
    QRegularExpressionMatch match = regex.match(value);
    if (match.hasMatch()) {
        return QPair<bool, QPair<QString, QString> >(true, QPair<QString, QString>("", match.captured(1)));
    }
    else {
        return QPair<bool, QPair<QString, QString> >(false, QPair<QString, QString>());
    }
}

QPair<bool, QPair<QString, QString> > Parser::_cueout_simple(QString line) {
    QPair<QString, QString> temp = Parser::split(line, ":");
    QString param = temp.first;
    QString value = temp.second;
    QRegularExpression regex("^(\d+(?:\.\d)?\d*)$");
    QRegularExpressionMatch match = regex.match(value);
    if (match.hasMatch()) {
        return QPair<bool, QPair<QString, QString> >(true, QPair<QString, QString>("", match.captured(1)));
    }
    else {
        return QPair<bool, QPair<QString, QString> >(false, QPair<QString, QString>());
    }
}

void Parser::_parse_cueout(QString line, QMap<QString, QVariant> &state) {
    QPair<bool, QPair<QString, QString> > _cueout_state = Parser::_cueout_no_duration(line);
    if (!_cueout_state.first) {
        _cueout_state = Parser::_cueout_envivio(line, state);
    }
    if (!_cueout_state.first) {
        _cueout_state = Parser::_cueout_duration(line);
    }
    if (!_cueout_state.first) {
        _cueout_state = Parser::_cueout_simple(line);
    }

    if (_cueout_state.first) {
        QPair<QString, QString> temp = _cueout_state.second;
        QString cue_out_scte35 = temp.first;
        QString cue_out_duration = temp.second;
        QVariant current_cue_out_scte35 = state["current_cue_out_scte35"];
        if (cue_out_scte35 != "") {
            state["current_cue_out_scte35"] = cue_out_scte35;
        }
        state["current_cue_out_duration"] = cue_out_duration;
    }
}

void Parser::_parse_oatcls_scte35(QString line, QMap<QString, QVariant> &state) {
    QString scte35_cue =  Parser::split(line, ":").second;
    state["current_cue_out_oatcls_scte35"] = scte35_cue;
    state["current_cue_out_scte35"] = scte35_cue;
}

void Parser::_parse_asset(QString line, QMap<QString, QVariant> &state) {
    // # EXT-X-ASSET attribute values may or may not be quoted, and need to be URL-encoded.
    // # They are preserved as-is here to prevent loss of information.
    state["asset_metadata"] = Parser::_parse_attribute_list(
        Protocol::ext_x_asset,
        line,
        QMap<QString, QString>(),
        "str"
    );
}

QMap<QString, QVariant> Parser::_parse_key(QString line) {
    // params = ATTRIBUTELISTPATTERN.split(line.replace(protocol.ext_x_key + ':', ''))[1::2]
    QStringList temp = line.replace(Protocol::ext_x_key + ":", "").split(Parser::ATTRIBUTELISTPATTERN);
    QStringList params;
    for (int i = 1; i < temp.size(); i += 2) {
        params.append(temp[i]);
    }
    QMap<QString, QVariant> key;
    for (int i = 0; i < params.size(); i++) {
        QString param = params[i];
        QPair<QString, QString> t = Parser::split(param, "=");
        QString name = t.first;
        QString value = t.second;
        key[Parser::normalize_attribute(name)] = Parser::remove_quotes(value);
    }
    return key;
}

void Parser::_parse_extinf(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state, int lineno, bool strict) {
    // chunks = line.replace(protocol.extinf + ':', '').split(',', 1)
    QPair<QString, QString> chunks = Parser::split(line.replace(Protocol::extinf + ":", ""), ",");
    QString duration;
    QString title;
    if (chunks.second != "") {
        // if len(chunks) == 2:
        duration = chunks.first;
        title = chunks.second;
    }
    else if (chunks.first != "") {
        // len(chunks) == 1:
        if (strict) {
            throw QString("lineno:")+QString::number(lineno)+QString(",line:")+line;
        }
        else {
            duration = chunks.first;
            title = "";
        }
    }
    if (state.find("segment") == state.end()) {
        // if ('segment' not in state) {
        state["segment"] = QMap<QString, QVariant>();
    }
    QMap<QString, QVariant> segment = state["segment"].toMap();
    segment["duration"] = duration.toDouble();
    segment["title"] = title;
    state["segment"] = segment;
}

void Parser::_parse_stream_inf(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    data["is_variant"] = true;
    data["media_sequence"] = -1;
    QMap<QString, QString> atribute_parser;
    atribute_parser["codecs"] = "remove_quotes";
    atribute_parser["audio"] = "remove_quotes";
    atribute_parser["video"] = "remove_quotes";
    atribute_parser["video_range"] = "remove_quotes";
    atribute_parser["subtitles"] = "remove_quotes";
    atribute_parser["pathway_id"] = "remove_quotes";
    atribute_parser["stable_variant_id"] = "remove_quotes";
    atribute_parser["program_id"] = "int";
    atribute_parser["bandwidth"] = "int_float";
    atribute_parser["average_bandwidth"] = "int";
    atribute_parser["frame_rate"] = "float";
    atribute_parser["video_range"] = "str";
    atribute_parser["hdcp_level"] = "str";
    state["stream_info"] = Parser::_parse_attribute_list(Protocol::ext_x_stream_inf, line, atribute_parser);
}

void Parser::_parse_i_frame_stream_inf(QString line, QMap<QString, QVariant> &data) {
    QMap<QString, QString> atribute_parser;
    atribute_parser["codecs"] = "remove_quotes";
    atribute_parser["uri"] = "remove_quotes";
    atribute_parser["pathway_id"] = "remove_quotes";
    atribute_parser["stable_variant_id"] = "remove_quotes";
    atribute_parser["program_id"] = "int";
    atribute_parser["bandwidth"] = "int";
    atribute_parser["average_bandwidth"] = "int";
    atribute_parser["video_range"] = "str";
    atribute_parser["hdcp_level"] = "str";
    QMap<QString, QVariant> iframe_stream_info = Parser::_parse_attribute_list(Protocol::ext_x_i_frame_stream_inf, line, atribute_parser);
    QMap<QString, QVariant> iframe_playlist;

    if (iframe_stream_info.find("uri") != iframe_stream_info.end()) {
        iframe_playlist["uri"] = iframe_stream_info["uri"];
        iframe_stream_info.remove("uri");
    }
    iframe_playlist["iframe_stream_info"] = iframe_stream_info;
    QList<QVariant> iframe_playlists = data["iframe_playlists"].toList();
    iframe_playlists.append(iframe_playlist);
    data["iframe_playlists"] = iframe_playlists;
}

void Parser::_parse_media(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> atribute_parser;
    atribute_parser["uri"] = "remove_quotes";
    atribute_parser["group_id"] = "remove_quotes";
    atribute_parser["language"] = "remove_quotes";
    atribute_parser["assoc_language"] = "remove_quotes";
    atribute_parser["name"] = "remove_quotes";
    atribute_parser["instream_id"] = "remove_quotes";
    atribute_parser["characteristics"] = "remove_quotes";
    atribute_parser["channels"] = "remove_quotes";
    atribute_parser["stable_rendition_id"] = "remove_quotes";
    QMap<QString, QVariant> media = Parser::_parse_attribute_list(Protocol::ext_x_media, line, atribute_parser);
    QList<QVariant> media_list = data["media"].toList();
    media_list.append(media);
    data["media"] = media_list;
}

void Parser::_parse_server_control(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["can_block_reload"] = "str";
    attribute_parser["hold_back"] = "float";
    attribute_parser["part_hold_back"] = "float";
    attribute_parser["can_skip_until"] = "float";
    attribute_parser["can_skip_dateranges"] = "str";

    data["server_control"] = Parser::_parse_attribute_list(Protocol::ext_x_server_control, line, attribute_parser);
}

void Parser::_parse_part_inf(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["part_target"] = "float";

    data["part_inf"] = Parser::_parse_attribute_list(Protocol::ext_x_part_inf, line, attribute_parser);
}

void Parser::_parse_rendition_report(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["uri"] = "remove_quotes";
    attribute_parser["last_msn"] = "int";
    attribute_parser["last_part"] = "int";

    QMap<QString, QVariant> rendition_report = Parser::_parse_attribute_list(Protocol::ext_x_rendition_report, line, attribute_parser);

    QList<QVariant> rendition_reports = data["rendition_reports"].toList();
    rendition_reports.append(rendition_report);
    data["rendition_reports"] = rendition_reports;
}

void Parser::_parse_part(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["uri"] = "remove_quotes";
    attribute_parser["duration"] = "float";
    attribute_parser["independent"] = "str";
    attribute_parser["gap"] = "str";
    attribute_parser["byterange"] = "str";
    QMap<QString, QVariant> part = Parser::_parse_attribute_list(Protocol::ext_x_part, line, attribute_parser);

    // # this should always be true according to spec
    if (state.find("current_program_date_time") != state.end()) {
        part["program_date_time"] = state["current_program_date_time"];
        QDateTime current_program_date_time = state["current_program_date_time"].toDateTime();
        current_program_date_time.addSecs((int)part["duration"].toDouble());
        state["current_program_date_time"] = current_program_date_time;
        // state['current_program_date_time'] += datetime.timedelta(seconds=part['duration']);
    }

    if (state.find("dateranges") != state.end()) {
        part["dateranges"] = state["dateranges"];
        state.remove("dateranges");
    }
    if (state.find("gap") != state.end()) {
        part["gap_tag"] = state["gap"];
        state.remove("gap");
    }

    if (state.find("segment") == state.end()) {
        state["segment"] = QMap<QString, QVariant>();
    }
    QMap<QString, QVariant> segment = state["segment"].toMap();
    if (segment.find("parts") == segment.end()) {
        segment["parts"] = QList<QVariant>();
    }

    QList<QVariant> the_parts = segment["parts"].toList();
    the_parts.append(part);
    segment["parts"] = the_parts;
    state["segment"] = segment;
}

void Parser::_parse_skip(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["recently_removed_dateranges"] = "remove_quotes";
    attribute_parser["skipped_segments"] = "int";

    data["skip"] = Parser::_parse_attribute_list(Protocol::ext_x_skip, line, attribute_parser);
}

void Parser::_parse_session_data(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> quoted;
    quoted["data_id"] = "remove_quotes";
    quoted["value"] = "remove_quotes";
    quoted["uri"] = "remove_quotes";
    quoted["language"] = "remove_quotes";
    QMap<QString, QVariant> session_data = Parser::_parse_attribute_list(Protocol::ext_x_session_data, line, quoted);
    QList<QVariant> session_data_list = data["session_data"].toList();
    session_data_list.append(session_data);
    data["session_data"] = session_data_list;
}

void Parser::_parse_session_key(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    // params = ATTRIBUTELISTPATTERN.split(line.replace(protocol.ext_x_session_key + ':', ''))[1::2]
    QStringList temp = line.replace(Protocol::ext_x_session_key + ":", "").split(Parser::ATTRIBUTELISTPATTERN);
    QStringList params;
    for (int i = 1; i < temp.size(); i += 2) {
        params.append(temp[i]);
    }

    QMap<QString, QVariant> key;
    for (int i = 0; i < params.size(); i++) {
        QString param = params[i];
        QPair<QString, QString> t = Parser::split(param, "=");
        QString name = t.first;
        QString value = t.second;
        key[Parser::normalize_attribute(name)] = Parser::remove_quotes(value);
    }

    QList<QVariant> session_keys = data["session_keys"].toList();
    session_keys.append(key);
    data["session_keys"] = session_keys;
}

void Parser::_parse_preload_hint(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["uri"] = "remove_quotes";
    attribute_parser["type"] = "str";
    attribute_parser["byterange_start"] = "int";
    attribute_parser["byterange_length"] = "int";

    data["preload_hint"] = Parser::_parse_attribute_list(Protocol::ext_x_preload_hint, line, attribute_parser);
}

void Parser::_parse_daterange(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["id"] = "remove_quotes";
    attribute_parser["class"] = "remove_quotes";
    attribute_parser["start_date"] = "remove_quotes";
    attribute_parser["end_date"] = "remove_quotes";
    attribute_parser["duration"] = "float";
    attribute_parser["planned_duration"] = "float";
    attribute_parser["end_on_next"] = "str";
    attribute_parser["scte35_cmd"] = "str";
    attribute_parser["scte35_out"] = "str";
    attribute_parser["scte35_in"] = "str";

    QMap<QString, QVariant> parsed = Parser::_parse_attribute_list(Protocol::ext_x_daterange, line, attribute_parser);

    if (state.find("dateranges") == state.end()) {
        state["dateranges"] = QList<QVariant>();
    }

    QList<QVariant> dateranges = state["dateranges"].toList();
    dateranges.append(parsed);
    state["dateranges"] = dateranges;
}

void Parser::_parse_content_steering(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QString> attribute_parser;
    attribute_parser["server_uri"] = "remove_quotes";
    attribute_parser["pathway_id"] = "remove_quotes";

    data["content_steering"] = Parser::_parse_attribute_list(Protocol::ext_x_content_steering, line, attribute_parser);
}

void Parser::_parse_ts_chunk(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QVariant> segment = state["segment"].toMap();
    state.remove("segment");
    if (state.find("program_date_time") != state.end()) {
        segment["program_date_time"] = state["program_date_time"];
        state.remove("program_date_time");
    }
    if (state.find("current_program_date_time") != state.end()) {
        segment["current_program_date_time"] = state["current_program_date_time"];
        QDateTime current_program_date_time = state["current_program_date_time"].toDateTime();
        current_program_date_time.addSecs((int)segment["duration"].toDouble());
        state["current_program_date_time"] = current_program_date_time;
    }
    segment["uri"] = line;
    if (state.find("cue_in") != state.end()) {
        segment["cue_in"] = state["cue_in"];
        state.remove("cue_in");
    }
    else {
        segment["cue_in"] = false;
    }
    if (state.find("cue_out") != state.end()) {
        segment["cue_out"] = state["cue_out"];
        state.remove("cue_out");
    }
    else {
        segment["cue_out"] = false;
    }
    if (state.find("cue_out_start") != state.end()) {
        segment["cue_out_start"] = state["cue_out_start"];
        state.remove("cue_out_start");
    }
    else {
        segment["cue_out_start"] = false;
    }
    // scte_op = state.pop if segment['cue_in'] else state.get
    bool need_pop = false;
    if (segment["cue_in"].toBool()) {
        need_pop = true;
    }
    if (state.find("current_cue_out_scte35") != state.end()) {
        segment["scte35"] = state["current_cue_out_scte35"];
        if (need_pop) {
            state.remove("current_cue_out_scte35");
        }
    }
    if (state.find("current_cue_out_oatcls_scte35") != state.end()) {
        segment["oatcls_scte35"] = state["current_cue_out_oatcls_scte35"];
        if (need_pop) {
            state.remove("current_cue_out_oatcls_scte35");
        }
    }
    if (state.find("current_cue_out_duration") != state.end()) {
        segment["scte35_duration"] = state["current_cue_out_duration"];
        if (need_pop) {
            state.remove("current_cue_out_duration");
        }
    }
    if (state.find("current_cue_out_elapsedtime") != state.end()) {
        segment["scte35_elapsedtime"] = state["current_cue_out_elapsedtime"];
        if (need_pop) {
            state.remove("current_cue_out_elapsedtime");
        }
    }
    if (state.find("asset_metadata") != state.end()) {
        segment["asset_metadata"] = state["asset_metadata"];
        if (need_pop) {
            state.remove("asset_metadata");
        }
    }
    if (state.find("discontinuity") != state.end()) {
        segment["discontinuity"] = state["discontinuity"];
        state.remove("discontinuity");
    }
    else {
        segment["discontinuity"] = false;
    }

    if (state.find("current_key") != state.end()) {
        segment["key"] = state["current_key"];
    }
    else {
        // # For unencrypted segments, the initial key would be None
        if (!data["keys"].toList().contains(QVariant(QString("")))) {
            QList<QVariant> keys_list = data["keys"].toList();
            keys_list.append(QVariant(QString("")));
            data["keys"] = keys_list;
        }
    }
    if (state.find("current_segment_map") != state.end()) {
        segment["init_section"] = state["current_segment_map"];
    }
    if (state.find("dateranges") != state.end()) {
        segment["dateranges"] = state["dateranges"];
        state.remove("dateranges");
    }
    if (state.find("gap") != state.end()) {
        segment["gap_tag"] = state["gap"];
        state.remove("gap");
    }
    QList<QVariant> segments_list = data["segments"].toList();
    segments_list.append(segment);
    data["segments"] = segments_list;
}

void Parser::_parse_variant_playlist(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state) {
    QMap<QString, QVariant> playlist;
    playlist["uri"] = line;
    playlist["stream_info"] = state["stream_info"];
    state.remove("stream_info");

    QList<QVariant> playlists_list = data["playlists"].toList();
    playlists_list.append(playlist);
    data["playlists"] = playlists_list;
}

QString Parser::normalize_attribute(QString attribute) {
    return Parser::strip(attribute.replace("-", "_").toLower());
}

QVariant Parser::cast_to(QString value, QString cast_to_type) {
    if (cast_to_type == "str") {
        return QVariant(value);
    }
    else if (cast_to_type == "int") {
        return QVariant(value.toInt());
    }
    else if (cast_to_type == "float") {
        return QVariant(value.toDouble());
    }
    else if (cast_to_type == "date_time") {
        return QVariant(Parser::cast_data_time(value));
    }
    qDebug() << "(ERROR)Parser::cast_to:" << "no vaild cast_to_type:" << cast_to_type;
    return QVariant();
}

QPair<QString, QVariant> Parser::_parse_simple_parameter_raw_value(QString line, QString cast_to_type, bool normalize) {
    //    param, value = line.split(':', 1);
    QStringList line_split = line.split(":");
    QString param = "";
    if (line_split.size() > 0) {
        param = line_split[0];
    }
    QString value = "";
    if (line_split.size() >= 2) {
        for (int i = 1; i < line_split.size(); i++) {
            if (i == 1) {
                value += line_split[i];
            }
            else {
                value += ":"+line_split[i];
            }
        }
    }

    param = Parser::normalize_attribute(param.replace("#EXT-X-", ""));
    if (normalize) {
        value = Parser::strip(value).toLower();
    }
    return QPair<QString, QVariant>(param, Parser::cast_to(value, cast_to_type));
}

QPair<QString, QString> Parser::split(QString str, QString flag) {
    QStringList temp = str.split(flag);
    QString first_str = temp[0];
    QString second_str = "";
    for (int i = 1; i < temp.size(); i++) {
        if (i == 1) {
            second_str += temp[i];
        }
        else {
            second_str += flag+temp[i];
        }
    }
    return QPair<QString, QString>(first_str, second_str);
}

QString Parser::strip(QString str) {
    if (str.length() == 0) {
        return "";
    }
    int from = 0;
    int to = str.length()-1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == " ") {
            from = i;
        }
        else {
            break;
        }
    }
    for (int i = str.length()-1; i >= 0; i--) {
        if (str[i] == " ") {
            to = i;
        }
        else {
            break;
        }
    }
    // qDebug() << "Parser::strip:" << "from:" << from << "to:" << to << "str.length():" << str.length();
    if (str[from] == " ") {
        from++;
    }
    if (str[to] == " ") {
        to--;
    }
    if (from > str.length()-1) {
        from = str.length()-1;
    }
    if (to < 0) {
        to = 0;
    }
    if (from > to) {
        from = to;
    }
    // qDebug() << "Parser::strip:" << "from:" << from << "to:" << to << "str.length():" << str.length();
    QString ans = str.mid(from, to-from+1);

    return ans;
}

QStringList Parser::string_to_lines(QString str) {
    QString ans = Parser::strip(str);
    ans = ans.replace("\r\n", "\n").replace("\r", "\n");
    QStringList ans_list = ans.split("\n");
    return ans_list;
}

bool Parser::is_url(QString uri) {
    for (int i = 0; i < Parser::URI_PREFIXES.size(); i++) {
        // qDebug() << "Parser::is_url:" << "i:" << i;
        // qDebug() << "Parser::URI_PREFIXES:" << Parser::URI_PREFIXES[i];
        if (uri.startsWith(Parser::URI_PREFIXES[i])) {
            // qDebug() << "uri.startWith:" << Parser::URI_PREFIXES[i];
            return true;
        }
    }
    return false;
}

QString Parser::urljoin(QString base, QString url) {
    base = base.replace("://", "\1");
    url = url.replace("://", "\1");
    while (base.contains("//")) {
        base = base.replace("//", "/\0/");
    }
    while (url.contains("//")) {
        url = url.replace("//", "/\0/");
    }

    // qDebug() << "Parser::urljoin:" << "base:" << base << "url:" << url;

    return UrlLib::urljoin(base.replace("\1", "://"), url.replace("\1", "://")).replace("\0", "");
}
