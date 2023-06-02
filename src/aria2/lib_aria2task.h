#ifndef LIB_ARIA2TAKSK_H
#define LIB_ARIA2TASK_H

#ifdef Q_OS_MAC
#include <aria2.h>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QVariant>

class QMutex;

class LibAria2Task : public QThread {
    Q_OBJECT

private:
    QString task_id;
    QString file_path;
    QString file_name;
    QStringList ts_list;
    aria2::Session* session;
    // 状态数据
    bool stoped = false;
    bool paused = false;
    // 线程锁
    QMutex *run_mutex;

public:
    Aria2Task(QString task_id, QString file_path, QString file_name, QStringList ts_list);
    ~Aria2Task();
    void initAria2Session();
    void run();
    void unpause();
    void pause();
    void stop();
    bool isStoped();
    aria2::Session *getSession();
    bool hasGid(const aria2::A2Gid gid);
    int downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event, const aria2::A2Gid gid, void* userData);
    QString getTempSaveFolder();

signals:
    void onDownloadState(QString task_id, QVariant data);
    void onDownloadPause(QString task_id);
    void onDownloadUnpause(QString task_id);
    void onDownloadStop(QString task_id);

public slots:
    void onFinishedCallback();
};

#endif
#endif // ARIA2TASK_H
