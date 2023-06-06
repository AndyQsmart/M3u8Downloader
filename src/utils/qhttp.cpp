#include "qhttp.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>

void QHttp::postJSON(QString url, QVariant arg, std::function<void()> callback) {
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        QByteArray replyData = reply->readAll();
        QString ans = QString::fromUtf8(replyData);
        qDebug() << "QHttp::postJSON::downloadCallback:ans:" << ans;
        reply->deleteLater();
        manager->deleteLater();
    });

    QNetworkRequest request(QUrl("http://localhost:6800/jsonrpc"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonDocument post_arg = QJsonDocument::fromVariant(arg);
    qDebug() << "QHttp::postJSON:aria2 shutdown arg" << post_arg.toJson(QJsonDocument::Compact);
    QNetworkReply *reply = manager->post(request, post_arg.toJson(QJsonDocument::Compact));
    connect(reply, &QIODevice::readyRead, [=]() {
        qDebug() << "QHttp::postJSON::readyReadCallback";
    });
    connect(reply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError error) {
        qDebug() << "QHttp::postJSON::errorOccurredCallback:error:" << error;
    });
    connect(reply, &QNetworkReply::sslErrors, [=](const QList<QSslError> &errors) {
        qDebug() << "QHttp::postJSON::sslErrorsCallback:errors:";
        for (int i = 0; i < errors.size(); i++) {
            qDebug() << errors[i].errorString();
        }
    });
}
