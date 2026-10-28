# Núcleo de dominio: C++ estándar puro, sin dependencias de Qt.
# Así la lógica de la kata puede probarse y reutilizarse sin interfaz gráfica.

add_library(tresenraya_core STATIC
    include/tresenraya/tictactoe/Board.h
    include/tresenraya/tictactoe/GameExceptions.h
    include/tresenraya/tictactoe/Player.h
    include/tresenraya/tictactoe/Position.h
    include/tresenraya/tictactoe/TicTacToe.h
    src/tictactoe/Board.cpp
    src/tictactoe/GameExceptions.cpp
    src/tictactoe/TicTacToe.cpp
)
add_library(TresEnRaya::Core ALIAS tresenraya_core)

target_include_directories(tresenraya_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(tresenraya_core PUBLIC cxx_std_20)
tresenraya_enable_warnings(tresenraya_core)
tresenraya_enable_coverage(tresenraya_core)
