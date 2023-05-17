#ifndef MIXINS_H
#define MIXINS_H

#include <QString>

class BasePathMixin {
public:
    BasePathMixin();
    QString uri;
    QString base_uri;

    QString _urijoin(QString base_uri, QString path);
    QString absolute_uri();
    QString get_path_from_uri();
    QString base_path();
    void set_base_path(QString newbase_path);
};

template <typename T>
class GroupedBasePathMixinList {
public:
    QList<T> list;

    void set_base_uri(QString new_base_uri)  {
        for (int i = 0; i < this->list.length(); i++) {
            this->list[i].base_uri = new_base_uri;
        }
    }
    void set_base_path(QString newbase_path) {
        for (int i = 0; i < this->list.length(); i++) {
            this->list[i].set_base_path(newbase_path);
        }
    }
};

#endif // MIXINS_H
