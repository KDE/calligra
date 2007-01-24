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

#include "TextTool.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

#include <kdebug.h>
#include <kstandardshortcut.h>
#include <QKeyEvent>
#include <QAction>
#include <QTextBlock>
#include <QTextLayout>
#include <QAbstractTextDocumentLayout>

static bool hit(const QKeySequence &input, KStandardShortcut::StandardShortcut shortcut) {
    foreach(QKeySequence ks, KStandardShortcut::shortcut(shortcut)) {
        if(input == ks)
            return true;
    }
    return false;
}

TextTool::TextTool(KoCanvasBase *canvas)
: KoTool(canvas)
, m_textShape(0)
, m_textShapeData(0)
{
}

TextTool::~TextTool() {
}

void TextTool::paint( QPainter &painter, KoViewConverter &converter) {
    if(painter.hasClipping()) {
        QRect shape = converter.documentToView(m_textShape->boundingRect()).toRect();
        if(painter.clipRegion().intersect( QRegion(shape) ).isEmpty())
            return;
    }

    painter.setMatrix( painter.matrix() * m_textShape->transformationMatrix(&converter) );
    double zoomX, zoomY;
    converter.zoom(&zoomX, &zoomY);
    painter.scale(zoomX, zoomY);
    //const QTextDocument *document = m_caret.block().document();

/*
    QAbstractTextDocumentLayout::PaintContext pc;
    pc.cursorPosition = m_caret.position();
    QAbstractTextDocumentLayout::Selection selection;
    selection.cursor = m_caret;
    selection.format.setTextOutline(QPen(Qt::red));
    pc.selections.append(selection);
    document->documentLayout()->draw( &painter, pc);
*/

    QTextBlock block = m_caret.block();
    if(! block.layout())
        return;

    // paint caret.
    QPen pen(Qt::black);
    if(! m_textShape->hasTransparency()) {
        QColor bg = m_textShape->background().color();
        QColor invert = QColor(255 - bg.red(), 255 - bg.green(), 255 - bg.blue());
        pen.setColor(invert);
    }
    painter.setPen(pen);
    painter.translate(0, -m_textShapeData->documentOffset());
    block.layout()->drawCursor(&painter, QPointF(0,0), m_caret.position() - block.position());
}

void TextTool::mousePressEvent( KoPointerEvent *event ) {
    if(! m_textShape->boundingRect().contains(event->point)) {
        QRectF area(event->point, QSizeF(1,1));
        foreach(KoShape *shape, m_canvas->shapeManager()->shapesAt(area, true)) {
            TextShape *textShape = dynamic_cast<TextShape*> (shape);
            if(textShape) {
                m_textShape = textShape;
                KoTextShapeData *d = static_cast<KoTextShapeData*> (textShape->userData());
                if(d->document() == m_textShapeData->document())
                    break; // stop looking.
            }
        }
        m_textShapeData = static_cast<KoTextShapeData*> (m_textShape->userData());
        // in case its a different doc...
        m_caret = QTextCursor(m_textShapeData->document());
    }

    int position = pointToPosition(event->point);
    repaint();
    m_caret.setPosition(position,
            (event->modifiers() & Qt::ShiftModifier) ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
    repaint();

    updateSelectionHandler();
}

void TextTool::updateSelectionHandler() {
    m_selectionHandler.setShape(m_textShape);
    m_selectionHandler.setShapeData(m_textShapeData);
    m_selectionHandler.setCaret(&m_caret);
}

int TextTool::pointToPosition(const QPointF & point) const {
    QPointF p = m_textShape->convertScreenPos(point);
    int caretPos = m_caret.block().document()->documentLayout()->hitTest(p, Qt::FuzzyHit);
    caretPos = qMax(caretPos, m_textShapeData->position());
    if(m_textShapeData->endPosition() == -1)
        kWarning() << "Clicking in not fully laid-out textframe\n";
    caretPos = qMin(caretPos, m_textShapeData->endPosition());
    return caretPos;
}

void TextTool::mouseDoubleClickEvent( KoPointerEvent *event ) {
    // TODO select whole word, or when clicking in between two words select 2 words.
}

void TextTool::mouseMoveEvent( KoPointerEvent *event ) {
    useCursor(Qt::IBeamCursor);
    if(event->buttons() == Qt::NoButton)
        return;
    int position = pointToPosition(event->point);
    if(position >= 0) {
        repaint();
        m_caret.setPosition(position, QTextCursor::KeepAnchor);
        repaint();
    }

    updateSelectionHandler();
}

void TextTool::mouseReleaseEvent( KoPointerEvent *event ) {
    event->ignore();
}

void TextTool::keyPressEvent(QKeyEvent *event) {
    QTextCursor::MoveOperation moveOperation = QTextCursor::NoMove;
    if(event->key() == Qt::Key_Backspace) {
//       if(event->modifiers() & Qt::ControlModifier)
//           m_caret.deletePreviousWord();
//       else
            m_caret.deletePreviousChar();
    }
    else if(event->key() == Qt::Key_Delete) {
//       if(event->modifiers() & Qt::ControlModifier)
//           m_caret.deleteWord();
//       else
            m_caret.deleteChar();
    }
    else if((event->key() == Qt::Key_Left) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Left;
    else if((event->key() == Qt::Key_Right) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Right;
    else if((event->key() == Qt::Key_Up) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Up;
    else if((event->key() == Qt::Key_Down) && (event->modifiers() | Qt::ShiftModifier) == Qt::ShiftModifier)
        moveOperation = QTextCursor::Down;
    else {
        // check for shortcuts.
        QKeySequence item(event->key() | event->modifiers());
        if(hit(item, KStandardShortcut::Home))
            // Goto beginning of the document. Default: Ctrl-Home
            moveOperation = QTextCursor::StartOfLine; // TODO
        else if(hit(item, KStandardShortcut::End))
            // Goto end of the document. Default: Ctrl-End
            moveOperation = QTextCursor::StartOfLine; // TODO
       else if(hit(item, KStandardShortcut::Prior)) // page up
            // Scroll up one page. Default: Prior
           moveOperation = QTextCursor::StartOfLine; // TODO
        else if(hit(item, KStandardShortcut::Next))
            // Scroll down one page. Default: Next
            moveOperation = QTextCursor::StartOfLine; // TODO
        else if(hit(item, KStandardShortcut::BeginningOfLine))
            // Goto beginning of current line. Default: Home
            moveOperation = QTextCursor::StartOfLine;
        else if(hit(item, KStandardShortcut::EndOfLine))
            // Goto end of current line. Default: End
            moveOperation = QTextCursor::EndOfLine;
        else if(hit(item, KStandardShortcut::BackwardWord))
            moveOperation = QTextCursor::WordLeft;
        else if(hit(item, KStandardShortcut::ForwardWord))
            moveOperation = QTextCursor::NextWord;
        else if((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) || event->text().length() == 0) {
            event->ignore();
            return;
        }
        m_caret.insertText(event->text());
    }
    if(moveOperation != QTextCursor::NoMove) {
        useCursor(Qt::BlankCursor);
        repaint();
    //  if RTL toggle direction of cursor movement.
        m_caret.movePosition(moveOperation,
            (event->modifiers() & Qt::ShiftModifier)?QTextCursor::KeepAnchor:QTextCursor::MoveAnchor);
        repaint();
    }

    updateSelectionHandler();
}

void TextTool::keyReleaseEvent(QKeyEvent *event) {
    event->ignore();
}

void TextTool::activate (bool temporary) {
    Q_UNUSED(temporary);
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach(KoShape *shape, selection->selectedShapes()) {
        m_textShape = dynamic_cast<TextShape*> (shape);
        if(m_textShape)
            break;
    }
    if(m_textShape == 0) { // none found
        emit sigDone();
        return;
    }
    foreach(KoShape *shape, selection->selectedShapes()) {
        // deselect others.
        if(m_textShape == shape) continue;
        selection->deselect(shape);
    }
    m_textShapeData = static_cast<KoTextShapeData*> (m_textShape->userData());
    m_textShapeData->document()->setUndoRedoEnabled(true); // allow undo history
    m_caret = QTextCursor(m_textShapeData->document());
    useCursor(Qt::IBeamCursor, true);
    m_textShape->repaint();

    updateSelectionHandler();
}

void TextTool::deactivate() {
    m_textShape = 0;
    if(m_textShapeData)
        m_textShapeData->document()->setUndoRedoEnabled(false); // erase undo history.
    m_textShapeData = 0;

    updateSelectionHandler();
}

void TextTool::repaint() {
    QTextBlock block = m_caret.block();
    if(block.isValid()) {
        QTextLine tl = block.layout()->lineForTextPosition(m_caret.position() - block.position());
        QRectF repaintRect;
        if(tl.isValid()) {
            repaintRect = tl.rect();
            repaintRect.setX(tl.cursorToX(m_caret.position() - block.position()));
            repaintRect.setWidth(2);
        }
        else // layouting info was removed already :(
            repaintRect = block.layout()->boundingRect();
        repaintRect.moveTop(repaintRect.y() - m_textShapeData->documentOffset());
        repaintRect = m_textShape->transformationMatrix(0).mapRect(repaintRect);
        m_canvas->updateCanvas(repaintRect);
    }
}

KoToolSelection* TextTool::selection() {
    return &m_selectionHandler;
}

#include "TextTool.moc"
