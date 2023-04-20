import QtQuick 2.13
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.15
import "./styles"

// 原版
MButton {
    id: list_item
    // button模式下，传给button
//    property string color: Palatte.lightTextPrimary

    property string alignItems: 'center' // 'flex-start' 'center'
    property bool dense: false
    // disabled: false
    property bool disableGutters: false
    property bool divider: false
    property bool selected: false
    property bool button: false
    disableCursor: !button
    disableRipple: !button

    property var m_padding: {
        let ans = [Palatte.unit, 0, Palatte.unit, 0]

        if (dense) {
            ans = [Palatte.unit/2, 0, Palatte.unit/2, 0]
        }
        if (!disableGutters) {
            ans[1] = ans[3] = Palatte.unit*2
        }
        return ans
    }
    topPadding: m_padding[0]
    rightPadding: m_padding[1]
    bottomPadding: m_padding[2]
    leftPadding: m_padding[3]

    background: Rectangle {
        color: {
            if (list_item.selected) {
                return Palatte.lightActionSelected
            }

            if (button && list_item.hovered) {
                return Colors.alpha('#000000', 0.04)
            }

            return '#00000000'
        }

        Rectangle {
            visible: divider
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: Palatte.lightTextDivider
        }
    }
}

