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
//#include <klocale.h>
//#include <kstdaction.h>

#include "basicelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "kformulacontainer.h"
#include "kformulawidget.h"


KFormulaWidget::KFormulaWidget(KFormulaContainer* doc, QWidget* parent, const char* name, WFlags f)
    : QWidget(parent, name, f | WRepaintNoErase | WResizeNoErase),
      cursorVisible(false), cursorHasChanged(true), document(doc)
{
    leftBracket = '(';
    rightBracket = ')';
    
    // This is buggy. We do need more/other messages.
    connect(document, SIGNAL(elementWillVanish(BasicElement*)),
            this, SLOT(slotElementWillVanish(BasicElement*)));
    connect(document, SIGNAL(formulaLoaded(FormulaElement*)),
            this, SLOT(slotFormulaLoaded(FormulaElement*)));
    connect(document, SIGNAL(formulaChanged(int, int)),
            this, SLOT(slotFormulaChanged(int, int)));
    connect(document, SIGNAL(cursorMoved(FormulaCursor*)),
            this, SLOT(slotCursorMoved(FormulaCursor*)));

    cursor = document->createCursor();

    setFocusPolicy(QWidget::StrongFocus);
    setBackgroundMode(QWidget::PaletteBase);

    QRect rect = document->boundingRect();
    slotFormulaChanged(rect.width(), rect.height());
}

KFormulaWidget::~KFormulaWidget()
{
    delete cursor;
}


QPoint KFormulaWidget::getCursorPoint() const 
{
    return cursor->getCursorPoint();
}


void KFormulaWidget::paintEvent(QPaintEvent* event)
{
    //cerr << "void KFormulaWidget::paintEvent(QPaintEvent*)\n";
    hideCursor();
    
    QPainter painter;
    painter.begin(this);
    painter.fillRect(event->rect(), backgroundColor());
    document->draw(painter);
    painter.end();

    showCursor();
    emitCursorChanged();
}

void KFormulaWidget::keyPressEvent(QKeyEvent* event)
{
    if (readOnly) {
        return;
    }
    
    document->setActiveCursor(cursor);
    
    QChar ch = event->text().at(0);
    if (ch.isPrint()) {
        int latin1 = ch.latin1();
        switch (latin1) {
        case '(':
            document->addBracket(leftBracket, rightBracket);
            break;
        case '[':
            document->addSquareBracket();
            break;
        case '{':
            break;
        case '|':
            document->addLineBracket();
            break;
        case '+':
        case '-':
        case '*':
        case '=':
        case '<':
        case '>':
            document->addOperator(ch);
            break;
        case '^':
            document->addUpperRightIndex();
            break;
        case '_':
            document->addLowerRightIndex();
            break;
        case ' ':
        case '}':
        case ']':
        case ')':
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            document->addNumber(ch);
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
                    //cerr << "Key: " << event->key() << endl;
                    break;
                }
            }
        }
    }
}


void KFormulaWidget::focusInEvent(QFocusEvent*)
{
    //cerr << "void KFormulaWidget::focusInEvent(QFocusEvent*)\n";
    document->setActiveCursor(cursor);
    showCursor();
    cursorHasChanged = true;
    emitCursorChanged();
}

void KFormulaWidget::focusOutEvent(QFocusEvent*)
{
    //cerr << "void KFormulaWidget::focusOutEvent(QFocusEvent*)\n";
    hideCursor();
    cursorHasChanged = true;
    emitCursorChanged();
}

void KFormulaWidget::mousePressEvent(QMouseEvent* event)
{
    hideCursor();
    
    int flags = movementFlag(event->state());
    cursor->mousePress(event->pos(), flags);

    showCursor();
}

void KFormulaWidget::mouseReleaseEvent(QMouseEvent* event)
{
    hideCursor();
    
    int flags = movementFlag(event->state());
    cursor->mouseRelease(event->pos(), flags);

    showCursor();

    emitCursorChanged();
}

void KFormulaWidget::mouseDoubleClickEvent(QMouseEvent*)
{
}

void KFormulaWidget::mouseMoveEvent(QMouseEvent* event)
{
    hideCursor();
    
    int flags = movementFlag(event->state());
    cursor->mouseMove(event->pos(), flags);

    showCursor();
}

void KFormulaWidget::wheelEvent(QWheelEvent*)
{
}



void KFormulaWidget::slotFormulaLoaded(FormulaElement* formula)
{
    cursor->formulaLoaded(formula);
}

void KFormulaWidget::slotCursorMoved(FormulaCursor* c)
{
    if (c == cursor) {
        update();
    }
}

void KFormulaWidget::slotElementWillVanish(BasicElement* element)
{
    cursor->elementWillVanish(element);
}

void KFormulaWidget::slotFormulaChanged(int width, int height)
{
    resize(width+5, height+5);
    // repaint is needed even if the size doesn't change.
    update();
}


void KFormulaWidget::slotSelectAll()
{
    hideCursor();
    cursor->moveHome();
    cursor->moveEnd(SelectMovement);
    showCursor();
    emitCursorChanged();
}


void KFormulaWidget::slotMoveLeft(MoveFlag flag)
{
    hideCursor();
    cursor->moveLeft(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaWidget::slotMoveRight(MoveFlag flag)
{
    hideCursor();
    cursor->moveRight(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaWidget::slotMoveUp(MoveFlag flag)
{
    hideCursor();
    cursor->moveUp(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaWidget::slotMoveDown(MoveFlag flag)
{
    hideCursor();
    cursor->moveDown(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaWidget::slotMoveHome(MoveFlag flag)
{
    hideCursor();
    cursor->moveHome(flag);
    showCursor();
    emitCursorChanged();
}

void KFormulaWidget::slotMoveEnd(MoveFlag flag)
{
    hideCursor();
    cursor->moveEnd(flag);
    showCursor();
    emitCursorChanged();
}


MoveFlag KFormulaWidget::movementFlag(int state)
{
    int flag = NormalMovement;

    if (state & Qt::ControlButton)
        flag |= WordMovement;

    if (state & Qt::ShiftButton)
        flag |= SelectMovement;

    return static_cast<MoveFlag>(flag);
}


void KFormulaWidget::hideCursor()
{
    if (cursorVisible) {
        //cerr << "void KFormulaWidget::hideCursor()\n";
        cursorVisible = false;

        QPainter painter;
        painter.begin(this);
        cursor->draw(painter);
        painter.end();
    }
}

void KFormulaWidget::showCursor()
{
    if (readOnly && !cursor->isSelection()) {
        return;
    }
    
    if ((!cursorVisible) && hasFocus()) {
        //cerr << "void KFormulaWidget::showCursor()\n";
        cursorVisible = true;

        QPainter painter;
        painter.begin(this);
        cursor->draw(painter);
        painter.end();
    }
}

void KFormulaWidget::emitCursorChanged()
{
    if (cursor->hasChanged() || cursorHasChanged) {
        cursor->clearChangedFlag();
        cursorHasChanged = false;
        emit cursorChanged(cursorVisible, cursor->isSelection());
    }
}
