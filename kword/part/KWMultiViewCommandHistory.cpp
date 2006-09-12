/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWMultiViewCommandHistory.h"

#include <kaction.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kdebug.h>

void KWMultiViewCommandHistory::undo() {
    KCommandHistory::undo();
    updateActions();
}

void KWMultiViewCommandHistory::redo() {
    KCommandHistory::redo();
    updateActions();
}

void KWMultiViewCommandHistory::updateActions() {
    KCommand *current = presentCommand();
    foreach(QPointer<KAction> p, m_undoCommands) {
        if(p.isNull()) {
            m_undoCommands.removeAll(p);
            continue;
        }
        p->setEnabled(isUndoAvailable());
        if(current)
            p->setText(i18n("Undo %1", current->name()));
        else
            p->setText(i18n("No Undo"));
    }
    current = 0;
    if(isRedoAvailable())
        current = redoCommands(1)[0];
    foreach(QPointer<KAction> p, m_redoCommands) {
        if(p.isNull()) {
            m_undoCommands.removeAll(p);
            continue;
        }
        p->setEnabled(isRedoAvailable());
        if(current)
            p->setText(i18n("Redo %1", current->name()));
        else
            p->setText(i18n("No Redo"));
    }
}

KAction *KWMultiViewCommandHistory::createUndoAction(KActionCollection *actionCollection) {
    KAction *undo = KStdAction::undo(0, 0, actionCollection, "koffice_undo");
    undo->setEnabled(isUndoAvailable());
    QPointer<KAction> p(undo);
    m_undoCommands.append(p);
    return undo;
}

KAction *KWMultiViewCommandHistory::createRedoAction(KActionCollection *actionCollection) {
    KAction *redo = KStdAction::redo(0, 0, actionCollection, "koffice_redo");
    redo->setEnabled(isRedoAvailable());
    QPointer<KAction> p(redo);
    m_redoCommands.append(p);
    return redo;
}

void KWMultiViewCommandHistory::addCommand2(KCommand *command, bool execute) {
    KCommandHistory::addCommand(command, execute);
    updateActions();
}
