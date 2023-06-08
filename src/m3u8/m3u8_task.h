#ifndef M3U8TASK_H
#define M3U8TASK_H

#include "core/m3u8.h"
#include "core/model.h"
#include <QObject>
#include <QString>

class M3u8Task : public QObject {
    Q_OBJECT

private:
    QString download_link;
    QString file_path;
    QString file_name;
    M3U8 *m3u8=NULL;

public:
    M3u8Task();
    ~M3u8Task();
    void download(QString download_link, QString file_path, QString file_name);
    QString getDownloadLink();
    QString getFilePath();
    QString getFileName();
    M3U8 *getM3u8();

signals:
    void onDownloadFinished(M3u8Task *m3u8_task);

public slots:
    void onLoadM3u8Callback(M3u8 *m3u8, M3U8 *m3u8_obj);
};

#endif // M3U8TASK_H
