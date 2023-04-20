import QtQuick 2.13
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "../../common_component/MaterialUI"

Pane {
    id: container
    x: 0
    y: 0
    padding: 10

    ScrollView {
        id: scroll_view
        anchors.fill: parent
        contentWidth: scroll_view.width

        ColumnLayout {
            id: main_area
            width: scroll_view.width
            spacing: 0

            RowLayout {
                ColumnLayout.fillWidth: true

                MTypography {
                    RowLayout.fillWidth: true
                    variant: 'h1'
                    text: 'h1. Heading'
                }
            }

            Pane {
                background: Rectangle {
                    color: '#55000000'
                }
                padding: 10
                width: 600

                Pane {
                    background: Rectangle {
                        color: '#FFFFFF'
                    }

                    MList {
                        MListItem {
                            width: 100
                            selected: true
                            text: '测试'
                            divider: true
                        }
                        MListItem {
                            width: 100
                            text: '测试'
                            divider: true
                        }
                    }
                }
            }

            Pane {
                background: Rectangle {
                    color: '#55000000'
                }
                padding: 10
                width: 600

                Pane {
                    background: Rectangle {
                        color: '#FFFFFF'
                    }

                    MList {
                        MListItem {
                            width: 100
                            button: true
                            MTypography {
                                text: '测试'
                            }
                        }
                        MListItem {
                            width: 100
                            button: true
                            MTypography {
                                text: '测试'
                            }
                        }
                    }
                }
            }
        }
    }
}
