/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMULARENDERER_H
#define FORMULARENDERER_H

#include "koformula_export.h"
#include <QPainter>

class AttributeManager;
class BasicElement;

/**
 * @short FormulaRenderer takes care of painting and layouting the elements
 *
 * FormulaRenderer follows the visitor pattern. It iterates through the element
 * tree and calls layout() or paint() methods to let the single elements layout
 * or paint itsselves. This more generic approach allows more efficient repainting
 * and relayouting.
 * The update() method is the most interesting of the class as it is used to update
 * the visuals when the formula tree has changed somehow. The method calls
 * layoutElement() and then paintElement(). The former is made to be efficient so it
 * layouts only as many parental elements as needed.
 * Using a central class for painting parts or the whole tree structure that makes
 * up a formula has several advantages. First it reduces a lot of code duplication.
 * Second it takes care of painting and layouting in the right order so that there
 * no need anymore for the single element classes to do so. Third we can control
 * instance AttributeManager and destroying and constructing it often is not needed
 * anymore.
 *
 * @author Martin Pfeiffer <hubipete@gmx.net>
 */
class KOFORMULA_EXPORT FormulaRenderer
{
public:
    /// The constructor
    FormulaRenderer();

    /// The destructor
    ~FormulaRenderer();

    /**
     * Paint an element and all its children
     * @param p The QPainter that should be used to paint the element
     * @param element The element to be painted
     * @param hints Whether to show the hints
     */
    void paintElement(QPainter &p, BasicElement *element, bool hints = false);

    /**
     * Layout an element and all its children
     * @param element The element to be layouted
     */
    void layoutElement(BasicElement *element);

    /**
     * Update an element after it has changed
     * @param p The QPainter that should be used to paint the element
     * @param element The element that has changed
     */
    void update(QPainter &p, BasicElement *element);

    /// Just for updating one elements layout after a change
    void updateElementLayout(BasicElement *element);

private:
    qreal elementScaleFactor(BasicElement *element) const;

    /// The attribute manager used for rendering and layouting
    AttributeManager *m_attributeManager;

    /// Used by update() to store the highest element in the tree that needs repaint
    BasicElement *m_dirtyElement;
};

#endif // FORMULARENDERER_H
