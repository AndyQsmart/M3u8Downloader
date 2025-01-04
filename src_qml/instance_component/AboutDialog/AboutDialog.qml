import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "../../common_component/MaterialUI"
import "../../common_component/MaterialUI/styles"
import "../../common_qml"

MDialog {
    id: root
    width: 600
    height: 400
    padding: 20
    property string currentTab: "about"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: 20
            spacing: 0

            MButtonGroup {
                variant: "outlined"

                MButton {
                    text: "关于"
                    textColor: root.currentTab == "about" ? Palette.primaryMain : Palette.lightTextPrimary

                    onClicked: {
                        root.currentTab = "about"
                    }
                }

                MButton {
                    text: "许可"
                    textColor: root.currentTab == "license" ? Palette.primaryMain : Palette.lightTextPrimary

                    onClicked: {
                        root.currentTab = "license"
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            MouseArea {
                width: 32
                height: 32
                cursorShape: Qt.PointingHandCursor

                MSvgIcon {
                    name: 'Close'
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                onClicked: {
                    root.close()
                }
            }
        }

        ColumnLayout {
            visible: root.currentTab === "about"
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                MTypography {
                    Layout.fillWidth: true
                    gutterBottom: true
                    text: qsTr('M3u8Downloader用于下载m3u8视频。')
                }

                MTypography {
                    Layout.fillWidth: true
                    text: qsTr('M3u8Downloader使用了Qt5进行开发，包含了aria2程序。')
                }

                Item {
                    Layout.fillHeight: true
                }
            }

            MTypography {
                topPadding: 10
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                text: qsTr('版本号：ver'+StringUtil.version)
                color: Color.text_secondary
                variant: 'caption'
            }

            MTypography {
                topPadding: 10
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                text: qsTr('Copyright © 2023')
                color: Color.text_secondary
                variant: 'caption'
            }
        }

        MOverflowYBox {
            id: license_root
            visible: root.currentTab === "license"
            contentWidth: width
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                width: license_root.width

                MTypography {
                    Layout.fillWidth: true
                    text: `This software uses the Qt5 library, which is licensed under the LGPL v3 License.
The full text of the LGPL v3 License can be found at:`
                }

                MTypography {
                    Layout.fillWidth: true
                    gutterBottom: true
                    text: "<u>https://www.gnu.org/licenses/lgpl-3.0.html</u>"
                    textColor: "primary"

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onPressed: {
                            Qt.openUrlExternally("https://www.gnu.org/licenses/lgpl-3.0.html")
                        }
                    }
                }

                MTypography {
                    Layout.fillWidth: true
                    text: "The Qt5 library source code can be obtained at:"
                }

                MTypography {
                    Layout.fillWidth: true
                    gutterBottom: true
                    text: "<u>https://github.com/qt/qt5</u>"
                    textColor: "primary"

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onPressed: {
                            Qt.openUrlExternally("https://github.com/qt/qt5")
                        }
                    }
                }

                MTypography {
                    Layout.fillWidth: true
                    gutterBottom: true
                    text: `This software links to the Qt5 library, which is licensed under the LGPL v3 License.
You have the freedom to replace Qt5 with another version of your choice.`
                }

                MTypography {
                    Layout.fillWidth: true
                    gutterBottom: true
                    text: `This software uses the unmodified Qt5 library, which is licensed under the LGPL v3 License.`
                }
            }
        }



    }
}
