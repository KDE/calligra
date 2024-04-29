/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FRACTIONELEMENT_H
#define FRACTIONELEMENT_H

#include "BasicElement.h"
#include "FixedElement.h"
#include "koformula_export.h"
#include <QLineF>

/**
 * @short Implementation of the MathML mfrac element
 *
 * The mfrac element is specified in the MathML spec section 3.3.2. The
 * FractionElement holds two child elements that are the numerator and the
 * denominator.
 */
class KOFORMULA_EXPORT FractionElement : public FixedElement
{
public:
    /// The standard constructor
    explicit FractionElement(BasicElement *parent = nullptr);

    /// The standard destructor
    ~FractionElement() override;

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

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    /// inherited from BasicElement
    bool replaceChild(BasicElement *oldelement, BasicElement *newelement) override;

    /// inherited from BasicElement
    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;

    /// inherited from BasicElement
    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    /// inherited from BasicElement
    int endPosition() const override;

    /// inherited from BasicElement
    int positionOfChild(BasicElement *child) const override;

    /// inherited from BasicElement
    //     virtual QLineF cursorLine(int position) const;

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue(const QString &attribute) const override;

    /// @return The element's ElementType
    ElementType elementType() const override;

    QList<BasicElement *> elementsBetween(int pos1, int pos2) const override;

protected:
    /// Read all content from the node - reimplemented by child elements
    bool readMathMLContent(const KoXmlElement &parent) override;

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
    /// Layout the fraction in a bevelled way
    void layoutBevelledFraction(const AttributeManager *am);

    /// The element representing the fraction's numerator
    RowElement *m_numerator;

    /// The element representing the fraction's denominator
    RowElement *m_denominator;

    /// The line that separates the denominator and the numerator
    QLineF m_fractionLine;

    /// Hold the thickness of the fraction line
    qreal m_lineThickness;
};

#endif // FRACTIONELEMENT_H
