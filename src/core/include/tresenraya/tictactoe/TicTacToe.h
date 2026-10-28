#pragma once

#include "tresenraya/tictactoe/Board.h"
#include "tresenraya/tictactoe/Player.h"
#include "tresenraya/tictactoe/Position.h"

#include <array>
#include <optional>

namespace tresenraya::tictactoe {

enum class GameStatus {
    InProgress,
    Won,
    Draw,
};

/// Tres casillas alineadas: una fila, una columna o una diagonal.
using Line = std::array<Position, static_cast<std::size_t>(kBoardSize)>;

/// Partida de Tres en Raya.
///
///  - Requerimiento 1: delega en Board la validación de la posición.
///  - Requerimiento 2: alterna los turnos, empezando siempre por 'X'.
///  - Requerimiento 3: detecta al ganador (línea horizontal, vertical o diagonal).
///
/// Las jugadas inválidas lanzan una excepción y no modifican el estado.
class TicTacToe
{
public:
    static constexpr Player kFirstPlayer = Player::X;

    /// Coloca la pieza del jugador en turno y devuelve el estado resultante.
    /// @throws OutOfBoardException, OccupiedCellException, GameOverException
    GameStatus play(Position position);

    [[nodiscard]] Player nextPlayer() const noexcept;
    [[nodiscard]] std::optional<Player> lastPlayer() const noexcept;
    [[nodiscard]] std::optional<Player> winner() const noexcept;
    [[nodiscard]] std::optional<Line> winningLine() const noexcept;
    [[nodiscard]] GameStatus status() const noexcept;
    [[nodiscard]] bool isOver() const noexcept;
    [[nodiscard]] const Board &board() const noexcept;

    /// Deja el tablero vacío y devuelve el primer turno a 'X'.
    void reset() noexcept;

private:
    [[nodiscard]] std::optional<Line> findLineOf(Player player) const;

    Board m_board;
    std::optional<Player> m_lastPlayer;
    std::optional<Line> m_winningLine;
    GameStatus m_status = GameStatus::InProgress;
};

} // namespace tresenraya::tictactoe
