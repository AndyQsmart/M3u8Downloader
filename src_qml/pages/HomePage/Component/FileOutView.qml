import QtQuick 2.13
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.11
import "../../../common_component/MaterialUI"

ColumnLayout {
    MTypography {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        textColor: 'textSecondary'
        text: qsTr("请选择docx文件进行处理。\n结果输出到文件所在文件夹下。")
    }
}
