#ifdef Q_OS_MAC
#include "lib_aria2task.h"
#include "lib_aria2util.h"
#include <QMutex>
#include <QMutexLocker>
#include <QTime>
#include <QDebug>

void Aria2Task::run() {
    qDebug() << "Aria2Task::run";
    this->run_mutex->lock();
    this->stoped = false;
    this->paused = false;
    this->run_mutex->unlock();

    int result = 1;
    QTime last_time = QTime::currentTime();
    while (true) {
        result = aria2::run(this->session, aria2::RUN_ONCE);
        if (result != 1) {
            break;
        }
        if (this->stoped) {
            break;
        }
        if (this->paused) {
            break;
        }
        QTime now_time = QTime::currentTime();
        int ms_diff = last_time.msecsTo(now_time);
        if (ms_diff >= 500) {
            last_time = now_time;
            QMap<QString, QVariant> state_data;
            aria2::GlobalStat gstat = aria2::getGlobalStat(this->session);
            //            qDebug() << "Overall #Active:" << gstat.numActive
            //                     << " #waiting:" << gstat.numWaiting
            //                     << " D:" << gstat.downloadSpeed / 1024 << "KiB/s"
            //                     << " U:" << gstat.uploadSpeed / 1024 << "KiB/s ";
            state_data["downloadSpeed"] = gstat.downloadSpeed;
            state_data["uploadSpeed"] = gstat.uploadSpeed;
            state_data["numActive"] = gstat.numActive;
            state_data["numWaiting"] = gstat.numWaiting;
            state_data["numStopped"] = gstat.numStopped;

            std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(this->session);
            QList<QVariant> file_data;
            for (const auto& gid : gids) {
                aria2::DownloadHandle* download_handle = aria2::getDownloadHandle(this->session, gid);
                if (download_handle) {
                    //                    qDebug() << "    [" << QString::fromStdString(aria2::gidToHex(gid)) << "] "
                    //                             << download_handle->getCompletedLength() << "/" << download_handle->getTotalLength()
                    //                             << "("
                    //                             << (download_handle->getTotalLength() > 0
                    //                                     ? (100 * download_handle->getCompletedLength() /
                    //                                        download_handle->getTotalLength())
                    //                                     : 0)
                    //                             << "%)"
                    //                             << " D:" << download_handle->getDownloadSpeed() / 1024
                    //                             << "KiB/s, U:" << download_handle->getUploadSpeed() / 1024 << "KiB/s";
                    QMap<QString, QVariant> item_data;
                    item_data["completedLength"] = download_handle->getCompletedLength();
                    item_data["totalLength"] = download_handle->getTotalLength();
                    item_data["downloadSpeed"] = download_handle->getDownloadSpeed();
                    item_data["uploadSpeed"] = download_handle->getUploadSpeed();
                    // item_data["dir"] = QString::fromStdString(download_handle->getDir());
                    if (download_handle->getNumFiles() > 0) {
                        aria2::FileData file_data = download_handle->getFile(1);
                        if (!file_data.path.empty()) {
                            item_data["file"] = QString::fromStdString(file_data.path);
                        }
                    }
                    file_data.append(item_data);

                    aria2::deleteDownloadHandle(download_handle);
                }
            }
            state_data["downloadFile"] = file_data;
            emit onDownloadState(this->task_id, QVariant(state_data));
        }
    }

    if (this->stoped) {
        // 只有停止才需要做，暂停不需要
        result = aria2::sessionFinal(this->session);
    }
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
    aria2::Session *session;
    // Create default configuration. The libaria2 takes care of signal
    // handling.
    aria2::SessionConfig config;
    // Add event callback
    config.downloadEventCallback = Aria2Util::downloadEventCallback;
    session = aria2::sessionNew(aria2::KeyVals(), config);

    // Add download item to session
    QString save_folder = this->getTempSaveFolder();
    for (int i = 0; i < this->ts_list.size(); i++) {
        std::vector<std::string> uris = {
            this->ts_list[i].toStdString()
        };
        aria2::KeyVals options;

        options.push_back(std::pair<std::string, std::string> ("dir", save_folder.toStdString()));
        options.push_back(std::pair<std::string, std::string> ("out", QString("%1.ts").arg(i+1).toStdString()));
        int result = aria2::addUri(session, nullptr, uris, options);

        if (result < 0) {
            qDebug() << "Aria2Task::initAria2Session:error:fail add uri:" << this->ts_list[i];
        }
    }

    this->session = session;
    qDebug() << "Aria2Task::initAria2Session:" << "ts_save_folder:" << save_folder;
}

aria2::Session *Aria2Task::getSession() {
    return this->session;
}

bool Aria2Task::hasGid(const aria2::A2Gid gid) {
    std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(this->session);
    for (const auto& the_gid : gids) {
        if (the_gid == gid) {
            return true;
        }
    }
    return false;
}

int Aria2Task::downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event, const aria2::A2Gid gid, void* userData) {
    bool debug_log = true;
    QString gid_str = QString::fromStdString(aria2::gidToHex(gid));
    switch(event) {
        case aria2::EVENT_ON_DOWNLOAD_START:
            if (debug_log) {
                qDebug() << "Aria2Task::downloadEventCallback:" << "gid:" << gid_str << "event:start";
            }
            break;
        case aria2::EVENT_ON_DOWNLOAD_PAUSE:
            if (debug_log) {
                qDebug() << "Aria2Task::downloadEventCallback:" << "gid:" << gid_str << "event:pause";
            }
            break;
        case aria2::EVENT_ON_DOWNLOAD_STOP:
            if (debug_log) {
                qDebug() << "Aria2Task::downloadEventCallback:" << "gid:" << gid_str << "event:stop";
            }
            break;
        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
            if (debug_log) {
                qDebug() << "Aria2Task::downloadEventCallback:" << "gid:" << gid_str << "event:complete";
            }
            break;
        case aria2::EVENT_ON_DOWNLOAD_ERROR:
            if (debug_log) {
                qDebug() << "Aria2Task::downloadEventCallback:" << "gid:" << gid_str << "event:error";
            }
            break;
        case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
            if (debug_log) {
                qDebug() << "Aria2Task::downloadEventCallback:" << "gid:" << gid_str << "event:bt complete";
            }
            break;
        default:
            return 0;
    }

    aria2::DownloadHandle* download_handle = aria2::getDownloadHandle(session, gid);
    if (!download_handle)
        return 0;
    if (debug_log) {
        qDebug() << "Aria2Task::downloadEventCallback:" << "error_code:" << download_handle->getErrorCode();
    }
    if (download_handle->getNumFiles() > 0) {
        aria2::FileData file_data = download_handle->getFile(1);
        // Path may be empty if the file name has not been determined yet.
        if (file_data.path.empty()) {
            if (!file_data.uris.empty()) {
                if (debug_log) {
                    qDebug() << QString::fromStdString(file_data.uris[0].uri);
                }
            }
        }
        else {
            if (debug_log) {
                qDebug() << QString::fromStdString(file_data.path);
            }
        }
    }
    aria2::deleteDownloadHandle(download_handle);
    return 0;
}
#endif
