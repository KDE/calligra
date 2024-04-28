/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_RECT_STORAGE_UNDO_COMMAND
#define KSPREAD_RECT_STORAGE_UNDO_COMMAND

// Qt
#include <QPair>
#include <QVector>
#include <kundo2command.h>

// Sheets
#include "ModelSupport.h"
#include "SheetModel.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief An undo command for RectStorage data.
 *
 * Implements undo functionality only. Glue it to another command,
 * that provides the appropriate applying (redoing).
 *
 * Used for recording undo data in CellStorage.
 */
template<typename T>
class RectStorageUndoCommand : public KUndo2Command
{
public:
    typedef QPair<QRectF, T> Pair;

    RectStorageUndoCommand(QAbstractItemModel *const model, int role, KUndo2Command *parent = 0);

    void undo() override;

    void add(const QVector<Pair> &pairs);

    RectStorageUndoCommand &operator<<(const Pair &pair);
    RectStorageUndoCommand &operator<<(const QVector<Pair> &pairs);

private:
    QAbstractItemModel *const m_model;
    const int m_role;
    QVector<Pair> m_undoData;
};

template<typename T>
RectStorageUndoCommand<T>::RectStorageUndoCommand(QAbstractItemModel *const model, int role, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_model(model)
    , m_role(role)
{
}

template<typename T>
void RectStorageUndoCommand<T>::undo()
{
    SheetModel *const model = static_cast<SheetModel *>(m_model);
    for (int i = 0; i < m_undoData.count(); ++i) {
        QVariant data;
        data.setValue(m_undoData[i].second);
        model->setData(fromRange(m_undoData[i].first.toRect(), model), data, m_role);
    }
    KUndo2Command::undo(); // undo possible child commands
}

template<typename T>
void RectStorageUndoCommand<T>::add(const QVector<Pair> &pairs)
{
    m_undoData << pairs;
}

template<typename T>
RectStorageUndoCommand<T> &RectStorageUndoCommand<T>::operator<<(const Pair &pair)
{
    m_undoData << pair;
    return *this;
}

template<typename T>
RectStorageUndoCommand<T> &RectStorageUndoCommand<T>::operator<<(const QVector<Pair> &pairs)
{
    m_undoData << pairs;
    return *this;
}

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_RECT_STORAGE_UNDO_COMMAND
