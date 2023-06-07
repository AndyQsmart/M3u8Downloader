#ifndef QMLSIGNAL_H
#define QMLSIGNAL_H

#include <QObject>
#include <QVariant>

enum QMLSignalCMD {
    M3U8_DOWNLOAD_FINISHED = 1,
    ARIA2_DOWNLOAD_STATE = 2,
    ARIA2_DOWNLOAD_PAUSE = 3,
    ARIA2_DOWNLOAD_UNPAUSE = 4,
    MAIN_DELETE_TASK = 5,
    APP_EVENT = 6,
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
