/* This file is part of the KDE project
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CalloutShape.h"

#include "EnhancedPathCommand.h"
#include "EnhancedPathFormula.h"
#include "EnhancedPathHandle.h"

#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoShapeSavingContext.h>
#include <KoUnit.h>
#include <KoOdfWorkaround.h>
#include <KoPathPoint.h>

CalloutShape::CalloutShape(const QRect &viewBox)
    : EnhancedPathShape(viewBox)
{
}

CalloutShape::~CalloutShape()
{
}

void CalloutShape::setType(const QString &type)
{
    m_type = type;
}

QString CalloutShape::type() const
{
    return m_type;
}

bool CalloutShape::isZero(qreal value)
{
    return qAbs(value) < 1E-5;
}

void CalloutShape::saveOdf(KoShapeSavingContext &context) const
{
    context.xmlWriter().startElement("draw:custom-shape");

    QSizeF currentSize = size();
    QPointF currentPos = position();

    // NOTE:
    // A callout shape position and size shall be the position and size
    // of the 'bubble' part of the callout.
    // The viewbox shall also be the size of the 'bubble' part.
    // This implies that the 'pointer' part will (normally)
    // go *outside* the viewbox (and shape).
    // This is imo (danders) not according to odf spec,
    // but is the way LO/OO does it.

    QRectF rect;
    if (m_viewBound.top() < m_viewBox.top()) {
        qreal topOffset = m_viewBound.top() - m_viewBox.top();
        if (!isZero(topOffset)) {
            topOffset *= currentSize.height() / m_viewBound.height();
            rect.setTop(topOffset);
        }
    }
    if (m_viewBound.left() < m_viewBox.left()) {
        qreal leftOffset = m_viewBound.left() - m_viewBox.left();
        if (!isZero(leftOffset)) {
            leftOffset *= currentSize.width() / m_viewBound.width();
            rect.setLeft(leftOffset);
        }
    }
    if (m_viewBound.bottom() > m_viewBox.bottom()) {
        qreal bottomOffset = m_viewBound.bottom() - m_viewBox.bottom();
        if (!isZero(bottomOffset)) {
            bottomOffset *= currentSize.height() / m_viewBound.height();
            rect.setBottom(bottomOffset);
        }
    }
    if (m_viewBound.right() > m_viewBox.right()) {
        qreal rightOffset = m_viewBound.right() - m_viewBox.right();
        if (!isZero(rightOffset)) {
            rightOffset *= currentSize.width() / m_viewBound.width();
            rect.setRight(rightOffset);
        }
    }
    qInfo()<<Q_FUNC_INFO<<"offsets:"<<rect;
    //FIXME: this needs to be fixed for shapes that are transformed by rotation or skewing
    saveOdfAttributes(context, OdfAllAttributes&~OdfSize&~OdfPosition&~OdfTransformation);

    currentPos -= rect.topLeft();
    context.xmlWriter().addAttributePt("svg:x", currentPos.x());
    context.xmlWriter().addAttributePt("svg:y", currentPos.y());

    currentSize -= rect.size();
    context.xmlWriter().addAttributePt("svg:width", currentSize.width());
    context.xmlWriter().addAttributePt("svg:height", currentSize.height());
    qInfo()<<Q_FUNC_INFO<<"pos:"<<position()<<"size:"<<this->size()<<"saved:"<<currentPos<<currentSize;

    saveText(context);

    context.xmlWriter().startElement("draw:enhanced-geometry");
    if (!m_type.isEmpty()) {
        context.xmlWriter().addAttribute("draw:type", m_type);
    }
    context.xmlWriter().addAttribute("svg:viewBox", QString("%1 %2 %3 %4").arg(m_viewBox.x()).arg(m_viewBox.y()).arg(m_viewBox.width()).arg(m_viewBox.height()));

    if (m_pathStretchPointX != -1) {
        context.xmlWriter().addAttribute("draw:path-stretchpoint-x", m_pathStretchPointX);
    }
    if (m_pathStretchPointY != -1) {
        context.xmlWriter().addAttribute("draw:path-stretchpoint-y", m_pathStretchPointY);
    }

    if (m_mirrorHorizontally) {
        context.xmlWriter().addAttribute("draw:mirror-horizontal", "true");
    }
    if (m_mirrorVertically) {
        context.xmlWriter().addAttribute("draw:mirror-vertical", "true");
    }

    QString modifiers;
    foreach (qreal modifier, m_modifiers) {
        modifiers += QString::number(modifier) + ' ';
    }
    context.xmlWriter().addAttribute("draw:modifiers", modifiers.trimmed());

    if (m_textArea.size() >= 4) {
        context.xmlWriter().addAttribute("draw:text-areas", m_textArea.join(" "));
    }

    QString path;
    foreach (EnhancedPathCommand * c, m_commands) {
        path += c->toString() + ' ';
    }
    context.xmlWriter().addAttribute("draw:enhanced-path", path.trimmed());

    FormulaStore::const_iterator i = m_formulae.constBegin();
    for (; i != m_formulae.constEnd(); ++i) {
        context.xmlWriter().startElement("draw:equation");
        context.xmlWriter().addAttribute("draw:name", i.key());
        context.xmlWriter().addAttribute("draw:formula", i.value()->toString());
        context.xmlWriter().endElement(); // draw:equation
    }

    foreach (EnhancedPathHandle * handle, m_enhancedHandles) {
        handle->saveOdf(context);
    }

    context.xmlWriter().endElement(); // draw:enhanced-geometry
    saveOdfCommonChildElements(context);
    context.xmlWriter().endElement(); // draw:custom-shape
}

bool CalloutShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
    reset();

    const KoXmlElement enhancedGeometry(KoXml::namedItemNS(element, KoXmlNS::draw, "enhanced-geometry" ) );
    if (!enhancedGeometry.isNull() ) {
        m_type = enhancedGeometry.attributeNS(KoXmlNS::draw, "type", "callout");

        setPathStretchPointX(enhancedGeometry.attributeNS(KoXmlNS::draw, "path-stretchpoint-x","-1").toDouble());
        setPathStretchPointY(enhancedGeometry.attributeNS(KoXmlNS::draw, "path-stretchpoint-y","-1").toDouble());

        // load the modifiers
        QString modifiers = enhancedGeometry.attributeNS(KoXmlNS::draw, "modifiers", "");
        if (! modifiers.isEmpty()) {
            addModifiers(modifiers);
        }

        m_textArea = enhancedGeometry.attributeNS(KoXmlNS::draw, "text-areas", "").split(' ');
        if (m_textArea.size() >= 4) {
            setResizeBehavior(TextFollowsPreferredTextRect);
        }

        KoXmlElement grandChild;
        forEachElement(grandChild, enhancedGeometry) {
            if (grandChild.namespaceURI() != KoXmlNS::draw)
                continue;
            if (grandChild.localName() == "equation") {
                QString name = grandChild.attributeNS(KoXmlNS::draw, "name");
                QString formula = grandChild.attributeNS(KoXmlNS::draw, "formula");
                addFormula(name, formula);
            } else if (grandChild.localName() == "handle") {
                EnhancedPathHandle * handle = new EnhancedPathHandle(this);
                if (handle->loadOdf(grandChild, context)) {
                    m_enhancedHandles.append(handle);
                    evaluateHandles();
                } else {
                    delete handle;
                }
            }

        }

        setMirrorHorizontally(enhancedGeometry.attributeNS(KoXmlNS::draw, "mirror-horizontal") == "true");
        setMirrorVertically(enhancedGeometry.attributeNS(KoXmlNS::draw, "mirror-vertical") == "true");

        // load the enhanced path data
        QString path = enhancedGeometry.attributeNS(KoXmlNS::draw, "enhanced-path", "");

        // load the viewbox
        m_viewBox = loadOdfViewbox(enhancedGeometry);

        if (!path.isEmpty()) {
            parsePathData(path);
        }

        if (m_viewBox.isEmpty()) {
            // if there is no view box defined make it is big as the path.
            m_viewBox = m_viewBound.toAlignedRect();
        }
    }

    QSizeF size;
    size.setWidth(KoUnit::parseValue(element.attributeNS(KoXmlNS::svg, "width", QString())));
    size.setHeight(KoUnit::parseValue(element.attributeNS(KoXmlNS::svg, "height", QString())));
    // the viewbox is to be fitted into the size of the shape, so before setting
    // the size we just loaded, we set the viewbox to be the basis to calculate
    // the viewbox matrix from
    m_viewBound = m_viewBox;
    setSize(size);

    QPointF pos;
    pos.setX(KoUnit::parseValue(element.attributeNS(KoXmlNS::svg, "x", QString())));
    pos.setY(KoUnit::parseValue(element.attributeNS(KoXmlNS::svg, "y", QString())));
    setPosition(pos - m_viewMatrix.map(QPointF(0, 0)) - m_viewBoxOffset);

    loadOdfAttributes(element, context, OdfMandatories | OdfTransformation | OdfAdditionalAttributes | OdfCommonChildElements);

    loadText(element, context);

    qInfo()<<Q_FUNC_INFO<<"pos:"<<pos<<"size:"<<size<<"->"<<position()<<this->size();
    return true;
}

