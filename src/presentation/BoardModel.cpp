#include "BoardModel.h"

#include <algorithm>

using namespace tresenraya::tictactoe;

BoardModel::BoardModel(const TicTacToe &game, QObject *parent)
    : QAbstractListModel(parent)
    , m_game(game)
{
}

int BoardModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(Board::kCellCount);
}

QVariant BoardModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid))
        return {};

    const Position position = positionAt(index.row());
    switch (role) {
    case PieceRole: {
        const auto piece = m_game.board().pieceAt(position);
        return piece ? QString(QChar::fromLatin1(symbolOf(*piece))) : QString();
    }
    case ColumnRole:
        return position.x;
    case RowRole:
        return position.y;
    case WinningRole:
        return isWinningCell(position);
    default:
        return {};
    }
}

QHash<int, QByteArray> BoardModel::roleNames() const
{
    return {
        {PieceRole, "piece"},
        {ColumnRole, "column"},
        {RowRole, "row"},
        {WinningRole, "winning"},
    };
}

void BoardModel::refresh()
{
    emit dataChanged(index(0), index(rowCount() - 1), {PieceRole, WinningRole});
}

Position BoardModel::positionAt(int cellIndex)
{
    return {cellIndex % kBoardSize + kMinCoordinate, cellIndex / kBoardSize + kMinCoordinate};
}

int BoardModel::cellIndexOf(Position position)
{
    return (position.y - kMinCoordinate) * kBoardSize + (position.x - kMinCoordinate);
}

bool BoardModel::isWinningCell(Position position) const
{
    const auto line = m_game.winningLine();
    return line && std::ranges::find(*line, position) != line->end();
}
