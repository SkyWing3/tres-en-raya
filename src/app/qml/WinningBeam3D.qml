import QtQuick
import QtQuick3D
import QtQuick3D.Helpers

// Haz de luz que une las tres piezas de la línea ganadora.
Node {
    id: beam

    property vector3d from: Qt.vector3d(0, 0, 0)
    property vector3d to: Qt.vector3d(0, 0, 0)
    property color color: "white"
    property bool active: false
    property real progress: 0

    readonly property vector3d delta: to.minus(from)
    readonly property real length: delta.length() + 70

    visible: progress > 0
    position: from.plus(to).times(0.5)
    eulerRotation.y: Math.atan2(-delta.z, delta.x) * 180 / Math.PI

    onActiveChanged: {
        if (active)
            growAnimation.restart()
        else
            progress = 0
    }

    NumberAnimation {
        id: growAnimation
        target: beam; property: "progress"
        from: 0; to: 1
        duration: 550; easing.type: Easing.OutCubic
    }

    // CapsuleGeometry se genera a lo largo del eje X, que el nodo padre orienta.
    Model {
        scale: Qt.vector3d(beam.progress, 1, 1)
        castsShadows: false
        geometry: CapsuleGeometry {
            height: beam.length
            diameter: 9
        }
        materials: PrincipledMaterial {
            baseColor: beam.color
            lighting: PrincipledMaterial.NoLighting
            emissiveFactor: Qt.vector3d(beam.color.r * 2.2, beam.color.g * 2.2, beam.color.b * 2.2)
        }
    }
}
