# Instrumentación opcional para medir la cobertura de código de las pruebas.
# Se activa con -DTRESENRAYA_ENABLE_COVERAGE=ON (preset "coverage").

option(TRESENRAYA_ENABLE_COVERAGE "Instrumentar el código para medir cobertura" OFF)

function(tresenraya_enable_coverage target)
    if(NOT TRESENRAYA_ENABLE_COVERAGE)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
        target_link_options(${target} PUBLIC -fprofile-instr-generate)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} PRIVATE --coverage)
        target_link_options(${target} PUBLIC --coverage)
    else()
        message(WARNING "La cobertura sólo está disponible con Clang o GCC")
    endif()
endfunction()
