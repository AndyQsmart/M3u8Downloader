#ifndef M3U8_H
#define M3U8_H

 #include "model.h"
#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>

class DefaultHTTPClient;

class M3u8 : public QObject {
    Q_OBJECT

private:
    QString custom_tags_parser;

public:
    void loadContent(QString content, QString uri="", QString custom_tags_parser="");
    void loadUri(QString uri, int timeout=0, QMap<QString, QVariant> headers=QMap<QString, QVariant>(), QString custom_tags_parser="", bool verify_ssl=true);

signals:
    void onLoad(M3u8 *m3u8, M3U8 *m3u8_obj);

public slots:
    void httpDownloadCallback(DefaultHTTPClient * http_client, QString base_uri, QString content);
};

#endif // M3U8_H
