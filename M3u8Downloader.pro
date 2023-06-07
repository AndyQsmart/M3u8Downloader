QT += quick
QT += quickcontrols2
QT += gui
QT += core
QT += multimedia
QT += network
QT += sql

#LIBS += -lgdi32 -ldwmapi

#ffmpeg
macx {
#    INCLUDEPATH += $$PWD/lib/aria2/include
#    LIBS += -L$$PWD/lib/aria2/lib/mac/ -laria2.0
}

CONFIG += c++11
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    src/aria2/aria2util.h \
    src/download_m3u8.h \
    src/m3u8/core/httpclient.h \
    src/m3u8/core/m3u8.h \
    src/m3u8/core/mixins.h \
    src/m3u8/core/model.h \
    src/m3u8/core/parser.h \
    src/m3u8/core/protocol.h \
    src/m3u8/core/url_lib.h \
    src/m3u8/m3u8_task.h \
    #    src/utils/qhttp.h \
    src/utils/application_event_filter.h \
    src/utils/qml_signal.h

SOURCES += \
    main.cpp \
    src/aria2/aria2util.cpp \
    src/download_m3u8.cpp \
    src/m3u8/core/httpclient.cpp \
    src/m3u8/core/m3u8.cpp \
    src/m3u8/core/mixins.cpp \
    src/m3u8/core/model.cpp \
    src/m3u8/core/parser.cpp \
    src/m3u8/core/protocol.cpp \
    src/m3u8/core/url_lib.cpp \
    src/m3u8/m3u8_task.cpp \
#        src/utils/qhttp.cpp \
    src/utils/application_event_filter.cpp \
    src/utils/qml_signal.cpp

RESOURCES += qml.qrc

# aria2
win32 {
    contains(QT_ARCH, x86_64) {
        RESOURCES += lib/aria2/exe/win64/aria2c.qrc
    }
    else {
    }
}

macx {
    RESOURCES += lib/aria2/exe/mac/aria2c.qrc
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
