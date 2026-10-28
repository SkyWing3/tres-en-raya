pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TresEnRaya.Presentation

// Página del juego: tablero 3D a la izquierda y panel de control a la derecha.
Item {
    id: page

    required property GameController game

    signal resetScoreRequested()

    function resetCamera() {
        board.resetCamera()
    }

    function focusBoard() {
        board.forceActiveFocus()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            GameBoard3D {
                id: board
                anchors.fill: parent
                game: page.game
                focus: true
            }

            NoticeBanner {
                id: notice
                anchors.top: parent.top
                anchors.topMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(implicitWidth, parent.width - 32)
            }

            Label {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter
                text: page.game.status === GameController.Status.Won
                      ? qsTr("¡%1 conectó tres en raya!").arg(page.game.winner)
                      : page.game.status === GameController.Status.Draw
                        ? qsTr("Tablero lleno: empate")
                        : ""
                visible: text !== ""
                color: Theme.sceneText
                font.pixelSize: 20
                font.weight: Font.DemiBold
            }
        }

        Rectangle {
            Layout.fillHeight: true
            implicitWidth: 1
            color: palette.mid
            opacity: 0.4
        }

        GameInspector {
            Layout.fillHeight: true
            Layout.preferredWidth: 310
            game: page.game
            onResetScoreRequested: page.resetScoreRequested()
        }
    }

    Connections {
        target: page.game
        function onMoveRejected(reason: string) {
            notice.show(reason)
        }
        function onStateChanged() {
            notice.hide()
        }
    }
}
