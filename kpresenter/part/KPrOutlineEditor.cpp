/* This file is part of the KDE project
* Copyright (C) 2010 Ludovic Delfau <ludovicdelfau@gmail.com>
* Copyright (C) 2010 Julien Desgats <julien.desgats@gmail.com>
* Copyright (C) 2010-2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
* Copyright (C) 2010-2011 Benjamin Port <port.benjamin@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (  at your option ) any later version.
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

#include "KPrOutlineEditor.h"

#include <QKeyEvent>
#include <KDebug>

KPrOutlineEditor::KPrOutlineEditor ( KPrViewModeOutline* out, QWidget * parent )
    : QTextEdit(parent), m_outlineView(out)
{
}

KPrOutlineEditor::KPrOutlineEditor ( KPrViewModeOutline* out, const QString & text, QWidget * parent)
    : QTextEdit(text, parent), m_outlineView(out)
{
}

KPrOutlineEditor::~KPrOutlineEditor()
{
}

void KPrOutlineEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Tab:
          /*  if (int(event->modifiers()) == 0) {
                if(!outline->indent(true)) {
                    // if no indentation has been done, a regular tab is inserted
                    QTextEdit::keyPressEvent(event);
                }
            } else {*/
                QTextEdit::keyPressEvent(event);
            //}
            break;
        case Qt::Key_Backtab:
            if (int(event->modifiers()) == Qt::ShiftModifier) {
                // if no indentation has been done, event is not passed to ancestor because this will
                // result in a focus lost
                m_outlineView->indent(false);
            } else {
                QTextEdit::keyPressEvent(event);
            }
            break;
        case Qt::Key_Return:
            if (int(event->modifiers()) == 0) {

                if (m_outlineView->link().value(textCursor().currentFrame()).type == Title) {
                    m_outlineView->addSlide();
                } else {
                    // the native behaviour when adding an item is buggy as hell !
                    QTextCursor cur = textCursor();
                    QTextDocument* target = m_outlineView->link().value(cur.currentFrame()).textDocument;
                    if (!(cur.currentList() && target)) {
                        QTextEdit::keyPressEvent(event);
                        break;
                    }

                    m_outlineView->disableSync();
                    QTextEdit::keyPressEvent(event);
                    m_outlineView->enableSync();
                    QTextCursor targetCur(target);
                    targetCur.setPosition(cur.position() - cur.currentFrame()->firstPosition() - 1);
                    targetCur.insertText("\n");
                }
            } else if (int(event->modifiers()) == Qt::ControlModifier) {
                m_outlineView->addSlide();
            } else {
                QTextEdit::keyPressEvent(event);
            }
            break;
        case Qt::Key_Backspace:
            if (int(event->modifiers()) == 0 && m_outlineView->link().value(m_outlineView->currentFrame()).type == Title
                    && m_outlineView->currentFrame()->firstPosition() ==  textCursor().position()) {
                m_outlineView->deleteSlide();
                break;
            }
            // outline->disableSync();
            QTextEdit::keyPressEvent(event);
            // outline->enableSync();
            break;
        default:
            kDebug()<< event->key();
            QTextEdit::keyPressEvent(event);
            break;
    }
}
