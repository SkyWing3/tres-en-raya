import QtQuick
import QtQuick3D

// Casilla del tablero. Es el único objeto "pickable" de la escena: al hacer
// clic, GameBoard3D obtiene el cellIndex de la casilla bajo el puntero.
Model {
    id: tile

    required property int cellIndex
    property color tint: "transparent"
    property real glow: 0

    readonly property real size: 108
    readonly property real thickness: 10

    source: "#Cube"
    pickable: true
    receivesShadows: true
    castsShadows: true
    scale: Qt.vector3d(size / 100, thickness / 100, size / 100)
    y: thickness / 2

    materials: PrincipledMaterial {
        baseColor: Qt.tint(Theme.tileColor, tile.tint)
        roughness: 0.4
        clearcoatAmount: 0.4
        emissiveFactor: Qt.vector3d(tile.tint.r * tile.glow,
                                    tile.tint.g * tile.glow,
                                    tile.tint.b * tile.glow)

        Behavior on baseColor { ColorAnimation { duration: 160 } }
    }
}
