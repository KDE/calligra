/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                  Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ROOTELEMENT_H
#define ROOTELEMENT_H

#include "BasicElement.h"
#include "FixedElement.h"
#include "koformula_export.h"

#include <QPainterPath>

/**
 * @short Implementation of the MathML mroot and msqrt elements
 */
class KOFORMULA_EXPORT RootElement : public FixedElement
{
public:
    /// The standard constructor
    explicit RootElement(BasicElement *parent = nullptr);

    /// The standard destructor
    ~RootElement() override;

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    //     QList<BasicElement*> elementsBetween(int pos1, int pos2) const;

    /// inherited from BasicElement
    bool replaceChild(BasicElement *oldelement, BasicElement *newelement) override;

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
    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;

    /// inherited from BasicElement
    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    //     virtual QLineF cursorLine(int position) const;

    /// inherited from BasicElement
    //     virtual int positionOfChild(BasicElement* child) const;

    /// @return The element's ElementType
    ElementType elementType() const override;

    /// @return The element's length
    int endPosition() const override;

protected:
    /// update the selection in cursor so that a proper range is selected
    //     void fixSelection (FormulaCursor& cursor);

    /// Read root contents - reimplemented from BasicElement
    bool readMathMLContent(const KoXmlElement &element) override;

    /// Write root contents - reimplemented from BasicElement
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
    /// The element that is the radicand of the root
    RowElement *m_radicand;

    /// The element that is the exponent of the root
    RowElement *m_exponent;

    /// The point the artwork relates to.
    QPointF m_rootOffset;

    /// The QPainterPath that holds the lines for the root sign
    QPainterPath m_rootSymbol;

    /// Line thickness, in pixels
    qreal m_lineThickness;
};

#endif // ROOTELEMENT_H
