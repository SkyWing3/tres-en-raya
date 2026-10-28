// TDD Kata para Tres en Raya
// Requerimiento 1: Una pieza puede estar colocada en un espacio de un tablero 3x3

#include "tresenraya/tictactoe/GameExceptions.h"
#include "tresenraya/tictactoe/TicTacToe.h"

#include <QtTest>

using namespace tresenraya::tictactoe;

class TestBoardPlacement : public QObject
{
    Q_OBJECT

private slots:
    void init();

    // Cuando una pieza está fuera del eje X, entonces se debe lanzar una excepción
    void pieceOutsideXAxisThrows_data();
    void pieceOutsideXAxisThrows();

    // Cuando una pieza está fuera del eje Y, entonces se debe lanzar una excepción
    void pieceOutsideYAxisThrows_data();
    void pieceOutsideYAxisThrows();

    // Cuando una pieza esté en un lugar ya ocupado, se debe lanzar una excepción
    void pieceOnOccupiedCellThrows();

    // Comportamiento complementario
    void pieceInsideBoardIsPlaced_data();
    void pieceInsideBoardIsPlaced();
    void rejectedMoveDoesNotChangeTheGame();

private:
    TicTacToe m_game;
};

void TestBoardPlacement::init()
{
    m_game.reset();
}

void TestBoardPlacement::pieceOutsideXAxisThrows_data()
{
    QTest::addColumn<int>("x");

    QTest::newRow("x = 0") << 0;
    QTest::newRow("x = 4") << 4;
    QTest::newRow("x = -1") << -1;
}

void TestBoardPlacement::pieceOutsideXAxisThrows()
{
    QFETCH(int, x);

    try {
        m_game.play({x, 2});
        QFAIL("Se esperaba OutOfBoardException");
    } catch (const OutOfBoardException &exception) {
        QCOMPARE(exception.axis(), Axis::X);
        QCOMPARE(exception.value(), x);
    }
}

void TestBoardPlacement::pieceOutsideYAxisThrows_data()
{
    QTest::addColumn<int>("y");

    QTest::newRow("y = 0") << 0;
    QTest::newRow("y = 4") << 4;
    QTest::newRow("y = 10") << 10;
}

void TestBoardPlacement::pieceOutsideYAxisThrows()
{
    QFETCH(int, y);

    try {
        m_game.play({2, y});
        QFAIL("Se esperaba OutOfBoardException");
    } catch (const OutOfBoardException &exception) {
        QCOMPARE(exception.axis(), Axis::Y);
        QCOMPARE(exception.value(), y);
    }
}

void TestBoardPlacement::pieceOnOccupiedCellThrows()
{
    m_game.play({2, 1});

    try {
        m_game.play({2, 1});
        QFAIL("Se esperaba OccupiedCellException");
    } catch (const OccupiedCellException &exception) {
        QCOMPARE(exception.position(), (Position{2, 1}));
        QCOMPARE(exception.occupant(), Player::X);
    }
}

void TestBoardPlacement::pieceInsideBoardIsPlaced_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");

    for (int y = kMinCoordinate; y <= kMaxCoordinate; ++y) {
        for (int x = kMinCoordinate; x <= kMaxCoordinate; ++x)
            QTest::addRow("(%d, %d)", x, y) << x << y;
    }
}

void TestBoardPlacement::pieceInsideBoardIsPlaced()
{
    QFETCH(int, x);
    QFETCH(int, y);

    m_game.play({x, y});

    QCOMPARE(m_game.board().pieceAt({x, y}), Player::X);
}

void TestBoardPlacement::rejectedMoveDoesNotChangeTheGame()
{
    m_game.play({1, 1});

    QVERIFY_THROWS_EXCEPTION(InvalidMoveException, m_game.play({1, 1}));
    QVERIFY_THROWS_EXCEPTION(InvalidMoveException, m_game.play({5, 1}));

    QCOMPARE(m_game.nextPlayer(), Player::O);
    QCOMPARE(m_game.board().pieceAt({1, 1}), Player::X);
}

QTEST_APPLESS_MAIN(TestBoardPlacement)
#include "tst_board_placement.moc"
