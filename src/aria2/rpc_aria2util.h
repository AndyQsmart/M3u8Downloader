#ifndef RPC_ARIA2UTIL_H
#define RPC_ARIA2UTIL_H

#include "qglobal.h"

#ifndef Q_OS_MAC
#include "aria2task.h"
#include <QString>
#include <QStringList>
#include <QList>

class QProcess;

class Aria2Util {
private:
    QProcess *process=NULL;
    static Aria2Util *_instance;
    static bool is_init;
    static QList<Aria2Task *> task_list;

public:
    Aria2Util();
    ~Aria2Util();
    static void init();
    static void uninit();
    static Aria2Task *createTask(QString task_id, QString file_path, QString file_name, QStringList ts_list);
};

#endif
#endif // RPCARIA2UTIL_H
