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

#include <kapp.h>
#include <qpainter.h>

#include <kstdaction.h>

#include "basicelement.h"
#include "formulacursor.h"
#include "kformulacontainer.h"
#include "kformulawidget.h"
#include "kformulamimesource.h"


KFormulaWidget::KFormulaWidget(KFormulaContainer* doc, QWidget* parent, const char* name, WFlags f)
    : QWidget(parent, name, f), cursorVisible(false), document(doc)
{
    cursor = document->createCursor();

    connect(document, SIGNAL(formulaChanged()), this, SLOT(formulaChanged()));
    clipboard = QApplication::clipboard(); 

    accel = new KAccel(this);
    
    (openFile = KStdAction::open(this, SLOT(open())))->plugAccel(accel);
    (saveFile = KStdAction::save(this, SLOT(save())))->plugAccel(accel);
    (quitAction = KStdAction::quit(kapp, SLOT(quit())))->plugAccel(accel);
    (undoAction = KStdAction::undo(this, SLOT(undo())))->plugAccel(accel);
    (redoAction = KStdAction::redo(this, SLOT(redo())))->plugAccel(accel);

    (cutAction = KStdAction::cut(this, SLOT(cut())))->plugAccel(accel);
    (copyAction = KStdAction::copy(this, SLOT(copy())))->plugAccel(accel);
    (pasteAction = KStdAction::paste(this, SLOT(paste())))->plugAccel(accel);
    (selectAllAction = KStdAction::selectAll(this, SLOT(selectAll())))->plugAccel(accel);

    /*
    integralElement;
    productElement;
    sumElement;
    rootElement;
    fractionElement;
    matrixElement;

    generalUpperIndex;
    generalLowerIndex;

    upperLeftIndex;
    lowerLeftIndex;
    upperRightIndex;
    lowerRightIndex;
    */
}

KFormulaWidget::~KFormulaWidget()
{
    document->destroyCursor(cursor);
}


void KFormulaWidget::paintEvent(QPaintEvent*)
{
    QPainter painter;
    painter.begin(this);
    document->draw(painter);
    painter.end();

    cursorVisible = false;
    showCursor();
}

void KFormulaWidget::keyPressEvent(QKeyEvent* event)
{
    hideCursor();
    
    QChar ch = event->text().at(0);
    if (ch.isPrint()) {
        int latin1 = ch.latin1();
        switch (latin1) {
        case '(':
            document->addBracket(cursor, '(', ')');
            break;
        case '[':
            document->addBracket(cursor, '[', ']');
            break;
        case '{':
            break;
        case '|':
            document->addBracket(cursor, '|', '|');
            break;
        case '/':
            document->addFraction(cursor);
            break;
        case '#':
            // here we need a dialog!
            document->addMatrix(cursor, 4, 5);
            break;
        case '\\':
            document->addRoot(cursor);
            break;
        case '+':
        case '-':
        case '*':
        case '=':
        case '<':
        case '>':
            document->addOperator(cursor, ch);
            break;
        case '^':
            document->addUpperRightIndex(cursor);
            break;
        case '_':
            document->addLowerRightIndex(cursor);
            break;
        case ' ':
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
            document->addNumber(cursor, ch);
            break;
        default:
            document->addText(cursor, ch);
        }
    }
    else {
        int action = event->key();
        int state = event->state();
	int flag = movementFlag(state);

	switch (action) {
	case Qt::Key_Left:
            cursor->moveLeft(flag);
            break;
        case Qt::Key_Right:
            cursor->moveRight(flag);
            break;
        case Qt::Key_Up:
            cursor->moveUp(flag);
            break;
        case Qt::Key_Down:
            cursor->moveDown(flag);
            break;
        case Qt::Key_BackSpace:
            document->remove(cursor, BasicElement::beforeCursor);
            break;
        case Qt::Key_Delete:
            document->remove(cursor, BasicElement::afterCursor);
            break;
        case Qt::Key_Home:
            cursor->moveHome(flag);
            break;
        case Qt::Key_End:
            cursor->moveEnd(flag);
            break;
        case Qt::Key_F1:
            document->addSymbol(cursor, Artwork::Product);
            break;
        case Qt::Key_F2:
            document->addSymbol(cursor, Artwork::Sum);
            break;
        case Qt::Key_F3:
            document->addSymbol(cursor, Artwork::Integral);
            break;
        default:
            if (state & Qt::ControlButton) {
                switch (event->key()) {
                case Qt::Key_AsciiCircum:
                    document->addUpperLeftIndex(cursor);
                    break;
                case Qt::Key_Underscore:
                    document->addLowerLeftIndex(cursor);
                    break;
                case Qt::Key_U:
                    document->addGenericUpperIndex(cursor);
                    break;
                case Qt::Key_L:
                    document->addGenericLowerIndex(cursor);
                    break;
                case Qt::Key_D:
                    document->replaceElementWithMainChild(cursor, BasicElement::afterCursor);
                    break;
                case Qt::Key_R:
                    document->replaceElementWithMainChild(cursor, BasicElement::beforeCursor);
                    break;
                default:
                    //cerr << "Key: " << event->key() << endl;
                    break;
                }
            }
        }
    }

    showCursor();
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
}

void KFormulaWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
}

void KFormulaWidget::mouseMoveEvent(QMouseEvent* event)
{
    hideCursor();
    
    int flags = movementFlag(event->state());
    cursor->mouseMove(event->pos(), flags);

    showCursor();
}

void KFormulaWidget::wheelEvent(QWheelEvent* event)
{
}


void KFormulaWidget::formulaChanged()
{
    update();
}


void KFormulaWidget::open()
{
    document->load("test.xml");
}

void KFormulaWidget::save()
{
    document->save("test.xml");
}

void KFormulaWidget::undo()
{
    document->undo(cursor);
}

void KFormulaWidget::redo()
{
    document->redo(cursor);
}


void KFormulaWidget::cut()
{
    if (cursor->isSelection()) {
        copy();
        document->remove(cursor, BasicElement::beforeCursor);
    }
}

void KFormulaWidget::copy()
{
    QDomDocument formula = cursor->copy();
    clipboard->setData(new KFormulaMimeSource(formula));
}

void KFormulaWidget::paste()
{
    document->paste(cursor, clipboard->data());
}

void KFormulaWidget::selectAll()
{
    hideCursor();
    cursor->moveHome();
    cursor->moveEnd(FormulaCursor::SelectMovement);
    showCursor();
}


int KFormulaWidget::movementFlag(int state)
{
    int flag = FormulaCursor::NormalMovement;

    if (state & Qt::ControlButton)
        flag |= FormulaCursor::WordMovement;

    if (state & Qt::ShiftButton)
        flag |= FormulaCursor::SelectMovement;

    return flag;
}

void KFormulaWidget::hideCursor()
{
    if (cursorVisible) {
        cursorVisible = false;

        QPainter painter;
        painter.begin(this);
        cursor->draw(painter);
        painter.end();
    }
}

void KFormulaWidget::showCursor()
{
    if (!cursorVisible) {
        cursorVisible = true;

        QPainter painter;
        painter.begin(this);
        cursor->draw(painter);
        painter.end();
    }
}
