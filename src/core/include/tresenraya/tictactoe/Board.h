#pragma once

#include "tresenraya/tictactoe/Player.h"
#include "tresenraya/tictactoe/Position.h"

#include <array>
#include <cstddef>
#include <optional>

namespace tresenraya::tictactoe {

/// Tablero de 3x3 (Requerimiento 1).
///
/// Sólo se ocupa de dónde están las piezas: valida que la posición esté
/// dentro del tablero y que la casilla esté libre. Las reglas de turnos y de
/// victoria pertenecen a TicTacToe.
class Board
{
public:
    static constexpr std::size_t kCellCount = static_cast<std::size_t>(kBoardSize * kBoardSize);

    /// Coloca una pieza.
    /// @throws OutOfBoardException si x o y están fuera de [1, 3].
    /// @throws OccupiedCellException si la casilla ya tiene una pieza.
    void place(Position position, Player player);

    /// Pieza en la casilla, o std::nullopt si está vacía.
    /// @throws OutOfBoardException si la posición está fuera del tablero.
    [[nodiscard]] std::optional<Player> pieceAt(Position position) const;

    [[nodiscard]] bool isFull() const noexcept;
    [[nodiscard]] bool isEmpty() const noexcept;

    void clear() noexcept;

private:
    static void requireInside(Position position);
    [[nodiscard]] static std::size_t indexOf(Position position) noexcept;

    std::array<std::optional<Player>, kCellCount> m_cells{};
};

} // namespace tresenraya::tictactoe
