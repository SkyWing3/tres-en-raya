// ADAPTADOR — escrito por el equipo, NO por la IA.
//
// La respuesta al prompt 1 supuso una API propia (TicTacToe::play devuelve un texto y
// nextPlayer devuelve un char). Este adaptador implementa esa interfaz delegando en el
// dominio real, para poder ejecutar las pruebas de la IA sin modificarlas.
#pragma once

#include "tresenraya/tictactoe/TicTacToe.h"

#include <QString>

class TicTacToe
{
public:
    QString play(int x, int y)
    {
        using tresenraya::tictactoe::GameStatus;
        using tresenraya::tictactoe::symbolOf;

        if (m_game.play({x, y}) == GameStatus::Won) {
            const QChar winner = QChar::fromLatin1(symbolOf(*m_game.winner()));
            return QStringLiteral("%1 es el ganador").arg(winner);
        }
        return QStringLiteral("No hay ganador");
    }

    char nextPlayer() const { return tresenraya::tictactoe::symbolOf(m_game.nextPlayer()); }

private:
    tresenraya::tictactoe::TicTacToe m_game;
};
