/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TABLEELEMENT_H
#define TABLEELEMENT_H

#include "BasicElement.h"
#include "koformula_export.h"
#include <QPainterPath>

class TableRowElement;

/**
 * @short A matrix or table element in a formula
 *
 * A table element contains a list of rows which are of class TableRowElement.
 * These rows contain single entries which are of class TableEntryElement. The
 * TableElement takes care that the different TableRowElements are informed how
 * to lay out their children correctly as they need to be synced.
 */
class KOFORMULA_EXPORT TableElement : public BasicElement
{
public:
    /// The standard constructor
    explicit TableElement(BasicElement *parent = nullptr);

    /// The standard destructor
    ~TableElement() override;

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

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    /// inherited from BasicElement
    bool acceptCursor(const FormulaCursor &cursor) override;

    /// inherited from BasicElement
    int positionOfChild(BasicElement *child) const override;

    /// inherited from BasicElement
    int endPosition() const override;

    /// inherited from BasicElement
    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;

    virtual bool insertChild(int position, BasicElement *child);
    bool removeChild(BasicElement *child);

    /// inherited from BasicElement
    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue(const QString &attribute) const override;

    /// @return The width of the column with the index @p column
    qreal columnWidth(int column);

    /// @return The height of the @p TableRowElement
    qreal rowHeight(TableRowElement *row);

    /// inherited from BasicElement
    QLineF cursorLine(int position) const override;

    /// inherited from BasicElement
    QPainterPath selectionRegion(const int pos1, const int pos2) const override;

    /// @return The element's ElementType
    ElementType elementType() const override;

protected:
    /// Read all content from the node - reimplemented by child elements
    bool readMathMLContent(const KoXmlElement &element) override;

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
    /// @return The base line computed out of the align attribute
    qreal parseTableAlign() const;

    /// Calculate the dimensions of each row and column at a centralized point
    void determineDimensions();

    /// Storage for heights of each row calculated in determineDimensions()
    QList<qreal> m_rowHeights;

    /// Storage for widths of each column calculated in determineDimensions()
    QList<qreal> m_colWidths;

    /// The rows a matrix contains
    QList<TableRowElement *> m_rows;

    /// Buffer for the pen style used for the table's frame
    Qt::PenStyle m_framePenStyle;

    QList<Qt::PenStyle> m_rowLinePenStyles;

    QList<Qt::PenStyle> m_colLinePenStyles;
};

#endif // TABLEELEMENT_H
