/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef UNDEROVERELEMENT_H
#define UNDEROVERELEMENT_H

#include "FixedElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML mover, munder and moverunder elements
 *
 */
class KOFORMULA_EXPORT UnderOverElement : public FixedElement
{
public:
    /// The standard constructor
    explicit UnderOverElement(BasicElement *parent = nullptr, ElementType elementType = UnderOver);

    /// The standard destructor
    ~UnderOverElement() override;

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    int endPosition() const override;

    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;
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

protected:
    /// Read all content from the node - reimplemented by child elements
    bool readMathMLContent(const KoXmlElement &element) override;

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
    /// The element used as basis for the under and the over element
    RowElement *m_baseElement;

    /// The element that is layouted under the base element
    RowElement *m_underElement;

    /// The element that is layouted over the base element
    RowElement *m_overElement;

    /// The type - one of Under, Over, UnderOver
    ElementType m_elementType;
};

#endif // UNDEROVERELEMENT_H
