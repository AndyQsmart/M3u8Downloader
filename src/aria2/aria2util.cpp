#include "aria2util.h"
#include "src/utils/qml_signal.h"
#include <QProcess>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QDebug>

#ifdef Q_OS_MAC
QString ariac_exe_qrc_path = ":/aria2c/aria2c";
QString ariac_exe_path = "../Resources/aria2c";
QString ariac_conf_qrc_path = ":/aria2c/aria2.conf";
QString ariac_conf_path = "../Resources/aria2.conf";
#else
QString ariac_exe_qrc_path = ":/aria2c/aria2c.exe";
QString ariac_exe_path = "aria2c.exe";
QString ariac_conf_qrc_path = ":/aria2c/aria2.conf";
QString ariac_conf_path = "aria2.conf";
#endif

bool Aria2Util::is_init = false;
Aria2Util *Aria2Util::_instance = NULL;

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
    QFile aria2c_exe_qrc_file(ariac_exe_qrc_path);
    QFile aria2c_exe_file(ariac_exe_path);

    qDebug() << "Aria2Util::Aria2Util:check ariac exe file";
    if (!aria2c_exe_file.exists()) {
        if (aria2c_exe_qrc_file.copy(ariac_exe_path)) {
            qDebug() << "Aria2Util::Aria2Util:copy ariac exe file success" << ariac_exe_path;
        }
        else {
            qDebug() << "Aria2Util::Aria2Util:copy ariac exe file error";
        }
    }
    else {
        qDebug() << "Aria2Util::Aria2Util:ariac exe file exist";
    }

    qDebug() << "Aria2Util::Aria2Util:check ariac exe permission";
    if (!(aria2c_exe_file.permissions() & QFileDevice::ExeUser)) {
        aria2c_exe_file.setPermissions(QFileDevice::ExeUser);
        qDebug() << "Aria2Util::Aria2Util:set ariac exe permission";
    }
    else {
        qDebug() << "Aria2Util::Aria2Util:ariac exe permission ok";
    }

    this->checkAria2cConfFile();

    this->process = new QProcess();
    this->process->setProgram(ariac_exe_path);
    QStringList arguments;
    arguments.append(QString("--conf-path=%1").arg(ariac_conf_path));
    this->process->setArguments(arguments);
    connect(this->process, &QProcess::started, []() {
        QMLSignal::instance()->emitSignal(QMLSignalCMD::ARIA2_INIT, QVariant());
    });
    this->process->startDetached();
    // qDebug() << "exe error" << this->process->errorString();
    // qDebug() << "exe out" << QString(this->process->readAllStandardOutput());
}

Aria2Util::~Aria2Util() {
    delete this->process;
}

void Aria2Util::checkAria2cConfFile() {
    QFile aria2c_conf_qrc_file(ariac_conf_qrc_path);
    QFile aria2c_conf_file(ariac_conf_path);

    qDebug() << "Aria2Util::checkAria2cConfFile:check ariac conf file";
    if (!aria2c_conf_file.exists()) {
        if (aria2c_conf_qrc_file.copy(ariac_conf_path)) {
            qDebug() << "Aria2Util::checkAria2cConfFile:copy ariac conf file success" << ariac_conf_path;
        }
        else {
            qDebug() << "Aria2Util::checkAria2cConfFile:copy ariac conf file error";
        }
    }
    else {
        qDebug() << "Aria2Util::checkAria2cConfFile:ariac conf file exist";
    }
}
