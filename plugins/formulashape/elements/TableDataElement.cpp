/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2007 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TableDataElement.h"
#include "FormulaCursor.h"
#include "TableElement.h"
#include "TableRowElement.h"

TableDataElement::TableDataElement(BasicElement *parent)
    : RowElement(parent)
{
}

// void TableDataElement::layout( const AttributeManager* am )
// {}

QString TableDataElement::attributesDefaultValue(const QString &attribute) const
{
    if (attribute == "rowspan" || attribute == "columnspan")
        return "1";
    else
        return QString();
}

bool TableDataElement::moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor)
{
    if (newcursor.isSelecting() || newcursor.direction() == MoveLeft || newcursor.direction() == MoveRight) {
        return RowElement::moveCursor(newcursor, oldcursor);
    } else {
        TableRowElement *tr = static_cast<TableRowElement *>(parentElement());
        TableElement *te = static_cast<TableElement *>(tr->parentElement());
        int rn = te->positionOfChild(tr) / 2; // table elements have a cursor
        int cn = tr->positionOfChild(this);
        // positions before and after each element
        if (newcursor.direction() == MoveUp) {
            if (rn > 1) {
                return newcursor.moveCloseTo(te->childElements()[rn - 1]->childElements()[cn], oldcursor);
            } else {
                return false;
            }
        } else {
            if (rn < te->endPosition() / 2) {
                return newcursor.moveCloseTo(te->childElements()[rn + 1]->childElements()[cn], oldcursor);
            } else {
                return false;
            }
        }
    }
}

ElementType TableDataElement::elementType() const
{
    return TableData;
}
