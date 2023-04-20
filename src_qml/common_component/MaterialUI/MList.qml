import QtQuick 2.13
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "./styles"

Column {
    id: root
    property bool disablePadding: false
    property bool dense: false
    // property var subheader: null

    topPadding: {
        if (disablePadding) {
            return 0
        }
        let ans = Palatte.unit
        if (dense) {
            ans = Palatte.unit
        }
        return ans
    }
    bottomPadding: root.topPadding
}
