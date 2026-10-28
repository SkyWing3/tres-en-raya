#pragma once

namespace tresenraya::tictactoe {

/// Cantidad de filas y de columnas del tablero.
inline constexpr int kBoardSize = 3;

/// Primera y última coordenada válidas en cualquiera de los dos ejes.
inline constexpr int kMinCoordinate = 1;
inline constexpr int kMaxCoordinate = kBoardSize;

/// Coordenada de una casilla del tablero.
///  - x: columna (eje horizontal), de 1 a 3.
///  - y: fila (eje vertical), de 1 a 3.
struct Position
{
    int x = 1;
    int y = 1;

    friend constexpr bool operator==(const Position &, const Position &) = default;
};

/// Eje de coordenadas, usado para informar cuál quedó fuera del tablero.
enum class Axis : char {
    X = 'X',
    Y = 'Y',
};

} // namespace tresenraya::tictactoe
