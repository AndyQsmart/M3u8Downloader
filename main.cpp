#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFont>
#include "src/m3u8/download_m3u8.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // 高分辨率适配
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QGuiApplication app(argc, argv);
    // QQuickStyle::setStyle("Material");
    QFont defualt_font;
    defualt_font.setFamily("Arial");
    app.setFont(defualt_font);
    app.setOrganizationName("Zhibing");
    app.setOrganizationDomain("www.jiuzhangzaixian.com");

    QQmlApplicationEngine engine;
    // m3u8下载
    DownloadM3u8 download_m3u8;
    engine.rootContext()->setContextProperty("DownloadM3u8", &download_m3u8);
//    // 图片传递相关
//    ImageTools image_tools;
//    engine.rootContext()->setContextProperty("ImageTools", &image_tools);
//    ImageProvider image_provider;
//    engine.addImageProvider(QString("image_provider"), &image_provider);
    // 信号相关
//    engine.rootContext()->setContextProperty("QMLSignal", QMLSignal::instance());

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
