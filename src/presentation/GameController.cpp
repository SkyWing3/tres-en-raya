#include "GameController.h"

#include "tresenraya/tictactoe/GameExceptions.h"

using namespace tresenraya::tictactoe;

namespace {

QString symbolText(Player player)
{
    return QString(QChar::fromLatin1(symbolOf(player)));
}

GameController::Status toStatus(GameStatus status)
{
    switch (status) {
    case GameStatus::Won:
        return GameController::Status::Won;
    case GameStatus::Draw:
        return GameController::Status::Draw;
    case GameStatus::InProgress:
        break;
    }
    return GameController::Status::InProgress;
}

} // namespace

GameController::GameController(QObject *parent)
    : QObject(parent)
{
}

BoardModel *GameController::board()
{
    return &m_board;
}

GameController::Status GameController::status() const
{
    return toStatus(m_game.status());
}

bool GameController::isGameOver() const
{
    return m_game.isOver();
}

QString GameController::nextPlayer() const
{
    return symbolText(m_game.nextPlayer());
}

QString GameController::winner() const
{
    const auto winner = m_game.winner();
    return winner ? symbolText(*winner) : QString();
}

QList<int> GameController::winningCells() const
{
    QList<int> cells;
    if (const auto line = m_game.winningLine()) {
        for (const auto &position : *line)
            cells.append(BoardModel::cellIndexOf(position));
    }
    return cells;
}

int GameController::xWins() const
{
    return m_xWins;
}

int GameController::oWins() const
{
    return m_oWins;
}

int GameController::draws() const
{
    return m_draws;
}

int GameController::minCoordinate()
{
    return kMinCoordinate;
}

int GameController::maxCoordinate()
{
    return kMaxCoordinate;
}

bool GameController::play(int x, int y)
{
    const Player player = m_game.nextPlayer();
    GameStatus result = GameStatus::InProgress;

    try {
        result = m_game.play({x, y});
    } catch (const OutOfBoardException &exception) {
        emit moveRejected(tr("La pieza está fuera del eje %1: %2 no está entre %3 y %4.")
                              .arg(QChar::fromLatin1(static_cast<char>(exception.axis())))
                              .arg(exception.value())
                              .arg(kMinCoordinate)
                              .arg(kMaxCoordinate));
        return false;
    } catch (const OccupiedCellException &exception) {
        emit moveRejected(tr("La casilla (%1, %2) ya está ocupada por %3.")
                              .arg(exception.position().x)
                              .arg(exception.position().y)
                              .arg(symbolText(exception.occupant())));
        return false;
    } catch (const GameOverException &) {
        emit moveRejected(tr("La partida terminó. Empieza un juego nuevo para seguir jugando."));
        return false;
    }

    m_board.refresh();
    emit piecePlaced(BoardModel::cellIndexOf({x, y}), symbolText(player));
    emit stateChanged();

    if (m_game.isOver()) {
        updateScore(result);
        emit gameFinished(toStatus(result), winner());
    }
    return true;
}

bool GameController::playCell(int cellIndex)
{
    const Position position = BoardModel::positionAt(cellIndex);
    return play(position.x, position.y);
}

void GameController::newGame()
{
    m_game.reset();
    m_board.refresh();
    emit stateChanged();
}

void GameController::resetScore()
{
    m_xWins = 0;
    m_oWins = 0;
    m_draws = 0;
    emit scoreChanged();
}

void GameController::updateScore(GameStatus status)
{
    if (status == GameStatus::Draw)
        ++m_draws;
    else if (m_game.winner() == Player::X)
        ++m_xWins;
    else
        ++m_oWins;
    emit scoreChanged();
}
