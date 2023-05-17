#include "url_lib.h"
#include <QUrl>
#include <QList>
#include <QDebug>

UrlLib::UrlLib() {

}

QList<QString> uses_relative = {
    "", "ftp", "http", "gopher", "nntp", "imap",
    "wais", "file", "https", "shttp", "mms",
    "prospero", "rtsp", "rtspu", "sftp",
    "svn", "svn+ssh", "ws", "wss"
};

QList<QString> uses_netloc = {
    "", "ftp", "http", "gopher", "nntp", "telnet",
    "imap", "wais", "file", "mms", "https", "shttp",
    "snews", "prospero", "rtsp", "rtspu", "rsync",
    "svn", "svn+ssh", "sftp", "nfs", "git", "git+ssh",
    "ws", "wss"
};

QString UrlLib::urljoin(QString base, QString url, bool allow_fragments) {
    /*
        Join a base URL and a possibly relative URL to form an absolute
        interpretation of the latter.
    */
    // qDebug() << "UrlLib::urljoin:" << "base:" << base << "url:" << url;
    if (base == "") {
        return url;
    }
    if (url == "") {
        return base;
    }

    QUrl base_url = QUrl(base);
    QString bscheme = base_url.scheme();
    QString bpath = base_url.path();
    QString buser_info = base_url.userInfo();
    int bport = base_url.port();
    QString bquery = base_url.query();
    QUrl url_url = QUrl(url);
    QString scheme = url_url.scheme();
    QString path = url_url.path();
    QString user_info = url_url.userInfo();
    int port = url_url.port();
    QString query = url_url.query();

//    bscheme, bnetloc, bpath, bparams, bquery, bfragment =  urlparse(base, "", allow_fragments)
//    scheme, netloc, path, params, query, fragment = urlparse(url, bscheme, allow_fragments)

    if ((bscheme != scheme && scheme != "") || !uses_relative.contains(scheme)) {
        // qDebug() << "bscheme != scheme || !uses_relative.contains(scheme)";
        // qDebug() << "scheme:" << scheme;
        return url;
    }
    if (scheme == "") {
        url_url.setScheme(bscheme);
    }

    if (uses_netloc.contains(scheme)) {
        if (url_url.host() != "") {
            return url;
        }
        url_url.setHost(base_url.host());
    }

    if (path == "" && user_info == "" && port == -1) {
        qDebug() << "if (path == "" && user_info == "" && port == -1):";
        url_url.setPath(bpath);
        url_url.setUserInfo(buser_info);
        url_url.setPort(bport);
        if (query == "") {
            url_url.setQuery(bquery);
        }
        return url_url.toString();
    }

    QStringList base_parts = bpath.split("/");
    if (base_parts[base_parts.size()-1] != "") {
        // # the last item is not a directory, so will not be taken into account
        // # in resolving the relative path
        base_parts.removeLast();
    }

    // # for rfc3986, ignore all base path should the first character be root.
    QStringList segments;
    if (path[0] == "/") {
        segments = path.split("/");
    }
    else {
        segments = base_parts + path.split("/");
        // # filter out elements that would cause redundant slashes on re-joining
        // # the resolved_path
        QStringList new_segments;
        new_segments.append(segments[0]);
        for (int i = 1; i < segments.size()-1; i++) {
            if (segments[i] != "") {
                new_segments.append(segments[i]);
            }
        }
        new_segments.append(segments[segments.size()-1]);
        segments = new_segments;
    }

    QStringList resolved_path;
//    for (int i = 0; i < resolved_path.size(); i++) {
//        qDebug() << resolved_path[i];
//    }

    for (int i = 0; i < segments.size(); i++) {
        QString seg = segments[i];
        if (seg == "..") {
            if (resolved_path.size() > 0) {
                resolved_path.removeLast();
            }
            else {
                // # ignore any .. segments that would otherwise cause an IndexError
                // # when popped from resolved_path if resolving for rfc3986
            }
        }
        else if (seg == ".") {
            continue;
        }
        else {
            resolved_path.append(seg);
        }
    }

    if (segments.last() == "." || segments.last() == "..") {
        // # do some post-processing here. if the last segment was a relative dir,
        // # then we need to append the trailing "/"
        resolved_path.append("");
    }

    if (resolved_path.size() > 0) {
        url_url.setPath(resolved_path.join("/"));
    }
    else {
        url_url.setPath("/");
    }

    return url_url.toString();
}

