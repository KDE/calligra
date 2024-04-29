/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                  Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
         2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2007 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FENCEDELEMENT_H
#define FENCEDELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"
#include <QPainterPath>

/**
 * A left and/or right bracket around one child.
 */
class KOFORMULA_EXPORT FencedElement : public RowElement
{
public:
    /// The standard constructor
    explicit FencedElement(BasicElement *parent = nullptr);

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    void paint(QPainter &painter, AttributeManager *am) override;

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout(const AttributeManager *am) override;

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue(const QString &attribute) const override;

    /// @return The element's ElementType
    ElementType elementType() const override;

private:
    /// The buffer the element paints its visual content
    QPainterPath m_fence;
};

#endif // FENCEDELEMENT_H
