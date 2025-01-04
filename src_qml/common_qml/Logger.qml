pragma Singleton

import QtQuick 2.15

QtObject {
    id: root
    property real startTimeStamp: 0

    function init() {
        root.startTimeStamp = Tools.getTimeStampFloat()
    }

    function logTime(log_str) {
        let current_time_stamp = Tools.getTimeStampFloat()
        console.log(`LogTime:${current_time_stamp-root.startTimeStamp}s`, log_str)
    }
}
