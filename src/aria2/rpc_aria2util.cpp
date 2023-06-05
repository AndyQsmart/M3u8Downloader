#include "qglobal.h"

#ifndef Q_OS_MAC

#include "rpc_aria2util.h"
#include <QProcess>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QDebug>

bool Aria2Util::is_init = false;
Aria2Util *Aria2Util::_instance = NULL;
QList<Aria2Task *> Aria2Util::task_list = QList<Aria2Task *>();

void Aria2Util::init() {
    if (Aria2Util::is_init) {
        return;
    }
    Aria2Util::_instance = new Aria2Util();
    Aria2Util::is_init = true;
    qDebug() << "Aria2Util::init";
}

void Aria2Util::uninit() {
    if (Aria2Util::is_init) {
        delete Aria2Util::_instance;
        Aria2Util::is_init = false;
        qDebug() << "Aria2Util::uninit";
    }
}

Aria2Util::Aria2Util() {
    // Q_INIT_RESOURCE(aria2c);
    this->process = new QProcess();
    QFile aria2c_file(":/aria2c/aria2c.exe");
    QFile aria2c_exe_file("aria2c.exe");
    // qDebug() << "exe path" << QDir::current().absoluteFilePath("ariac2.exe");
    if (!aria2c_exe_file.exists()) {
        aria2c_file.copy("aria2c.exe");
    }
    this->process->setProgram("aria2c.exe");
    this->process->startDetached();
    qDebug() << "exe error" << this->process->errorString();
    //qDebug() << "exe out" << QString(this->process->readAllStandardOutput());

    // this->process->start("");
}

Aria2Util::~Aria2Util() {
    delete this->process;
}

Aria2Task *Aria2Util::createTask(QString task_id, QString file_path, QString file_name, QStringList ts_list) {
    Aria2Task *aria2_task = new Aria2Task(task_id, file_path, file_name, ts_list);
    Aria2Util::task_list.append(aria2_task);
    return aria2_task;
}

#endif
