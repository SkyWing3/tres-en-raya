# Advertencias estrictas para el código propio del proyecto.
# Las cabeceras de Qt se incluyen como cabeceras de sistema, por lo que no
# generan ruido; el código generado (moc, qmlcachegen) se compila aparte.

function(tresenraya_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
            /utf-8
            $<$<BOOL:${TRESENRAYA_WARNINGS_AS_ERRORS}>:/WX>)
    else()
        target_compile_options(${target} PRIVATE
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
endfunction()
