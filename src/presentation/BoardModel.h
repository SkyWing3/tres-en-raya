#pragma once

#include "tresenraya/tictactoe/TicTacToe.h"

#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>

/// Expone las nueve casillas del tablero como un modelo de lista para QML.
///
/// El índice de la lista recorre el tablero por filas:
///   0 1 2
///   3 4 5
///   6 7 8
/// Los roles "column" y "row" usan las mismas coordenadas que el dominio (1 a 3).
class BoardModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("BoardModel se obtiene desde GameController.board")

public:
    enum Role {
        PieceRole = Qt::UserRole + 1,
        ColumnRole,
        RowRole,
        WinningRole,
    };
    Q_ENUM(Role)

    explicit BoardModel(const tresenraya::tictactoe::TicTacToe &game, QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    /// Notifica a las vistas que el contenido del tablero cambió.
    void refresh();

    [[nodiscard]] static tresenraya::tictactoe::Position positionAt(int cellIndex);
    [[nodiscard]] static int cellIndexOf(tresenraya::tictactoe::Position position);

private:
    [[nodiscard]] bool isWinningCell(tresenraya::tictactoe::Position position) const;

    const tresenraya::tictactoe::TicTacToe &m_game;
};
