#include "NativeStyle.h"

#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlExtensionPlugin>

#include <cstdlib>

// El módulo de presentación se enlaza de forma estática como plugin QML.
Q_IMPORT_QML_PLUGIN(TresEnRaya_PresentationPlugin)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("TresEnRaya"));
    QGuiApplication::setApplicationVersion(QStringLiteral(TRESENRAYA_VERSION));
    QGuiApplication::setOrganizationName(QStringLiteral("UCB"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("ucb.edu.bo"));
    QGuiApplication::setWindowIcon(
        QIcon(QStringLiteral(":/qt/qml/TresEnRaya/App/resources/icon.svg")));

    platform::applyNativeControlsStyle();

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        [] { QCoreApplication::exit(EXIT_FAILURE); }, Qt::QueuedConnection);
    engine.loadFromModule("TresEnRaya.App", "Main");

    return QGuiApplication::exec();
}
