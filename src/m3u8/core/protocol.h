#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QString>

class Protocol {
public:
    static const QString ext_m3u;
    static const QString ext_x_targetduration;
    static const QString ext_x_media_sequence;
    static const QString ext_x_discontinuity_sequence;
    static const QString ext_x_program_date_time;
    static const QString ext_x_media;
    static const QString ext_x_playlist_type;
    static const QString ext_x_key;
    static const QString ext_x_stream_inf;
    static const QString ext_x_version;
    static const QString ext_x_allow_cache;
    static const QString ext_x_endlist;
    static const QString extinf;
    static const QString ext_i_frames_only;
    static const QString ext_x_asset;
    static const QString ext_x_bitrate;
    static const QString ext_x_byterange;
    static const QString ext_x_i_frame_stream_inf;
    static const QString ext_x_discontinuity;
    static const QString ext_x_cue_out;
    static const QString ext_x_cue_out_cont;
    static const QString ext_x_cue_in;
    static const QString ext_x_cue_span;
    static const QString ext_oatcls_scte35;
    static const QString ext_is_independent_segments;
    static const QString ext_x_map;
    static const QString ext_x_start;
    static const QString ext_x_server_control;
    static const QString ext_x_part_inf;
    static const QString ext_x_part;
    static const QString ext_x_rendition_report;
    static const QString ext_x_skip;
    static const QString ext_x_session_data;
    static const QString ext_x_session_key;
    static const QString ext_x_preload_hint;
    static const QString ext_x_daterange;
    static const QString ext_x_gap;
    static const QString ext_x_content_steering;
};

#endif // PROTOCOL_H
