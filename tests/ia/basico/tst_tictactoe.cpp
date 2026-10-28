#include <QtTest>
#include <stdexcept>
#include "TicTacToe.h"

class TicTacToeTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // Requerimiento 1
    void whenXOutsideBoardThenException();
    void whenYOutsideBoardThenException();
    void whenOccupiedThenException();

    // Requerimiento 2
    void givenFirstTurnWhenNextPlayerThenX();
    void givenLastTurnWasXWhenNextPlayerThenO();
    void givenLastTurnWasOWhenNextPlayerThenX();

    // Requerimiento 3
    void whenPlayThenNoWinner();
    void whenPlayAndWholeHorizontalLineThenWinner();
    void whenPlayAndWholeVerticalLineThenWinner();
    void whenPlayAndTopBottomDiagonalLineThenWinner();
    void whenPlayAndBottomTopDiagonalLineThenWinner();

private:
    TicTacToe *ticTacToe = nullptr;
};

void TicTacToeTest::init()
{
    ticTacToe = new TicTacToe();
}

void TicTacToeTest::cleanup()
{
    delete ticTacToe;
}

// ---------- Requerimiento 1 ----------

void TicTacToeTest::whenXOutsideBoardThenException()
{
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, ticTacToe->play(5, 2));
}

void TicTacToeTest::whenYOutsideBoardThenException()
{
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, ticTacToe->play(2, 5));
}

void TicTacToeTest::whenOccupiedThenException()
{
    ticTacToe->play(2, 1);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, ticTacToe->play(2, 1));
}

// ---------- Requerimiento 2 ----------

void TicTacToeTest::givenFirstTurnWhenNextPlayerThenX()
{
    QCOMPARE(ticTacToe->nextPlayer(), 'X');
}

void TicTacToeTest::givenLastTurnWasXWhenNextPlayerThenO()
{
    ticTacToe->play(1, 1);
    QCOMPARE(ticTacToe->nextPlayer(), 'O');
}

void TicTacToeTest::givenLastTurnWasOWhenNextPlayerThenX()
{
    ticTacToe->play(1, 1);
    ticTacToe->play(2, 2);
    QCOMPARE(ticTacToe->nextPlayer(), 'X');
}

// ---------- Requerimiento 3 ----------

void TicTacToeTest::whenPlayThenNoWinner()
{
    QString actual = ticTacToe->play(1, 1);
    QCOMPARE(actual, QString("No hay ganador"));
}

void TicTacToeTest::whenPlayAndWholeHorizontalLineThenWinner()
{
    ticTacToe->play(1, 1); // X
    ticTacToe->play(1, 2); // O
    ticTacToe->play(2, 1); // X
    ticTacToe->play(2, 2); // O
    QString actual = ticTacToe->play(3, 1); // X
    QCOMPARE(actual, QString("X es el ganador"));
}

void TicTacToeTest::whenPlayAndWholeVerticalLineThenWinner()
{
    ticTacToe->play(2, 1); // X
    ticTacToe->play(1, 1); // O
    ticTacToe->play(3, 1); // X
    ticTacToe->play(1, 2); // O
    ticTacToe->play(2, 2); // X
    QString actual = ticTacToe->play(1, 3); // O
    QCOMPARE(actual, QString("O es el ganador"));
}

void TicTacToeTest::whenPlayAndTopBottomDiagonalLineThenWinner()
{
    ticTacToe->play(1, 1); // X
    ticTacToe->play(1, 2); // O
    ticTacToe->play(2, 2); // X
    ticTacToe->play(1, 3); // O
    QString actual = ticTacToe->play(3, 3); // X
    QCOMPARE(actual, QString("X es el ganador"));
}

void TicTacToeTest::whenPlayAndBottomTopDiagonalLineThenWinner()
{
    ticTacToe->play(1, 3); // X
    ticTacToe->play(1, 1); // O
    ticTacToe->play(2, 2); // X
    ticTacToe->play(1, 2); // O
    QString actual = ticTacToe->play(3, 1); // X
    QCOMPARE(actual, QString("X es el ganador"));
}

QTEST_APPLESS_MAIN(TicTacToeTest)
#include "tst_tictactoe.moc"
