/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoUndoStackAction.h"

#include <KoIcon.h>

#include <KLocalizedString>
#include <kstandardshortcut.h>
#include <kundo2stack.h>

KoUndoStackAction::KoUndoStackAction(KUndo2Stack *stack, Type type)
    : QAction(stack)
    , m_type(type)
{
    if (m_type == UNDO) {
        connect(this, &QAction::triggered, stack, &KUndo2QStack::undo);
        connect(stack, &KUndo2QStack::canUndoChanged, this, &QAction::setEnabled);
        connect(stack, &KUndo2QStack::undoTextChanged, this, &KoUndoStackAction::slotUndoTextChanged);
        setIcon(koIcon("edit-undo"));
        setText(i18n("Undo"));
        setShortcuts(KStandardShortcut::undo());
        setEnabled(stack->canUndo());
    } else {
        connect(this, &QAction::triggered, stack, &KUndo2QStack::redo);
        connect(stack, &KUndo2QStack::canRedoChanged, this, &QAction::setEnabled);
        connect(stack, &KUndo2QStack::redoTextChanged, this, &KoUndoStackAction::slotUndoTextChanged);
        setIcon(koIcon("edit-redo"));
        setText(i18n("Redo"));
        setShortcuts(KStandardShortcut::redo());
        setEnabled(stack->canRedo());
    }
}

void KoUndoStackAction::slotUndoTextChanged(const QString &text)
{
    QString actionText = (m_type == UNDO) ? i18n("Undo %1", text) : i18n("Redo %1", text);
    setToolTip(actionText);
}
