pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import TresEnRaya.Presentation

ApplicationWindow {
    id: window

    readonly property bool isMacOS: Qt.platform.os === "osx" || Qt.platform.os === "macos"

    width: 1180
    height: 780
    minimumWidth: 900
    minimumHeight: 620
    visible: true
    title: qsTr("Tres en Raya")

    GameController { id: gameController }

    // Preferencias guardadas en el almacén nativo del sistema
    // (NSUserDefaults en macOS, Registro en Windows).
    Settings {
        id: preferences
        category: "Apariencia"
        property int colorScheme: Qt.ColorScheme.Unknown
        property alias width: window.width
        property alias height: window.height
    }

    Binding {
        target: Application.styleHints
        property: "colorScheme"
        value: preferences.colorScheme
    }

    // En macOS esta barra se integra en la barra de menús del sistema;
    // en Windows se dibuja dentro de la ventana con el estilo nativo.
    menuBar: MenuBar {
        Menu {
            title: qsTr("&Juego")

            Action {
                text: qsTr("&Nuevo juego")
                shortcut: StandardKey.New
                onTriggered: gameController.newGame()
            }
            Action {
                text: qsTr("&Reiniciar marcador…")
                onTriggered: resetScoreDialog.open()
            }
            MenuSeparator { visible: !window.isMacOS }
            MenuItem {
                // macOS ya incluye "Salir" en el menú de la aplicación.
                visible: !window.isMacOS
                action: Action {
                    text: qsTr("&Salir")
                    shortcut: StandardKey.Quit
                    onTriggered: Qt.quit()
                }
            }
        }

        Menu {
            title: qsTr("&Ver")

            Menu {
                title: qsTr("&Apariencia")

                ActionGroup { id: appearanceGroup }

                Action {
                    text: qsTr("Según el sistema")
                    checkable: true
                    checked: preferences.colorScheme === Qt.ColorScheme.Unknown
                    ActionGroup.group: appearanceGroup
                    onTriggered: preferences.colorScheme = Qt.ColorScheme.Unknown
                }
                Action {
                    text: qsTr("Clara")
                    checkable: true
                    checked: preferences.colorScheme === Qt.ColorScheme.Light
                    ActionGroup.group: appearanceGroup
                    onTriggered: preferences.colorScheme = Qt.ColorScheme.Light
                }
                Action {
                    text: qsTr("Oscura")
                    checkable: true
                    checked: preferences.colorScheme === Qt.ColorScheme.Dark
                    ActionGroup.group: appearanceGroup
                    onTriggered: preferences.colorScheme = Qt.ColorScheme.Dark
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("Restablecer &cámara")
                shortcut: "Ctrl+0"
                onTriggered: gamePage.resetCamera()
            }
        }

        Menu {
            title: qsTr("Ay&uda")

            Action {
                text: qsTr("&Acerca de Tres en Raya")
                onTriggered: aboutDialog.open()
            }
        }
    }

    GamePage {
        id: gamePage
        anchors.fill: parent
        game: gameController
        onResetScoreRequested: resetScoreDialog.open()
        Component.onCompleted: focusBoard()
    }

    // --- Diálogos: NSAlert nativo en macOS; en Windows, Qt Quick con estilo nativo --

    MessageDialog {
        id: resetScoreDialog
        title: qsTr("Reiniciar marcador")
        text: qsTr("¿Quieres reiniciar el marcador?")
        informativeText: qsTr("Se perderán las victorias y los empates acumulados.")
        buttons: MessageDialog.Yes | MessageDialog.No
        onButtonClicked: (button, role) => {
            if (button === MessageDialog.Yes)
                gameController.resetScore()
        }
    }

    MessageDialog {
        id: aboutDialog
        title: qsTr("Acerca de Tres en Raya")
        text: qsTr("Tres en Raya %1").arg(Qt.application.version)
        informativeText: qsTr("TDD Kata de Tres en Raya para la materia Gestión de Calidad de Sistemas.\n\nHecho con Qt Quick 3D y controles nativos.")
        buttons: MessageDialog.Ok
    }
}
