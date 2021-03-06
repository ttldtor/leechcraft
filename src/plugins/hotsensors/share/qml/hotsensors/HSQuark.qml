import QtQuick 1.1
import org.LC.common 1.0

Rectangle {
    id: rootRect

    property real itemSize: parent.quarkBaseSize
    property real length: sensorsView.count * itemSize + (addSensorButton.visible ? addSensorButton.height : 0)
    width: viewOrient == "vertical" ? itemSize : length
    height: viewOrient == "vertical" ? length : itemSize

    radius: 2

    color: "transparent"

    property variant tooltip

    Common { id: commonJS }

    Component.onCompleted: HS_plotManager.setContext(quarkContext)

    ActionButton {
        id: addSensorButton
        visible: quarkDisplayRoot.settingsMode
        anchors.bottom: viewOrient == "vertical" ? parent.bottom : undefined
        anchors.right: viewOrient == "vertical" ? undefined : parent.right
        anchors.horizontalCenter: viewOrient == "vertical" ? parent.horizontalCenter : undefined
        anchors.verticalCenter: viewOrient == "vertical" ? undefined : parent.verticalCenter
        width: parent.itemSize * 2 / 3
        height: parent.itemSize * 2 / 3

        actionIconURL: "image://ThemeIcons/list-add"

        onTriggered: commonJS.showTooltip(addSensorButton, function(x, y) { HS_plotManager.sensorUnhideListRequested(x, y, quarkProxy.getWinRect()) })
    }

    ListView {
        id: sensorsView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: viewOrient == "vertical" ? undefined : addSensorButton.left
        anchors.bottom: viewOrient == "vertical" ? addSensorButton.top : undefined
        boundsBehavior: Flickable.StopAtBounds

        model: HS_plotManager.getModel()

        orientation: viewOrient == "vertical" ? ListView.Vertical : ListView.Horizontal

        delegate: Item {
            id: delegateItem

            height: rootRect.itemSize
            width: rootRect.itemSize

            Image {
                id: sensorImage

                height: rootRect.itemSize
                width: rootRect.itemSize

                sourceSize.width: width
                sourceSize.height: height

                source: "data:," + rawSvg
            }

            Text {
                anchors.centerIn: parent
                text: lastTemp
                font.pixelSize: parent.height / 3
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onEntered: {
                    var global = commonJS.getTooltipPos(delegateItem);
                    var params = {
                        "x": global.x,
                        "y": global.y,
                        "existing": "ignore",
                        "svg": rawSvg,
                        "colorProxy": colorProxy,
                        "sensorName": sensorName
                    };
                    tooltip = quarkProxy.openWindow(sourceURL, "Tooltip.qml", params);
                    tooltip.svg = (function() { return rawSvg; });
                }
                onExited: if (tooltip != null) { tooltip.closeRequested(); tooltip = null; }
            }

            ListView.onRemove: if (tooltip != null) { tooltip.closeRequested(); tooltip = null; }

            ActionButton {
                id: removeButton

                visible: quarkDisplayRoot.settingsMode
                opacity: 0

                width: parent.width / 2
                height: parent.height / 2
                anchors.top: parent.top
                anchors.right: parent.right

                actionIconURL: "image://ThemeIcons/list-remove"
                transparentStyle: true

                onTriggered: HS_plotManager.hideSensor(sensorName)

                states: [
                    State {
                        name: "active"
                        when: quarkDisplayRoot.settingsMode
                        PropertyChanges { target: removeButton; opacity: 1 }
                    }
                ]

                transitions: [
                    Transition {
                        from: ""
                        to: "active"
                        reversible: true
                        PropertyAnimation { properties: "opacity"; duration: 200 }
                    }
                ]
            }
        }
    }
}
