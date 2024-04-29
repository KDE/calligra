/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                  Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef UNKNOWNELEMENT_H
#define UNKNOWNELEMENT_H

#include "BasicElement.h"
#include "koformula_export.h"

#include <QPainterPath>

class FormulaCursor;

/**
 * @short Implementation of an unknown element.
 *
 * Used when we see a tag that we do not recognise. This element draws nothing, takes up
 * no space, and ignores any calls to insert children etc.
 */
class KOFORMULA_EXPORT UnknownElement : public BasicElement
{
public:
    /// The standard constructor
    explicit UnknownElement(BasicElement *parent = nullptr);

    /// The standard destructor
    ~UnknownElement() override;

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    /**
     * Remove a child element
     * @param element The BasicElement to remove
     */
    // void removeChild( BasicElement* element );

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

    /// inherited from BasicElement
    bool acceptCursor(const FormulaCursor &cursor) override;

    /// @return The element's ElementType
    ElementType elementType() const override;

protected:
    /// Read all attributes - reimplemented from BasicElement
    bool readMathMLAttributes(const KoXmlElement &element) override;

    /// Read root contents - reimplemented from BasicElement
    bool readMathMLContent(const KoXmlElement &element) override;

    /// Write element attributes - reimplemented from BasicElement
    void writeMathMLAttributes(KoXmlWriter *writer) const override;

    /// Write root contents - reimplemented from BasicElement
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
};

#endif // UNKNOWNELEMENT_H
