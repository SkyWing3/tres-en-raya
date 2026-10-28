pragma Singleton

import QtQuick

// Colores de la escena 3D. Siguen el modo claro/oscuro del sistema operativo;
// los controles 2D usan la paleta nativa del estilo activo.
QtObject {
    readonly property bool dark: Application.styleHints.colorScheme === Qt.ColorScheme.Dark

    readonly property color xColor: dark ? "#FF6A63" : "#E5484D"
    readonly property color oColor: dark ? "#3DB5E6" : "#1F8DC2"

    readonly property color sceneBackground: dark ? "#202329" : "#E7E2DA"
    readonly property color floorColor: dark ? "#2A2D34" : "#E2DCD2"
    readonly property color boardColor: dark ? "#454B57" : "#C9BFAF"
    readonly property color tileColor: dark ? "#6B7383" : "#FBF8F3"
    readonly property color sceneText: dark ? "#E8E8EA" : "#2A2A2E"

    readonly property color warningColor: dark ? "#FFB340" : "#B25000"
    readonly property color errorColor: dark ? "#FF6B6B" : "#C62828"

    function pieceColor(symbol: string): color {
        return symbol === "O" ? oColor : xColor
    }
}
