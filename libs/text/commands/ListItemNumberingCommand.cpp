/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ListItemNumberingCommand.h"

#include <KLocalizedString>

#include <KoParagraphStyle.h>
#include <KoTextBlockData.h>
#include <QTextCursor>

ListItemNumberingCommand::ListItemNumberingCommand(const QTextBlock &block, bool numbered, KUndo2Command *parent)
    : KoTextCommandBase(parent)
    , m_block(block)
    , m_numbered(numbered)
    , m_first(true)
{
    m_wasNumbered = !block.blockFormat().boolProperty(KoParagraphStyle::UnnumberedListItem);
    setText(kundo2_i18n("Change List Numbering"));
}

ListItemNumberingCommand::~ListItemNumberingCommand() = default;

void ListItemNumberingCommand::setNumbered(bool numbered)
{
    QTextCursor cursor(m_block);
    QTextBlockFormat blockFormat = cursor.blockFormat();
    if (numbered) {
        blockFormat.clearProperty(KoParagraphStyle::UnnumberedListItem);
    } else {
        blockFormat.setProperty(KoParagraphStyle::UnnumberedListItem, true);
    }
    cursor.setBlockFormat(blockFormat);

    KoTextBlockData data(m_block);
    data.setCounterWidth(-1.0);
}

void ListItemNumberingCommand::redo()
{
    if (!m_first) {
        KoTextCommandBase::redo();
        UndoRedoFinalizer finalizer(this);

        KoTextBlockData data(m_block);
        data.setCounterWidth(-1.0);
    } else {
        setNumbered(m_numbered);
    }
    m_first = false;
}

void ListItemNumberingCommand::undo()
{
    KoTextCommandBase::undo();
    UndoRedoFinalizer finalizer(this);

    KoTextBlockData data(m_block);
    data.setCounterWidth(-1.0);
}

bool ListItemNumberingCommand::mergeWith(const KUndo2Command *other)
{
    Q_UNUSED(other);
    return false;
}
