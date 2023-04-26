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
    id: container
    x: 0
    y: 0
    padding: 0

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

                        onClicked: {
                            create_dialog.open()
                        }
                    }

                    Rectangle {
                        RowLayout.fillWidth: true
                    }
                }

                MDivider {
                    ColumnLayout.fillWidth: true
                }

                RowLayout {
                    MOverflowYBox {
                        RowLayout.fillWidth: true
                        RowLayout.fillHeight: true
                        padding: 10

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

                            MChip {
                                //                            size: 'small'
                                clickable: true
                                label: '测试1'
                                onClicked: {
                                    console.log('click')
                                }
                                deletable: true
                                onDeleted: {
                                    console.log('ondelete1')
                                }
                            }

                            Rectangle {
                                height: 20
                            }

                            MChip {
                                color: 'primary'
                                label: '测试1'
                                deleteIcon: Rectangle {
                                    border.width: 1
                                    border.color: 'red'
                                    width: 18
                                    height: 18
                                    radius: 12

                                    MIcon {
                                        anchors.centerIn: parent
                                        name: 'download'
                                    }
                                }
                                deletable: true
                                onDeleted: {
                                    console.log('ondelete1')
                                }
                            }

                            Rectangle {
                                height: 20
                            }

                            MChip {
                                variant: 'outlined'
                                color: 'primary'
                                label: '测试1'
                                //                            size: 'small'
                                avatar: Rectangle {
                                    border.width: 1
                                    border.color: 'red'
                                    width: 18
                                    height: 18
                                    radius: 12

                                    MIcon {
                                        anchors.centerIn: parent
                                        name: 'download'
                                    }
                                }
                            }

                            Rectangle {
                                height: 20
                            }

                            MChip {
                                size: 'small'
                                variant: 'outlined'
                                label: '测试1'
                            }

                            MButton {
                                text: '打开'
                                onClicked: {
                                    theDialog.open()
                                }
                            }
                        }
                    }
                    Rectangle {
                        width: 1
                        RowLayout.fillHeight: true
                        color: '#dddddd'
                    }

                    MOverflowYBox {
                        width: 350
                        RowLayout.fillHeight: true
                        padding: 10
                    }
                }
            }
        }
    }

    CreateDialog {
        id: create_dialog
    }

    MToast {
        id: toast
    }
}
