#ifndef QHTTP_H
#define QHTTP_H

#include <functional>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariant>

class QHttp : public QObject {
    Q_OBJECT

public:
    static void postJSON(QString url, QVariant arg, std::function<void()> callback);
};

#endif // QHTTP_H
