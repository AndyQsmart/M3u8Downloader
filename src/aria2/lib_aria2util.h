#ifndef LIB_ARIA2UTIL_H
#define LIB_ARIA2UTIL_H

#include "qglobal.h"

#ifdef Q_OS_MAC
#include <aria2.h>
#include <QString>
#include <QStringList>
#include <QList>

class Aria2Task;

class Aria2Util {
private:
    static bool is_init;
    static QList<Aria2Task *> task_list;

public:
    static void init();
    static void uninit();
    static Aria2Task *createTask(QString task_id, QString file_path, QString file_name, QStringList ts_list);
    static int downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event, const aria2::A2Gid gid, void* userData);
};

#endif
#endif // ARIA2UTIL_H
