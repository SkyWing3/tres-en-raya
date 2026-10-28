pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TresEnRaya.Presentation

// Panel lateral de la partida: turno, marcador y jugada por coordenadas.
// Usa únicamente controles estándar para que el estilo nativo los dibuje.
ScrollView {
    id: root

    required property GameController game

    signal resetScoreRequested()

    readonly property bool won: game.status === GameController.Status.Won
    readonly property bool draw: game.status === GameController.Status.Draw

    contentWidth: availableWidth
    padding: 16

    ColumnLayout {
        width: root.availableWidth
        spacing: 14

        GroupBox {
            title: qsTr("Turno")
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 12

                PiecePreview3D {
                    symbol: root.won ? root.game.winner : root.game.nextPlayer
                    highlighted: root.won
                    visible: !root.draw
                    Layout.preferredWidth: 72
                    Layout.preferredHeight: 72
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Label {
                        text: root.won ? qsTr("¡Ganó %1!").arg(root.game.winner)
                                       : root.draw ? qsTr("Empate")
                                                   : qsTr("Juega %1").arg(root.game.nextPlayer)
                        font.pixelSize: 22
                        font.weight: Font.DemiBold
                        color: root.draw ? palette.windowText
                                         : Theme.pieceColor(root.won ? root.game.winner : root.game.nextPlayer)
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                        opacity: 0.7
                        text: root.game.gameOver ? qsTr("Empieza un juego nuevo para continuar.")
                                                 : qsTr("Haz clic en una casilla del tablero.")
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Marcador")
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2
                rowSpacing: 6

                Label { text: qsTr("Victorias de X"); color: Theme.xColor; Layout.fillWidth: true }
                Label { text: root.game.xWins; font.bold: true; font.features: { "tnum": 1 } }

                Label { text: qsTr("Victorias de O"); color: Theme.oColor; Layout.fillWidth: true }
                Label { text: root.game.oWins; font.bold: true; font.features: { "tnum": 1 } }

                Label { text: qsTr("Empates"); Layout.fillWidth: true }
                Label { text: root.game.draws; font.bold: true; font.features: { "tnum": 1 } }
            }
        }

        GroupBox {
            title: qsTr("Jugar por coordenadas")
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    opacity: 0.7
                    text: qsTr("Las casillas válidas van de %1 a %2 en cada eje. Prueba un valor fuera de rango para ver la excepción.")
                          .arg(root.game.minCoordinate).arg(root.game.maxCoordinate)
                }

                GridLayout {
                    columns: 2
                    Layout.fillWidth: true

                    Label { text: qsTr("Eje X (columna)") }
                    SpinBox {
                        id: xInput
                        from: -1
                        to: 5
                        value: 2
                        editable: true
                        Layout.fillWidth: true
                        Accessible.name: qsTr("Coordenada X")
                    }

                    Label { text: qsTr("Eje Y (fila)") }
                    SpinBox {
                        id: yInput
                        from: -1
                        to: 5
                        value: 2
                        editable: true
                        Layout.fillWidth: true
                        Accessible.name: qsTr("Coordenada Y")
                    }
                }

                Button {
                    text: qsTr("Colocar pieza")
                    Layout.alignment: Qt.AlignRight
                    onClicked: root.game.play(xInput.value, yInput.value)
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: qsTr("Nuevo juego")
                highlighted: root.game.gameOver
                Layout.fillWidth: true
                onClicked: root.game.newGame()
            }

            Button {
                text: qsTr("Reiniciar marcador…")
                Layout.fillWidth: true
                onClicked: root.resetScoreRequested()
            }
        }

        Label {
            Layout.fillWidth: true
            Layout.topMargin: 4
            wrapMode: Text.Wrap
            opacity: 0.55
            font.pixelSize: 11
            text: qsTr("Arrastra para girar el tablero · Rueda o pellizco para acercar · Flechas y Enter para jugar con el teclado")
        }
    }
}
