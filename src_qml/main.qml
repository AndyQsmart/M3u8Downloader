import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1
import "./common_component/Route"
import "./common_component/MaterialUI"
import "./common_component/Signal/QtSignal"
import "./common_qml"
import "./instance_component/SQLTable"
import "./instance_component/SystemTray"
import "./pages/HomePage"

MFramelessWindow {
    id: mainWindow
    width: 900
    height: 600
    minimumWidth: 900
    minimumHeight: 600
    visible: true
    title: qsTr("m3u8下载器")

    function onQtSignal(cmd, data) {
        QtSignal.runCallback(cmd, data)
//        console.log(cmd)
//        console.log(JSON.stringify(data))
    }

    function backupRoute() {
        let route_stack = Route.getStack()
        console.log("(main.qml)Save route", JSON.stringify(route_stack))
        SettingData.setValue('RouteStack', route_stack)
    }

    function quitApp() {
        try {
            backupRoute()
        }
        catch (e) {
            console.log("main.backupRoute error", e)
        }
        console.log("quitApp:start shutdown aria2")
        Aria2Util.shutdown(function (res) {
            console.log(res)
            Qt.exit(0)
        })
    }

    function onAppEvent(data) {
        const { event_name, event_data } = data
        if (event_name === "MAC_ApplicationActive") {
            mainWindow.visible = true
        }
        else if (event_name === "MAC_Quit") {
            quitApp()
        }
    }

    // 可能是qmltype信息不全，有M16警告，这里屏蔽下
    // @disable-check M16
    onClosing: function(closeevent) {
        mainWindow.hide()
//        CloseEvent的accepted设置为false就能忽略该事件
        closeevent.accepted = false
    }

    StackView {
        id: stackView
        x: 0
        y: 0
        padding: 0
        width: parent.width
        height: parent.height
//        initialItem: test_page
        initialItem: home_page

        Component.onCompleted: {
            console.log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> main.onCompleted start")
            console.log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
            console.log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
            QMLSignal.qmlSignal.connect(onQtSignal)
            QtSignal.registerCallback(QtSignal.signalCmd.APP_EVENT, onAppEvent)
            Aria2Util.init()
            SettingData.getValue('RouteStack', function(value) {
                console.log("(main.qml)Recover route", JSON.stringify(value))
                if (!value || value.length === 0) {
                    return
                }

                if (value.length === 1 && value[0].url === '/') {
                    return
                }

                Route.setStack(value)

                stackView.clear()

                for (let i = 0; i < value.length; i++) {
                    let url = value[i].url
                    console.log('(main.qml)Recover', i, url)
                    stackView.push(route_map[url], StackView.Immediate)
                }
            })

            console.log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
            console.log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
            console.log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< main.onCompleted end")
            Logger.logTime("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< main.onCompleted end")
        }
    }

    Component {
        id: home_page
        HomePage { }
    }

    property var route_map: {
        "/": home_page,
//        "/live/historyrecordpage": history_record_page,
//        "/live/liverecordpage": live_record_page,
    }

    Connections {
        target: Route

        function onSwitchToCallback(url) {
            stackView.push(route_map[url], StackView.Immediate)
        }

        function onRedirectToCallback(url) {
            stackView.pop()
            stackView.push(route_map[url], StackView.Immediate)
        }

        function onNavigateToCallback(url) {
            stackView.push(route_map[url])
        }

        function onNavigateBackCallback(count) {
            for (let i = 0; i < count; i++) {
                stackView.pop()
            }
        }
    }

    SystemTray {
        onShowWindow: {
            mainWindow.visible = true
            mainWindow.requestActivate()
        }

        onQuitApp: {
            mainWindow.quitApp()
        }
    }
}
