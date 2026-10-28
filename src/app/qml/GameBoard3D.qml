pragma ComponentBehavior: Bound

import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import TresEnRaya.Presentation

// Tablero de Tres en Raya en 3D.
//  - Clic: coloca la pieza del jugador en turno.
//  - Arrastrar: gira la cámara alrededor del tablero.
//  - Rueda o pellizco: acerca o aleja la cámara.
//  - Flechas + Enter/Espacio: juega con el teclado.
View3D {
    id: root

    required property GameController game

    readonly property real cellPitch: 122
    property int hoveredCell: -1
    property int keyboardCell: 4
    property bool keyboardMode: false

    readonly property int activeCell: keyboardMode && activeFocus ? keyboardCell : hoveredCell

    function resetCamera() {
        cameraResetAnimation.restart()
    }

    function cellPosition(cellIndex: int): vector3d {
        return Qt.vector3d((cellIndex % 3 - 1) * cellPitch, 0, (Math.floor(cellIndex / 3) - 1) * cellPitch)
    }

    function cellAt(position: point): int {
        const tile = root.pick(position.x, position.y).objectHit as Tile3D
        return tile ? tile.cellIndex : -1
    }

    function moveKeyboardCell(dx: int, dy: int) {
        const column = Math.max(0, Math.min(2, keyboardCell % 3 + dx))
        const row = Math.max(0, Math.min(2, Math.floor(keyboardCell / 3) + dy))
        keyboardCell = row * 3 + column
    }

    activeFocusOnTab: true
    camera: boardCamera
    Accessible.role: Accessible.Table
    Accessible.name: qsTr("Tablero de Tres en Raya")
    Accessible.description: qsTr("Usa las flechas para elegir una casilla y Enter para jugar")

    environment: SceneEnvironment {
        backgroundMode: SceneEnvironment.Color
        clearColor: Theme.sceneBackground
        antialiasingMode: SceneEnvironment.MSAA
        antialiasingQuality: SceneEnvironment.High
        specularAAEnabled: true

        // Iluminación ambiental basada en un cielo procedural (sin archivos HDR).
        lightProbe: Texture {
            textureData: ProceduralSkyTextureData {
                skyTopColor: Theme.dark ? "#5A6E92" : "#A9C6EE"
                skyHorizonColor: Theme.dark ? "#8A8F9C" : "#F2EEE6"
                groundHorizonColor: Theme.dark ? "#4A4D55" : "#E3DCD0"
                groundBottomColor: Theme.dark ? "#22242A" : "#8A8174"
                sunColor: "#FFF4E0"
                sunEnergy: 1.2
            }
        }
        probeExposure: Theme.dark ? 1.0 : 0.9

        // La niebla funde el suelo con el fondo para que no se vea su borde.
        fog: Fog {
            enabled: true
            color: Theme.sceneBackground
            depthEnabled: true
            depthNear: 1700
            depthFar: 4200
        }
    }

    // --- Cámara orbital ---------------------------------------------------------

    Node {
        id: cameraRig

        readonly property real defaultYaw: 0
        readonly property real defaultPitch: -52
        readonly property real defaultDistance: 860

        property real yaw: -38
        property real pitch: -70
        property real distance: 1500

        eulerRotation: Qt.vector3d(pitch, yaw, 0)

        PerspectiveCamera {
            id: boardCamera
            z: cameraRig.distance
            fieldOfView: 34
            clipNear: 5
            clipFar: 8000
        }
    }

    ParallelAnimation {
        id: cameraResetAnimation
        running: true // También sirve como animación de entrada.
        NumberAnimation { target: cameraRig; property: "yaw"; to: cameraRig.defaultYaw; duration: 1100; easing.type: Easing.OutCubic }
        NumberAnimation { target: cameraRig; property: "pitch"; to: cameraRig.defaultPitch; duration: 1100; easing.type: Easing.OutCubic }
        NumberAnimation { target: cameraRig; property: "distance"; to: cameraRig.defaultDistance; duration: 1100; easing.type: Easing.OutCubic }
    }

    // --- Luces --------------------------------------------------------------------

    DirectionalLight {
        eulerRotation: Qt.vector3d(-58, -32, 0)
        color: "#FFF6EA"
        brightness: Theme.dark ? 1.35 : 1.1
        castsShadow: true
        shadowMapQuality: Light.ShadowMapQualityVeryHigh
        softShadowQuality: Light.PCF16
        pcfFactor: 6
        shadowFactor: 55
        shadowMapFar: 2600
    }

    DirectionalLight {
        eulerRotation: Qt.vector3d(-25, 150, 0)
        color: "#CFE0FF"
        brightness: 0.4
    }

    // --- Escena -------------------------------------------------------------------

    Model {
        id: floor
        source: "#Rectangle"
        eulerRotation.x: -90
        y: -34
        scale: Qt.vector3d(90, 90, 1)
        receivesShadows: true
        materials: PrincipledMaterial {
            baseColor: Theme.floorColor
            roughness: 0.92
        }
    }

    Model {
        id: boardBase
        source: "#Cube"
        y: -16
        scale: Qt.vector3d(4.0, 0.32, 4.0)
        castsShadows: true
        receivesShadows: true
        materials: PrincipledMaterial {
            baseColor: Theme.boardColor
            roughness: 0.5
            clearcoatAmount: 0.5
            clearcoatRoughnessAmount: 0.2
        }
    }

    Repeater3D {
        model: root.game.board

        delegate: BoardCell3D {
            position: root.cellPosition(index)
            gameWon: root.game.status === GameController.Status.Won
            previewSymbol: root.activeCell === index && piece === "" && !root.game.gameOver
                           ? root.game.nextPlayer : ""
        }
    }

    WinningBeam3D {
        readonly property list<int> cells: root.game.winningCells
        active: cells.length === 3
        from: active ? root.cellPosition(cells[0]).plus(Qt.vector3d(0, 21, 0)) : Qt.vector3d(0, 0, 0)
        to: active ? root.cellPosition(cells[2]).plus(Qt.vector3d(0, 21, 0)) : Qt.vector3d(0, 0, 0)
        color: Theme.pieceColor(root.game.winner)
    }

    // --- Interacción ---------------------------------------------------------------

    TapHandler {
        acceptedButtons: Qt.LeftButton
        onTapped: (eventPoint) => {
            root.forceActiveFocus()
            root.keyboardMode = false
            const cell = root.cellAt(eventPoint.position)
            if (cell >= 0)
                root.game.playCell(cell)
        }
    }

    HoverHandler {
        id: hover
        onPointChanged: root.hoveredCell = hovered ? root.cellAt(point.position) : -1
        onHoveredChanged: {
            if (!hovered)
                root.hoveredCell = -1
        }
        cursorShape: root.hoveredCell >= 0 && !root.game.gameOver ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    DragHandler {
        id: orbit
        target: null
        property vector2d previous: Qt.vector2d(0, 0)
        onActiveChanged: previous = Qt.vector2d(0, 0)
        onActiveTranslationChanged: {
            const delta = activeTranslation.minus(previous)
            previous = activeTranslation
            cameraResetAnimation.stop()
            cameraRig.yaw -= delta.x * 0.35
            cameraRig.pitch = Math.max(-86, Math.min(-18, cameraRig.pitch - delta.y * 0.25))
        }
    }

    WheelHandler {
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onWheel: (event) => {
            cameraResetAnimation.stop()
            cameraRig.distance = Math.max(620, Math.min(1800, cameraRig.distance - event.angleDelta.y * 0.6))
        }
    }

    PinchHandler {
        target: null
        property real startDistance: cameraRig.distance
        onActiveChanged: startDistance = cameraRig.distance
        onActiveScaleChanged: cameraRig.distance = Math.max(620, Math.min(1800, startDistance / activeScale))
    }

    Keys.onPressed: (event) => {
        switch (event.key) {
        case Qt.Key_Left: moveKeyboardCell(-1, 0); break
        case Qt.Key_Right: moveKeyboardCell(1, 0); break
        case Qt.Key_Up: moveKeyboardCell(0, -1); break
        case Qt.Key_Down: moveKeyboardCell(0, 1); break
        case Qt.Key_Return:
        case Qt.Key_Enter:
        case Qt.Key_Space:
            root.game.playCell(keyboardCell)
            break
        default:
            return
        }
        keyboardMode = true
        event.accepted = true
    }
}
