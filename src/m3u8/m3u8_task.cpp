#include "m3u8_task.h"
#include <QDebug>

M3u8Task::M3u8Task(QString task_id) {
    this->task_id = task_id;
}

M3u8Task::~M3u8Task() {
    delete this->m3u8;
}

QString M3u8Task::getTaskId() {
    return this->task_id;
}

QString M3u8Task::getFilePath() {
    return this->file_path;
}

QString M3u8Task::getFileName() {
    return this->file_name;
}

M3U8 *M3u8Task::getM3u8() {
    return this->m3u8;
}

void M3u8Task::download(QString download_link, QString file_path, QString file_name) {
    qDebug() << "M3u8Task::download:download_link:" << download_link;
    this->download_link = download_link;
    this->file_path = file_path;
    this->file_name = file_name;
    M3u8 *m3u8 = new M3u8();
    m3u8->loadUri(download_link);
    connect(m3u8, &M3u8::onLoad, this, &M3u8Task::onLoadM3u8Callback);
}

void M3u8Task::onLoadM3u8Callback(M3u8 *m3u8, M3U8 *m3u8_obj) {
    QStringList m3u8_obj_str = m3u8_obj->toString().split(";");
    qDebug() << "M3u8Task::onLoadM3u8Callback:m3u8_obj:";
    for (int i = 0; i < m3u8_obj_str.size(); i++) {
        qDebug() << m3u8_obj_str[i];
    }
    this->m3u8 = m3u8_obj;
    emit onDownloadFinished(this);
    m3u8->deleteLater();
}
