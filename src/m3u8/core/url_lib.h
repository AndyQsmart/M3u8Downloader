#ifndef URLLIB_H
#define URLLIB_H

#include <QString>

class UrlLib {
public:
    UrlLib();
    static QString urljoin(QString base, QString url, bool allow_fragments=true);
};

#endif // URLLIB_H
