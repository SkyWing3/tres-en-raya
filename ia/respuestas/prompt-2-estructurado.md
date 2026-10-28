# Respuesta de la IA al prompt 2 (estructurado)

> Generada por Claude (Anthropic) en una sola respuesta, sin correcciones. Se copia tal
> cual para poder compararla.

---

```cpp
// tst_ia_estructurado.cpp
// Pruebas unitarias de la kata Tres en Raya (Qt Test, C++20).

#include "tresenraya/tictactoe/GameExceptions.h"
#include "tresenraya/tictactoe/TicTacToe.h"

#include <QtTest>

#include <initializer_list>

using namespace tresenraya::tictactoe;

Q_DECLARE_METATYPE(Position)

class TestTicTacToeIA : public QObject
{
    Q_OBJECT

private slots:
    void init();

    // Requerimiento 1: Una pieza puede estar colocada en un espacio de un tablero 3x3
    // - Cuando una pieza está fuera del eje X, se debe lanzar una excepción
    void playOutsideXAxisThrowsOutOfBoard_data();
    void playOutsideXAxisThrowsOutOfBoard();
    // - Cuando una pieza está fuera del eje Y, se debe lanzar una excepción
    void playOutsideYAxisThrowsOutOfBoard_data();
    void playOutsideYAxisThrowsOutOfBoard();
    // - Cuando una pieza esté en un lugar ya ocupado, se debe lanzar una excepción
    void playOnOccupiedCellThrowsOccupiedCell();
    // Caso límite: una jugada inválida no cambia el turno ni el tablero
    void invalidMoveDoesNotChangeTurn();

    // Requerimiento 2: Debe existir una forma de determinar qué jugador juega después
    // - El primer turno siempre debe ser jugado por 'X'
    void firstTurnIsX();
    // - Si el último turno fue jugado por 'X', el siguiente turno es para 'O'
    void afterXNextIsO();
    // - Si el último turno fue jugado por 'O', el siguiente turno es para 'X'
    void afterONextIsX();

    // Requerimiento 3: Un jugador gana si es el primero en conectar una línea
    // - Si no se cumple la condición de victoria, no hay ganador
    void noWinnerWithoutLine();
    // - Gana cuando toda una línea horizontal es ocupada por sus piezas
    void horizontalLineWins_data();
    void horizontalLineWins();
    // - Gana cuando toda una línea vertical es ocupada por sus piezas
    void verticalLineWins_data();
    void verticalLineWins();
    // - Gana cuando toda una línea diagonal es ocupada por sus piezas
    void diagonalLineWins_data();
    void diagonalLineWins();
    // Casos límite
    void fullBoardWithoutLineIsDraw();
    void winOnLastMoveIsNotDraw();
    void playAfterGameOverThrows();

private:
    void playSequence(std::initializer_list<Position> moves);

    TicTacToe m_game;
};

void TestTicTacToeIA::init()
{
    m_game.reset();
}

void TestTicTacToeIA::playSequence(std::initializer_list<Position> moves)
{
    for (const Position &move : moves)
        m_game.play(move);
}

// ---------------------------------------------------------------- Requerimiento 1

void TestTicTacToeIA::playOutsideXAxisThrowsOutOfBoard_data()
{
    QTest::addColumn<int>("x");
    QTest::newRow("x = 0") << 0;
    QTest::newRow("x = 4") << 4;
    QTest::newRow("x negativo") << -3;
}

void TestTicTacToeIA::playOutsideXAxisThrowsOutOfBoard()
{
    QFETCH(int, x);
    try {
        m_game.play({x, 1});
        QFAIL("No se lanzó OutOfBoardException");
    } catch (const OutOfBoardException &e) {
        QCOMPARE(e.axis(), Axis::X);
        QCOMPARE(e.value(), x);
    }
}

void TestTicTacToeIA::playOutsideYAxisThrowsOutOfBoard_data()
{
    QTest::addColumn<int>("y");
    QTest::newRow("y = 0") << 0;
    QTest::newRow("y = 4") << 4;
    QTest::newRow("y negativo") << -1;
}

void TestTicTacToeIA::playOutsideYAxisThrowsOutOfBoard()
{
    QFETCH(int, y);
    try {
        m_game.play({1, y});
        QFAIL("No se lanzó OutOfBoardException");
    } catch (const OutOfBoardException &e) {
        QCOMPARE(e.axis(), Axis::Y);
        QCOMPARE(e.value(), y);
    }
}

void TestTicTacToeIA::playOnOccupiedCellThrowsOccupiedCell()
{
    m_game.play({2, 2}); // X
    try {
        m_game.play({2, 2}); // O intenta la misma casilla
        QFAIL("No se lanzó OccupiedCellException");
    } catch (const OccupiedCellException &e) {
        QCOMPARE(e.position(), (Position{2, 2}));
        QCOMPARE(e.occupant(), Player::X);
    }
}

void TestTicTacToeIA::invalidMoveDoesNotChangeTurn()
{
    m_game.play({1, 1}); // X
    QVERIFY_THROWS_EXCEPTION(OccupiedCellException, m_game.play({1, 1}));
    QVERIFY_THROWS_EXCEPTION(OutOfBoardException, m_game.play({4, 4}));
    QCOMPARE(m_game.nextPlayer(), Player::O);
    QCOMPARE(m_game.board().pieceAt({1, 1}), Player::X);
}

// ---------------------------------------------------------------- Requerimiento 2

void TestTicTacToeIA::firstTurnIsX()
{
    QCOMPARE(m_game.nextPlayer(), Player::X);
    QVERIFY(!m_game.lastPlayer().has_value());
}

void TestTicTacToeIA::afterXNextIsO()
{
    m_game.play({1, 1});
    QCOMPARE(m_game.lastPlayer(), Player::X);
    QCOMPARE(m_game.nextPlayer(), Player::O);
}

void TestTicTacToeIA::afterONextIsX()
{
    m_game.play({1, 1});
    m_game.play({2, 1});
    QCOMPARE(m_game.lastPlayer(), Player::O);
    QCOMPARE(m_game.nextPlayer(), Player::X);
}

// ---------------------------------------------------------------- Requerimiento 3

void TestTicTacToeIA::noWinnerWithoutLine()
{
    QCOMPARE(m_game.play({1, 1}), GameStatus::InProgress);
    QCOMPARE(m_game.play({2, 2}), GameStatus::InProgress);
    QVERIFY(!m_game.winner().has_value());
    QVERIFY(!m_game.winningLine().has_value());
}

void TestTicTacToeIA::horizontalLineWins_data()
{
    QTest::addColumn<int>("row");
    QTest::newRow("fila 1") << 1;
    QTest::newRow("fila 2") << 2;
    QTest::newRow("fila 3") << 3;
}

void TestTicTacToeIA::horizontalLineWins()
{
    QFETCH(int, row);
    const int other = row == 3 ? 1 : row + 1;
    playSequence({{1, row}, {1, other}, {2, row}, {2, other}});
    QCOMPARE(m_game.play({3, row}), GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(), (Line{Position{1, row}, Position{2, row}, Position{3, row}}));
}

void TestTicTacToeIA::verticalLineWins_data()
{
    QTest::addColumn<int>("column");
    QTest::newRow("columna 1") << 1;
    QTest::newRow("columna 2") << 2;
    QTest::newRow("columna 3") << 3;
}

void TestTicTacToeIA::verticalLineWins()
{
    QFETCH(int, column);
    const int other = column == 3 ? 1 : column + 1;
    playSequence({{column, 1}, {other, 1}, {column, 2}, {other, 2}});
    QCOMPARE(m_game.play({column, 3}), GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(),
             (Line{Position{column, 1}, Position{column, 2}, Position{column, 3}}));
}

void TestTicTacToeIA::diagonalLineWins_data()
{
    QTest::addColumn<Position>("first");
    QTest::addColumn<Position>("last");
    QTest::newRow("diagonal principal") << Position{1, 1} << Position{3, 3};
    QTest::newRow("diagonal secundaria") << Position{3, 1} << Position{1, 3};
}

void TestTicTacToeIA::diagonalLineWins()
{
    QFETCH(Position, first);
    QFETCH(Position, last);
    // O juega en la fila 2, columnas 1 y 3, sin completar ninguna línea
    playSequence({first, {1, 2}, {2, 2}, {3, 2}});
    QCOMPARE(m_game.play(last), GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QCOMPARE(m_game.winningLine(), (Line{first, Position{2, 2}, last}));
}

void TestTicTacToeIA::fullBoardWithoutLineIsDraw()
{
    // X O X
    // X O O
    // O X X
    playSequence({{1, 1}, {2, 1}, {3, 1}, {2, 2}, {1, 2}, {3, 2}, {2, 3}, {1, 3}});
    QCOMPARE(m_game.play({3, 3}), GameStatus::Draw);
    QVERIFY(!m_game.winner().has_value());
    QVERIFY(m_game.isOver());
}

void TestTicTacToeIA::winOnLastMoveIsNotDraw()
{
    // X O X
    // O O X
    // X X X  <- X completa la fila 3 en la novena jugada
    playSequence({{1, 1}, {2, 1}, {3, 1}, {1, 2}, {3, 2}, {2, 2}, {1, 3}, {2, 3}});
    QCOMPARE(m_game.play({3, 3}), GameStatus::Won);
    QCOMPARE(m_game.winner(), Player::X);
    QVERIFY(m_game.board().isFull());
}

void TestTicTacToeIA::playAfterGameOverThrows()
{
    playSequence({{1, 1}, {1, 2}, {2, 1}, {2, 2}, {3, 1}});
    QCOMPARE(m_game.status(), GameStatus::Won);
    QVERIFY_THROWS_EXCEPTION(GameOverException, m_game.play({3, 3}));
}

QTEST_APPLESS_MAIN(TestTicTacToeIA)
#include "tst_ia_estructurado.moc"
```
