import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Aviso no modal que aparece unos segundos (por ejemplo, una jugada inválida).
Rectangle {
    id: banner

    property color accent: Theme.warningColor
    property int timeout: 3500

    function show(message: string) {
        label.text = message
        state = "shown"
        hideTimer.restart()
    }

    function hide() {
        state = ""
    }

    implicitWidth: Math.min(560, row.implicitWidth + 28)
    implicitHeight: row.implicitHeight + 18
    radius: 10
    color: Qt.alpha(palette.window, 0.94)
    border.color: Qt.alpha(accent, 0.6)
    border.width: 1
    opacity: 0
    visible: opacity > 0
    scale: 0.96

    Accessible.role: Accessible.AlertMessage
    Accessible.name: label.text

    states: State {
        name: "shown"
        PropertyChanges { banner.opacity: 1; banner.scale: 1 }
    }

    transitions: Transition {
        NumberAnimation { properties: "opacity,scale"; duration: 180; easing.type: Easing.OutCubic }
    }

    Timer {
        id: hideTimer
        interval: banner.timeout
        onTriggered: banner.hide()
    }

    RowLayout {
        id: row
        anchors.centerIn: parent
        width: Math.min(implicitWidth, banner.width - 28)
        spacing: 10

        Rectangle {
            Layout.alignment: Qt.AlignVCenter
            implicitWidth: 22
            implicitHeight: 22
            radius: 11
            color: banner.accent

            Label {
                anchors.centerIn: parent
                text: "!"
                color: "white"
                font.bold: true
            }
        }

        Label {
            id: label
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            color: palette.windowText
        }
    }

    TapHandler {
        onTapped: banner.hide()
    }
}
