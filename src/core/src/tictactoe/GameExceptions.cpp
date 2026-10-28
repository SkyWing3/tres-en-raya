#include "tresenraya/tictactoe/GameExceptions.h"

#include <string>

namespace tresenraya::tictactoe {

namespace {

std::string describeOutOfBoard(Axis axis, int value)
{
    return std::string("La pieza está fuera del eje ") + static_cast<char>(axis) + ": "
        + std::to_string(value) + " no está entre " + std::to_string(kMinCoordinate) + " y "
        + std::to_string(kMaxCoordinate);
}

std::string describeOccupied(Position position, Player occupant)
{
    return "La casilla (" + std::to_string(position.x) + ", " + std::to_string(position.y)
        + ") ya está ocupada por " + symbolOf(occupant);
}

} // namespace

OutOfBoardException::OutOfBoardException(Axis axis, int value)
    : InvalidMoveException(describeOutOfBoard(axis, value))
    , m_axis(axis)
    , m_value(value)
{
}

Axis OutOfBoardException::axis() const noexcept
{
    return m_axis;
}

int OutOfBoardException::value() const noexcept
{
    return m_value;
}

OccupiedCellException::OccupiedCellException(Position position, Player occupant)
    : InvalidMoveException(describeOccupied(position, occupant))
    , m_position(position)
    , m_occupant(occupant)
{
}

Position OccupiedCellException::position() const noexcept
{
    return m_position;
}

Player OccupiedCellException::occupant() const noexcept
{
    return m_occupant;
}

GameOverException::GameOverException()
    : InvalidMoveException("La partida ya terminó")
{
}

} // namespace tresenraya::tictactoe
