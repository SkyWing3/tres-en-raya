#pragma once

#include "BoardModel.h"

#include "tresenraya/tictactoe/TicTacToe.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QtQmlIntegration/qqmlintegration.h>

/// ViewModel de la partida de Tres en Raya.
///
/// Traduce las jugadas de la interfaz al dominio, convierte las excepciones en
/// mensajes para el usuario y lleva el marcador entre partidas.
class GameController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(BoardModel *board READ board CONSTANT FINAL)
    Q_PROPERTY(Status status READ status NOTIFY stateChanged FINAL)
    Q_PROPERTY(bool gameOver READ isGameOver NOTIFY stateChanged FINAL)
    Q_PROPERTY(QString nextPlayer READ nextPlayer NOTIFY stateChanged FINAL)
    Q_PROPERTY(QString winner READ winner NOTIFY stateChanged FINAL)
    Q_PROPERTY(QList<int> winningCells READ winningCells NOTIFY stateChanged FINAL)
    Q_PROPERTY(int xWins READ xWins NOTIFY scoreChanged FINAL)
    Q_PROPERTY(int oWins READ oWins NOTIFY scoreChanged FINAL)
    Q_PROPERTY(int draws READ draws NOTIFY scoreChanged FINAL)
    Q_PROPERTY(int minCoordinate READ minCoordinate CONSTANT FINAL)
    Q_PROPERTY(int maxCoordinate READ maxCoordinate CONSTANT FINAL)

public:
    enum class Status {
        InProgress,
        Won,
        Draw,
    };
    Q_ENUM(Status)

    explicit GameController(QObject *parent = nullptr);

    [[nodiscard]] BoardModel *board();
    [[nodiscard]] Status status() const;
    [[nodiscard]] bool isGameOver() const;
    [[nodiscard]] QString nextPlayer() const;
    [[nodiscard]] QString winner() const;
    [[nodiscard]] QList<int> winningCells() const;
    [[nodiscard]] int xWins() const;
    [[nodiscard]] int oWins() const;
    [[nodiscard]] int draws() const;
    [[nodiscard]] static int minCoordinate();
    [[nodiscard]] static int maxCoordinate();

    /// Juega en la columna x y la fila y (de 1 a 3).
    /// Devuelve false y emite moveRejected si la jugada no es válida.
    Q_INVOKABLE bool play(int x, int y);

    /// Igual que play(), pero con el índice de casilla del BoardModel (0 a 8).
    Q_INVOKABLE bool playCell(int cellIndex);

    Q_INVOKABLE void newGame();
    Q_INVOKABLE void resetScore();

signals:
    void stateChanged();
    void scoreChanged();
    void piecePlaced(int cellIndex, const QString &piece);
    void moveRejected(const QString &reason);
    void gameFinished(GameController::Status status, const QString &winner);

private:
    void updateScore(tresenraya::tictactoe::GameStatus status);

    tresenraya::tictactoe::TicTacToe m_game;
    BoardModel m_board{m_game};
    int m_xWins = 0;
    int m_oWins = 0;
    int m_draws = 0;
};
