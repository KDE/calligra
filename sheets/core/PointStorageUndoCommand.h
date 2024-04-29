/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_POINT_STORAGE_UNDO_COMMAND
#define KSPREAD_POINT_STORAGE_UNDO_COMMAND

// Qt
#include <QAbstractItemModel>
#include <QPair>
#include <QVector>
#include <kundo2command.h>

// Sheets
#include "Formula.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief An undo command for PointStorage data.
 *
 * Implements undo functionality only. Glue it to another command,
 * that provides the appropriate applying (redoing).
 *
 * Used for recording undo data in CellStorage.
 */
template<typename T>
class PointStorageUndoCommand : public KUndo2Command
{
public:
    typedef QPair<QPoint, T> Pair;

    PointStorageUndoCommand(QAbstractItemModel *const model, int role, KUndo2Command *parent = nullptr);

    void undo() override;

    void add(const QVector<Pair> &pairs);

    PointStorageUndoCommand &operator<<(const Pair &pair);
    PointStorageUndoCommand &operator<<(const QVector<Pair> &pairs);

private:
    QAbstractItemModel *const m_model;
    const int m_role;
    QVector<Pair> m_undoData;
};

template<typename T>
PointStorageUndoCommand<T>::PointStorageUndoCommand(QAbstractItemModel *const model, int role, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_model(model)
    , m_role(role)
{
}

template<typename T>
void PointStorageUndoCommand<T>::undo()
{
    // In reverse order for the case that a location was altered multiple times.
    for (int i = m_undoData.count() - 1; i >= 0; --i) {
        const int column = m_undoData[i].first.x();
        const int row = m_undoData[i].first.y();
        const QModelIndex index = m_model->index(row - 1, column - 1);
        QVariant data;
        data.setValue(m_undoData[i].second);
        m_model->setData(index, data, m_role);
    }
    KUndo2Command::undo(); // undo possible child commands
}

template<typename T>
void PointStorageUndoCommand<T>::add(const QVector<Pair> &pairs)
{
    m_undoData << pairs;
}

template<typename T>
PointStorageUndoCommand<T> &PointStorageUndoCommand<T>::operator<<(const Pair &pair)
{
    m_undoData << pair;
    return *this;
}

template<typename T>
PointStorageUndoCommand<T> &PointStorageUndoCommand<T>::operator<<(const QVector<Pair> &pairs)
{
    m_undoData << pairs;
    return *this;
}

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_POINT_STORAGE_UNDO_COMMAND
