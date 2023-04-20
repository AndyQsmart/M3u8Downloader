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
            RowLayout.fillHeight: true
            RowLayout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                RowLayout {
                    id: button_area
                    Layout.margins: 10
                    ColumnLayout.fillWidth: true

                    MFileButton {
                        variant: 'outlined'
                        text: qsTr("选择文件")
                        color: Color.text_secondary
                        onChange: function(file_list) {
                            onSelectFile(file_list[0])
                        }
                        ToolTip.text: qsTr("选择Word文件")
                        ToolTip.visible: hovered
                        ToolTip.timeout: 3000
                        ToolTip.delay: 0
                    }

                    // MButton {
                    //     variant: 'outlined'
                    //     text: qsTr("测试")
                    //     color: Color.text_secondary
                    //     onClicked: {
                    //        WordTools.test()
                    //     }
                    // }

                    Rectangle {
                        RowLayout.fillWidth: true
                    }

                    MSelect {
                        id: out_select
                        model: ['输出到文件', '直接显示结果']
                    }

                    MSelect {
                        id: out_select2
                        variant: 'outlined'
                        color: 'primary'
                        model: ['输出到文件', '直接显示结果']
                    }

                    MSelect {
                        id: out_select3
                        variant: 'filled'
                        model: ['输出到文件', '直接显示结果']
                    }
                }

                MDivider {
                    ColumnLayout.fillWidth: true
                }

                ScrollView {
                    ColumnLayout.fillWidth: true
                    ColumnLayout.fillHeight: true
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        MTypography {
                            variant: 'h1'
                            text: 'h1. Heading'
                        }
                        MTypography {
                            variant: 'h2'
                            text: 'h2. Heading'
                        }
                        MTypography {
                            variant: 'h3'
                            text: 'h3. Heading'
                        }
                        MTypography {
                            variant: 'h4'
                            text: 'h4. Heading'
                        }
                        MTypography {
                            variant: 'h5'
                            text: 'h5. Heading'
                        }
                        MTypography {
                            variant: 'h6'
                            text: 'h6. Heading'
                        }
                        MTypography {
                            variant: 'subtitle1'
                            text: 'subtitle1. Lorem ipsum dolor sit amet, consectetur adipisicing elit. Quos blanditiis tenetur'
                        }
                        MTypography {
                            variant: 'subtitle2'
                            text: 'subtitle2. Lorem ipsum dolor sit amet, consectetur adipisicing elit. Quos blanditiis tenetur'
                        }
                        MTypography {
                            variant: 'body1'
                            text: 'body1. Lorem ipsum dolor sit amet, consectetur adipisicing elit. Quos blanditiis tenetur unde suscipit, quam beatae rerum inventore consectetur, neque doloribus, cupiditate numquam dignissimos laborum fugiat deleniti? Eum quasi quidem quibusdam.'
                        }
                        MTypography {
                            variant: 'body2'
                            text: 'body2. Lorem ipsum dolor sit amet, consectetur adipisicing elit. Quos blanditiis tenetur unde suscipit, quam beatae rerum inventore consectetur, neque doloribus, cupiditate numquam dignissimos laborum fugiat deleniti? Eum quasi quidem quibusdam.'
                        }
                        MTypography {
                            variant: 'button'
                            text: 'BUTTON TEXT'
                        }
                        MTypography {
                            variant: 'caption'
                            text: 'caption text'
                        }
                        MTypography {
                            variant: 'overline'
                            text: 'OVERLINE TEXT'
                        }
//                        Rectangle {
//                            RowLayout.fillHeight: true
//                        }
                    }
                }
            }
        }
    }

    MToast {
        id: toast
    }
}
