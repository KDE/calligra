/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormulaRenderer.h"

#include "AttributeManager.h"
#include "BasicElement.h"
#include "FormulaDebug.h"

FormulaRenderer::FormulaRenderer()
{
    m_dirtyElement = nullptr;
    m_attributeManager = new AttributeManager();
}

FormulaRenderer::~FormulaRenderer()
{
    delete m_attributeManager;
}

void FormulaRenderer::paintElement(QPainter &p, BasicElement *element, bool hints)
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(element->origin()); // setup painter
    if (!hints) {
        element->paint(p, m_attributeManager); // let element paint itself
    } else {
        element->paintEditingHints(p, m_attributeManager);
    }

    // eventually paint all its children
    if (!element->childElements().isEmpty() && element->elementType() != Phantom) {
        foreach (BasicElement *tmpElement, element->childElements()) {
            paintElement(p, tmpElement, hints);
        }
    }

    p.restore();
}

void FormulaRenderer::layoutElement(BasicElement *element)
{
    int i = 0;
    element->setDisplayStyle(m_attributeManager->boolOf("displaystyle", element));
    foreach (BasicElement *tmp, element->childElements()) {
        int scale = m_attributeManager->scriptLevel(element, i++);
        tmp->setScaleLevel(scale);
        layoutElement(tmp); // first layout all children
    }
    element->layout(m_attributeManager); // actually layout the element
    element->stretch();
}

void FormulaRenderer::update(QPainter &p, BasicElement *element)
{
    updateElementLayout(element); // relayout the changed element
    paintElement(p, m_dirtyElement); // and then repaint as much as needed
}

void FormulaRenderer::updateElementLayout(BasicElement *element)
{
    QRectF tmpBoundingRect;
    bool parentLayoutAffected = true;
    BasicElement *tmpElement = element;
    while (parentLayoutAffected) {
        tmpBoundingRect = tmpElement->boundingRect(); // cache the former boundingRect
        tmpElement->layout(m_attributeManager); // layout the element

        // check whether the new layout affects the parent element's layout
        if (tmpBoundingRect == tmpElement->boundingRect()) {
            parentLayoutAffected = false; // stop the layouting
            m_dirtyElement = tmpElement;
        } else
            tmpElement = tmpElement->parentElement(); // prepare layouting the parent
    }
}

qreal FormulaRenderer::elementScaleFactor(BasicElement *element) const
{
    Q_UNUSED(element)
    AttributeManager am;
    return -1; // FIXME!
}
