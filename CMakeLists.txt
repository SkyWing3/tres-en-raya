cmake_minimum_required(VERSION 3.21)

project(TresEnRaya
    VERSION 1.0.0
    DESCRIPTION "TDD Kata: Tres en Raya en 3D"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TRESENRAYA_BUILD_APP "Compilar la aplicación gráfica" ON)
option(TRESENRAYA_BUILD_TESTS "Compilar las pruebas unitarias" ON)
option(TRESENRAYA_WARNINGS_AS_ERRORS "Tratar las advertencias del compilador como errores" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Coverage)

find_package(Qt6 6.10 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Quick3D)
qt_standard_project_setup(REQUIRES 6.10)

# Todos los módulos QML en un mismo árbol, para que qmllint y Qt Creator los encuentren.
set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

add_subdirectory(src)

if(TRESENRAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
