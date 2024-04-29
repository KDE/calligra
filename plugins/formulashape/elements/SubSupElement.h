/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SUBSUPELEMENT_H
#define SUBSUPELEMENT_H

#include "FixedElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the msub, msup, msubsup elements
 */
class KOFORMULA_EXPORT SubSupElement : public FixedElement
{
public:
    /// The standard constructor
    explicit SubSupElement(BasicElement *parent = nullptr, ElementType elementType = SubSupScript);

    /// The destructor
    ~SubSupElement() override;

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    /// inherited from BasicElement
    bool replaceChild(BasicElement *oldElement, BasicElement *newElement) override;

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

    int endPosition() const override;

    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;

protected:
    /// Read all content from the node
    bool readMathMLContent(const KoXmlElement &element) override;

    /// Write all content to the KoXmlWriter
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
    /// The base element
    RowElement *m_baseElement;

    /// The subscript right to the m_baseElement
    RowElement *m_subScript;

    /// The superscript right to the m_baseElement
    RowElement *m_superScript;

    /// Whether this is a SubScript, SupScript or SubSupScript
    ElementType m_elementType;
};

#endif // SUBSUPELEMENT_H
