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
    void oWinIsScored();
    void drawIsScored();
    void newGameKeepsTheScore();
    void resetScoreClearsCounters();
    void playCellMapsEachIndexToItsCoordinates_data();
    void playCellMapsEachIndexToItsCoordinates();
    void boardModelExposesRolesForQml();
    void winningCellsIsEmptyWhileInProgress();
    void moveAfterTheGameIsOverIsRejected();
    void newGameRefreshesTheBoardModel();
    void coordinateLimitsMatchTheDomain();
    void boardModelIgnoresInvalidRequests();

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

void TestGameController::oWinIsScored()
{
    // X: 0, 1, 8 | O: 3, 4, 5 (fila central)
    playCells(*m_controller, {0, 3, 1, 4, 8, 5});

    QCOMPARE(m_controller->winner(), QStringLiteral("O"));
    QCOMPARE(m_controller->winningCells(), (QList<int>{3, 4, 5}));
    QCOMPARE(m_controller->oWins(), 1);
    QCOMPARE(m_controller->xWins(), 0);
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

void TestGameController::playCellMapsEachIndexToItsCoordinates_data()
{
    QTest::addColumn<int>("cellIndex");
    QTest::addColumn<int>("column");
    QTest::addColumn<int>("row");

    for (int cell = 0; cell < 9; ++cell)
        QTest::addRow("casilla %d", cell) << cell << cell % 3 + 1 << cell / 3 + 1;
}

void TestGameController::playCellMapsEachIndexToItsCoordinates()
{
    QFETCH(int, cellIndex);
    QFETCH(int, column);
    QFETCH(int, row);
    QSignalSpy placedSpy(m_controller.get(), &GameController::piecePlaced);

    QVERIFY(m_controller->playCell(cellIndex));

    const auto *model = m_controller->board();
    const QModelIndex index = model->index(cellIndex);
    QCOMPARE(model->data(index, BoardModel::ColumnRole).toInt(), column);
    QCOMPARE(model->data(index, BoardModel::RowRole).toInt(), row);
    QCOMPARE(pieceAt(*m_controller, cellIndex), QStringLiteral("X"));
    QCOMPARE(placedSpy.first().at(0).toInt(), cellIndex);

    // La misma casilla, pedida por coordenadas, ya está ocupada.
    QVERIFY(!m_controller->play(column, row));
}

void TestGameController::boardModelExposesRolesForQml()
{
    const QHash<int, QByteArray> roles = m_controller->board()->roleNames();

    QCOMPARE(roles.value(BoardModel::PieceRole), QByteArray("piece"));
    QCOMPARE(roles.value(BoardModel::ColumnRole), QByteArray("column"));
    QCOMPARE(roles.value(BoardModel::RowRole), QByteArray("row"));
    QCOMPARE(roles.value(BoardModel::WinningRole), QByteArray("winning"));
}

void TestGameController::winningCellsIsEmptyWhileInProgress()
{
    playCells(*m_controller, {0, 4});

    QVERIFY(m_controller->winningCells().isEmpty());
    QVERIFY(m_controller->winner().isEmpty());
    QVERIFY(!m_controller->isGameOver());
}

void TestGameController::moveAfterTheGameIsOverIsRejected()
{
    playCells(*m_controller, {0, 3, 1, 4, 2});
    QSignalSpy rejectedSpy(m_controller.get(), &GameController::moveRejected);

    QVERIFY(!m_controller->playCell(8));

    QCOMPARE(rejectedSpy.count(), 1);
    QVERIFY(rejectedSpy.first().at(0).toString().contains(QStringLiteral("terminó")));
    QVERIFY(pieceAt(*m_controller, 8).isEmpty());
}

void TestGameController::newGameRefreshesTheBoardModel()
{
    playCells(*m_controller, {0, 4});
    QSignalSpy dataSpy(m_controller->board(), &QAbstractItemModel::dataChanged);

    m_controller->newGame();

    QCOMPARE(dataSpy.count(), 1);
    for (int cell = 0; cell < 9; ++cell)
        QVERIFY(pieceAt(*m_controller, cell).isEmpty());
}

void TestGameController::coordinateLimitsMatchTheDomain()
{
    QCOMPARE(GameController::minCoordinate(), 1);
    QCOMPARE(GameController::maxCoordinate(), 3);
}

void TestGameController::boardModelIgnoresInvalidRequests()
{
    const auto *model = m_controller->board();

    QVERIFY(!model->data(QModelIndex(), BoardModel::PieceRole).isValid());
    QVERIFY(!model->data(model->index(0), Qt::DisplayRole).isValid());
    QCOMPARE(model->rowCount(model->index(0)), 0); // Es una lista: sin hijos
}

QTEST_GUILESS_MAIN(TestGameController)
#include "tst_gamecontroller.moc"
