# Capa de presentación: adapta el dominio (C++ puro) a QML siguiendo MVVM.
# Es una biblioteca independiente para poder probarla sin cargar la interfaz.

qt_add_library(tresenraya_presentation STATIC)
add_library(TresEnRaya::Presentation ALIAS tresenraya_presentation)

qt_add_qml_module(tresenraya_presentation
    URI TresEnRaya.Presentation
    VERSION 1.0
    SOURCES
        BoardModel.cpp
        BoardModel.h
        GameController.cpp
        GameController.h
)

target_include_directories(tresenraya_presentation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(tresenraya_presentation PUBLIC
    Qt6::Core
    Qt6::Qml
    TresEnRaya::Core)

tresenraya_enable_warnings(tresenraya_presentation)
