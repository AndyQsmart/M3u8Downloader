QT += quick
QT += quickcontrols2
QT += gui
QT += core
QT += multimedia

#LIBS += -lgdi32 -ldwmapi

# ffmpeg
win32 {
    INCLUDEPATH += $$PWD/lib/ffmpeg/include

    PRE_TARGETDEPS += $$PWD/lib/ffmpeg/lib/libavcodec.dll.a \
        $$PWD/lib/ffmpeg/lib/libavdevice.dll.a \
        $$PWD/lib/ffmpeg/lib/libavfilter.dll.a \
        $$PWD/lib/ffmpeg/lib/libavformat.dll.a \
        $$PWD/lib/ffmpeg/lib/libavutil.dll.a \
        $$PWD/lib/ffmpeg/lib/libswresample.dll.a \
        $$PWD/lib/ffmpeg/lib/libswscale.dll.a

    LIBS += -L$$PWD/lib/ffmpeg/lib/ \
        -llibavcodec.dll \
        -llibavdevice.dll \
        -llibavfilter.dll \
        -llibavformat.dll \
        -llibavutil.dll \
        -llibswresample.dll \
        -llibswscale.dll
}

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/m3u8/download_m3u8.cpp \
        src/m3u8/m3u8_task.cpp \
        src/utils/qml_signal.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/m3u8/download_m3u8.h \
    src/m3u8/m3u8_task.h \
    src/utils/qml_signal.h
