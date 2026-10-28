#include "tresenraya/tictactoe/TicTacToe.h"

#include "tresenraya/tictactoe/GameExceptions.h"

#include <algorithm>

namespace tresenraya::tictactoe {

namespace {

static_assert(kBoardSize == 3, "Las líneas ganadoras están definidas para un tablero de 3x3");

// Las ocho formas de conectar el tablero de un lado a otro o de esquina a esquina.
constexpr std::array kWinningLines{
    // Horizontales
    Line{{{1, 1}, {2, 1}, {3, 1}}},
    Line{{{1, 2}, {2, 2}, {3, 2}}},
    Line{{{1, 3}, {2, 3}, {3, 3}}},
    // Verticales
    Line{{{1, 1}, {1, 2}, {1, 3}}},
    Line{{{2, 1}, {2, 2}, {2, 3}}},
    Line{{{3, 1}, {3, 2}, {3, 3}}},
    // Diagonales
    Line{{{1, 1}, {2, 2}, {3, 3}}},
    Line{{{3, 1}, {2, 2}, {1, 3}}},
};

} // namespace

GameStatus TicTacToe::play(Position position)
{
    if (isOver())
        throw GameOverException();

    const Player player = nextPlayer();
    m_board.place(position, player);
    m_lastPlayer = player;

    m_winningLine = findLineOf(player);
    if (m_winningLine)
        m_status = GameStatus::Won;
    else if (m_board.isFull())
        m_status = GameStatus::Draw;

    return m_status;
}

Player TicTacToe::nextPlayer() const noexcept
{
    return m_lastPlayer ? opponentOf(*m_lastPlayer) : kFirstPlayer;
}

std::optional<Player> TicTacToe::lastPlayer() const noexcept
{
    return m_lastPlayer;
}

std::optional<Player> TicTacToe::winner() const noexcept
{
    return m_status == GameStatus::Won ? m_lastPlayer : std::nullopt;
}

std::optional<Line> TicTacToe::winningLine() const noexcept
{
    return m_winningLine;
}

GameStatus TicTacToe::status() const noexcept
{
    return m_status;
}

bool TicTacToe::isOver() const noexcept
{
    return m_status != GameStatus::InProgress;
}

const Board &TicTacToe::board() const noexcept
{
    return m_board;
}

void TicTacToe::reset() noexcept
{
    m_board.clear();
    m_lastPlayer.reset();
    m_winningLine.reset();
    m_status = GameStatus::InProgress;
}

std::optional<Line> TicTacToe::findLineOf(Player player) const
{
    const auto ownedBy = [&](Position position) { return m_board.pieceAt(position) == player; };
    const auto line = std::ranges::find_if(kWinningLines, [&](const Line &candidate) {
        return std::ranges::all_of(candidate, ownedBy);
    });
    return line != kWinningLines.end() ? std::optional<Line>(*line) : std::nullopt;
}

} // namespace tresenraya::tictactoe
