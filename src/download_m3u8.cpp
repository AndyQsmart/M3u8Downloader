#include "download_m3u8.h"
#include "src/utils/qml_signal.h"
#include "aria2/aria2util.h"
#include <QStringList>
#include <QDebug>

DownloadM3u8::DownloadM3u8() {

}

DownloadM3u8::~DownloadM3u8() {
    Aria2Util::uninit();
}

void DownloadM3u8::download(QVariant task_id, QVariant download_link, QVariant file_path, QVariant file_name) {
    QString task_id_str = task_id.toString();
    M3u8Task *m3u8_task = new M3u8Task(task_id_str);
    m3u8_task->download(download_link.toString(), file_path.toString(), file_name.toString());
    connect(m3u8_task, &M3u8Task::onDownloadFinished, this, &DownloadM3u8::onDownloadM3u8Finished);
    this->task_map[task_id_str] = m3u8_task;
    qDebug() << "DownloadM3u8::download:" << "task_id:" << task_id_str << "download_link:" << download_link << "file_path:" << file_path << "file_name:" << file_name;
}

void DownloadM3u8::onDownloadM3u8Finished(M3u8Task *m3u8_task) {
    QString task_id = m3u8_task->getTaskId();
    auto m3u8 = m3u8_task->getM3u8();
    QMap<QString, QVariant> ans;
    ans["task_id"] = task_id;
    ans["file_path"] = m3u8_task->getFilePath();
    ans["file_name"] = m3u8_task->getFileName();

    QMap<QString, QVariant> m3u8_arg;
    QStringList files = m3u8->files;
    QList<QVariant> files_arg;
    for (int i = 0 ; i < files.size(); i++) {
        files_arg.append(files[i]);
    }
    m3u8_arg["files"] = files_arg;
    m3u8_arg["base_uri"] = m3u8->base_uri();
    m3u8_arg["base_path"] = m3u8->base_path;

    ans["m3u8"] = m3u8_arg;

    QMLSignal::instance()->emitSignal(QMLSignalCMD::M3U8_DOWNLOAD_FINISHED, ans);
}

void DownloadM3u8::onAria2StateCallback(QString task_id, QVariant data) {
    QMap<QString, QVariant> ans;
    ans["task_id"] = task_id;
    ans["data"] = data;
    QMLSignal::instance()->emitSignal(QMLSignalCMD::ARIA2_DOWNLOAD_STATE, ans);
}

void DownloadM3u8::downloadTs(QVariant task_id, QVariant file_path, QVariant file_name, QVariant ts_list) {
    QString task_id_str = task_id.toString();
    QList<QVariant> ts_list_obj = ts_list.toList();
    QStringList ts_list_v;
    for (int i = 0; i < ts_list_obj.size(); i++) {
        ts_list_v.append(ts_list_obj[i].toString());
    }
    QString file_path_str = file_path.toString();
    QString file_name_str = file_name.toString();

    Aria2Util::init();
    auto aria2_task = Aria2Util::createTask(task_id_str, file_path_str, file_name_str, ts_list_v);
    connect(aria2_task, &Aria2Task::onDownloadState, this, &DownloadM3u8::onAria2StateCallback);
    connect(aria2_task, &Aria2Task::onDownloadPause, this, &DownloadM3u8::onAria2PauseCallback);
    connect(aria2_task, &Aria2Task::onDownloadUnpause, this, &DownloadM3u8::onAria2UnpauseCallback);
    aria2_task->start();
    aria2_task_map[task_id_str] = aria2_task;
}

void DownloadM3u8::pauseAria2Task(QVariant task_id) {
    QString task_id_str = task_id.toString();
    if (aria2_task_map.find(task_id_str) != aria2_task_map.end()) {
        auto aria2_task = aria2_task_map[task_id_str];
        aria2_task->pause();
    }
}

void DownloadM3u8::onAria2PauseCallback(QString task_id) {
    qDebug() << "DownloadM3u8::onAria2PauseCallback" << "task_id:" << task_id;
    QMLSignal::instance()->emitSignal(QMLSignalCMD::ARIA2_DOWNLOAD_PAUSE, task_id);
}

void DownloadM3u8::unpauseAria2Task(QVariant task_id) {
    qDebug() << "DownloadM3u8::unpauseAria2Task" << "task_id:" << task_id;
    QString task_id_str = task_id.toString();
    if (aria2_task_map.find(task_id_str) != aria2_task_map.end()) {
        auto aria2_task = aria2_task_map[task_id_str];
        aria2_task->unpause();
    }
}

void DownloadM3u8::onAria2UnpauseCallback(QString task_id) {
    qDebug() << "DownloadM3u8::onAria2UnpauseCallback" << "task_id:" << task_id;
    QMLSignal::instance()->emitSignal(QMLSignalCMD::ARIA2_DOWNLOAD_UNPAUSE, task_id);
}
