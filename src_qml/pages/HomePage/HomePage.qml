import QtQuick 2.13
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.11
import "../../common_component/MaterialUI"
import "../../common_component/Route"
import "../../instance_component/Navbar"
import "./Component"
import "../../common_js/Color.js" as Color
import "../../common_js/Tools.js" as Tools

Pane {
    // property var task_list: []
    // property int current_menu_index: -1

    id: container
    x: 0
    y: 0
    padding: 0

    function onSelectFile(file_path) {
        if (out_select.currentIndex == 0) {
            let file_dir = FileTools.getFileDir(file_path)
            let file_name = FileTools.getFileName(file_path)
            let file_name_list = file_name.split('.')
            let out_file_name = ''
            if (file_name_list.length > 1) {
                file_name_list[file_name_list.length-2] += '_latex'
                out_file_name = file_name_list.join('.')
            }
            else {
                out_file_name = file_name+'_latex'
            }

            WordTools.processWordFileToSaveFile(file_path, FileTools.joinPath(file_dir, out_file_name))
            toast.success(`公式转换成功，请查看${out_file_name}文件`)
        }
        else {
            let ans_str = WordTools.processWordFile(file_path)
            console.log(ans_str)
            direct_out_view.showText(ans_str)
        }
    }

    Component.onCompleted: {
        // requestList()
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

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                RowLayout {
                    id: button_area
                    Layout.margins: 10
                    ColumnLayout.fillWidth: true

                    MButton {
                        variant: 'outlined'
                        text: qsTr("新建")
                        color: Color.text_secondary
                        ToolTip.text: qsTr("新建下载任务")
                        ToolTip.visible: hovered
                        ToolTip.timeout: 3000
                        ToolTip.delay: 0
                    }

                    Rectangle {
                        RowLayout.fillWidth: true
                    }
                }

                MDivider {
                    ColumnLayout.fillWidth: true
                }

                ScrollView {
                    ColumnLayout.fillWidth: true
                    ColumnLayout.fillHeight: true
                    clip: true
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    contentWidth: availableWidth
                    leftPadding: 10
                    topPadding: 10
                    rightPadding: 10
                    bottomPadding: 10

                    ColumnLayout {
//                        ColumnLayout.fillWidth: true
                        width: right_area.width
                        spacing: 0

                        MLinearProgress {
                            width: 300
                        }
                        Rectangle {
                            height: 20
                        }

                        MLinearProgress {
                            width: 300
                            variant: 'determinate'
                            value: 0.5
                            //variant: 'buffer'
                            //valueBuffer: 0.7
                        }

                        Rectangle {
                            height: 20
                        }
                        MLinearProgress {
                            width: 300
                            value: 0.5
                            variant: 'buffer'
                            valueBuffer: 0.7
                        }


                        MButton {
                            text: '打开'
                            onClicked: {
                                theDialog.open()
                            }
                        }
                    }
                }
            }
        }
    }

    MDialog {
        id: theDialog
        width: 200
        height: 200
        transitionComponent: MFade {
        }

        MTypography {
            text: '测试'
        }
    }

    MToast {
        id: toast
    }
}
