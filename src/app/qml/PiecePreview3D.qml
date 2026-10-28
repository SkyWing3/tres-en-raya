import QtQuick
import QtQuick3D

// Pequeña vista 3D que muestra una pieza girando sobre sí misma.
View3D {
    id: root

    property string symbol: "X"
    property bool highlighted: false

    implicitWidth: 84
    implicitHeight: 84
    camera: previewCamera

    environment: SceneEnvironment {
        backgroundMode: SceneEnvironment.Transparent
        antialiasingMode: SceneEnvironment.MSAA
        antialiasingQuality: SceneEnvironment.High
    }

    PerspectiveCamera {
        id: previewCamera
        position: Qt.vector3d(0, 120, 150)
        eulerRotation.x: -38
        fieldOfView: 40
    }

    DirectionalLight {
        eulerRotation: Qt.vector3d(-45, -30, 0)
        brightness: 1.4
    }

    DirectionalLight {
        eulerRotation: Qt.vector3d(-10, 150, 0)
        brightness: 0.4
    }

    Node {
        id: spinner

        NumberAnimation on eulerRotation.y {
            from: 0; to: 360
            duration: 6000
            loops: Animation.Infinite
        }

        Piece3D {
            symbol: root.symbol
            highlighted: root.highlighted
        }
    }
}
