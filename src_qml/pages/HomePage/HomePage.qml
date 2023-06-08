import QtQuick 2.13
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.11
import "../../common_component/MaterialUI"
import "../../common_component/Route"
import "../../common_component/Signal/QtSignal"
import "../../common_qml"
import "../../instance_component/Navbar"
import "./Component"
import "./View"
import "../../common_js/Color.js" as Color
import "../../common_js/Tools.js" as Tools

Pane {
    id: container
    x: 0
    y: 0
    padding: 0

    property int currentSelectIndex: -1 // 下载页面的
    property string currentViewName: "DownloadView"
    property var task_id_map: ({})

    ListModel {
        id: download_list
        dynamicRoles: true
    }

    Timer {
        interval: 500
        running: true
        repeat: true
        onTriggered: {
            refreshDownloadStatus()
        }
    }

    function refreshDownloadStatus() {
        // Aria2Util.getGlobalStat(function(res) {
        //     // console.log(res)
        // })
        Aria2Util.tellActive(function(res) {
            //console.log("Aria2Util.tellActive")
            //console.log(res)
            const status_map = {}
            var i = 0;
            var item;
            for (i = 0; i < res.length; i++) {
                item = res[i]
                // const { gid, uploadSpeed, downloadSpeed, completedLength, totalLength } = item
                const { gid } = item
                status_map[gid] = item
            }

            let download_list = GlobalTaskList.getDownloadList()
            let ans = []
            for (i = 0 ; i < download_list.length; i++) {
                item = download_list[i]
                var downloadSpeed = 0
                var uploadSpeed = 0
                var numActive = 0
                var numWaiting = 0
                var numStopped = 0
                var downloadFile = []
                const { gid_list } = item
                var last_active_gid = ""
                for (let j = 0; j < gid_list.length; j++) {
                    let the_gid = gid_list[j]
                    if (status_map[the_gid]) {
                        let the_status = status_map[the_gid]
                        downloadSpeed += the_status.downloadSpeed ? parseInt(the_status.downloadSpeed) : 0
                        uploadSpeed += the_status.uploadSpeed ? parseInt(the_status.uploadSpeed) : 0
                        numActive += 1
                        downloadFile.push({
                            completedLength: the_status.completedLength,
                            totalLength: the_status.totalLength,
                            downloadSpeed: the_status.downloadSpeed,
                            uploadSpeed: the_status.uploadSpeed,
                        })
                        last_active_gid = the_gid
                    }
                }
                // 需要刷新直到last_active_gid的下载状态
                ans.push({
                    downloadSpeed,
                    uploadSpeed,
                    numActive,
                    numWaiting,
                    numStopped,
                    numTotal: gid_list.length,
                    downloadFile,
                })
            }
            // console.log(JSON.stringify(ans))
            onAria2DownloadState(ans)
        })
    }

    function getFreeTaskId() {
        let task_id_list = []
        for (let task_id in task_id_map) {
            task_id_list.push(task_id)
        }
        task_id_list.sort()
        let maybe_id = 0
        for (let i = 0; i < task_id_list.length; i++) {
            let item = task_id_list[i]
            if (item === maybe_id) {
                maybe_id += 1
            }
            else {
                return maybe_id
            }
        }
        return maybe_id
    }

    function tryDownloadM3u8(download_link, save_path, save_file) {
        let task_id = getFreeTaskId()
        console.log("tryDownloadM3u8:")
        console.log("task_id:", task_id)
        DownloadM3u8.download(`${task_id}`, download_link, save_path, save_file)
        create_dialog.close()
    }

    function onM3u8DownloadFinished(arg) {
        const { task_id, file_name, file_path, m3u8 } = arg
        const { base_uri, base_path, files } = m3u8
        console.log("onM3u8DownloadFinished:")
        console.log("task_id:", task_id)
        console.log("file_name:", file_name)
        console.log("file_path:", file_path)
        console.log("m3u8.base_uri", base_uri)
        // console.log("m3u8.base_path", base_path)
        // console.log("m3u8.files:", JSON.stringify(m3u8))

        let file_urls = []
        for (let i = 0; i < files.length; i++) {
            let item = files[i]
            if (item.indexOf("http://") !== 0 && item.indexOf("https://") !== 0) {
                if (base_uri[base_uri.length-1] === "/") {
                    file_urls.push(`${base_uri}${item}`)
                }
                else {
                    file_urls.push(`${base_uri}/${item}`)
                }
            }
            else {
                file_urls.push(item)
            }
        }
        // console.log("file_urls:", JSON.stringify(file_urls))
        Aria2Util.downloadUri(file_path, file_name, file_urls, function(gid_list) {
            // console.log(JSON.stringify(gid_list))
            GlobalTaskList.addDownloadItem({
                task_id,
                file_path,
                file_name,
                m3u8,
                file_urls,
                gid_list,
            })
        })
        download_list.append({
            task_id,
            file_path,
            file_name,
            file_urls,
        })
    }

    function onAria2DownloadState(data) {
        for (let i = 0; i < download_list.count; i++) {
            download_list.set(i, data[i] ? data[i] : {})
        }
    }

    function onAria2DownloadPause(task_id) {
        console.log("HomePage.onAria2DownloadPause:", task_id)
        for (let i = 0; i < download_list.count; i++) {
            let item = download_list.get(i)
            if (item.task_id === task_id) {
                download_list.set(i, {
                    pause: true,
                })
                break
            }
        }
    }

    function onAria2DownloadUnpause(task_id) {
        console.log("HomePage.onAria2DownloadUnpause:", task_id)
        for (let i = 0; i < download_list.count; i++) {
            let item = download_list.get(i)
            if (item.task_id === task_id) {
                download_list.set(i, {
                    pause: false,
                })
                break
            }
        }
    }

    function onMainDeleteTask(arg) {
        const { task_id, delete_file } = arg
        for (let i = 0; i < download_list.count; i++) {
            let item = download_list.get(i)
            if (item.task_id === task_id) {
                download_list.remove(i, 1)
                delete task_id_map[parseInt(task_id)]
                if (!delete_file) {
                    console.log("onMainDeleteTask:need move to trash")
                }
                break
            }
        }
    }

    Component.onCompleted: {
//        for (let i = 0 ; i < 140; i++) {
//            download_list.append({})
//        }

        // requestList()
        QtSignal.registerCallback(QtSignal.signalCmd.M3U8_DOWNLOAD_FINISHED, onM3u8DownloadFinished)
//        QtSignal.registerCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_STATE, onAria2DownloadState)
//        QtSignal.registerCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_PAUSE, onAria2DownloadPause)
//        QtSignal.registerCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_UNPAUSE, onAria2DownloadUnpause)
//        QtSignal.registerCallback(QtSignal.signalCmd.MAIN_DELETE_TASK, onMainDeleteTask)
    }

    Component.onDestruction: {
        QtSignal.unregisterCallback(QtSignal.signalCmd.M3U8_DOWNLOAD_FINISHED, onM3u8DownloadFinished)
//        QtSignal.unregisterCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_STATE, onAria2DownloadState)
//        QtSignal.unregisterCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_PAUSE, onAria2DownloadPause)
//        QtSignal.unregisterCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_UNPAUSE, onAria2DownloadUnpause)
//        QtSignal.unregisterCallback(QtSignal.signalCmd.MAIN_DELETE_TASK, onMainDeleteTask)
    }

    RowLayout {
        id: main_area
        anchors.fill: parent
        spacing: 0

        Navbar {
            RowLayout.fillHeight: true
        }

        Rectangle {
            id: right_area
            RowLayout.fillHeight: true
            RowLayout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 0

                ColumnLayout {
                    RowLayout.fillWidth: true
                    RowLayout.fillHeight: true
                    spacing: 0

                    RowLayout {
                        id: button_area
                        Layout.margins: 10
                        ColumnLayout.fillWidth: true
                        spacing: 10

                        MButton {
                            text: qsTr("下载中")
                            onClicked: {
                                stackView.replace(download_view)
                                currentViewName = "DownloadView"
                            }
                            color: currentViewName === "DownloadView" ? "primary" : "default"
                            variant: currentViewName === "DownloadView" ? "outlined" : "default"
                        }

                        MButton {
                            text: qsTr("已完成")
                            onClicked: {
                                stackView.replace(history_view)
                                currentViewName = "HistoryView"
                            }
                            color: currentViewName === "HistoryView" ? "primary" : "default"
                            variant: currentViewName === "HistoryView" ? "outlined" : "default"
                        }

                        MButton {
                            text: qsTr("回收站")
                            onClicked: {
                                stackView.replace(delete_view)
                                currentViewName = "DeleteView"
                            }
                            color: currentViewName === "DeleteView" ? "primary" : "default"
                            variant: currentViewName === "DeleteView" ? "outlined" : "default"
                        }

                        Rectangle {
                            RowLayout.fillWidth: true
                            height: 20
                            color: Color.none
                        }

                        MButton {
                            variant: 'outlined'
                            text: qsTr("新建")
                            color: Color.text_secondary
                            ToolTip.text: qsTr("新建下载任务")
                            ToolTip.visible: hovered
                            ToolTip.timeout: 3000
                            ToolTip.delay: 0

                            onClicked: {
                                create_dialog.open()
                            }
                        }
                    }

                    MDivider {
                        ColumnLayout.fillWidth: true
                    }

                    StackView {
                        id: stackView
                        ColumnLayout.fillWidth: true
                        ColumnLayout.fillHeight: true
                        initialItem: download_view
                        replaceEnter: null
                        replaceExit: null
                    }
                }

                Rectangle {
                    width: 1
                    RowLayout.fillHeight: true
                    color: '#dddddd'
                }

                DownloadInfo {
                    visible: currentSelectIndex != -1
                    width: 350
                    RowLayout.fillHeight: true

                    download_info: {
                        if (currentSelectIndex == -1) {
                            return null
                        }
                        let ans = download_list.get(currentSelectIndex)
                        return ans
                    }
                }

                Rectangle {
                    visible: currentSelectIndex == -1
                    width: 350
                    RowLayout.fillHeight: true
                }
            }
        }
    }

    Component {
        id: download_view

        DownloadView {
            listModel: download_list
            onClick: function(index) {
                container.currentSelectIndex = index
            }
        }
    }

    Component {
        id: history_view

        HistoryView {
        }
    }

    Component {
        id: delete_view

        DeleteView {
        }
    }

    CreateDialog {
        id: create_dialog

        tryDownload: tryDownloadM3u8
    }

    MToast {
        id: toast
    }
}
