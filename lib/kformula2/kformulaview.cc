/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <iostream>

#include <qpainter.h>

#include <kapp.h>

#include "basicelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulaview.h"


KFormulaView::KFormulaView(KFormulaContainer* doc, QWidget* w)
        : cursorVisible(false), cursorHasChanged(true), document(doc), widget(w)
{
    
    // This is buggy. We do need more/other messages.
    connect(document, SIGNAL(elementWillVanish(BasicElement*)),
            this, SLOT(slotElementWillVanish(BasicElement*)));
    connect(document, SIGNAL(formulaLoaded(FormulaElement*)),
            this, SLOT(slotFormulaLoaded(FormulaElement*)));
    connect(document, SIGNAL(cursorMoved(FormulaCursor*)),
            this, SLOT(slotCursorMoved(FormulaCursor*)));

    cursor = document->createCursor();
}

KFormulaView::~KFormulaView()
{
    delete cursor;
}


QPoint KFormulaView::getCursorPoint() const 
{
    return cursor->getCursorPoint();
}

void KFormulaView::setReadOnly(bool ro)
{
    cursor->setReadOnly(ro);
}


#if 0
void KFormulaView::paintEvent(QPaintEvent* event)
{
    //cerr << "void KFormulaView::paintEvent(QPaintEvent*): " << cursorVisible << " " << hasFocus() << endl;
    hideCursor();
    
    QPainter painter;
    painter.begin(widget);
    painter.fillRect(event->rect(), widget->backgroundColor());
    document->draw(painter);
    painter.end();

    showCursor();
    emitCursorChanged();
}
#endif

void KFormulaView::draw(QPainter& painter, const QRect& rect)
{
    hideCursor(&painter);
    painter.fillRect(rect, widget->backgroundColor());
    document->draw(painter);
    showCursor(&painter);
    emitCursorChanged();
}

void KFormulaView::keyPressEvent(QKeyEvent* event)
{
    if (cursor->isReadOnly()) {
        return;
    }
    
    QChar ch = event->text().at(0);
    if (ch.isPrint()) {
        int latin1 = ch.latin1();
        switch (latin1) {
        case '(':
            document->getDocument()->addDefaultBracket();
            break;
        case '[':
            document->addSquareBracket();
            break;
        case '{':
            break;
        case '|':
            document->addLineBracket();
            break;
        case '^':
            document->addUpperRightIndex();
            break;
        case '_':
            document->addLowerRightIndex();
            break;
        case ' ':
            document->compactExpression();
            break;
        case '}':
        case ']':
        case ')':
            break;
        default:
            document->addText(ch);
        }
    }
    else {
        int action = event->key();
        int state = event->state();
	MoveFlag flag = movementFlag(state);

	switch (action) {
	case Qt::Key_Left:
            slotMoveLeft(flag);
            break;
        case Qt::Key_Right:
            slotMoveRight(flag);
            break;
        case Qt::Key_Up:
            slotMoveUp(flag);
            break;
        case Qt::Key_Down:
            slotMoveDown(flag);
            break;
        case Qt::Key_BackSpace:
            document->remove(BasicElement::beforeCursor);
            break;
        case Qt::Key_Delete:
            document->remove(BasicElement::afterCursor);
            break;
        case Qt::Key_Home:
            slotMoveHome(flag);
            break;
        case Qt::Key_End:
            slotMoveEnd(flag);
            break;
        case Qt::Key_Return:
            document->addLineBreak();
            break;
        default:
            if (state & Qt::ControlButton) {
                switch (event->key()) {
                case Qt::Key_AsciiCircum:
                    document->addUpperLeftIndex();
                    break;
                case Qt::Key_Underscore:
                    document->addLowerLeftIndex();
                    break;
                default:
                    // cerr << "Key: " << event->key() << endl;
                    break;
                }
            }
        }
    }
}


void KFormulaView::focusInEvent(QFocusEvent*)
{
    //cerr << "void KFormulaView::focusInEvent(QFocusEvent*): " << cursorVisible << " " << hasFocus() << endl;
    document->setActiveCursor(cursor);
    showCursor();
    cursorHasChanged = true;
    emitCursorChanged();
}

void KFormulaView::focusOutEvent(QFocusEvent*)
{
    //cerr << "void KFormulaView::focusOutEvent(QFocusEvent*): " << cursorVisible << " " << hasFocus() << endl;
    document->setActiveCursor(0);
    hideCursor();
    cursorHasChanged = true;
    emitCursorChanged();
}

void KFormulaView::mousePressEvent(QMouseEvent* event)
{
    hideCursor();
    
    int flags = movementFlag(event->state());
    cursor->mousePress(event->pos(), flags);

    showCursor();
}

void KFormulaView::mouseReleaseEvent(QMouseEvent* event)
{
    hideCursor();
    
    int flags = movementFlag(event->state());
    cursor->mouseRelease(event->pos(), flags);

    showCursor();

    emitCursorChanged();
}

void KFormulaView::mouseDoubleClickEvent(QMouseEvent*)
{
}

void KFormulaView::mouseMoveEvent(QMouseEvent* event)
{
    hideCursor();
    
    int flags = movementFlag(event->state());
    cursor->mouseMove(event->pos(), flags);

    showCursor();
}

void KFormulaView::wheelEvent(QWheelEvent*)
{
}


void KFormulaView::slotCursorMoved(FormulaCursor* c)
{
    if (c == cursor) {
        widget->update();
    }
}

void KFormulaView::slotFormulaLoaded(FormulaElement* formula)
{
    cursor->formulaLoaded(formula);
}

void KFormulaView::slotElementWillVanish(BasicElement* element)
{
    cursor->elementWillVanish(element);
}

void KFormulaView::slotSelectAll()
{
    hideCursor();
    cursor->moveHome();
    cursor->moveEnd(SelectMovement);
    showCursor();
    emitCursorChanged();
}


void KFormulaView::slotMoveLeft(MoveFlag flag)
{
    hideCursor();
    cursor->moveLeft(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaView::slotMoveRight(MoveFlag flag)
{
    hideCursor();
    cursor->moveRight(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaView::slotMoveUp(MoveFlag flag)
{
    hideCursor();
    cursor->moveUp(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaView::slotMoveDown(MoveFlag flag)
{
    hideCursor();
    cursor->moveDown(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaView::slotMoveHome(MoveFlag flag)
{
    hideCursor();
    cursor->moveHome(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaView::slotMoveEnd(MoveFlag flag)
{
    hideCursor();
    cursor->moveEnd(flag);
    showCursor();
    emitCursorChanged();
}


MoveFlag KFormulaView::movementFlag(int state)
{
    int flag = NormalMovement;

    if (state & Qt::ControlButton)
        flag |= WordMovement;

    if (state & Qt::ShiftButton)
        flag |= SelectMovement;

    return static_cast<MoveFlag>(flag);
}


void KFormulaView::hideCursor(QPainter* painter)
{
    if (cursorVisible) {
        //cerr << "void KFormulaView::hideCursor(): " << cursorVisible << " " << hasFocus() << endl;
        cursorVisible = false;

        if (painter != 0) {
            cursor->draw(*painter);
        }
        else {
            QPainter painter;
            painter.begin(widget);
            cursor->draw(painter);
            painter.end();
        }
    }
}

void KFormulaView::showCursor(QPainter* painter)
{
    if ((!cursorVisible) && widget->hasFocus()) {
        //cerr << "void KFormulaView::showCursor(): " << cursorVisible << " " << hasFocus() << endl;
        cursorVisible = true;

        if (painter != 0) {
            cursor->draw(*painter);
        }
        else {
            QPainter painter;
            painter.begin(widget);
            cursor->draw(painter);
            painter.end();
        }
    }
}

void KFormulaView::emitCursorChanged()
{
    if (cursor->hasChanged() || cursorHasChanged) {
        cursor->clearChangedFlag();
        cursorHasChanged = false;
        emit cursorChanged(cursorVisible, cursor->isSelection());
    }
}

#include "kformulaview.moc"
