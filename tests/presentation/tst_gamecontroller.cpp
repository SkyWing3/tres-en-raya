// Pruebas del ViewModel de Tres en Raya: verifica que la interfaz recibe las
// notificaciones correctas y que las excepciones del dominio no escapan a QML.

#include "GameController.h"

#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <QtTest>

class TestGameController : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void boardModelIsConsistent();
    void startsWithXAndAnEmptyBoard();
    void validMoveUpdatesBoardAndTurn();
    void outOfBoardMoveIsRejectedWithAMessage_data();
    void outOfBoardMoveIsRejectedWithAMessage();
    void occupiedCellIsRejected();
    void winIsReportedAndScored();
    void drawIsScored();
    void newGameKeepsTheScore();
    void resetScoreClearsCounters();

private:
    static QString pieceAt(GameController &controller, int cellIndex);
    static void playCells(GameController &controller, const QList<int> &cells);

    std::unique_ptr<GameController> m_controller;
};

void TestGameController::init()
{
    m_controller = std::make_unique<GameController>();
}

void TestGameController::cleanup()
{
    m_controller.reset();
}

QString TestGameController::pieceAt(GameController &controller, int cellIndex)
{
    const auto *model = controller.board();
    return model->data(model->index(cellIndex), BoardModel::PieceRole).toString();
}

void TestGameController::playCells(GameController &controller, const QList<int> &cells)
{
    for (const int cell : cells)
        QVERIFY(controller.playCell(cell));
}

void TestGameController::boardModelIsConsistent()
{
    QAbstractItemModelTester tester(m_controller->board(),
                                    QAbstractItemModelTester::FailureReportingMode::QtTest);
    m_controller->play(2, 2);
    m_controller->newGame();
}

void TestGameController::startsWithXAndAnEmptyBoard()
{
    QCOMPARE(m_controller->nextPlayer(), QStringLiteral("X"));
    QCOMPARE(m_controller->status(), GameController::Status::InProgress);
    QCOMPARE(m_controller->board()->rowCount(), 9);
    for (int cell = 0; cell < 9; ++cell)
        QVERIFY(pieceAt(*m_controller, cell).isEmpty());
}

void TestGameController::validMoveUpdatesBoardAndTurn()
{
    QSignalSpy stateSpy(m_controller.get(), &GameController::stateChanged);
    QSignalSpy placedSpy(m_controller.get(), &GameController::piecePlaced);

    QVERIFY(m_controller->play(3, 1));

    QCOMPARE(pieceAt(*m_controller, 2), QStringLiteral("X"));
    QCOMPARE(m_controller->nextPlayer(), QStringLiteral("O"));
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(placedSpy.count(), 1);
    QCOMPARE(placedSpy.first().at(0).toInt(), 2);
    QCOMPARE(placedSpy.first().at(1).toString(), QStringLiteral("X"));
}

void TestGameController::outOfBoardMoveIsRejectedWithAMessage_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");
    QTest::addColumn<QString>("axis");

    QTest::newRow("fuera del eje X") << 4 << 1 << QStringLiteral("eje X");
    QTest::newRow("fuera del eje Y") << 1 << 0 << QStringLiteral("eje Y");
}

void TestGameController::outOfBoardMoveIsRejectedWithAMessage()
{
    QFETCH(int, x);
    QFETCH(int, y);
    QFETCH(QString, axis);
    QSignalSpy rejectedSpy(m_controller.get(), &GameController::moveRejected);
    QSignalSpy stateSpy(m_controller.get(), &GameController::stateChanged);

    QVERIFY(!m_controller->play(x, y));

    QCOMPARE(rejectedSpy.count(), 1);
    QVERIFY(rejectedSpy.first().at(0).toString().contains(axis));
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(m_controller->nextPlayer(), QStringLiteral("X"));
}

void TestGameController::occupiedCellIsRejected()
{
    QSignalSpy rejectedSpy(m_controller.get(), &GameController::moveRejected);

    QVERIFY(m_controller->play(1, 1));
    QVERIFY(!m_controller->play(1, 1));

    QCOMPARE(rejectedSpy.count(), 1);
    QVERIFY(rejectedSpy.first().at(0).toString().contains(QStringLiteral("ocupada")));
}

void TestGameController::winIsReportedAndScored()
{
    QSignalSpy finishedSpy(m_controller.get(), &GameController::gameFinished);

    // X: 0, 1, 2 (fila superior) | +: 3, 4
    playCells(*m_controller, {0, 3, 1, 4, 2});

    QCOMPARE(m_controller->status(), GameController::Status::Won);
    QCOMPARE(m_controller->winner(), QStringLiteral("X"));
    QCOMPARE(m_controller->winningCells(), (QList<int>{0, 1, 2}));
    QCOMPARE(m_controller->xWins(), 1);
    QCOMPARE(finishedSpy.count(), 1);

    const auto *model = m_controller->board();
    QVERIFY(model->data(model->index(1), BoardModel::WinningRole).toBool());
    QVERIFY(!model->data(model->index(4), BoardModel::WinningRole).toBool());
}

void TestGameController::drawIsScored()
{
    playCells(*m_controller, {0, 1, 2, 4, 3, 5, 7, 6, 8});

    QCOMPARE(m_controller->status(), GameController::Status::Draw);
    QVERIFY(m_controller->winner().isEmpty());
    QCOMPARE(m_controller->draws(), 1);
}

void TestGameController::newGameKeepsTheScore()
{
    playCells(*m_controller, {0, 3, 1, 4, 2});

    m_controller->newGame();

    QCOMPARE(m_controller->status(), GameController::Status::InProgress);
    QCOMPARE(m_controller->nextPlayer(), QStringLiteral("X"));
    QVERIFY(pieceAt(*m_controller, 0).isEmpty());
    QCOMPARE(m_controller->xWins(), 1);
}

void TestGameController::resetScoreClearsCounters()
{
    playCells(*m_controller, {0, 3, 1, 4, 2});
    QSignalSpy scoreSpy(m_controller.get(), &GameController::scoreChanged);

    m_controller->resetScore();

    QCOMPARE(scoreSpy.count(), 1);
    QCOMPARE(m_controller->xWins(), 0);
    QCOMPARE(m_controller->oWins(), 0);
    QCOMPARE(m_controller->draws(), 0);
}

QTEST_GUILESS_MAIN(TestGameController)
#include "tst_gamecontroller.moc"
