/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2009 Jeremias Epperlein <jeeree@web.de>
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MULTISCRIPTELEMENT_H
#define MULTISCRIPTELEMENT_H

#include "FixedElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the mmultiscript element
 */
class KOFORMULA_EXPORT MultiscriptElement : public FixedElement
{
public:
    /// The standard constructor
    explicit MultiscriptElement(BasicElement *parent = nullptr);

    /// The destructor
    ~MultiscriptElement() override;

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement *> childElements() const override;

    bool setCursorTo(FormulaCursor &cursor, QPointF point) override;

    bool moveCursor(FormulaCursor &newcursor, FormulaCursor &oldcursor) override;

    //     virtual int length() const;
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
     * Implement the cursor behaviour for the element
     * @param cursor The FormulaCursor that is moved around
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    bool acceptCursor(const FormulaCursor &cursor) override;

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue(const QString &attribute) const override;

    /// @return The element's ElementType
    ElementType elementType() const override;

protected:
    /// Read all content from the node
    bool readMathMLContent(const KoXmlElement &element) override;

    /// Write all content to the KoXmlWriter
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

    /// Make sure that there are an even number of elements, as the spec says
    /// there must be.
    void ensureEvenNumberElements();

private:
    /// The BasicElement representing the base element of the multiscript
    BasicElement *m_baseElement;

    /// A list of BasicElements representing the sub- and super-scripts left to the base
    /// element.  The first item in the list is subscript, second is superscript, third
    /// subscript and so on.
    /// The first 2 items are drawn closest to the item, then moving increasingly
    /// further away
    QList<BasicElement *> m_preScripts;

    /// A list of BasicElements representing the sub- and super-scripts right to the base
    /// element.  The first item in the list is subscript, second is superscript, third
    /// subscript and so on.
    /// The first 2 items are drawn closest to the item, then moving increasingly
    /// further away
    QList<BasicElement *> m_postScripts;
};

#endif // MULTISCRIPTELEMENT_H
