import QtQuick
import QtQuick3D

// Una casilla del tablero con su pieza. Anima la caída de la pieza al jugar,
// su desaparición al empezar otra partida y una vista previa semitransparente
// de la pieza del siguiente jugador cuando el puntero está encima.
Node {
    id: cell

    required property int index
    required property string piece
    required property bool winning

    property string previewSymbol: ""
    property bool gameWon: false

    readonly property real restingHeight: 10 + 11
    property string shownSymbol: ""

    onPieceChanged: {
        if (piece !== "") {
            shownSymbol = piece
            removeAnimation.stop()
            dropAnimation.restart()
        } else if (shownSymbol !== "") {
            dropAnimation.stop()
            removeAnimation.restart()
        }
    }

    Tile3D {
        cellIndex: cell.index
        tint: {
            if (cell.winning)
                return Qt.alpha(Theme.pieceColor(cell.piece), 0.35)
            if (cell.previewSymbol !== "")
                return Qt.alpha(Theme.pieceColor(cell.previewSymbol), 0.22)
            return "transparent"
        }
        glow: cell.winning ? 0.35 : 0
    }

    Node {
        id: pieceHolder
        y: cell.restingHeight
        visible: cell.shownSymbol !== ""

        Piece3D {
            symbol: cell.shownSymbol
            highlighted: cell.winning
            // Las piezas que no forman la línea ganadora se apagan un poco.
            opacity: cell.gameWon && !cell.winning ? 0.55 : 1.0

            Behavior on opacity { NumberAnimation { duration: 300 } }
        }
    }

    Piece3D {
        id: ghost
        symbol: cell.previewSymbol !== "" ? cell.previewSymbol : "X"
        ghost: true
        visible: cell.previewSymbol !== "" && cell.piece === ""
        y: cell.restingHeight + 26

        SequentialAnimation on y {
            running: ghost.visible
            loops: Animation.Infinite
            NumberAnimation { to: cell.restingHeight + 36; duration: 700; easing.type: Easing.InOutSine }
            NumberAnimation { to: cell.restingHeight + 26; duration: 700; easing.type: Easing.InOutSine }
        }
    }

    ParallelAnimation {
        id: dropAnimation
        NumberAnimation {
            target: pieceHolder; property: "y"
            from: cell.restingHeight + 240; to: cell.restingHeight
            duration: 700; easing.type: Easing.OutBounce
        }
        NumberAnimation {
            target: pieceHolder; property: "eulerRotation.y"
            from: -270; to: 0
            duration: 650; easing.type: Easing.OutCubic
        }
        Vector3dAnimation {
            target: pieceHolder; property: "scale"
            from: Qt.vector3d(0.5, 0.5, 0.5); to: Qt.vector3d(1, 1, 1)
            duration: 400; easing.type: Easing.OutBack
        }
    }

    SequentialAnimation {
        id: removeAnimation
        Vector3dAnimation {
            target: pieceHolder; property: "scale"
            to: Qt.vector3d(0.01, 0.01, 0.01)
            duration: 260; easing.type: Easing.InBack
        }
        ScriptAction {
            script: {
                cell.shownSymbol = ""
                pieceHolder.scale = Qt.vector3d(1, 1, 1)
            }
        }
    }
}
