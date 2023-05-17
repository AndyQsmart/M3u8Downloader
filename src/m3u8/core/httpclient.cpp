#include "httpclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QByteArray>
#include <QIODevice>
#include <QDebug>

QString HttpClient::_parsed_url(QString url) {
    return Parser::urljoin(url, ".");
}

DefaultHTTPClient::DefaultHTTPClient() {
    this->manager = new QNetworkAccessManager(this);
    connect(this->manager, &QNetworkAccessManager::finished, this, &DefaultHTTPClient::downloadCallback);
}

DefaultHTTPClient::~DefaultHTTPClient() {
    delete this->manager;
}

void DefaultHTTPClient::download(QString uri, int timeout, QMap<QString, QVariant> headers, bool verify_ssl) {
    QNetworkRequest request;
    request.setUrl(QUrl(uri));

    QNetworkReply * reply = this->manager->get(request);
    connect(reply, &QIODevice::readyRead, this, &DefaultHTTPClient::readyReadCallback);
    connect(reply, &QNetworkReply::errorOccurred, this, &DefaultHTTPClient::errorOccurredCallback);
    connect(reply, &QNetworkReply::sslErrors, this, &DefaultHTTPClient::sslErrorsCallback);
}

void DefaultHTTPClient::readyReadCallback() {
    qDebug() << "DefaultHTTPClient::readyReadCallback";
}

void DefaultHTTPClient::downloadCallback(QNetworkReply *reply) {
    QByteArray replyData = reply->readAll();
    QString ans = QString::fromUtf8(replyData);
    // qDebug() << "DefaultHTTPClient::downloadCallback:ans:" << ans;
    // qDebug() << "DefaultHTTPClient::downloadCallback:reply.url:" << reply->url().toString();
    QString base_uri = HttpClient::_parsed_url(reply->url().toString());
    emit onDownload(this, base_uri, ans);
    reply->deleteLater();
}

void DefaultHTTPClient::errorOccurredCallback(QNetworkReply::NetworkError error) {
    qDebug() << "DefaultHTTPClient::errorOccurredCallback:error:" << error;
}

void DefaultHTTPClient::sslErrorsCallback(const QList<QSslError> &errors) {
    qDebug() << "DefaultHTTPClient::sslErrorsCallback:errors:";
    for (int i = 0; i < errors.size(); i++) {
        qDebug() << errors[i].errorString();
    }
}
