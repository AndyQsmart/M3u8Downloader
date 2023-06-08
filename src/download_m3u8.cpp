#include "download_m3u8.h"
#include "src/utils/qml_signal.h"
#include "aria2/aria2util.h"
#include <QStringList>
#include <QDir>
#include <QDebug>

DownloadM3u8::DownloadM3u8() {

}

DownloadM3u8::~DownloadM3u8() {
    Aria2Util::uninit();
}

void DownloadM3u8::download(QVariant task_id, QVariant download_link, QVariant file_path, QVariant file_name) {
    QString task_id_str = task_id.toString();
    M3u8Task *m3u8_task = new M3u8Task(task_id_str);
    connect(m3u8_task, &M3u8Task::onDownloadFinished, [](M3u8Task *m3u8_task) {
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
        m3u8_task->deleteLater();
    });
    m3u8_task->download(download_link.toString(), file_path.toString(), file_name.toString());
    qDebug() << "DownloadM3u8::download:" << "task_id:" << task_id_str << "download_link:" << download_link << "file_path:" << file_path << "file_name:" << file_name;
}

void DownloadM3u8::initAria2() {
    Aria2Util::init();
}

//void DownloadM3u8::deleteTask(QVariant task_id, QVariant deleteFile) {
////    QString task_id_str = task_id.toString();
////    bool delete_file = deleteFile.toBool();
////    qDebug() << "DownloadM3u8::deleteTask" << "task_id:" << task_id_str;

////    // 删除m3u8任务记录
////    auto m3u8_task = this->task_map[task_id_str];
////    m3u8_task->deleteLater();

////    // 终止并删除aria2任务
////    auto aria2_task = this->aria2_task_map[task_id_str];
////    if (delete_file) {
////        // 这个回调中删除临时下载路径
////        connect(aria2_task, &Aria2Task::onDownloadStop, this, &DownloadM3u8::deleteTaskAndTempFolderCallback);
////    }
////    else {
////        connect(aria2_task, &Aria2Task::finished, aria2_task, &Aria2Task::deleteLater);
////    }
////    aria2_task->stop();

////    QMap<QString, QVariant> ans;
////    ans["task_id"] = task_id_str;
////    ans["delete_file"] = delete_file;
////    QMLSignal::instance()->emitSignal(QMLSignalCMD::MAIN_DELETE_TASK, ans);
//}

void DownloadM3u8::deleteTempFolder(QVariant path) {
    // 删除临时下载文件夹
    QDir temp_save_folder(path.toString());
    temp_save_folder.removeRecursively();
}
