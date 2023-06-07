#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFont>
#include <QString>
#include <QVariant>
#include <QMap>
#include "src/download_m3u8.h"
#include "src/utils/qml_signal.h"
#include "src/utils/application_event_filter.h"
#include <QDebug>

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // 高分辨率适配
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QGuiApplication app(argc, argv);
#ifdef Q_OS_MAC
    QFont default_font;
    default_font.setFamily("Arial");
    app.setFont(default_font);
#endif
    app.setOrganizationName("Zhibing");
    app.setOrganizationDomain("www.jiuzhangzaixian.com");

    QQmlApplicationEngine engine;
    // m3u8下载
    DownloadM3u8 download_m3u8;
    engine.rootContext()->setContextProperty("DownloadM3u8", &download_m3u8);
    // 信号相关
    engine.rootContext()->setContextProperty("QMLSignal", QMLSignal::instance());

    // 全局特殊事件处理
    ApplicationEventFilter app_event_filter([](QString event_name, QVariant event_data) {
        QMap<QString, QVariant> data;
        data["event_name"] = event_name;
        data["event_data"] = event_data;
        QMLSignal::instance()->emitSignal(QMLSignalCMD::APP_EVENT, data);
    });
    app.installEventFilter(&app_event_filter);

    const QUrl url(QStringLiteral("qrc:/src_qml/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    engine.load(url);

    return app.exec();
}
