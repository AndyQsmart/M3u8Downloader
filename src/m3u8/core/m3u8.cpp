#include "m3u8.h"
#include "httpclient.h"
#include "parser.h"
#include <QDebug>

// M3U8(QString content="", QString base_path="", QString base_uri="", bool strict=false, QString custom_tags_parser="");

void M3u8::loadContent(QString content, QString uri, QString custom_tags_parser) {
    /*
        Given a string with a m3u8 content, returns a M3U8 object.
        Optionally parses a uri to set a correct base_uri on the M3U8 object.
        Raises ValueError if invalid content
    */
    if (uri == "") {
        // return M3U8(content, "", "", false, custom_tags_parser);
    }
    else {
        QString base_uri = HttpClient::_parsed_url(uri);
        // return M3U8(content, "", base_uri, false, custom_tags_parser);
    }
}

void M3u8::loadUri(QString uri, int timeout, QMap<QString, QVariant> headers, QString custom_tags_parser, bool verify_ssl) {
    /*
        Retrieves the content from a given URI and returns a M3U8 object.
        Raises ValueError if invalid content or IOError if request fails.
    */
    qDebug() << "M3u8::load:" << "uri:" << uri;
    this->custom_tags_parser = custom_tags_parser;
    if (Parser::is_url(uri)) {
        DefaultHTTPClient *http_client = new DefaultHTTPClient();
        http_client->download(uri);
        connect(http_client, &DefaultHTTPClient::onDownload, this, &M3u8::httpDownloadCallback);
    }
    else {
    }
}

void M3u8::httpDownloadCallback(DefaultHTTPClient *http_client, QString base_uri, QString content) {
    qDebug() << "M3u8::httpDownloadCallback:" << "base_uri:" << base_uri;
    M3U8 *m3u8_obj = new M3U8(content, "", base_uri, false, this->custom_tags_parser);
    qDebug() << "M3u8::httpDownloadCallback:M3u8_obj create success";
    emit onLoad(this, m3u8_obj);
    http_client->deleteLater();
}
