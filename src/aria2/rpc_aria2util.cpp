#include "qglobal.h"

#ifndef Q_OS_MAC

#include "rpc_aria2util.h"
#include <QDebug>

bool Aria2Util::is_init = false;
QList<Aria2Task *> Aria2Util::task_list = QList<Aria2Task *>();

void Aria2Util::init() {
    if (Aria2Util::is_init) {
        return;
    }
    Aria2Util::is_init = true;
    // qDebug() << "Aria2Util::init:res:" << res;
}

void Aria2Util::uninit() {
    if (Aria2Util::is_init) {
        Aria2Util::is_init = false;
        qDebug() << "Aria2Util::uninit";
    }
}

Aria2Task *Aria2Util::createTask(QString task_id, QString file_path, QString file_name, QStringList ts_list) {
    Aria2Task *aria2_task = new Aria2Task(task_id, file_path, file_name, ts_list);
    Aria2Util::task_list.append(aria2_task);
    return aria2_task;
}

#endif
