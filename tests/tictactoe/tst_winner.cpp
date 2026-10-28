// TDD Kata para Tres en Raya
// Requerimiento 3: Un jugador gana si es el primero en conectar una línea de un
// lado a otro, o de una esquina a otra

#include "tresenraya/tictactoe/GameExceptions.h"
#include "tresenraya/tictactoe/TicTacToe.h"

#include <QtTest>

#include <vector>

using namespace tresenraya::tictactoe;

namespace {

using Moves = std::vector<Position>;

/// Juega las posiciones alternando los turnos y devuelve el último estado.
GameStatus playAll(TicTacToe &game, const Moves &moves)
{
    GameStatus status = game.status();
    for (const auto &move : moves)
        status = game.play(move);
    return status;
}

} // namespace

class TestWinner : public QObject
{
    Q_OBJECT

private slots:
    void init();

    // Si no se cumple la condición de victoria, entonces no hay ganador
    void withoutWinningConditionThereIsNoWinner();

    // Un jugador gana cuando toda una línea horizontal es ocupada por sus piezas
    void wholeHorizontalLineWins_data();
    void wholeHorizontalLineWins();

    // Un jugador gana cuando toda una línea vertical es ocupada por sus piezas
    void wholeVerticalLineWins_data();
    void wholeVerticalLineWins();

    // Un jugador gana cuando toda una línea diagonal es ocupada por sus piezas
    void topLeftToBottomRightDiagonalWins();
    void topRightToBottomLeftDiagonalWins();

    // Comportamiento complementario
    void oCanAlsoWin();
    void fullBoardWithoutLineIsADraw();
    void winningOnTheLastMoveIsNotADraw();
    void noMovesAreAllowedAfterTheGameIsOver();
    void resetAfterAWinStartsACleanGame();

private:
    TicTacToe m_game;
};

void TestWinner::init()
{
    m_game.reset();
}

void TestWinner::withoutWinningConditionThereIsNoWinner()
{
    const GameStatus status = m_game.play({1, 1});

    QCOMPARE(status, GameStatus::InProgress);
    QCOMPARE(m_game.winner(), std::nullopt);
    QCOMPARE(m_game.winningLine(), std::nullopt);
}

void TestWinner::wholeHorizontalLineWins_data()
{
    QTest::addColumn<int>("y");

    QTest::newRow("fila superior") << 1;
    QTest::newRow("fila central") << 2;
    QTest::newRow("fila inferior") << 3;
}

void TestWinner::wholeHorizontalLineWins()
{
    QFETCH(int, y);
    const int otherRow = y == 1 ? 2 : 1;

    // X completa la fila y; 'O' juega en otra fila sin completarla.
    const auto status = playAll(m_game, {{1, y}, {1, otherRow}, {2, y}, {2, otherRow}, {3, y}});

    QCOMPARE(status, GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(), (Line{{{1, y}, {2, y}, {3, y}}}));
}

void TestWinner::wholeVerticalLineWins_data()
{
    QTest::addColumn<int>("x");

    QTest::newRow("columna izquierda") << 1;
    QTest::newRow("columna central") << 2;
    QTest::newRow("columna derecha") << 3;
}

void TestWinner::wholeVerticalLineWins()
{
    QFETCH(int, x);
    const int otherColumn = x == 1 ? 2 : 1;

    const auto status
        = playAll(m_game, {{x, 1}, {otherColumn, 1}, {x, 2}, {otherColumn, 2}, {x, 3}});

    QCOMPARE(status, GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(), (Line{{{x, 1}, {x, 2}, {x, 3}}}));
}

void TestWinner::topLeftToBottomRightDiagonalWins()
{
    const auto status = playAll(m_game, {{1, 1}, {1, 2}, {2, 2}, {1, 3}, {3, 3}});

    QCOMPARE(status, GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(), (Line{{{1, 1}, {2, 2}, {3, 3}}}));
}

void TestWinner::topRightToBottomLeftDiagonalWins()
{
    const auto status = playAll(m_game, {{3, 1}, {1, 2}, {2, 2}, {2, 1}, {1, 3}});

    QCOMPARE(status, GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(), (Line{{{3, 1}, {2, 2}, {1, 3}}}));
}

void TestWinner::oCanAlsoWin()
{
    const auto status = playAll(m_game, {{1, 1}, {1, 2}, {3, 3}, {2, 2}, {1, 3}, {3, 2}});

    QCOMPARE(status, GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::O);
}

void TestWinner::fullBoardWithoutLineIsADraw()
{
    // clang-format off
    const auto status = playAll(m_game, {{1, 1}, {2, 1}, {3, 1},
                                         {2, 2}, {1, 2}, {3, 2},
                                         {2, 3}, {1, 3}, {3, 3}});
    // clang-format on

    QCOMPARE(status, GameStatus::Draw);
    QCOMPARE(m_game.winner(), std::nullopt);
    QVERIFY(m_game.isOver());
}

void TestWinner::winningOnTheLastMoveIsNotADraw()
{
    // Tablero final (X gana con la diagonal al ocupar la última casilla):
    //   X O O
    //   O X X
    //   X O X
    // clang-format off
    const auto status = playAll(m_game, {{1, 1}, {2, 1}, {2, 2}, {3, 1}, {3, 2},
                                         {1, 2}, {1, 3}, {2, 3}, {3, 3}});
    // clang-format on

    QVERIFY(m_game.board().isFull());
    QCOMPARE(status, GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(), (Line{{{1, 1}, {2, 2}, {3, 3}}}));
}

void TestWinner::noMovesAreAllowedAfterTheGameIsOver()
{
    playAll(m_game, {{1, 1}, {1, 2}, {2, 1}, {2, 2}, {3, 1}});

    QVERIFY_THROWS_EXCEPTION(GameOverException, m_game.play({3, 3}));
}

void TestWinner::resetAfterAWinStartsACleanGame()
{
    playAll(m_game, {{1, 1}, {1, 2}, {2, 1}, {2, 2}, {3, 1}});
    QCOMPARE(m_game.status(), GameStatus::Won);

    m_game.reset();

    QCOMPARE(m_game.status(), GameStatus::InProgress);
    QCOMPARE(m_game.winner(), std::nullopt);
    QCOMPARE(m_game.winningLine(), std::nullopt);
    QVERIFY(!m_game.isOver());
    QCOMPARE(m_game.play({3, 3}), GameStatus::InProgress);
}

QTEST_APPLESS_MAIN(TestWinner)
#include "tst_winner.moc"
