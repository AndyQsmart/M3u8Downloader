#ifndef DOWNLOADM3U8_H
#define DOWNLOADM3U8_H

#include <QObject>
#include <QVariant>
#include <QString>

class DownloadM3u8 : public QObject {
    Q_OBJECT

public:
    DownloadM3u8();

    Q_INVOKABLE void download(QVariant download_link, QVariant file_path, QVariant file_name) const;
};

#endif // DOWNLOADM3U8_H
