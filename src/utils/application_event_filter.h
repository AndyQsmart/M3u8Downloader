#ifndef APPLICATIONEVENTFILTER_H
#define APPLICATIONEVENTFILTER_H

#include <functional>
#include <QString>
#include <QVariant>
#include <QObject>

class ApplicationEventFilter : public QObject {
    Q_OBJECT

private:
    std::function<void(QString, QVariant)> callback;

public:
    ApplicationEventFilter(std::function<void(QString, QVariant)> callback);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // APPLICATIONEVENTFILTER_H
