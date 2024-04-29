/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TABLEDATAELEMENT_H
#define TABLEDATAELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML mtd element
 *
 * The lines behaviour is (a little) different from that
 * of ordinary sequences. Its MathML tag is \<mtd\>.
 */
class KOFORMULA_EXPORT TableDataElement : public RowElement
{
public:
    /// The standard constructor
    explicit TableDataElement(BasicElement *parent = nullptr);

    //    /**
    //     * Calculate the size of the element and the positions of its children
    //     * @param am The AttributeManager providing information about attributes values
    //     */
    //    void layout( const AttributeManager* am );

    /// @return The element's ElementType
    ElementType elementType() const override;

    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue(const QString &attribute) const override;
};

#endif // TABLEDATAELEMENT_H
