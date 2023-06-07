#include "application_event_filter.h"
#include <QEvent>
#include <QApplicationStateChangeEvent>
#include <QDebug>

ApplicationEventFilter::ApplicationEventFilter(std::function<void(QString, QVariant)> callback) {
    this->callback = callback;
}

bool ApplicationEventFilter::eventFilter(QObject *obj, QEvent *event) {
    // qDebug() << event->type();
    auto event_type = event->type();

#ifdef Q_OS_MAC
    if (event_type == QEvent::ApplicationStateChange) {
        Qt::ApplicationState state = static_cast<QApplicationStateChangeEvent*>(event)->applicationState();
        if (state == Qt::ApplicationActive) {
            this->callback("MAC_ApplicationActive", QVariant());
        }
    }
    else if (event_type == QEvent::Quit) {
        this->callback("MAC_Quit", QVariant());
    }
#endif

    return QObject::eventFilter(obj, event);
}
