#ifndef RPC_ARIA2UTIL_H
#define RPC_ARIA2UTIL_H

#include "qglobal.h"

#ifndef Q_OS_MAC
#include "rpc_aria2task.h"
#include <QString>
#include <QStringList>
#include <QList>

class Aria2Util {
private:
    static bool is_init;
    static QList<Aria2Task *> task_list;

public:
    static void init();
    static void uninit();
    static Aria2Task *createTask(QString task_id, QString file_path, QString file_name, QStringList ts_list);
};

#endif
#endif // RPCARIA2UTIL_H
