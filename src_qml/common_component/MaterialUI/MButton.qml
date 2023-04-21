import QtQuick 2.13
import "./styles"
import "./colors"

// 原版
MButtonBase {
    id: button
    property string color: 'default' // 'default' 'inherit' 'primary' 'secondary'
    disabled: false
    property string size: 'medium' // 'large' 'medium' 'small'
    // property bool fullWidth: false
    // disableRipple: false
    property bool disableElevation: false
    property string variant: 'text' // contained、outlined、text、default: text

    implicitWidth: 64

    property var m_padding: {
        let ans = [Palatte.unit, Palatte.unit*2, Palatte.unit, Palatte.unit*2]

        switch (variant) {
            case 'contained': {
                ans = [6, 16, 6, 16]
                switch (size) {
                    case 'small': {
                        ans = [4, 10, 4, 10]
                        break
                    }
                    case 'large': {
                        ans = [8, 22, 8, 22]
                        break
                    }
                }
                break
            }
            case 'outlined': {
                ans = [5, 15, 5, 15]
                switch (size) {
                    case 'small': {
                        ans = [3, 9, 3, 9]
                        break
                    }
                    case 'large': {
                        ans = [7, 21, 7, 21]
                        break
                    }
                }
                break
            }
            default: {
                ans = [6, 8, 6, 8]
                switch (size) {
                    case 'small': {
                        ans = [4, 5, 4, 5]
                        break
                    }
                    case 'large': {
                        ans = [8, 11, 8, 11]
                        break
                    }
                }
            }
        }

        return ans
    }
    topPadding: m_padding[0]
    rightPadding: m_padding[1]
    bottomPadding: m_padding[2]
    leftPadding: m_padding[3]

    textColor: {
        if (disabled) {
            return Palatte.lightActionDisabled
        }

        switch (variant) {
            case 'contained': {
                return color === 'primary' || color === 'secondary' ? Colors.commonWhite : Colors.commonBlack
            }
            case 'outlined': {
                return Palatte.string2Color(color, Colors.commonBlack)
            }
            default: {
                return Palatte.string2Color(color, Colors.commonBlack)
            }
        }
    }

    background: Rectangle {
        border.width: {
            if (variant == 'outlined') {
                return 1
            }
            return 0
        }
        border.color: {
            if (disabled) {
                return Palatte.lightActionDisabled
            }

            let ans_color = Palatte.string2Color(button.color, null)
            if (ans_color) {
                return Colors.alpha(ans_color, 0.5)
            }
            else {
                return Colors.alpha('#000000', 0.23)
            }
        }
        radius: 4
        color: {
            let ans = ''

            if (button.hovered) {
                ans = Colors.alpha('#000000', 0.04)
                switch (variant) {
                    case 'contained': {
                        ans = Palatte.string2ColorDark(button.color, Grey.a100)
                        break
                    }
                    case 'outlined': {
                        ans = Colors.alpha(Palatte.string2Color(button.color, '#000000'), 0.04)
                        break
                    }
                    default: {
                        ans = Colors.alpha(Palatte.string2Color(button.color, '#000000'), 0.04)
                    }
                }
            }
            else {
                switch (variant) {
                    case 'contained': {
                        ans = button.color ? Palatte.string2Color(button.color, Grey._300) : Grey._300
                        break
                    }
                    case 'outlined': {
                        ans = Colors.commonTransparent
                        break
                    }
                    default: {
                        ans = Colors.commonTransparent
                    }
                }
            }

            return ans
        }

        layer.enabled: variant === 'contained' && !disabled && !disableElevation
        layer.effect: MShadow {
            elevation: 2
        }
    }
}
