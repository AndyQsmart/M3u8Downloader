#ifndef QMLSIGNAL_H
#define QMLSIGNAL_H

#include <QObject>
#include <QVariant>

enum QMLSignalCMD {
    M3U8_DOWNLOAD_FINISHED = 1,
    APP_EVENT = 2,
    ARIA2_INIT = 3,
};

class QMLSignal : public QObject {
    Q_OBJECT

private:
    static QMLSignal *_instance;

public:
    static QMLSignal *instance();
    void emitSignal(QMLSignalCMD cmd, QVariant data);

signals:
    void qmlSignal(int cmd, QVariant data);
};

#endif // QMLSIGNAL_H
