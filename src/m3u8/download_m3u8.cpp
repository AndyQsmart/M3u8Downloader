#include "download_m3u8.h"
#include <QDebug>

DownloadM3u8::DownloadM3u8() {

}

void DownloadM3u8::download(QVariant download_link, QVariant file_path, QVariant file_name) const {
    QString download_link_str = download_link.toString();

    qDebug() << download_link_str;
}
