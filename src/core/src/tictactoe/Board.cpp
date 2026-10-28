#include "tresenraya/tictactoe/Board.h"

#include "tresenraya/tictactoe/GameExceptions.h"

#include <algorithm>

namespace tresenraya::tictactoe {

namespace {

constexpr bool isWithinBoard(int coordinate) noexcept
{
    return coordinate >= kMinCoordinate && coordinate <= kMaxCoordinate;
}

} // namespace

void Board::place(Position position, Player player)
{
    if (const auto occupant = pieceAt(position))
        throw OccupiedCellException(position, *occupant);
    m_cells[indexOf(position)] = player;
}

std::optional<Player> Board::pieceAt(Position position) const
{
    requireInside(position);
    return m_cells[indexOf(position)];
}

bool Board::isFull() const noexcept
{
    return std::ranges::all_of(m_cells, [](const auto &cell) { return cell.has_value(); });
}

bool Board::isEmpty() const noexcept
{
    return std::ranges::none_of(m_cells, [](const auto &cell) { return cell.has_value(); });
}

void Board::clear() noexcept
{
    m_cells.fill(std::nullopt);
}

void Board::requireInside(Position position)
{
    if (!isWithinBoard(position.x))
        throw OutOfBoardException(Axis::X, position.x);
    if (!isWithinBoard(position.y))
        throw OutOfBoardException(Axis::Y, position.y);
}

std::size_t Board::indexOf(Position position) noexcept
{
    const int row = position.y - kMinCoordinate;
    const int column = position.x - kMinCoordinate;
    return static_cast<std::size_t>(row * kBoardSize + column);
}

} // namespace tresenraya::tictactoe
