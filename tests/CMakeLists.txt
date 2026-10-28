find_package(Qt6 REQUIRED COMPONENTS Test)

# Crea un ejecutable de Qt Test y lo registra en CTest.
function(tresenraya_add_test name)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBRARIES" ${ARGN})
    qt_add_executable(${name} ${ARG_SOURCES})
    set_target_properties(${name} PROPERTIES
        MACOSX_BUNDLE FALSE
        WIN32_EXECUTABLE FALSE)
    target_link_libraries(${name} PRIVATE Qt6::Test ${ARG_LIBRARIES})
    tresenraya_enable_warnings(${name})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# Requerimiento 1: colocar piezas en un tablero de 3x3
tresenraya_add_test(tst_board
    SOURCES tictactoe/tst_board.cpp
    LIBRARIES TresEnRaya::Core)

tresenraya_add_test(tst_board_placement
    SOURCES tictactoe/tst_board_placement.cpp
    LIBRARIES TresEnRaya::Core)

# Requerimiento 2: determinar qué jugador juega después
tresenraya_add_test(tst_turns
    SOURCES tictactoe/tst_turns.cpp
    LIBRARIES TresEnRaya::Core)

# Requerimiento 3: determinar al ganador
tresenraya_add_test(tst_winner
    SOURCES tictactoe/tst_winner.cpp
    LIBRARIES TresEnRaya::Core)

# Capa de presentación (adaptador Qt sobre el dominio)
tresenraya_add_test(tst_gamecontroller
    SOURCES presentation/tst_gamecontroller.cpp
    LIBRARIES TresEnRaya::Presentation)

# Pruebas generadas por IA para la comparación (ver ia/prompts.md).
# Se conservan tal como las entregó la IA; tienen la etiqueta "ia" y se ejecutan
# aparte con: ctest --preset ia
tresenraya_add_test(tst_ia_basico
    SOURCES ia/basico/tst_tictactoe.cpp ia/basico/TicTacToe.h
    LIBRARIES TresEnRaya::Core)
target_include_directories(tst_ia_basico PRIVATE ia/basico)

tresenraya_add_test(tst_ia_estructurado
    SOURCES ia/estructurado/tst_ia_estructurado.cpp
    LIBRARIES TresEnRaya::Core)

set_tests_properties(tst_ia_basico tst_ia_estructurado PROPERTIES LABELS "ia")
