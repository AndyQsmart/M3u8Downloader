import QtQuick 2.13
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.11
import "../../../common_component/MaterialUI"

ColumnLayout {
    function showText(text_str) {
        default_view.visible = false
        scroll_container.visible = true
        text_view.text = `<pre style="white-space: pre-wrap;word-wrap: break-all;">${text_str}<pre>`
    }

    MTypography {
        id: default_view
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        textColor: 'textSecondary'
        text: qsTr("请选择docx文件进行处理。")
    }

    ScrollView {
        id: scroll_container
        visible: false
        anchors.fill: parent
        clip: true
        contentWidth: scroll_container.width

        TextEdit {
            id: text_view
            width: scroll_container.width
            topPadding: 10
            rightPadding: 10
            bottomPadding: 10
            leftPadding: 10
            text: qsTr("")
            font.pixelSize: 16
            wrapMode: Text.WrapAnywhere
            textFormat: Text.RichText
            selectByKeyboard: true
            selectByMouse: true
            readOnly: true
        }
    }
}
