/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "FoExternalEditor.h"

// Tables
#include <sheets/ui/CellToolBase.h>
#include <sheets/Map.h>
#include <sheets/Sheet.h>

#include <FoCellEditor.h>

#include <QApplication>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QTextCursor>

FoExternalEditor::FoExternalEditor(Calligra::Sheets::CellToolBase* cellToolBase, QWidget *parent)
    : QTextEdit(parent),
    KoExternalEditorInterface(cellToolBase),
    cellTool(cellToolBase),
    isArray(false)
{
    setMinimumHeight(58);
    setMaximumHeight(58);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    //setStyle(new QGtkStyle());
     setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     //setFrameStyle(QFrame::NoFrame);
     setLineWidth(0);
     document()->setDocumentMargin(0);
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(slotCursorPositionChanged()));
}

FoExternalEditor::~FoExternalEditor()
{
}

void FoExternalEditor::setCellTool(Calligra::Sheets::CellToolBase* cellToolBase)
{
    cellTool=cellToolBase;
}

void FoExternalEditor::applyChanges()
{
    Q_ASSERT(cellTool);
    cellTool->deleteEditor(true, isArray); // save changes
    isArray = false;
}

void FoExternalEditor::discardChanges()
{
    Q_ASSERT(cellTool);
    clear();
    cellTool->deleteEditor(false); // discard changes
    cellTool->selection()->update();
}

void FoExternalEditor::setText(const QString &text)
{
    Q_ASSERT(cellTool);
    if (toPlainText() == text) {
        return;
    }
    // This method is called from the embedded editor. Do not send signals back.
    blockSignals(true);
    QTextEdit::setPlainText(text);
    QTextCursor textCursor = this->textCursor();
    textCursor.setPosition(((FoCellEditor *)(cellTool->editor()))->cursorPosition());
    setTextCursor(textCursor);
    blockSignals(false);
}

void FoExternalEditor::keyPressEvent(QKeyEvent *event)
{
    Q_ASSERT(cellTool);
    if (!cellTool->selection()->activeSheet()->map()->isReadWrite()) {
        return;
    }

    // Create the embedded editor, if necessary.
    if (!cellTool->editor()) {
        cellTool->createEditor(false /* keep content */, false /* no focus */);
    }

   /* if (event->modifiers() & Qt::ShiftModifier) {
        if(( event->key() == Qt::Key_Right ) ||
           ( event->key() == Qt::Key_Left ) ||
           ( event->key() == Qt::Key_Up ) ||
           ( event->key() == Qt::Key_Down )) {
            QApplication::sendEvent(cellTool->editor()->widget(), &QKeyEvent(QEvent::KeyPress,event->key(),Qt::NoModifier));
            event->accept();
            return;
        }
    }*/

    // the Enter and Esc key are handled by the embedded editor
    if ((event->key() == Qt::Key_Return) || (event->key() == Qt::Key_Enter) ||
        (event->key() == Qt::Key_Escape)) {
        ((FoCellEditor *)(cellTool->editor()))->widget()->setFocus();
        QApplication::sendEvent(((FoCellEditor *)(cellTool->editor()))->widget(), event);
        event->accept();
        return;
    }
    // call inherited handler
    QTextEdit::keyPressEvent(event);
}

void FoExternalEditor::focusInEvent(QFocusEvent* event)
{
    Q_ASSERT(cellTool);
    if (event->reason() != Qt::OtherFocusReason) {
        cellTool->setLastEditorWithFocus(Calligra::Sheets::CellToolBase::ExternalEditor);
    }
   // grabKeyboard();
    // when the external editor gets focus, create also the internal editor
    // this in turn means that ranges will be instantly highlighted right
    if (!cellTool->editor())
        cellTool->createEditor(false /* keep content */, false /* no focus */);
    QTextEdit::focusInEvent(event);
}

void FoExternalEditor::focusOutEvent(QFocusEvent* event)
{
    Q_ASSERT(cellTool);
    if(event->reason()==Qt::MouseFocusReason || event->reason() == Qt::ActiveWindowFocusReason) {
        releaseKeyboard();
    }
    QTextEdit::focusOutEvent(event);
}

void FoExternalEditor::slotTextChanged()
{
    emit textChanged(toPlainText());
    // Update the cursor position again, because this slot is invoked after
    // slotCursorPositionChanged().
    if (cellTool->editor()) {
        cellTool->editor()->setCursorPosition(textCursor().position());
    }
}

void FoExternalEditor::slotCursorPositionChanged()
{
    if (!hasFocus() || !cellTool->editor()) {
        return;
    }
    // Suppress updates, if this slot got invoked by a text change. It is done
    // later by slotTextChanged().
    if (cellTool->editor()->toPlainText() == toPlainText()) {
        cellTool->editor()->setCursorPosition(textCursor().position());
    }
}

void FoExternalEditor::insertOperator(const QString& operatorCharacter)
{
    insertPlainText(operatorCharacter);
    emit textChanged(toPlainText());
}

void FoExternalEditor::setCursorPosition(int position)
{
    QTextCursor textCursor = this->textCursor();
    textCursor.setPosition(position);
    setTextCursor(textCursor);
}
