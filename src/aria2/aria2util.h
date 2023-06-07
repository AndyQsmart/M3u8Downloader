#ifndef ARIA2UTIL_H
#define ARIA2UTIL_H

#include <QString>
#include <QStringList>
#include <QList>

class QProcess;

class Aria2Util {
private:
    QProcess *process=NULL;
    static Aria2Util *_instance;
    static bool is_init;

public:
    Aria2Util();
    ~Aria2Util();
    void checkAria2cConfFile();
    static void init();
    static void uninit();
};

#endif // ARIA2UTIL_H
