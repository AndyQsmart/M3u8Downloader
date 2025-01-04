pragma Singleton

import QtQuick 2.15
import "../../../common_qml"

Item {
    readonly property var signalCmd: ({
        M3U8_DOWNLOAD_FINISHED: 1,
        APP_EVENT: 2,
        ARIA2_INIT: 3,
    })

    property var signalCallback: ({})

    function registerCallback(cmd, func) {
        signalCallback[cmd] = func
    }

    function unregisterCallback(cmd, func) {
        delete signalCallback[cmd]
    }

    function runCallback(cmd, data) {
        let func = signalCallback[cmd]
        if (func) {
            func(data)
        }
    }
}
