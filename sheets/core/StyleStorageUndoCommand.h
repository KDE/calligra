/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_STYLE_STORAGE_UNDO_COMMAND
#define KSPREAD_STYLE_STORAGE_UNDO_COMMAND

// Qt
#include <QPair>
#include <QVector>
#include <kundo2command.h>

// Sheets
#include "StyleStorage.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief An undo command for StyleStorage data.
 *
 * Implements undo functionality only. Glue it to another command,
 * that provides the appropriate applying (redoing).
 *
 * Used for recording undo data in CellStorage.
 */
class StyleStorageUndoCommand : public KUndo2Command
{
public:
    typedef QPair<QRectF, SharedSubStyle> Pair;

    explicit StyleStorageUndoCommand(StyleStorage *storage, KUndo2Command *parent = nullptr);

    void undo() override;

    void add(const QVector<Pair> &pairs);

    StyleStorageUndoCommand &operator<<(const Pair &pair);
    StyleStorageUndoCommand &operator<<(const QVector<Pair> &pairs);

private:
    StyleStorage *const m_storage;
    QVector<Pair> m_undoData;
};

StyleStorageUndoCommand::StyleStorageUndoCommand(StyleStorage *storage, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_storage(storage)
{
}

void StyleStorageUndoCommand::undo()
{
    for (int i = 0; i < m_undoData.count(); ++i) {
        QRect rect = m_undoData[i].first.toRect();
        m_storage->insert(rect, m_undoData[i].second);
    }
    KUndo2Command::undo(); // undo possible child commands
}

void StyleStorageUndoCommand::add(const QVector<Pair> &pairs)
{
    m_undoData << pairs;
}

StyleStorageUndoCommand &StyleStorageUndoCommand::operator<<(const Pair &pair)
{
    m_undoData << pair;
    return *this;
}

StyleStorageUndoCommand &StyleStorageUndoCommand::operator<<(const QVector<Pair> &pairs)
{
    m_undoData << pairs;
    return *this;
}

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_STYLE_STORAGE_UNDO_COMMAND
