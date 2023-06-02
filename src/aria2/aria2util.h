#ifndef ARIA2UTIL_H
#define ARIA2UTIL_H

#include "qglobal.h"

#ifdef Q_OS_MAC
#include "lib_aria2util.h"
#else
#include "rpc_aria2util.h"
#endif

#endif // ARIA2UTIL_H
