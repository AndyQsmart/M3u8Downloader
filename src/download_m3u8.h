#ifndef DOWNLOADM3U8_H
#define DOWNLOADM3U8_H

#include "m3u8/m3u8_task.h"
#include <QObject>
#include <QVariant>
#include <QMap>

class DownloadM3u8 : public QObject {
    Q_OBJECT

private:
    QMap<QString, M3u8Task *> task_map;

public:
    DownloadM3u8();
    ~DownloadM3u8();

    Q_INVOKABLE void download(QVariant task_id, QVariant download_link, QVariant file_path, QVariant file_name);
    Q_INVOKABLE void downloadTs(QVariant task_id, QVariant file_path, QVariant file_name, QVariant ts_list);
    Q_INVOKABLE void pauseAria2Task(QVariant task_id);
    Q_INVOKABLE void unpauseAria2Task(QVariant task_id);
    Q_INVOKABLE void deleteTask(QVariant task_id, QVariant deleteFile);

public slots:
    void onDownloadM3u8Finished(M3u8Task *m3u8_task);
    void onAria2StateCallback(QString task_id, QVariant data);
    void onAria2PauseCallback(QString task_id);
    void onAria2UnpauseCallback(QString task_id);
    void deleteTaskAndTempFolderCallback(QString task_id);
};

#endif // DOWNLOADM3U8_H
