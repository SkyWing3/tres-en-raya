// TDD Kata para Tres en Raya
// Pruebas unitarias de Board: la pieza del dominio que sostiene el Requerimiento 1.
// Verifican el tablero de forma aislada, sin las reglas de turnos ni de victoria.

#include "tresenraya/tictactoe/Board.h"
#include "tresenraya/tictactoe/GameExceptions.h"

#include <QtTest>

#include <limits>
#include <tuple>

using namespace tresenraya::tictactoe;

class TestBoard : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void newBoardIsEmpty();
    void placedPieceCanBeRead();
    void placingOutsideTheBoardThrows_data();
    void placingOutsideTheBoardThrows();
    void readingOutsideTheBoardThrows();
    void whenBothAxesAreOutsideTheXAxisIsReported();
    void occupiedCellKeepsTheOriginalPiece();
    void boardIsFullOnlyAfterNinePieces();
    void clearEmptiesEveryCell();

private:
    Board m_board;
};

void TestBoard::init()
{
    m_board.clear();
}

void TestBoard::newBoardIsEmpty()
{
    const Board board;

    QVERIFY(board.isEmpty());
    QVERIFY(!board.isFull());
    for (int y = kMinCoordinate; y <= kMaxCoordinate; ++y) {
        for (int x = kMinCoordinate; x <= kMaxCoordinate; ++x)
            QCOMPARE(board.pieceAt({x, y}), std::nullopt);
    }
}

void TestBoard::placedPieceCanBeRead()
{
    m_board.place({3, 2}, Player::O);

    QCOMPARE(m_board.pieceAt({3, 2}), Player::O);
    QCOMPARE(m_board.pieceAt({2, 3}), std::nullopt); // Coordenadas no intercambiadas
    QVERIFY(!m_board.isEmpty());
}

void TestBoard::placingOutsideTheBoardThrows_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");
    QTest::addColumn<Axis>("axis");
    QTest::addColumn<int>("value");

    constexpr int minInt = std::numeric_limits<int>::min();
    constexpr int maxInt = std::numeric_limits<int>::max();

    // Valores límite justo fuera del rango [1, 3] y valores extremos de int.
    QTest::newRow("x = 0") << 0 << 2 << Axis::X << 0;
    QTest::newRow("x = 4") << 4 << 2 << Axis::X << 4;
    QTest::newRow("x = INT_MIN") << minInt << 2 << Axis::X << minInt;
    QTest::newRow("x = INT_MAX") << maxInt << 2 << Axis::X << maxInt;
    QTest::newRow("y = 0") << 2 << 0 << Axis::Y << 0;
    QTest::newRow("y = 4") << 2 << 4 << Axis::Y << 4;
    QTest::newRow("y = INT_MIN") << 2 << minInt << Axis::Y << minInt;
    QTest::newRow("y = INT_MAX") << 2 << maxInt << Axis::Y << maxInt;
}

void TestBoard::placingOutsideTheBoardThrows()
{
    QFETCH(int, x);
    QFETCH(int, y);
    QFETCH(Axis, axis);
    QFETCH(int, value);

    try {
        m_board.place({x, y}, Player::X);
        QFAIL("Se esperaba OutOfBoardException");
    } catch (const OutOfBoardException &exception) {
        QCOMPARE(exception.axis(), axis);
        QCOMPARE(exception.value(), value);
    }
    QVERIFY(m_board.isEmpty());
}

void TestBoard::readingOutsideTheBoardThrows()
{
    QVERIFY_THROWS_EXCEPTION(OutOfBoardException, std::ignore = m_board.pieceAt({0, 1}));
    QVERIFY_THROWS_EXCEPTION(OutOfBoardException, std::ignore = m_board.pieceAt({1, 4}));
}

void TestBoard::whenBothAxesAreOutsideTheXAxisIsReported()
{
    try {
        m_board.place({9, 9}, Player::X);
        QFAIL("Se esperaba OutOfBoardException");
    } catch (const OutOfBoardException &exception) {
        QCOMPARE(exception.axis(), Axis::X);
    }
}

void TestBoard::occupiedCellKeepsTheOriginalPiece()
{
    m_board.place({1, 1}, Player::X);

    try {
        m_board.place({1, 1}, Player::O);
        QFAIL("Se esperaba OccupiedCellException");
    } catch (const OccupiedCellException &exception) {
        QCOMPARE(exception.position(), (Position{1, 1}));
        QCOMPARE(exception.occupant(), Player::X);
    }
    QCOMPARE(m_board.pieceAt({1, 1}), Player::X);
}

void TestBoard::boardIsFullOnlyAfterNinePieces()
{
    int placed = 0;
    for (int y = kMinCoordinate; y <= kMaxCoordinate; ++y) {
        for (int x = kMinCoordinate; x <= kMaxCoordinate; ++x) {
            QVERIFY(!m_board.isFull());
            m_board.place({x, y}, placed % 2 == 0 ? Player::X : Player::O);
            ++placed;
        }
    }

    QCOMPARE(placed, 9);
    QVERIFY(m_board.isFull());
}

void TestBoard::clearEmptiesEveryCell()
{
    m_board.place({1, 1}, Player::X);
    m_board.place({3, 3}, Player::O);

    m_board.clear();

    QVERIFY(m_board.isEmpty());
    QCOMPARE(m_board.pieceAt({1, 1}), std::nullopt);
    QCOMPARE(m_board.pieceAt({3, 3}), std::nullopt);
}

QTEST_APPLESS_MAIN(TestBoard)
#include "tst_board.moc"
