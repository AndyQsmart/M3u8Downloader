#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QRegExp>
#include <QDateTime>
#include <QException>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QPair>

class ParseError : public QException {

};

class Parser {
public:
    static const QStringList URI_PREFIXES;
    static const QRegExp ATTRIBUTELISTPATTERN;
    static QDateTime cast_data_time(QString value);
    static QString format_date_time(QDateTime value, QString timespec);
    static QMap<QString, QVariant> parse(QString content, bool strict=false, QString custom_tags_parser=""); // custom_tags_parser 不实现
    static QString strip(QString str);
    static QPair<QString, QString> split(QString str, QString flag);
    static QStringList string_to_lines(QString str);
    static void _parse_byterange(QString line, QMap<QString, QVariant> &state);
    static void _parse_bitrate(QString line, QMap<QString, QVariant> &state);
    static QVariant _parse_simple_parameter(QString line, QMap<QString, QVariant> &data, QString cast_to="str");
    static QVariant _parse_and_set_simple_parameter_raw_value(QString line, QMap<QString, QVariant> &data, QString cast_to="str", bool normalize=false);
    static QPair<QString, QVariant> _parse_simple_parameter_raw_value(QString line, QString cast_to="str", bool normalize=false);
    static void _parse_cueout_cont(QString line, QMap<QString, QVariant> &state);
    static void _parse_cueout(QString line, QMap<QString, QVariant> &state);
    static QPair<bool, QPair<QString, QString> > _cueout_no_duration(QString line);
    static QPair<bool, QPair<QString, QString> > _cueout_envivio(QString line, QMap<QString, QVariant> &state);
    static QPair<bool, QPair<QString, QString> > _cueout_duration(QString line);
    static QPair<bool, QPair<QString, QString> > _cueout_simple(QString line);
    static void _parse_oatcls_scte35(QString line, QMap<QString, QVariant> &state);
    static void _parse_asset(QString line, QMap<QString, QVariant> &state);
    static QMap<QString, QVariant> _parse_key(QString line);
    static void _parse_extinf(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state, int lineno, bool strict);
    static void _parse_stream_inf(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_i_frame_stream_inf(QString line, QMap<QString, QVariant> &data);
    static void _parse_media(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_server_control(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_part_inf(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_rendition_report(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_part(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_skip(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_session_data(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_session_key(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_preload_hint(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_daterange(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_content_steering(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_ts_chunk(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static void _parse_variant_playlist(QString line, QMap<QString, QVariant> &data, QMap<QString, QVariant> &state);
    static QString remove_quotes(QString str);
    static QMap<QString, QVariant> _parse_attribute_list(QString prefix, QString line, QMap<QString, QString> atribute_parser, QString default_parser="");
    static QString normalize_attribute(QString attribute);
    static QVariant cast_to(QString value, QString cast_to_type);
    static bool is_url(QString uri);
    static QString urljoin(QString base, QString url);
};

#endif // PARSER_H
