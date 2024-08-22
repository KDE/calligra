/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FixedElement.h"

#include "FormulaCursor.h"
#include "FormulaDebug.h"

#include <KoXmlReader.h>

#include <QPainter>
#include <QPainterPath>

FixedElement::FixedElement(BasicElement *parent)
    : BasicElement(parent)
{
}

FixedElement::~FixedElement() = default;

BasicElement *FixedElement::elementAfter(int position) const
{
    if (position % 2 == 0) {
        return elementNext(position);
    } else {
        return nullptr;
    }
}

BasicElement *FixedElement::elementBefore(int position) const
{
    if (position % 2 == 1) {
        return elementNext(position);
    } else {
        return nullptr;
    }
}

BasicElement *FixedElement::elementNext(int position) const
{
    return childElements()[position / 2];
}

QPainterPath FixedElement::selectionRegion(const int pos1, const int pos2) const
{
    QPainterPath temp;
    Q_UNUSED(pos1);
    Q_UNUSED(pos2);
    return temp;
}

bool FixedElement::moveHorSituation(FormulaCursor &newcursor, FormulaCursor &oldcursor, int pos1, int pos2)
{
    if ((newcursor.position() / 2 == pos1 && newcursor.direction() == MoveUp) || (newcursor.position() / 2 == pos2 && newcursor.direction() == MoveDown)
        || (newcursor.position() == 2 * pos1 && newcursor.direction() == MoveLeft)
        || (newcursor.position() == 2 * pos2 + 1 && newcursor.direction() == MoveRight)) {
        return false;
    }
    switch (newcursor.direction()) {
    case MoveLeft:
        if (newcursor.position() == 2 * pos2 + 1) {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos2]);
        } else {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos1]);
        }
        break;
    case MoveRight:
        if (newcursor.position() == 2 * pos1) {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos1]);
        } else {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos2]);
        }
        break;
    case MoveUp:
    case MoveDown:
        return newcursor.moveCloseTo(childElements()[newcursor.direction() == MoveUp ? pos1 : pos2], oldcursor);
    case NoDirection:
        break;
    }
    return true;
}

bool FixedElement::moveVertSituation(FormulaCursor &newcursor, FormulaCursor &oldcursor, int pos1, int pos2)
{
    if ((newcursor.position() / 2 == pos1 && newcursor.direction() == MoveUp) || (newcursor.position() / 2 == pos2 && newcursor.direction() == MoveDown)
        || (newcursor.position() % 2 == 0 && newcursor.direction() == MoveLeft) || (newcursor.position() % 2 == 1 && newcursor.direction() == MoveRight)) {
        return false;
    }
    switch (newcursor.direction()) {
    case MoveLeft:
    case MoveRight:
        if (newcursor.position() / 2 == pos1) {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos1]);
        } else {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos2]);
        }
        break;
    case MoveUp:
    case MoveDown:
        return newcursor.moveCloseTo(childElements()[newcursor.direction() == MoveUp ? pos1 : pos2], oldcursor);
    case NoDirection:
        break;
    }
    return true;
}

bool FixedElement::moveSingleSituation(FormulaCursor &newcursor, FormulaCursor &oldcursor, int pos)
{
    Q_UNUSED(oldcursor)
    switch (newcursor.direction()) {
    case MoveLeft:
        if (newcursor.position() % 2 == 1) {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos]);
            break;
        }
        return false;
    case MoveRight:
        if (newcursor.position() % 2 == 0) {
            newcursor.moveTo(newcursor.currentElement()->childElements()[pos]);
            break;
        }
        // fall through
    case MoveUp:
    case MoveDown:
        return false;
    case NoDirection:
        break;
    }
    return true;
}

bool FixedElement::acceptCursor(const FormulaCursor &cursor)
{
    Q_UNUSED(cursor)
    return false;
}

QLineF FixedElement::cursorLine(int position) const
{
    QRectF tmp;
    if (position % 2 == 1) {
        tmp = elementBefore(position)->absoluteBoundingRect();
        return QLineF(tmp.topRight(), tmp.bottomRight());
    } else {
        tmp = elementAfter(position)->absoluteBoundingRect();
        return QLineF(tmp.topLeft(), tmp.bottomLeft());
    }
}

int FixedElement::positionOfChild(BasicElement *child) const
{
    int tmp = childElements().indexOf(child);
    if (tmp == -1) {
        return -1;
    } else {
        return 2 * tmp;
    }
}

bool FixedElement::loadElement(KoXmlElement &tmp, RowElement **child)
{
    BasicElement *element;
    element = ElementFactory::createElement(tmp.tagName(), this);
    if (!element->readMathML(tmp)) {
        return false;
    }
    if (element->elementType() == Row) {
        delete (*child);
        (*child) = static_cast<RowElement *>(element);
    } else {
        (*child)->insertChild(0, element);
    }
    return true;
}

int FixedElement::endPosition() const
{
    return childElements().length() * 2 - 1;
}
