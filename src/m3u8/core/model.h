#ifndef MODEL_H
#define MODEL_H

#include "mixins.h"
#include <QList>
#include <QPair>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QDateTime>

template <typename T>
class TagList {
public:
    QList<T> list;

    QString toString() {
        QStringList output;
        for (int i = 0; i < this->list.size(); i++) {
            output.append(this->list[i].toString());
        }
        return output.join("\n");
    }
};

class PartialSegment : public BasePathMixin {
    /*
    A partial segment from a M3U8 playlist

    `uri`
      a string with the segment uri

    `program_date_time`
      Returns the EXT-X-PROGRAM-DATE-TIME as a datetime. This field is only set
      if EXT-X-PROGRAM-DATE-TIME exists for this segment
      http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.5

    `current_program_date_time`
      Returns a datetime of this segment, either the value of `program_date_time`
      when EXT-X-PROGRAM-DATE-TIME is set or a calculated value based on previous
      segments' EXT-X-PROGRAM-DATE-TIME and EXTINF values

    `duration`
      duration attribute from EXTINF parameter

    `byterange`
      byterange attribute from EXT-X-BYTERANGE parameter

    `independent`
      the Partial Segment contains an independent frame

    `gap`
      GAP attribute indicates the Partial Segment is not available

    `dateranges`
      any dateranges that should precede the partial segment

    `gap_tag`
      GAP tag indicates one or more of the parent Media Segment's Partial
      Segments have a GAP=YES attribute. This tag should appear immediately
      after the first EXT-X-PART tag in the Parent Segment with a GAP=YES
      attribute.
    */
public:
    PartialSegment(QString base_uri, QString uri, double duration, QDateTime program_date_time=QDateTime(), QDateTime current_program_date_time=QDateTime(), QString byterange="", QString independent="", QString gap="", QList<QVariant> dateranges=QList<QVariant>(), QString gap_tag="");
    PartialSegment(QString base_uri, QMap<QString, QVariant> kwargs);
    QString toString();
};

class PartialSegmentList : public GroupedBasePathMixinList<PartialSegment> {
public:
    QString toString();
};

class DateRange {
public:
    QString id = "";
    QString start_date = "";
    QString class_ = "";
    QString end_date = "";
    double duration = -1;
    double planned_duration = -1;
    QString scte35_cmd = "";
    QString scte35_out = "";
    QString scte35_in = "";
    QString end_on_next = "";
    QList<QPair<QString, QString> > x_client_attrs;

    DateRange(QMap<QString, QVariant> kwargs);
    QString dumps();
    QString toString();
};

class DateRangeList : public TagList<DateRange> {

};

class Key : public BasePathMixin {
    /*
    Key used to encrypt the segments in a m3u8 playlist (EXT-X-KEY)

    `method`
      is a string. ex.: "AES-128"

    `uri`
      is a string. ex:: "https://priv.example.com/key.php?r=52"

    `base_uri`
      uri the key comes from in URI hierarchy. ex.: http://example.com/path/to

    `iv`
      initialization vector. a string representing a hexadecimal number. ex.: 0X12A
*/
public:
    QString method;
    QString uri;
    QString iv;
    QString keyformat;
    QString keyformatversions;
    QString base_uri;
    QMap<QString, QString> _extra_params;

    Key(QString method, QString base_uri, QString uri="", QString iv="", QString keyformat="", QString keyformatversions="", QMap<QString, QString> kwargs=QMap<QString, QString>());
    Key(QString base_uri, QMap<QString, QString> kwargs=QMap<QString, QString>());
    static QString tag();
    QString toString();

    bool operator==(const Key &other) const;
    bool operator!=(const Key &other) const;
};

class InitializationSection : public BasePathMixin {
    /*
        Used to obtain Media Initialization Section required to
            parse the applicable Media Segments (EXT-X-MAP)

    `uri`
        is a string. ex:: "https://priv.example.com/key.php?r=52"

    `byterange`
        value of BYTERANGE attribute

    `base_uri`
        uri the segment comes from in URI hierarchy. ex.: http://example.com/path/to
    */
public:
    QString base_uri = "";
    QString uri = "";
    QString byterange = "";

    static QString tag();
    InitializationSection(QString base_uri, QString uri, QString byterange="");
    InitializationSection(QString base_uri="", QMap<QString, QVariant> kwargs=QMap<QString, QVariant>());
    QString toString();
    bool operator==(InitializationSection &other) const;
    bool operator!=(InitializationSection &other) const;
};

class Segment : public BasePathMixin {
    /*
    A video segment from a M3U8 playlist

    `uri`
      a string with the segment uri

    `title`
      title attribute from EXTINF parameter

    `program_date_time`
      Returns the EXT-X-PROGRAM-DATE-TIME as a datetime. This field is only set
      if EXT-X-PROGRAM-DATE-TIME exists for this segment
      http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.5

    `current_program_date_time`
      Returns a datetime of this segment, either the value of `program_date_time`
      when EXT-X-PROGRAM-DATE-TIME is set or a calculated value based on previous
      segments' EXT-X-PROGRAM-DATE-TIME and EXTINF values

    `discontinuity`
      Returns a boolean indicating if a EXT-X-DISCONTINUITY tag exists
      http://tools.ietf.org/html/draft-pantos-http-live-streaming-13#section-3.4.11

    `cue_out_start`
      Returns a boolean indicating if a EXT-X-CUE-OUT tag exists

    `cue_out`
      Returns a boolean indicating if a EXT-X-CUE-OUT-CONT tag exists
      Note: for backwards compatibility, this will be True when cue_out_start
            is True, even though this tag did not exist in the input, and
            EXT-X-CUE-OUT-CONT will not exist in the output

    `cue_in`
      Returns a boolean indicating if a EXT-X-CUE-IN tag exists

    `scte35`
      Base64 encoded SCTE35 metadata if available

    `scte35_duration`
      Planned SCTE35 duration

    `duration`
      duration attribute from EXTINF parameter

    `base_uri`
      uri the key comes from in URI hierarchy. ex.: http://example.com/path/to

    `bitrate`
      bitrate attribute from EXT-X-BITRATE parameter

    `byterange`
      byterange attribute from EXT-X-BYTERANGE parameter

    `key`
      Key used to encrypt the segment (EXT-X-KEY)

    `parts`
      partial segments that make up this segment

    `dateranges`
      any dateranges that should  precede the segment

    `gap_tag`
      GAP tag indicates that a Media Segment is missing

    `custom_parser_values`
        Additional values which custom_tags_parser might store per segment
    */
public:
    int media_sequence = -1;
    QString uri = "";
    double duration = -1;
    QString title = "";
    QString _base_uri = "";
    QString bitrate = "";
    QString byterange = "";
    QDateTime program_date_time;
    QDateTime current_program_date_time;
    bool discontinuity = false;
    bool cue_out_start = false;
    bool cue_out = false;
    bool cue_in = false;
    QString scte35 = "";
    QString oatcls_scte35 = "";
    QString scte35_duration = "";
    QString scte35_elapsedtime = "";
    QMap<QString, QVariant> asset_metadata;
    Key *key;
    PartialSegmentList parts;
    InitializationSection *init_section;
    DateRangeList dateranges;
    bool gap_tag = false;
    QMap<QString, QVariant> custom_parser_values;

    Segment(QString uri="", QString base_uri="", QDateTime program_date_time=QDateTime(), QDateTime current_program_date_time=QDateTime(),
            double duration=-1, QString title="", QString bitrate="", QString byterange="", bool cue_out=false,
            bool cue_out_start=false, bool cue_in=false, bool discontinuity=false, QString key="", QString scte35="",
            QString oatcls_scte35="", QString scte35_duration="", QString scte35_elapsedtime="", QMap<QString, QVariant> asset_metadata=QMap<QString, QVariant>(),
            Key *keyobject=NULL, QList<QVariant> parts=QList<QVariant>(), QMap<QString, QVariant> init_section=QMap<QString, QVariant>(), QList<QVariant> dateranges=QList<QVariant>(), bool gap_tag=false,
            int media_sequence=-1, QMap<QString, QVariant> custom_parser_values=QMap<QString, QVariant>());
    Segment(QString base_uri="", Key *keyobject=NULL, QMap<QString, QVariant> kwargs=QMap<QString, QVariant>());
    ~Segment();
    void add_part(PartialSegment part);
    QString dumps(Segment *last_segment, QString timespec="milliseconds");
    QString toString();
    void set_base_path(QString newbase_path);
    QString base_uri();
    void set_base_uri(QString newbase_uri);
};

class SegmentList : public GroupedBasePathMixinList<Segment> {
public:
    QString toString();
    QStringList uri();
    QList<Segment> by_key(Key *key);
};

class SessionKey : public Key {
public:
    static QString tag();
    SessionKey(QString base_uri, QMap<QString, QString> kwargs=QMap<QString, QString>()) : Key(base_uri, kwargs) {

    }
};

class StreamInfo {
public:
    int bandwidth = -1;
    QString closed_captions = "";
    int average_bandwidth = -1;
    int program_id = -1;
    QPair<int, int> resolution = QPair<int, int>(-1, -1);
    QString codecs = "";
    QString audio = "";
    QString video = "";
    QString subtitles = "";
    double frame_rate = -1;
    QString video_range = "";
    QString hdcp_level = "";
    QString pathway_id = "";
    QString stable_variant_id = "";

    StreamInfo(QMap<QString, QVariant> kwargs=QMap<QString, QVariant>());
    QString toString();
};

class IFramePlaylist : public BasePathMixin {
    /*
        IFramePlaylist object representing a link to a
            variant M3U8 i-frame playlist with a specific bitrate.

          Attributes:

    `iframe_stream_info` is a named tuple containing the attributes:
     `program_id`, `bandwidth`, `average_bandwidth`, `codecs`, `video_range`,
     `hdcp_level` and `resolution` which is a tuple (w, h) of integers

        More info: http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.13
    */
public:
    StreamInfo iframe_stream_info;

    IFramePlaylist(QString base_uri, QString uri, QMap<QString, QVariant> iframe_stream_info);
    QString toString();
};

class IFramePlaylistList : public TagList<IFramePlaylist> {

};

class Media : public BasePathMixin {
    /*
        A media object from a M3U8 playlist
            https://tools.ietf.org/html/draft-pantos-http-live-streaming-16#section-4.3.4.1

    `uri`
                    a string with the media uri

    `type`
    `group_id`
    `language`
    `assoc-language`
    `name`
    `default`
    `autoselect`
    `forced`
    `instream_id`
    `characteristics`
    `channels`
    `stable_rendition_id`
                    attributes in the EXT-MEDIA tag

    `base_uri`
                      uri the media comes from in URI hierarchy. ex.: http://example.com/path/to
    */
public:
    QString uri="";
    QString type="";
    QString group_id="";
    QString language="";
    QString name="";
    QString arg_default="";
    QString autoselect="";
    QString forced="";
    QString characteristics="";
    QString channels="";
    QString stable_rendition_id="";
    QString assoc_language="";
    QString instream_id="";
    QString base_uri="";
    QMap<QString, QVariant> extras;

    Media(QString uri="", QString type="", QString group_id="", QString language="",
          QString name="", QString arg_default="", QString autoselect="", QString forced="",
          QString characteristics="", QString channels="", QString stable_rendition_id="",
          QString assoc_language="", QString instream_id="", QString base_uri="", QMap<QString, QVariant> extras=QMap<QString, QVariant>());
    Media(QString base_uri="", QMap<QString, QVariant> kwargs=QMap<QString, QVariant>());
    QString dumps();
    QString toString();
};

class MediaList : public TagList<Media> {
public:
    QStringList uri();
};

class Playlist : public BasePathMixin {
    /*
        Playlist object representing a link to a variant M3U8 with a specific bitrate.

        Attributes:

    `stream_info` is a named tuple containing the attributes: `program_id`,
    `bandwidth`, `average_bandwidth`, `resolution`, `codecs` and `resolution`
                                                     which is a a tuple (w, h) of integers

    `media` is a list of related Media entries.

                                                     More info: http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.10
    */
public:
    StreamInfo stream_info;
    QList<Media> media;

    Playlist(QString uri, QMap<QString, QVariant> stream_info, MediaList media, QString base_uri);
    Playlist(QString base_uri, MediaList media, QMap<QString, QVariant> kwargs);
    QString toString();
};

class PlaylistList : public TagList<Playlist> {

};

class SessionData {
public:
    QString data_id;
    QString value;
    QString uri;
    QString language;

    SessionData(QString data_id, QString value="", QString uri="", QString language="");
    QString dumps();
    QString toString();
};

class SessionDataList : public TagList<SessionData> {

};

class Start {
public:
    double time_offset;
    QString precise = "";
    Start(double time_offset, QString precise="");
    QString toString();
};

class RenditionReport : public BasePathMixin {
public:
    QString base_uri;
    QString uri;
    double last_msn;
    double last_part;

    RenditionReport(QString base_uri, QString uri, double last_msn, double last_part=-1);
    QString dumps();
    QString toString();
};

class RenditionReportList : public GroupedBasePathMixinList<RenditionReport> {
public:
    QString toString();
};

class ServerControl {
public:
    double can_skip_until = -1;
    QString can_block_reload = "";
    double hold_back = -1;
    double part_hold_back = -1;
    QString can_skip_dateranges = "";

    ServerControl(double can_skip_until=-1, QString can_block_reload="", double hold_back=-1, double part_hold_back=-1, QString can_skip_dateranges="");
    QString dumps();
    QString toString();
};

class Skip {
public:
    double skipped_segments;
    QString recently_removed_dateranges;

    Skip(double skipped_segments, QString recently_removed_dateranges="");
    QString dumps();
    QString toString();
};

class PartInformation {
public:
    double part_target;

    PartInformation(double part_target=-1);
    QString dumps();
    QString toString();
};

class PreloadHint : public BasePathMixin {
public:
    QString hint_type;
    QString base_uri;
    QString uri;
    double byterange_start;
    double byterange_length;

    PreloadHint(QString type, QString base_uri, QString uri, double byterange_start=-1, double byterange_length=-1);
    QString dumps();
    QString toString();
};

class ContentSteering : public BasePathMixin {
public:
    QString base_uri;
    QString uri;
    QString pathway_id;

    ContentSteering(QString base_uri, QString server_uri, QString pathway_id="");
    QString dumps();
    QString toString();
};


class M3U8 {
    /*
    Represents a single M3U8 playlist. Should be instantiated with
    the content as string.

    Parameters:

     `content`
       the m3u8 content as string

     `base_path`
       all urls (key and segments url) will be updated with this base_path,
       ex.:
           base_path = "http://videoserver.com/hls"

            /foo/bar/key.bin           -->  http://videoserver.com/hls/key.bin
            http://vid.com/segment1.ts -->  http://videoserver.com/hls/segment1.ts

       can be passed as parameter or setted as an attribute to ``M3U8`` object.
     `base_uri`
      uri the playlist comes from. it is propagated to SegmentList and Key
      ex.: http://example.com/path/to

    Attributes:

     `keys`
       Returns the list of `Key` objects used to encrypt the segments from m3u8.
       It covers the whole list of possible situations when encryption either is
       used or not.

       1. No encryption.
       `keys` list will only contain a `None` element.

       2. Encryption enabled for all segments.
       `keys` list will contain the key used for the segments.

       3. No encryption for first element(s), encryption is applied afterwards
       `keys` list will contain `None` and the key used for the rest of segments.

       4. Multiple keys used during the m3u8 manifest.
       `keys` list will contain the key used for each set of segments.

     `session_keys`
       Returns the list of `SessionKey` objects used to encrypt multiple segments from m3u8.

     `segments`
       a `SegmentList` object, represents the list of `Segment`s from this playlist

     `is_variant`
        Returns true if this M3U8 is a variant playlist, with links to
        other M3U8s with different bitrates.

        If true, `playlists` is a list of the playlists available,
        and `iframe_playlists` is a list of the i-frame playlists available.

     `is_endlist`
        Returns true if EXT-X-ENDLIST tag present in M3U8.
        http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.8

      `playlists`
        If this is a variant playlist (`is_variant` is True), returns a list of
        Playlist objects

      `iframe_playlists`
        If this is a variant playlist (`is_variant` is True), returns a list of
        IFramePlaylist objects

      `playlist_type`
        A lower-case string representing the type of the playlist, which can be
        one of VOD (video on demand) or EVENT.

      `media`
        If this is a variant playlist (`is_variant` is True), returns a list of
        Media objects

      `target_duration`
        Returns the EXT-X-TARGETDURATION as an integer
        http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.2

      `media_sequence`
        Returns the EXT-X-MEDIA-SEQUENCE as an integer
        http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.3

      `program_date_time`
        Returns the EXT-X-PROGRAM-DATE-TIME as a string
        http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.5

      `version`
        Return the EXT-X-VERSION as is

      `allow_cache`
        Return the EXT-X-ALLOW-CACHE as is

      `files`
        Returns an iterable with all files from playlist, in order. This includes
        segments and key uri, if present.

      `base_uri`
        It is a property (getter and setter) used by
        SegmentList and Key to have absolute URIs.

      `is_i_frames_only`
        Returns true if EXT-X-I-FRAMES-ONLY tag present in M3U8.
        http://tools.ietf.org/html/draft-pantos-http-live-streaming-07#section-3.3.12

      `is_independent_segments`
        Returns true if EXT-X-INDEPENDENT-SEGMENTS tag present in M3U8.
        https://tools.ietf.org/html/draft-pantos-http-live-streaming-13#section-3.4.16
*/

public:
    static const QList<QPair<QString, QString> > simple_attributes;
    QMap<QString, QVariant> data;
    QString _base_uri;
    QString base_path;
    QList<Key *> keys;
    QList<InitializationSection> segment_map;
    SegmentList segments;
    QStringList files;
    MediaList media;
    PlaylistList playlists;
    IFramePlaylistList iframe_playlists;
    Start *start = NULL;
    ServerControl *server_control = NULL;
    PartInformation *part_inf = NULL;
    Skip *skip = NULL;
    RenditionReportList rendition_reports;
    SessionDataList session_data;
    QList<SessionKey *> session_keys;
    PreloadHint *preload_hint = NULL;
    ContentSteering *content_steering = NULL;

    bool is_variant = false;
    bool is_endlist = false;
    bool is_i_frames_only = false;
    int target_duration = -1;
    int media_sequence = -1;
    QDateTime program_date_time;
    bool is_independent_segments = false;
    int version = -1;
    QString allow_cache = "";
    QString playlist_type = "";
    int discontinuity_sequence = -1;

    M3U8(QString content="", QString base_path="", QString base_uri="", bool strict=false, QString custom_tags_parser="");
    ~M3U8();
    void _initialize_attributes();
    QString base_uri();
    QString toString();
};

#endif // MODEL_H
