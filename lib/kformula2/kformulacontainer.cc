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

#include <qpainter.h>
#include <qevent.h>
#include <qstring.h>

#include "basicelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "indexelement.h"
#include "kformulacontainer.h"
#include "sequenceelement.h"
#include "textelement.h"


KFormulaContainer::KFormulaContainer()
    : rootElement(this)
{
    dirty = true;
    testDirty();
}

KFormulaContainer::~KFormulaContainer()
{
}


FormulaCursor* KFormulaContainer::createCursor()
{
    FormulaCursor* cursor = new FormulaCursor(&rootElement);
    return cursor;
}

void KFormulaContainer::destroyCursor(FormulaCursor* cursor)
{
    delete cursor;
}


/**
 * Gets called just before the child is removed from
 * the element tree.
 */
void KFormulaContainer::elementRemoval(BasicElement* child)
{
}

/**
 * Gets called whenever something changes and we need to
 * recalc.
 */
void KFormulaContainer::changed()
{
    dirty = true;
}


/**
 * Draws the whole thing.
 */
void KFormulaContainer::draw(QPainter& painter)
{
    rootElement.draw(painter, context);
}

/**
 * One of our view got a key.
 */
void KFormulaContainer::keyPressEvent(FormulaCursor* cursor, QKeyEvent* event)
{
    QChar ch = event->text().at(0);
    if (ch.isPrint()) {
        QList<BasicElement> list;
        list.setAutoDelete(true);
        list.append(new TextElement(ch));
        cursor->insert(list);
    }
    else {
        int action = event->key();
        switch (action) {
        case Qt::Key_Left:
            cursor->moveLeft();
            break;
        case Qt::Key_Right:
            cursor->moveRight();
            break;
        case Qt::Key_Up:
            cursor->moveUp();
            break;
        case Qt::Key_Down:
            cursor->moveDown();
            break;
        case Qt::Key_BackSpace: {
            QList<BasicElement> list;
            list.setAutoDelete(true);
            cursor->remove(list);
            cursor->normalize();
            //list.at(0)->output(cout) << endl;
            break;
        }
        case Qt::Key_Delete: {
            QList<BasicElement> list;
            list.setAutoDelete(true);
            cursor->remove(list, BasicElement::afterCursor);
            cursor->normalize();
            //list.at(0)->output(cout) << endl;
            break;
        }
        case Qt::Key_Home:
        case Qt::Key_End:
            break;
        }
    }
    
    testDirty();
}

void KFormulaContainer::testDirty()
{
    if (dirty) {
        dirty = false;
        rootElement.calcSizes(context);
        emit formulaChanged();
    }
}
