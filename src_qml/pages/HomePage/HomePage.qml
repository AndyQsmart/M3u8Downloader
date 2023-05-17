import QtQuick 2.13
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.11
import "../../common_component/MaterialUI"
import "../../common_component/Route"
import "../../common_component/Signal/QtSignal"
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

    ListModel {
        id: download_list
        dynamicRoles: true
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
        DownloadM3u8.downloadTs(task_id, file_path, file_name, file_urls)
        download_list.append({
            task_id,
            file_path,
            file_name,
            file_urls,
        })
    }

    function onAria2DownloadState(arg) {
        const { task_id, data } = arg
        for (let i = 0; i < download_list.count; i++) {
            let item = download_list.get(i)
            if (item.task_id === arg.task_id) {
                download_list.set(i, data)
                break
            }
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

    Component.onCompleted: {
//        for (let i = 0 ; i < 140; i++) {
//            download_list.append({})
//        }

        // requestList()
        QtSignal.registerCallback(QtSignal.signalCmd.M3U8_DOWNLOAD_FINISHED, onM3u8DownloadFinished)
        QtSignal.registerCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_STATE, onAria2DownloadState)
        QtSignal.registerCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_PAUSE, onAria2DownloadPause)
        QtSignal.registerCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_UNPAUSE, onAria2DownloadUnpause)
    }

    Component.onDestruction: {
        QtSignal.unregisterCallback(QtSignal.signalCmd.M3U8_DOWNLOAD_FINISHED, onM3u8DownloadFinished)
        QtSignal.unregisterCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_STATE, onAria2DownloadState)
        QtSignal.unregisterCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_PAUSE, onAria2DownloadPause)
        QtSignal.unregisterCallback(QtSignal.signalCmd.ARIA2_DOWNLOAD_UNPAUSE, onAria2DownloadUnpause)
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
    }

    MToast {
        id: toast
    }
}
