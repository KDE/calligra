/* This file is part of the KDE project
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
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

#include "KWInsertInlineNoteDialog.h"
#include "../KWView.h"
#include "../KWDocument.h"
#include "../frames/KWTextFrameSet.h"

#include <QTextCursor>
#include <KLocale>
#include <KoInlineNote.h>
#include <KoCanvasBase.h>
#include <KoToolProxy.h>
#include <KoTextEditor.h>
#include <KoTextDocumentLayout.h>
#include <KoInlineTextObjectManager.h>

KWInsertInlineNoteDialog::KWInsertInlineNoteDialog(KWDocument *document, KWView *view)
    : KDialog(view),
    m_document(document),
    m_view(view)
{
    m_widget.setupUi(mainWidget());
    setCaption(i18n("Insert Footnote/Endnote"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setModal(true);
    
    m_widget.automaticNumbering->setChecked(true);
    m_widget.footnoteType->setChecked(true);

    connect(this, SIGNAL(okClicked()), this, SLOT(doIt()));
}

KWInsertInlineNoteDialog::~KWInsertInlineNoteDialog()
{
}

void KWInsertInlineNoteDialog::doIt()
{
    KoInlineNote *note = 0;
    if (m_widget.footnoteType->isChecked()) {
        note = new KoInlineNote(KoInlineNote::Footnote);
    }
    else if (m_widget.endnoteType->isChecked()) {
        note = new KoInlineNote(KoInlineNote::Endnote);
    }
    Q_ASSERT(note);

    if (m_widget.automaticNumbering->isChecked()) {
        note->setLabel("1"); //TODO hmmmm....
    }
    else if (m_widget.characterNumbering->isChecked()) {
        note->setLabel(m_widget.characterEdit->text());
    }

    note->setAutoNumbering(m_widget.automaticNumbering->isChecked());

    //note->setText(...);

    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_view->canvasBase());
    editor->insertInlineObject(note);
    
    //TODO activate note and move cursor to start adding the note
}
