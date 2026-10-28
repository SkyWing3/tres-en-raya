#pragma once

#include "tresenraya/tictactoe/Player.h"
#include "tresenraya/tictactoe/Position.h"

#include <stdexcept>

namespace tresenraya::tictactoe {

/// Base de todas las jugadas inválidas. Permite capturarlas de forma uniforme.
class InvalidMoveException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

/// La pieza se colocó fuera del tablero en el eje indicado.
class OutOfBoardException : public InvalidMoveException
{
public:
    OutOfBoardException(Axis axis, int value);

    [[nodiscard]] Axis axis() const noexcept;
    [[nodiscard]] int value() const noexcept;

private:
    Axis m_axis;
    int m_value;
};

/// La casilla ya estaba ocupada por otra pieza.
class OccupiedCellException : public InvalidMoveException
{
public:
    OccupiedCellException(Position position, Player occupant);

    [[nodiscard]] Position position() const noexcept;
    [[nodiscard]] Player occupant() const noexcept;

private:
    Position m_position;
    Player m_occupant;
};

/// Se intentó jugar cuando la partida ya terminó (victoria o empate).
class GameOverException : public InvalidMoveException
{
public:
    GameOverException();
};

} // namespace tresenraya::tictactoe
