/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
   SPDX-FileCopyrightText: 2001 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TABLEROWELEMENT_H
#define TABLEROWELEMENT_H

#include "AttributeManager.h"
#include "BasicElement.h"
#include "koformula_export.h"

class TableDataElement;

/**
 * @short Representing the MathML mtr element.
 *
 * Each row is responsible for painting the rowline which is placed over its content.
 * For layouting each row in a table will query a list of height and width values
 * from the parental TableElement which can determine these without hustle.
 */
class KOFORMULA_EXPORT TableRowElement : public BasicElement
{
public:
    /// The standard constructor
    explicit TableRowElement(BasicElement *parent = nullptr);

    /// The standard destructor
    ~TableRowElement() override;

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

    /**
     * Insert a new child at the cursor position
     * @param position The cursor position where to insert
     * @param child A BasicElement to insert
     */
    bool insertChild(int position, BasicElement *child);

    /**
     * Remove a child element
     * @param child The BasicElement to remove
     */
    bool removeChild(BasicElement *child);

    /**
     * Implement the cursor behaviour for the element
     * @param cursor The cursor
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    bool acceptCursor(const FormulaCursor &cursor) override;

    /// inherited from BasicElement
    int positionOfChild(BasicElement *child) const override;

    /// inherited from BasicElement
    int endPosition() const override;

    /// inherited from BasicElement
    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    /// inherited from BasicElement
    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;

    /// inherited from BasicElement
    QLineF cursorLine(int position) const override;

    /// @return The element's ElementType
    ElementType elementType() const override;

protected:
    /// Read all content from the node - reimplemented by child elements
    bool readMathMLContent(const KoXmlElement &element) override;

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
    /// @return A list of alignments in @p orientation for each element of the table
    QList<Align> alignments(Qt::Orientation orientation);

    /// The list of entries in this row of the table
    QList<TableDataElement *> m_data;
};

#endif
