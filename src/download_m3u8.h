#ifndef DOWNLOADM3U8_H
#define DOWNLOADM3U8_H

#include "m3u8/m3u8_task.h"
#include <QObject>
#include <QVariant>
#include <QMap>

class DownloadM3u8 : public QObject {
    Q_OBJECT

public:
    DownloadM3u8();
    ~DownloadM3u8();

    Q_INVOKABLE void download(QVariant task_id, QVariant download_link, QVariant file_path, QVariant file_name);
    Q_INVOKABLE void initAria2();
    Q_INVOKABLE void deleteTempFolder(QVariant path);
};

#endif // DOWNLOADM3U8_H
