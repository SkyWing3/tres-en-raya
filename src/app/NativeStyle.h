#pragma once

#include <QString>

namespace platform {

/// Nombre del estilo de Qt Quick Controls que dibuja los controles con el
/// aspecto nativo del sistema operativo actual:
///  - macOS:      "macOS" (controles AppKit)
///  - Windows 11: "FluentWinUI3"
///  - Windows 10: "Windows" (controles de UxTheme)
///  - Otros:      "Fusion"
[[nodiscard]] QString nativeControlsStyle();

/// Aplica el estilo nativo antes de cargar QML. Respeta la variable de entorno
/// QT_QUICK_CONTROLS_STYLE para poder probar otros estilos sin recompilar.
void applyNativeControlsStyle();

} // namespace platform
