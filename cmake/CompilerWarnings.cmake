# Advertencias estrictas para el código escrito a mano.
#
# Se aplican archivo por archivo sólo a los .cpp del árbol de fuentes. El código que
# genera Qt (moc, qmltyperegistrar y, sobre todo, qmlcachegen) se compila con las
# opciones por defecto: con MSVC, por ejemplo, el C++ generado desde QML provoca la
# advertencia C4702 dentro de las cabeceras de Qt, algo que el proyecto no controla.

function(tresenraya_enable_warnings target)
    if(MSVC)
        set(warnings /W4 /permissive- $<$<BOOL:${TRESENRAYA_WARNINGS_AS_ERRORS}>:/WX>)
        # UTF-8 para todo el objetivo: los textos en español deben leerse igual siempre.
        target_compile_options(${target} PRIVATE /utf-8)
    else()
        set(warnings
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wnon-virtual-dtor
            -Woverloaded-virtual
            -Wold-style-cast
            -Wnull-dereference
            $<$<BOOL:${TRESENRAYA_WARNINGS_AS_ERRORS}>:-Werror>)
    endif()

    get_target_property(sources ${target} SOURCES)
    foreach(source IN LISTS sources)
        cmake_path(ABSOLUTE_PATH source BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                   OUTPUT_VARIABLE path)
        cmake_path(IS_PREFIX PROJECT_SOURCE_DIR "${path}" in_source_tree)
        cmake_path(IS_PREFIX PROJECT_BINARY_DIR "${path}" in_build_tree)
        if(source MATCHES "\\.cpp$" AND in_source_tree AND NOT in_build_tree)
            set_property(SOURCE "${source}" TARGET_DIRECTORY ${target}
                         APPEND PROPERTY COMPILE_OPTIONS "${warnings}")
        endif()
    endforeach()
endfunction()
