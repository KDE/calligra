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
    : QWidget(parent, name, f), cursorVisible(false), document(doc)
{
    // This is buggy. We do need more/other messages.
    connect(document, SIGNAL(elementWillVanish(BasicElement*)),
            this, SLOT(slotElementWillVanish(BasicElement*)));
    connect(document, SIGNAL(formulaLoaded(FormulaElement*)),
            this, SLOT(slotFormulaLoaded(FormulaElement*)));
    connect(document, SIGNAL(formulaChanged()),
            this, SLOT(slotFormulaChanged()));

    cursor = document->createCursor();

//     accel = new KAccel(this);
    
//     (openFile = KStdAction::open(this, SLOT(slotOpen())))->plugAccel(accel);
//     (saveFile = KStdAction::save(this, SLOT(slotSave())))->plugAccel(accel);
//     (quitAction = KStdAction::quit(kapp, SLOT(quit())))->plugAccel(accel);
//     (undoAction = KStdAction::undo(this, SLOT(slotUndo())))->plugAccel(accel);
//     (redoAction = KStdAction::redo(this, SLOT(slotRedo())))->plugAccel(accel);

//     (cutAction = KStdAction::cut(this, SLOT(slotCut())))->plugAccel(accel);
//     (copyAction = KStdAction::copy(this, SLOT(slotCopy())))->plugAccel(accel);
//     (pasteAction = KStdAction::paste(this, SLOT(slotPaste())))->plugAccel(accel);
//     (selectAllAction = KStdAction::selectAll(this, SLOT(slotSelectAll())))->plugAccel(accel);

//     (integralElement = new KAction(i18n("Integral"), CTRL+Key_3, this, SLOT(slotIntegral()), this))->plugAccel(accel);
//     (productElement = new KAction(i18n("Product"), CTRL+Key_2, this, SLOT(slotProduct()), this))->plugAccel(accel);
//     (sumElement = new KAction(i18n("Sum"), CTRL+Key_1, this, SLOT(slotSum()), this))->plugAccel(accel);
//     (rootElement = new KAction(i18n("Root"), CTRL+Key_R, this, SLOT(slotRoot()), this))->plugAccel(accel);
//     (fractionElement = new KAction(i18n("Fraction"), CTRL+Key_F, this, SLOT(slotFraction()), this))->plugAccel(accel);
//     (matrixElement = new KAction(i18n("Matrix"), CTRL+Key_M, this, SLOT(slotMatrix()), this))->plugAccel(accel);

//     (generalUpperIndex = new KAction(i18n("Upper Index"), CTRL+Key_U, this, SLOT(slotGeneralUpperIndex()), this))->plugAccel(accel);
//     (generalLowerIndex = new KAction(i18n("Lower Index"), CTRL+Key_L, this, SLOT(slotGeneralLowerIndex()), this))->plugAccel(accel);

    //upperLeftIndex = new KAction(i18n("Upper Left Index"), CTRL+Key_I, this, SLOT(openPageL), 0);
    //lowerLeftIndex = new KAction(i18n("Lower Left Index"), CTRL+Key_I, this, SLOT(openPageL), 0);
    //upperRightIndex = new KAction(i18n("Upper Right Index"), CTRL+Key_I, this, SLOT(openPageL), 0);
    //lowerRightIndex = new KAction(i18n("Lower Right Index"), CTRL+Key_I, this, SLOT(openPageL), 0);
}

KFormulaWidget::~KFormulaWidget()
{
    delete cursor;
}


void KFormulaWidget::paintEvent(QPaintEvent*)
{
    // we need this if we optimize the painting one day.
    //hideCursor();
    
    QPainter painter;
    painter.begin(this);
    document->draw(painter);
    painter.end();

    cursorVisible = false;
    showCursor();
}

void KFormulaWidget::keyPressEvent(QKeyEvent* event)
{
    document->setActiveCursor(cursor);
    
    QChar ch = event->text().at(0);
    if (ch.isPrint()) {
        int latin1 = ch.latin1();
        switch (latin1) {
        case '(':
            document->addRoundBracket();
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
//                 case Qt::Key_D:
//                     document->replaceElementWithMainChild(BasicElement::afterCursor);
//                     break;
//                 case Qt::Key_R:
//                     document->replaceElementWithMainChild(BasicElement::beforeCursor);
//                     break;
                default:
                    //cerr << "Key: " << event->key() << endl;
                    break;
                }
            }
        }
    }
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



void KFormulaWidget::slotFormulaLoaded(FormulaElement* formula)
{
    cursor->formulaLoaded(formula);
}

void KFormulaWidget::slotElementWillVanish(BasicElement* element)
{
    cursor->elementWillVanish(element);
}

void KFormulaWidget::slotFormulaChanged()
{
    update();
}


void KFormulaWidget::slotSelectAll()
{
    hideCursor();
    cursor->moveHome();
    cursor->moveEnd(SelectMovement);
    showCursor();
}


void KFormulaWidget::slotMoveLeft(MoveFlag flag)
{
    hideCursor();
    cursor->moveLeft(flag);
    showCursor();
}

void KFormulaWidget::slotMoveRight(MoveFlag flag)
{
    hideCursor();
    cursor->moveRight(flag);
    showCursor();
}

void KFormulaWidget::slotMoveUp(MoveFlag flag)
{
    hideCursor();
    cursor->moveUp(flag);
    showCursor();
}

void KFormulaWidget::slotMoveDown(MoveFlag flag)
{
    hideCursor();
    cursor->moveDown(flag);
    showCursor();
}

void KFormulaWidget::slotMoveHome(MoveFlag flag)
{
    hideCursor();
    cursor->moveHome(flag);
    showCursor();
}

void KFormulaWidget::slotMoveEnd(MoveFlag flag)
{
    hideCursor();
    cursor->moveEnd(flag);
    showCursor();
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
