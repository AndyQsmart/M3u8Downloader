pragma Singleton

import QtQuick 2.0
import "../../../common_js/Tools.js" as Tools

Item {
    readonly property var signalCmd: ({
        M3U8_DOWNLOAD_FINISHED: 1,
        ARIA2_DOWNLOAD_STATE: 2,
        ARIA2_DOWNLOAD_PAUSE: 3,
        ARIA2_DOWNLOAD_UNPAUSE: 4,
        MAIN_DELETE_TASK: 5,
        APP_EVENT: 6,
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
