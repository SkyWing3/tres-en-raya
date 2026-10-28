// TDD Kata para Tres en Raya
// Requerimiento 2: Debe existir una forma de determinar qué jugador juega después

#include "tresenraya/tictactoe/GameExceptions.h"
#include "tresenraya/tictactoe/TicTacToe.h"

#include <QtTest>

using namespace tresenraya::tictactoe;

class TestTurns : public QObject
{
    Q_OBJECT

private slots:
    void init();

    // El primer turno siempre debe ser jugado por 'X'
    void firstTurnIsPlayedByX();

    // Si el último turno fue jugado por 'X', entonces el siguiente turno es para 'O'
    void afterXComesO();

    // Si el último turno fue jugado por 'O', entonces el siguiente turno es para 'X'
    void afterOComesX();

    // Comportamiento complementario
    void turnsAlternateDuringTheWholeGame();
    void rejectedMoveDoesNotConsumeTheTurn();
    void resetGivesTheFirstTurnBackToX();
    void playersAreXAndO();

private:
    TicTacToe m_game;
};

void TestTurns::init()
{
    m_game.reset();
}

void TestTurns::firstTurnIsPlayedByX()
{
    QCOMPARE(m_game.nextPlayer(), Player::X);
    QCOMPARE(m_game.lastPlayer(), std::nullopt);
}

void TestTurns::afterXComesO()
{
    m_game.play({1, 1});

    QCOMPARE(m_game.lastPlayer(), Player::X);
    QCOMPARE(m_game.nextPlayer(), Player::O);
}

void TestTurns::afterOComesX()
{
    m_game.play({1, 1});
    m_game.play({2, 1});

    QCOMPARE(m_game.lastPlayer(), Player::O);
    QCOMPARE(m_game.nextPlayer(), Player::X);
}

void TestTurns::turnsAlternateDuringTheWholeGame()
{
    // Partida que termina en empate, para recorrer las nueve casillas.
    // clang-format off
    const std::array<Position, Board::kCellCount> moves{{
        {1, 1}, {2, 1}, {3, 1},
        {2, 2}, {1, 2}, {3, 2},
        {2, 3}, {1, 3}, {3, 3},
    }};
    // clang-format on

    Player expected = Player::X;
    for (const auto &move : moves) {
        QCOMPARE(m_game.nextPlayer(), expected);
        m_game.play(move);
        expected = opponentOf(expected);
    }
    QCOMPARE(m_game.status(), GameStatus::Draw);
}

void TestTurns::rejectedMoveDoesNotConsumeTheTurn()
{
    m_game.play({1, 1}); // X

    QVERIFY_THROWS_EXCEPTION(InvalidMoveException, m_game.play({1, 1}));
    QVERIFY_THROWS_EXCEPTION(InvalidMoveException, m_game.play({0, 1}));

    QCOMPARE(m_game.lastPlayer(), Player::X);
    QCOMPARE(m_game.nextPlayer(), Player::O);
}

void TestTurns::resetGivesTheFirstTurnBackToX()
{
    m_game.play({1, 1});
    m_game.reset();

    QCOMPARE(m_game.nextPlayer(), Player::X);
    QVERIFY(m_game.board().isEmpty());
}

void TestTurns::playersAreXAndO()
{
    // Se verifican en compilación: si cambian, el proyecto no compila.
    static_assert(symbolOf(Player::X) == 'X');
    static_assert(symbolOf(Player::O) == 'O');
    static_assert(opponentOf(Player::X) == Player::O);
    static_assert(opponentOf(Player::O) == Player::X);
    static_assert(TicTacToe::kFirstPlayer == Player::X);

    QCOMPARE(symbolOf(m_game.nextPlayer()), 'X');
}

QTEST_APPLESS_MAIN(TestTurns)
#include "tst_turns.moc"
