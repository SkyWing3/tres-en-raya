#include "NativeStyle.h"

#include <QQuickStyle>
#include <QtGlobal>

#ifdef Q_OS_WIN
#include <QOperatingSystemVersion>
#endif

namespace platform {

QString nativeControlsStyle()
{
#if defined(Q_OS_MACOS)
    return QStringLiteral("macOS");
#elif defined(Q_OS_WIN)
    return QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows11
        ? QStringLiteral("FluentWinUI3")
        : QStringLiteral("Windows");
#else
    return QStringLiteral("Fusion");
#endif
}

void applyNativeControlsStyle()
{
    if (qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_STYLE"))
        return;
    QQuickStyle::setStyle(nativeControlsStyle());
}

} // namespace platform
