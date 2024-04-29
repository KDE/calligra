/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                  Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SPACEELEMENT_H
#define SPACEELEMENT_H

#include "BasicElement.h"
#include "koformula_export.h"

/** Enum encoding all states of mspace's linebreak attribute */
enum LineBreak {
    Auto /**< Renderer should use default linebreaking algorithm*/,
    NewLine /**< Start a new line and do not indent*/,
    IndentingNewLine /**< Start a new line and do indent*/,
    NoBreak /**< Do not allow a linebreak here*/,
    GoodBreak /**< If a linebreak is needed on the line, here is a good spot*/,
    BadBreak /**< If a linebreak is needed on the line, try to avoid breaking here*/,
    InvalidLineBreak
};

/**
 * @short Implementation of the MathML mspace element
 *
 * The mspace element is specified in the MathML spec section 3.2.7. As
 * FormulaShape currently does not implement linebreaking the linebreaking
 * attributes of SpaceElement are ignored.
 */
class KOFORMULA_EXPORT SpaceElement : public BasicElement
{
public:
    /// The standard constructor
    explicit SpaceElement(BasicElement *parent = nullptr);

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am The AttributeManager providing information about attributes values
     */
    void paint(QPainter &painter, AttributeManager *am) override;

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout(const AttributeManager *am) override;

    /// @return The element's ElementType
    ElementType elementType() const override;

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue(const QString &attribute) const override;
};

#endif // SPACEELEMENT_H
