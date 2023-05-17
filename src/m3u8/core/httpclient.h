#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "parser.h"
#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class HttpClient {
public:
    static QString _parsed_url(QString url);
};

class DefaultHTTPClient : public QObject {
    Q_OBJECT
private:
    QNetworkAccessManager *manager;

public:
    DefaultHTTPClient();
    ~DefaultHTTPClient();
    void download(QString uri, int timeout=0, QMap<QString, QVariant> headers=QMap<QString, QVariant>(), bool verify_ssl=true);

signals:
    void onDownload(DefaultHTTPClient *http_client, QString base_uri, QString content);

public slots:
    void readyReadCallback();
    void downloadCallback(QNetworkReply *reply);
    void errorOccurredCallback(QNetworkReply::NetworkError error);
    void sslErrorsCallback(const QList<QSslError> &errors);
};

#endif // HTTPCLIENT_H
