#include "aria2util.h"
#include <QDebug>

bool Aria2Util::is_init = false;
QList<Aria2Task *> Aria2Util::task_list = QList<Aria2Task *>();

void Aria2Util::init() {
    if (Aria2Util::is_init) {
        return;
    }

    aria2::libraryInit();
    Aria2Util::is_init = true;
    qDebug() << "Aria2Util::init";
}

void Aria2Util::uninit() {
    if (Aria2Util::is_init) {
        aria2::libraryDeinit();
        Aria2Util::is_init = false;
        qDebug() << "Aria2Util::uninit";
    }
}

int Aria2Util::downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event, const aria2::A2Gid gid, void* userData) {
    for (auto& aria2_task : Aria2Util::task_list) {
        if (session == aria2_task->getSession()) {
            return aria2_task->downloadEventCallback(session, event, gid, userData);
        }
    }

    return 0;
}

Aria2Task *Aria2Util::createTask(QString task_id, QString file_path, QString file_name, QStringList ts_list) {
    Aria2Task *aria2_task = new Aria2Task(task_id, file_path, file_name, ts_list);
    Aria2Util::task_list.append(aria2_task);
    return aria2_task;
}
