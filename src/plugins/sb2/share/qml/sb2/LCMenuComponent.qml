import QtQuick 1.1
import org.LC.common 1.0

Rectangle {
    id: rootRect

    width: parent.quarkBaseSize
    height: parent.quarkBaseSize

    radius: 2

    color: "transparent"

    ActionButton {
        width: parent.width
        height: parent.width

        onTriggered: SB2_menuComponentProxy.execMenu()
        onHeld: SB2_menuComponentProxy.execMenu()

        actionIconURL: SB2_menuComponentLCIcon
        textTooltip: SB2_menuTooltipString
    }
}
