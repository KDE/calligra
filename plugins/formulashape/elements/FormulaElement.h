/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006-2007 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMULAELEMENT_H
#define FORMULAELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"
#include <QObject>

/**
 * @short The element of a formula at the highest position.
 *
 * A formula consists of a tree of elements. The FormulaElement is the root of this
 * tree and therefore is the only element that doesn't have a parent element.
 * It's functionality is reduced to layouting its children in a different way. It is
 * the element with highest size and can also dictate the size to all other elements.
 */
class KOFORMULA_EXPORT FormulaElement : public RowElement
{
public:
    /// The standard constructor
    FormulaElement();

    /// @return The element's ElementType
    ElementType elementType() const override;

    void writeMathMLAttributes(KoXmlWriter *writer) const override;
};

#endif // FORMULAELEMENT_H
