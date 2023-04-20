pragma Singleton

import QtQuick 2.0

QtObject {
    readonly property string commonBlack: '#000000'
    readonly property string commonWhite: '#ffffff'
    readonly property string commonTransparent: '#00000000'

    function alpha(color, value) {
        let simple_color = ''
        if (color.length === 9) {
            simple_color = color.substring(3, 9)
        }
        else {
            simple_color = color.substring(1, 7)
        }

        let alpha_str = parseInt(255*value).toString(16)
        if (alpha_str.length < 2) {
            alpha_str = '0'+alpha_str
        }

        return `#${alpha_str}${simple_color}`
    }
}
