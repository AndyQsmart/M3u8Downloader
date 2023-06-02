#ifndef Q_OS_MAC

#include "rpc_aria2task.h"
#include <QMutex>
#include <QMutexLocker>
#include <QTime>
#include <QDebug>

void Aria2Task::run() {

}

void Aria2Task::pause() {
    QMutexLocker locker(this->run_mutex);
    emit onDownloadPause(this->task_id);
    this->paused = true;
}

void Aria2Task::unpause() {
    qDebug() << "Aria2Task::unpause";
    this->start();
    emit onDownloadUnpause(this->task_id);
}

void Aria2Task::stop() {
    QMutexLocker locker(this->run_mutex);
    this->stoped = true;
}

bool Aria2Task::isStoped() {
    QMutexLocker locker(this->run_mutex);
    return this->stoped;
}

Aria2Task::Aria2Task(QString task_id, QString file_path, QString file_name, QStringList ts_list) {
    qDebug() << "Aria2Task::Aria2Task:" << "task_id:" << task_id << "file_path:" << file_path << "file_name:" << file_name;
    this->task_id = task_id;
    this->file_path = file_path;
    this->file_name = file_name;
    this->ts_list = ts_list;
    this->run_mutex = new QMutex();
    this->initAria2Session();
    connect(this, &Aria2Task::finished, this, &Aria2Task::onFinishedCallback);
}

Aria2Task::~Aria2Task() {
    delete this->run_mutex;
}

void Aria2Task::onFinishedCallback() {
    emit onDownloadStop(this->task_id);
}

QString Aria2Task::getTempSaveFolder() {
    QString save_folder = this->file_path[this->file_path.length()-1] == "/" ? "%1%2_ts" : "%1/%2_ts";
    save_folder = save_folder.arg(this->file_path, this->file_name);
    return save_folder;
}

void Aria2Task::initAria2Session() {
}

//aria2::Session *Aria2Task::getSession() {
//    return NULL;
//}

//bool Aria2Task::hasGid(const aria2::A2Gid gid) {
//    return false;
//}

#endif
