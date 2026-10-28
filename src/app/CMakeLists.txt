# Aplicación de escritorio: interfaz QML con Qt Quick 3D y controles nativos.

qt_add_executable(tresenraya_app
    main.cpp
    NativeStyle.cpp
    NativeStyle.h
)

# Debe declararse antes de qt_add_qml_module para que qmldir lo registre como singleton.
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(tresenraya_app
    URI TresEnRaya.App
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/GamePage.qml
        qml/GameBoard3D.qml
        qml/BoardCell3D.qml
        qml/Tile3D.qml
        qml/Piece3D.qml
        qml/PiecePreview3D.qml
        qml/WinningBeam3D.qml
        qml/GameInspector.qml
        qml/NoticeBanner.qml
    RESOURCES
        resources/icon.svg
)

target_compile_definitions(tresenraya_app PRIVATE
    TRESENRAYA_VERSION="${PROJECT_VERSION}")

target_link_libraries(tresenraya_app PRIVATE
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Quick3D
    tresenraya_presentationplugin)

tresenraya_enable_warnings(tresenraya_app)

# --- Integración con cada sistema operativo -----------------------------------

set_target_properties(tresenraya_app PROPERTIES
    OUTPUT_NAME "TresEnRaya"
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Tres en Raya"
    MACOSX_BUNDLE_GUI_IDENTIFIER "bo.edu.ucb.tresenraya"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/Info.plist.in"
    MACOSX_BUNDLE_ICON_FILE "TresEnRaya.icns")

if(APPLE)
    set(macos_icon "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/TresEnRaya.icns")
    set_source_files_properties(${macos_icon} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(tresenraya_app PRIVATE ${macos_icon})
elseif(WIN32)
    configure_file(platform/windows/TresEnRaya.rc.in
                   "${CMAKE_CURRENT_BINARY_DIR}/TresEnRaya.rc" @ONLY)
    target_sources(tresenraya_app PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/TresEnRaya.rc")
endif()

# --- Instalación y despliegue (macdeployqt / windeployqt) ---------------------

include(GNUInstallDirs)

install(TARGETS tresenraya_app
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_generate_deploy_qml_app_script(
    TARGET tresenraya_app
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR)
install(SCRIPT ${deploy_script})
