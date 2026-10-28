pragma ComponentBehavior: Bound

import QtQuick
import QtQuick3D
import QtQuick3D.Helpers

// Pieza 3D del juego:
//  - 'X': dos barras redondeadas cruzadas a 45 grados.
//  - 'O': un anillo (toroide) acostado sobre el tablero.
Node {
    id: piece

    property string symbol: "X"
    property bool highlighted: false
    property bool ghost: false
    property real glow: 0

    readonly property bool isO: symbol === "O"
    readonly property color color: Theme.pieceColor(symbol)

    PrincipledMaterial {
        id: material
        baseColor: piece.color
        roughness: 0.25
        metalness: 0.05
        clearcoatAmount: 0.7
        clearcoatRoughnessAmount: 0.08
        emissiveFactor: Qt.vector3d(piece.color.r * piece.glow,
                                    piece.color.g * piece.glow,
                                    piece.color.b * piece.glow)
        opacity: piece.ghost ? 0.35 : 1.0
        alphaMode: piece.ghost ? PrincipledMaterial.Blend : PrincipledMaterial.Opaque
    }

    component Bar: Model {
        // CapsuleGeometry se genera a lo largo del eje X.
        geometry: CapsuleGeometry {
            height: 64
            diameter: 22
        }
        materials: [material]
        castsShadows: !piece.ghost
        receivesShadows: true
    }

    Node {
        visible: !piece.isO
        eulerRotation.y: 45

        Bar {}
        Bar { eulerRotation.y: 90 }
    }

    Model {
        visible: piece.isO
        // TorusGeometry se genera sobre el plano XZ, es decir, ya acostado.
        geometry: TorusGeometry {
            radius: 30
            tubeRadius: 11
            rings: 48
            segments: 24
        }
        materials: [material]
        castsShadows: !piece.ghost
        receivesShadows: true
    }

    SequentialAnimation on glow {
        running: piece.highlighted
        loops: Animation.Infinite
        alwaysRunToEnd: false
        NumberAnimation { from: 0.15; to: 0.9; duration: 650; easing.type: Easing.InOutSine }
        NumberAnimation { from: 0.9; to: 0.15; duration: 650; easing.type: Easing.InOutSine }
        onStopped: piece.glow = 0
    }
}
