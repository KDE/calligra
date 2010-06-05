/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_POINT_STORAGE_UNDO_COMMAND
#define KSPREAD_POINT_STORAGE_UNDO_COMMAND

// Qt
#include <QAbstractItemModel>
#include <QPair>
#include <QUndoCommand>
#include <QVector>

// KSpread
#include "Formula.h"

namespace KSpread
{

/**
 * An undo command for PointStorage data.
 *
 * Implements undo functionality only. Glue it to another command,
 * that provides the appropriate applying (redoing).
 *
 * Used for recording undo data in CellStorage.
 */
template<typename T>
class PointStorageUndoCommand : public QUndoCommand
{
public:
    typedef QPair<QPoint, T> Pair;

    PointStorageUndoCommand(QAbstractItemModel *const model, int role, QUndoCommand *parent = 0);

    virtual void undo();

    void add(const QVector<Pair> &pairs);

    PointStorageUndoCommand& operator<<(const Pair &pair);
    PointStorageUndoCommand& operator<<(const QVector<Pair> &pairs);

private:
    QAbstractItemModel *const m_model;
    const int m_role;
    QVector<Pair> m_undoData;
};

template<typename T>
PointStorageUndoCommand<T>::PointStorageUndoCommand(QAbstractItemModel *const model,
                                                    int role, QUndoCommand *parent)
    : QUndoCommand(parent)
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
    QUndoCommand::undo(); // undo possible child commands
}

template<typename T>
void PointStorageUndoCommand<T>::add(const QVector<Pair>& pairs)
{
    m_undoData << pairs;
}

template<typename T>
PointStorageUndoCommand<T>& PointStorageUndoCommand<T>::operator<<(const Pair& pair)
{
    m_undoData << pair;
    return *this;
}

template<typename T>
PointStorageUndoCommand<T>& PointStorageUndoCommand<T>::operator<<(const QVector<Pair>& pairs)
{
    m_undoData << pairs;
    return *this;
}

} // namespace KSpread

#endif // KSPREAD_POINT_STORAGE_UNDO_COMMAND
