#include "mixins.h"
#include "parser.h"
#include <QDir>
#include <QStringList>

BasePathMixin::BasePathMixin() {
    this->uri = "";
    this->base_uri = "";
}

QString BasePathMixin::_urijoin(QString base_uri, QString path) {
    if (Parser::is_url(base_uri)) {
        if (base_uri[base_uri.length()-1] != "/") {
            base_uri += "/";
        }
        return Parser::urljoin(base_uri, path);
    }
    else {
        if (base_uri[base_uri.length()-1] != "/") {
            base_uri += "/";
        }
        if (path[0] == "/") {
            path.remove(0, 1);
        }
        if (base_uri[path.length()-1] != "/") {
            path += "/";
        }
        QDir base_path(base_uri+path);
        return base_path.path();
    }
}

QString BasePathMixin::absolute_uri() {
    if (this->uri == "") {
        return "";
    }

    if (Parser::is_url(this->uri)) {
        return this->uri;
    }
    else {
        if (this->base_uri == "") {
            throw "There can not be `absolute_uri` with no `base_uri` set";
        }
        return this->_urijoin(this->base_uri, this->uri);
    }
}

QString BasePathMixin::get_path_from_uri() {
    /*Some URIs have a slash in the query string.*/
    return this->uri.split("?")[0];
}

QString BasePathMixin::base_path() {
    if (this->uri == "") {
        return "";
    }
    QString temp = this->get_path_from_uri();
    if (temp[temp.length()-1] == "/") {
        return temp;
    }
    else {
        QStringList dir_list = temp.split("/");
        QString ans = "";
        for (int i = 0; i < dir_list.size()-1; i++) {
            if (i == 0) {
                ans += dir_list[i];
            }
            else {
                ans += "/"+dir_list[i];
            }
        }
        return ans;
    }
}

void BasePathMixin::set_base_path(QString newbase_path) {
    if (this->uri != "") {
        if (this->base_path() == "") {
            this->uri = newbase_path+"/"+this->uri;
        }
        else {
            this->uri = this->uri.replace(this->base_path(), newbase_path);
        }
    }
}
