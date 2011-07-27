/* This file is part of the KDE project
 * Copyright (C) 2002-2005,2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002-2004 Nicolas Goutte <nicolasg@snafu.de>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005-2009 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005,2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006-2007 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2007-2008,2010 Thorsten Zachmann <zachmann@kde.org>

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

#include "SvgParser_generic.h"
#include "SvgUtil.h"
#include "SvgTextHelper.h"

//#include <KarbonGlobal.h>
//#include <KarbonPart.h>

#include <KoShape.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeLayer.h>
#include <KoShapeContainer.h>
#include <KoShapeGroup.h>
#include <KoPathShape.h>
#include <KoResourceManager.h>
#include <KoPathShapeLoader.h>
#include <commands/KoShapeGroupCommand.h>
#include <commands/KoShapeUngroupCommand.h>
#include <KoImageData.h>
#include <KoImageCollection.h>

#include <pathshapes/rectangle/RectangleShape.h>
#include <pathshapes/ellipse/EllipseShape.h>
#include <plugins/artistictextshape/ArtisticTextShape.h>
#include <plugins/presentationviewportshape/PresentationViewPortShape.h>

#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoPatternBackground.h>
#include <KoFilterEffectRegistry.h>
#include <KoFilterEffect.h>
#include "KoFilterEffectStack.h"
#include "KoFilterEffectLoadingContext.h"
#include <KoClipPath.h>

#include <KDebug>
#include <QDebug>

#include <QtGui/QColor>


SvgParser_generic::SvgParser_generic(KoResourceManager *documentResourceManager)
    : m_documentResourceManager(documentResourceManager)
{
    // the order of the font attributes is important, don't change without reason !!!
    m_fontAttributes << "font-family" << "font-size" << "font-weight";
    m_fontAttributes << "text-decoration" << "letter-spacing" << "word-spacing" << "baseline-shift";
    // the order of the style attributes is important, don't change without reason !!!
    m_styleAttributes << "color" << "display";
    m_styleAttributes << "fill" << "fill-rule" << "fill-opacity";
    m_styleAttributes << "stroke" << "stroke-width" << "stroke-linejoin" << "stroke-linecap";
    m_styleAttributes << "stroke-dasharray" << "stroke-dashoffset" << "stroke-opacity" << "stroke-miterlimit";
    m_styleAttributes << "opacity" << "filter" << "clip-path" << "clip-rule";
     
}

SvgParser_generic::~SvgParser_generic()
{
}

void SvgParser_generic::setXmlBaseDir(const QString &baseDir)
{
    m_context.setInitialXmlBaseDir(baseDir);
}

QList<KoShape*> SvgParser_generic::shapes() const
{
    return m_shapes;
}

// Helper functions
// ---------------------------------------------------------------------------------------

KoShape * SvgParser_generic::findObject(const QString &name, const QList<KoShape*> & shapes)
{
    foreach(KoShape *shape, shapes) {
        if (shape->name() == name)
            return shape;

        KoShape *resultShape = findObject(name, dynamic_cast<KoShapeContainer*>(shape));
        if (resultShape)
            return resultShape;
    }

    return 0;
}

KoShape * SvgParser_generic::findObject(const QString &name, KoShapeContainer *group)
{
    if (! group)
        return 0;

    foreach(KoShape *shape, group->shapes()) {
        if (shape->name() == name)
            return shape;

        KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(shape);
        if (container) {
            KoShape *resultShape = findObject(name, container);
            if (resultShape)
                return resultShape;
        }
    }

    return 0;
}

KoShape * SvgParser_generic::findObject(const QString &name)
{
    return findObject(name, m_shapes);
}

SvgGradientHelper* SvgParser_generic::findGradient(const QString &id, const QString &href)
{
    // check if gradient was already parsed, and return it
    if (m_gradients.contains(id))
        return &m_gradients[ id ];

    // check if gradient was stored for later parsing
    if (!m_defs.contains(id))
        return 0;

    const KoXmlElement &e = m_defs[ id ];
    if (!e.tagName().contains("Gradient"))
        return 0;

    if (e.childNodesCount() == 0) {
        QString mhref = e.attribute("xlink:href").mid(1);

        if (m_defs.contains(mhref))
            return findGradient(mhref, id);
        else
            return 0;
    } else {
        // ok parse gradient now
        if (! parseGradient(m_defs[ id ], m_defs[ href ]))
            return 0;
    }

    // return successfully parsed gradient or NULL
    QString n;
    if (href.isEmpty())
        n = id;
    else
        n = href;

    if (m_gradients.contains(n))
        return &m_gradients[ n ];
    else
        return 0;
}

SvgPatternHelper* SvgParser_generic::findPattern(const QString &id)
{
    // check if pattern was already parsed, and return it
    if (m_patterns.contains(id))
        return &m_patterns[ id ];

    // check if pattern was stored for later parsing
    if (!m_defs.contains(id))
        return 0;

    SvgPatternHelper pattern;

    const KoXmlElement &e = m_defs[ id ];
    if (e.tagName() != "pattern")
        return 0;

    // are we referencing another pattern ?
    if (e.hasAttribute("xlink:href")) {
        QString mhref = e.attribute("xlink:href").mid(1);
        SvgPatternHelper *refPattern = findPattern(mhref);
        // inherit attributes of referenced pattern
        if (refPattern)
            pattern = *refPattern;
    }

    // ok parse pattern now
    parsePattern(pattern, m_defs[ id ]);
    // add to parsed pattern list
    m_patterns.insert(id, pattern);

    return &m_patterns[ id ];
}

SvgFilterHelper* SvgParser_generic::findFilter(const QString &id, const QString &href)
{
    // check if filter was already parsed, and return it
    if (m_filters.contains(id))
        return &m_filters[ id ];

    // check if filter was stored for later parsing
    if (!m_defs.contains(id))
        return 0;

    KoXmlElement e = m_defs[ id ];
    if (e.childNodesCount() == 0) {
        QString mhref = e.attribute("xlink:href").mid(1);

        if (m_defs.contains(mhref))
            return findFilter(mhref, id);
        else
            return 0;
    } else {
        // ok parse filter now
        if (! parseFilter(m_defs[ id ], m_defs[ href ]))
            return 0;
    }

    // return successfully parsed filter or NULL
    QString n;
    if (href.isEmpty())
        n = id;
    else
        n = href;

    if (m_filters.contains(n))
        return &m_filters[ n ];
    else
        return 0;
}

SvgClipPathHelper* SvgParser_generic::findClipPath(const QString &id, const QString &href)
{
    // check if clip path was already parsed, and return it
    if (m_clipPaths.contains(id))
        return &m_clipPaths[ id ];

    // check if clip path was stored for later parsing
    if (!m_defs.contains(id))
        return 0;

    KoXmlElement e = m_defs[ id ];
    if (e.childNodesCount() == 0) {
        QString mhref = e.attribute("xlink:href").mid(1);

        if (m_defs.contains(mhref))
            return findClipPath(mhref, id);
        else
            return 0;
    } else {
        // ok clip path filter now
        if (! parseClipPath(m_defs[ id ], m_defs[ href ]))
            return 0;
    }

    // return successfully parsed clip path or NULL
    const QString n = href.isEmpty() ? id : href;
    if (m_clipPaths.contains(n))
        return &m_clipPaths[ n ];
    else
        return 0;
}

SvgParser_generic::SvgStyles SvgParser_generic::mergeStyles(const SvgStyles &referencedBy, const SvgStyles &referencedStyles)
{
    // 1. use all styles of the referencing styles
    SvgStyles mergedStyles = referencedBy;
    // 2. use all styles of the referenced style which are not in the referencing styles
    SvgStyles::const_iterator it = referencedStyles.constBegin();
    for (; it != referencedStyles.constEnd(); ++it) {
        if (!referencedBy.contains(it.key())) {
            mergedStyles.insert(it.key(), it.value());
        }
    }
    return mergedStyles;
}

// Parsing functions
// ---------------------------------------------------------------------------------------

qreal SvgParser_generic::parseUnit(const QString &unit, bool horiz, bool vert, const QRectF &bbox)
{
    return SvgUtil::parseUnit(m_context.currentGC(), unit, horiz, vert, bbox);
}

qreal SvgParser_generic::parseUnitX(const QString &unit)
{
    return SvgUtil::parseUnitX(m_context.currentGC(), unit);
}

qreal SvgParser_generic::parseUnitY(const QString &unit)
{
    return SvgUtil::parseUnitY(m_context.currentGC(), unit);
}

qreal SvgParser_generic::parseUnitXY(const QString &unit)
{
    return SvgUtil::parseUnitXY(m_context.currentGC(), unit);
}

bool SvgParser_generic::parseColor(QColor &color, const QString &s)
{
    if (s.isEmpty() || s == "none")
        return false;

    if (s.startsWith(QLatin1String("rgb("))) {
        QString parse = s.trimmed();
        QStringList colors = parse.split(',');
        QString r = colors[0].right((colors[0].length() - 4));
        QString g = colors[1];
        QString b = colors[2].left((colors[2].length() - 1));

        if (r.contains("%")) {
            r = r.left(r.length() - 1);
            r = QString::number(int((double(255 * r.toDouble()) / 100.0)));
        }

        if (g.contains("%")) {
            g = g.left(g.length() - 1);
            g = QString::number(int((double(255 * g.toDouble()) / 100.0)));
        }

        if (b.contains("%")) {
            b = b.left(b.length() - 1);
            b = QString::number(int((double(255 * b.toDouble()) / 100.0)));
        }

        color = QColor(r.toInt(), g.toInt(), b.toInt());
    } else if (s == "currentColor") {
        color = m_context.currentGC()->currentColor;
    } else {
        // QColor understands #RRGGBB and svg color names
        color.setNamedColor(s.trimmed());
    }

    return true;
}

void SvgParser_generic::parseColorStops(QGradient *gradient, const KoXmlElement &e)
{
    QGradientStops stops;
    QColor c;

    for (KoXmlNode n = e.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement stop = n.toElement();
        if (stop.tagName() == "stop") {
            float offset;
            QString temp = stop.attribute("offset");
            if (temp.contains('%')) {
                temp = temp.left(temp.length() - 1);
                offset = temp.toFloat() / 100.0;
            } else
                offset = temp.toFloat();

            QString stopColorStr = stop.attribute("stop-color");
            if (!stopColorStr.isEmpty()) {
                if (stopColorStr == "inherit") {
                    stopColorStr = inheritedAttribute("stop-color", stop);
                }
                parseColor(c, stopColorStr);
            }
            else {
                // try style attr
                QString style = stop.attribute("style").simplified();
                QStringList substyles = style.split(';', QString::SkipEmptyParts);
                for (QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it) {
                    QStringList substyle = it->split(':');
                    QString command = substyle[0].trimmed();
                    QString params  = substyle[1].trimmed();
                    if (command == "stop-color")
                        parseColor(c, params);
                    if (command == "stop-opacity")
                        c.setAlphaF(params.toDouble());
                }

            }
            QString opacityStr = stop.attribute("stop-opacity");
            if (!opacityStr.isEmpty()) {
                if (opacityStr == "inherit") {
                    opacityStr = inheritedAttribute("stop-opacity", stop);
                }
                c.setAlphaF(opacityStr.toDouble());
            }
            stops.append(QPair<qreal, QColor>(offset, c));
        }
    }
    if (stops.count())
        gradient->setStops(stops);
}

bool SvgParser_generic::parseGradient(const KoXmlElement &e, const KoXmlElement &referencedBy)
{
    // IMPROVEMENTS:
    // - Store the parsed colorstops in some sort of a cache so they don't need to be parsed again.
    // - A gradient inherits attributes it does not have from the referencing gradient.
    // - Gradients with no color stops have no fill or stroke.
    // - Gradients with one color stop have a solid color.

    SvgGraphicsContext *gc = m_context.currentGC();
    if (!gc)
        return false;

    SvgGradientHelper gradhelper;

    if (e.hasAttribute("xlink:href")) {
        QString href = e.attribute("xlink:href").mid(1);
        if (! href.isEmpty()) {
            // copy the referenced gradient if found
            SvgGradientHelper *pGrad = findGradient(href);
            if (pGrad)
                gradhelper = *pGrad;
        } else {
            //gc->fillType = SvgGraphicsContext::None; // <--- TODO Fill OR Stroke are none
            return false;
        }
    }

    // Use the gradient that is referencing, or if there isn't one, the original gradient.
    KoXmlElement b;
    if (!referencedBy.isNull())
        b = referencedBy;
    else
        b = e;

    QString gradientId = b.attribute("id");

    if (! gradientId.isEmpty()) {
        // check if we have this gradient already parsed
        // copy existing gradient if it exists
        if (m_gradients.find(gradientId) != m_gradients.end())
            gradhelper.copyGradient(m_gradients[ gradientId ].gradient());
    }

    if (b.attribute("gradientUnits") == "userSpaceOnUse")
        gradhelper.setGradientUnits(SvgGradientHelper::UserSpaceOnUse);

    // parse color prop
    QColor c = gc->currentColor;

    if (!b.attribute("color").isEmpty()) {
        parseColor(c, b.attribute("color"));
    } else {
        // try style attr
        QString style = b.attribute("style").simplified();
        QStringList substyles = style.split(';', QString::SkipEmptyParts);
        for (QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it) {
            QStringList substyle = it->split(':');
            QString command = substyle[0].trimmed();
            QString params  = substyle[1].trimmed();
            if (command == "color")
                parseColor(c, params);
        }
    }
    gc->currentColor = c;

    if (b.tagName() == "linearGradient") {
        QLinearGradient *g = new QLinearGradient();
        if (gradhelper.gradientUnits() == SvgGradientHelper::ObjectBoundingBox) {
            g->setCoordinateMode(QGradient::ObjectBoundingMode);
            g->setStart(QPointF(SvgUtil::fromPercentage(b.attribute("x1", "0%")),
                                SvgUtil::fromPercentage(b.attribute("y1", "0%"))));
            g->setFinalStop(QPointF(SvgUtil::fromPercentage(b.attribute("x2", "100%")),
                                    SvgUtil::fromPercentage(b.attribute("y2", "0%"))));
        } else {
            g->setStart(QPointF(SvgUtil::fromUserSpace(b.attribute("x1").toDouble()),
                                SvgUtil::fromUserSpace(b.attribute("y1").toDouble())));
            g->setFinalStop(QPointF(SvgUtil::fromUserSpace(b.attribute("x2").toDouble()),
                                    SvgUtil::fromUserSpace(b.attribute("y2").toDouble())));
        }
        // preserve color stops
        if (gradhelper.gradient())
            g->setStops(gradhelper.gradient()->stops());
        gradhelper.setGradient(g);
    } else if (b.tagName() == "radialGradient") {
        QRadialGradient *g = new QRadialGradient();
        if (gradhelper.gradientUnits() == SvgGradientHelper::ObjectBoundingBox) {
            g->setCoordinateMode(QGradient::ObjectBoundingMode);
            g->setCenter(QPointF(SvgUtil::fromPercentage(b.attribute("cx", "50%")),
                                 SvgUtil::fromPercentage(b.attribute("cy", "50%"))));
            g->setRadius(SvgUtil::fromPercentage(b.attribute("r", "50%")));
            g->setFocalPoint(QPointF(SvgUtil::fromPercentage(b.attribute("fx", "50%")),
                                     SvgUtil::fromPercentage(b.attribute("fy", "50%"))));
        } else {
            g->setCenter(QPointF(SvgUtil::fromUserSpace(b.attribute("cx").toDouble()),
                                 SvgUtil::fromUserSpace(b.attribute("cy").toDouble())));
            g->setFocalPoint(QPointF(SvgUtil::fromUserSpace(b.attribute("fx").toDouble()),
                                     SvgUtil::fromUserSpace(b.attribute("fy").toDouble())));
            g->setRadius(SvgUtil::fromUserSpace(b.attribute("r").toDouble()));
        }
        // preserve color stops
        if (gradhelper.gradient())
            g->setStops(gradhelper.gradient()->stops());
        gradhelper.setGradient(g);
    } else {
        return false;
    }

    // handle spread method
    QString spreadMethod = b.attribute("spreadMethod");
    if (!spreadMethod.isEmpty()) {
        if (spreadMethod == "reflect")
            gradhelper.gradient()->setSpread(QGradient::ReflectSpread);
        else if (spreadMethod == "repeat")
            gradhelper.gradient()->setSpread(QGradient::RepeatSpread);
        else
            gradhelper.gradient()->setSpread(QGradient::PadSpread);
    } else
        gradhelper.gradient()->setSpread(QGradient::PadSpread);

    // Parse the color stops. The referencing gradient does not have colorstops,
    // so use the stops from the gradient it references to (e in this case and not b)
    parseColorStops(gradhelper.gradient(), e);
    gradhelper.setTransform(SvgUtil::parseTransform(b.attribute("gradientTransform")));
    m_gradients.insert(gradientId, gradhelper);

    return true;
}

void SvgParser_generic::parsePattern(SvgPatternHelper &pattern, const KoXmlElement &e)
{
    if (e.hasAttribute("patternUnits")) {
        if (e.attribute("patternUnits") == "userSpaceOnUse")
            pattern.setPatternUnits(SvgPatternHelper::UserSpaceOnUse);
    }
    if (e.hasAttribute("patternContentUnits")) {
        if (e.attribute("patternContentUnits") == "objectBoundingBox")
            pattern.setPatternContentUnits(SvgPatternHelper::ObjectBoundingBox);
    }
    if (e.hasAttribute("viewBox"))
        pattern.setPatternContentViewbox(SvgUtil::parseViewBox(e.attribute("viewBox")));
    if (e.hasAttribute("patternTransform"))
        pattern.setTransform(SvgUtil::parseTransform(e.attribute("patternTransform")));


    // parse tile reference rectangle
    if (pattern.patternUnits() == SvgPatternHelper::UserSpaceOnUse) {
        if (e.hasAttribute("x") && e.hasAttribute("y")) {
            pattern.setPosition(QPointF(parseUnitX(e.attribute("x")),
                                        parseUnitY(e.attribute("y"))));
        }
        if (e.hasAttribute("width") && e.hasAttribute("height")) {
            pattern.setSize(QSizeF(parseUnitX(e.attribute("width")),
                                   parseUnitY(e.attribute("height"))));
        }
    } else {
        // x, y, width, height are in percentages of the object referencing the pattern
        // so we just parse the percentages
        if (e.hasAttribute("x") && e.hasAttribute("y")) {
            pattern.setPosition(QPointF(SvgUtil::fromPercentage(e.attribute("x")),
                                        SvgUtil::fromPercentage(e.attribute("y"))));
        }
        if (e.hasAttribute("width") && e.hasAttribute("height")) {
            pattern.setSize(QSizeF(SvgUtil::fromPercentage(e.attribute("width")),
                                   SvgUtil::fromPercentage(e.attribute("height"))));
        }
    }

    if (e.hasChildNodes()) {
        pattern.setContent(e);
    }
}

bool SvgParser_generic::parseFilter(const KoXmlElement &e, const KoXmlElement &referencedBy)
{
    SvgFilterHelper filter;

    // Use the filter that is referencing, or if there isn't one, the original filter
    KoXmlElement b;
    if (!referencedBy.isNull())
        b = referencedBy;
    else
        b = e;

    // check if we are referencing another filter
    if (e.hasAttribute("xlink:href")) {
        QString href = e.attribute("xlink:href").mid(1);
        if (! href.isEmpty()) {
            // copy the referenced filter if found
            SvgFilterHelper *refFilter = findFilter(href);
            if (refFilter)
                filter = *refFilter;
        }
    } else {
        filter.setContent(b);
    }

    if (b.attribute("filterUnits") == "userSpaceOnUse")
        filter.setFilterUnits(SvgFilterHelper::UserSpaceOnUse);
    if (b.attribute("primitiveUnits") == "objectBoundingBox")
        filter.setPrimitiveUnits(SvgFilterHelper::ObjectBoundingBox);

    // parse filter region rectangle
    if (filter.filterUnits() == SvgFilterHelper::UserSpaceOnUse) {
        filter.setPosition(QPointF(parseUnitX(b.attribute("x")),
                                   parseUnitY(b.attribute("y"))));
        filter.setSize(QSizeF(parseUnitX(b.attribute("width")),
                              parseUnitY(b.attribute("height"))));
    } else {
        // x, y, width, height are in percentages of the object referencing the filter
        // so we just parse the percentages
        filter.setPosition(QPointF(SvgUtil::fromPercentage(b.attribute("x", "-0.1")),
                                   SvgUtil::fromPercentage(b.attribute("y", "-0.1"))));
        filter.setSize(QSizeF(SvgUtil::fromPercentage(b.attribute("width", "1.2")),
                              SvgUtil::fromPercentage(b.attribute("height", "1.2"))));
    }

    m_filters.insert(b.attribute("id"), filter);

    return true;
}

bool SvgParser_generic::parseClipPath(const KoXmlElement &e, const KoXmlElement &referencedBy)
{
    SvgClipPathHelper clipPath;

    // Use the filter that is referencing, or if there isn't one, the original filter
    KoXmlElement b;
    if (!referencedBy.isNull())
        b = referencedBy;
    else
        b = e;

    // check if we are referencing another clip path
    if (e.hasAttribute("xlink:href")) {
        QString href = e.attribute("xlink:href").mid(1);
        if (! href.isEmpty()) {
            // copy the referenced clip path if found
            SvgClipPathHelper *refClipPath = findClipPath(href);
            if (refClipPath)
                clipPath = *refClipPath;
        }
    } else {
        clipPath.setContent(b);
    }

    if (b.attribute("clipPathUnits") == "objectBoundingBox")
        clipPath.setClipPathUnits(SvgClipPathHelper::ObjectBoundingBox);


    m_clipPaths.insert(b.attribute("id"), clipPath);

    return true;
}

bool SvgParser_generic::parseImage(const QString &attribute, QImage &image)
{
    if (attribute.startsWith(QLatin1String("data:"))) {
        int start = attribute.indexOf("base64,");
        if (start > 0 && image.loadFromData(QByteArray::fromBase64(attribute.mid(start + 7).toLatin1())))
            return true;
    } else if (image.load(m_context.absoluteFilePath(attribute))) {
        return true;
    }

    return false;
}

void SvgParser_generic::parsePA(SvgGraphicsContext *gc, const QString &command, const QString &params)
{
    QColor fillcolor = gc->fillColor;
    QColor strokecolor = gc->stroke.color();

    if (params == "inherit")
        return;

    if (command == "fill") {
        if (params == "none") {
            gc->fillType = SvgGraphicsContext::None;
        } else if (params.startsWith(QLatin1String("url("))) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            QString key = params.mid(start, end - start);
            // try to find referenced gradient
            SvgGradientHelper *gradHelper = findGradient(key);
            if (gradHelper) {
                // great, we have a gradient fill
                gc->fillType = SvgGraphicsContext::Gradient;
                gc->fillId = key;
            } else {
                // try to find referenced pattern
                SvgPatternHelper *pattern = findPattern(key);
                if (pattern) {
                    // great we have a pattern fill
                    gc->fillType = SvgGraphicsContext::Pattern;
                    gc->fillId = key;
                } else {
                    // no referenced fill found -> reset fill id
                    gc->fillId.clear();
                    kDebug() << params.mid(end + 1).trimmed();
                    // check if there is a fallback color
                    if (parseColor(fillcolor, params.mid(end + 1).trimmed()))
                        gc->fillType = SvgGraphicsContext::Solid;
                    else
                        gc->fillType = SvgGraphicsContext::None;
                }
            }
        } else {
            // great we have a solid fill
            gc->fillType = SvgGraphicsContext::Solid;
            parseColor(fillcolor,  params);
        }
    } else if (command == "fill-rule") {
        if (params == "nonzero")
            gc->fillRule = Qt::WindingFill;
        else if (params == "evenodd")
            gc->fillRule = Qt::OddEvenFill;
    } else if (command == "stroke") {
        if (params == "none") {
            gc->strokeType = SvgGraphicsContext::None;
        } else if (params.startsWith(QLatin1String("url("))) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            QString key = params.mid(start, end - start);
            // try to find referenced gradient
            SvgGradientHelper *gradHelper = findGradient(key);
            if (gradHelper) {
                // great, we have a gradient stroke
                gc->strokeType = SvgGraphicsContext::Gradient;
                gc->strokeId = key;
            } else {
                // no referenced stroke found -> reset stroke id
                gc->strokeId.clear();
                // check if there is a fallback color
                if (parseColor(strokecolor, params.mid(end + 1).trimmed()))
                    gc->fillType = SvgGraphicsContext::Solid;
                else
                    gc->fillType = SvgGraphicsContext::None;
            }
        } else {
            // great we have a solid stroke
            gc->strokeType = SvgGraphicsContext::Solid;
            parseColor(strokecolor, params);
        }
    } else if (command == "stroke-width") {
        gc->stroke.setLineWidth(parseUnitXY(params));
    } else if (command == "stroke-linejoin") {
        if (params == "miter")
            gc->stroke.setJoinStyle(Qt::MiterJoin);
        else if (params == "round")
            gc->stroke.setJoinStyle(Qt::RoundJoin);
        else if (params == "bevel")
            gc->stroke.setJoinStyle(Qt::BevelJoin);
    } else if (command == "stroke-linecap") {
        if (params == "butt")
            gc->stroke.setCapStyle(Qt::FlatCap);
        else if (params == "round")
            gc->stroke.setCapStyle(Qt::RoundCap);
        else if (params == "square")
            gc->stroke.setCapStyle(Qt::SquareCap);
    } else if (command == "stroke-miterlimit") {
        gc->stroke.setMiterLimit(params.toFloat());
    } else if (command == "stroke-dasharray") {
        QVector<qreal> array;
        if (params != "none") {
            QString dashString = params;
            QStringList dashes = dashString.replace(',', ' ').simplified().split(' ');
            for (QStringList::Iterator it = dashes.begin(); it != dashes.end(); ++it)
                array.append((*it).toFloat());
        }
        gc->stroke.setLineStyle(Qt::CustomDashLine, array);
    } else if (command == "stroke-dashoffset") {
        gc->stroke.setDashOffset(params.toFloat());
    }
    // handle opacity
    else if (command == "stroke-opacity")
        strokecolor.setAlphaF(SvgUtil::fromPercentage(params));
    else if (command == "fill-opacity") {
        float opacity = SvgUtil::fromPercentage(params);
        if (opacity < 0.0)
            opacity = 0.0;
        if (opacity > 1.0)
            opacity = 1.0;
        fillcolor.setAlphaF(opacity);
    } else if (command == "opacity") {
        gc->opacity = SvgUtil::fromPercentage(params);
    } else if (command == "font-family") {
        QString family = params;
        family.replace('\'' , ' ');
        gc->font.setFamily(family);
    } else if (command == "font-size") {
        float pointSize = parseUnitY(params);
        if (pointSize > 0.0f)
            gc->font.setPointSizeF(pointSize);
    } else if (command == "font-weight") {
        int weight = QFont::Normal;

        // map svg weight to qt weight
        // svg value        qt value
        // 100,200,300      1, 17, 33
        // 400              50          (normal)
        // 500,600          58,66
        // 700              75          (bold)
        // 800,900          87,99

        if (params == "bold")
            weight = QFont::Bold;
        else if (params == "lighter") {
            weight = gc->font.weight();
            if (weight <= 17)
                weight = 1;
            else if (weight <= 33)
                weight = 17;
            else if (weight <= 50)
                weight = 33;
            else if (weight <= 58)
                weight = 50;
            else if (weight <= 66)
                weight = 58;
            else if (weight <= 75)
                weight = 66;
            else if (weight <= 87)
                weight = 75;
            else if (weight <= 99)
                weight = 87;
        } else if (params == "bolder") {
            weight = gc->font.weight();
            if (weight >= 87)
                weight = 99;
            else if (weight >= 75)
                weight = 87;
            else if (weight >= 66)
                weight = 75;
            else if (weight >= 58)
                weight = 66;
            else if (weight >= 50)
                weight = 58;
            else if (weight >= 33)
                weight = 50;
            else if (weight >= 17)
                weight = 50;
            else if (weight >= 1)
                weight = 17;
        } else {
            bool ok;
            // try to read numerical weight value
            weight = params.toInt(&ok, 10);

            if (!ok)
                return;

            switch (weight) {
            case 100: weight = 1; break;
            case 200: weight = 17; break;
            case 300: weight = 33; break;
            case 400: weight = 50; break;
            case 500: weight = 58; break;
            case 600: weight = 66; break;
            case 700: weight = 75; break;
            case 800: weight = 87; break;
            case 900: weight = 99; break;
            }
        }
        gc->font.setWeight(weight);
    } else if (command == "text-decoration") {
        if (params == "line-through")
            gc->font.setStrikeOut(true);
        else if (params == "underline")
            gc->font.setUnderline(true);
    } else if (command == "letter-spacing") {
        gc->letterSpacing = parseUnitX(params);
         } else if (command == "baseline-shift") {
        gc->baselineShift = params;
    } else if (command == "word-spacing") {
        gc->wordSpacing = parseUnitX(params);
    } else if (command == "color") {
        QColor color;
        parseColor(color, params);
        gc->currentColor = color;
    } else if (command == "display") {
        if (params == "none")
            gc->display = false;
    } else if (command == "filter") {
        if (params != "none" && params.startsWith("url(")) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            gc->filterId = params.mid(start, end - start);
        }
    } else if (command == "clip-path") {
        if (params != "none" && params.startsWith("url(")) {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.indexOf(')', start);
            gc->clipPathId = params.mid(start, end - start);
        }
    } else if (command == "clip-rule") {
        if (params == "nonzero")
            gc->clipRule = Qt::WindingFill;
        else if (params == "evenodd")
            gc->clipRule = Qt::OddEvenFill;
    }

    gc->fillColor = fillcolor;
    gc->stroke.setColor(strokecolor);
}

SvgParser_generic::SvgStyles SvgParser_generic::collectStyles(const KoXmlElement &e)
{
    SvgStyles styleMap;

    // collect individual presentation style attributes which have the priority 0
    foreach(const QString & command, m_styleAttributes) {
        if (e.hasAttribute(command))
            styleMap[command] = e.attribute(command);
    }
    foreach(const QString & command, m_fontAttributes) {
        if (e.hasAttribute(command))
            styleMap[command] = e.attribute(command);
    }

    // match css style rules to element
    QStringList cssStyles = m_cssStyles.matchStyles(e);

    // collect all css style attributes
    foreach(const QString &style, cssStyles) {
        QStringList substyles = style.split(';', QString::SkipEmptyParts);
        if (!substyles.count())
            continue;
        for (QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it) {
            QStringList substyle = it->split(':');
            if (substyle.count() != 2)
                continue;
            QString command = substyle[0].trimmed();
            QString params  = substyle[1].trimmed();
            // only use style and font attributes
            if (m_styleAttributes.contains(command) || m_fontAttributes.contains(command))
                styleMap[command] = params;
        }
    }

    // replace keyword "inherit" for style values
    QMutableMapIterator<QString, QString> it(styleMap);
    while (it.hasNext()) {
        it.next();
        if (it.value() == "inherit") {
            it.setValue(inheritedAttribute(it.key(), e));
        }
    }

    return styleMap;
}

void SvgParser_generic::parseStyle(KoShape *obj, const KoXmlElement &e)
{
    parseStyle(obj, collectStyles(e));
}

void SvgParser_generic::parseStyle(KoShape *obj, const SvgStyles &styles)
{
    SvgGraphicsContext *gc = m_context.currentGC();
    if (!gc)
        return;

    // make sure we parse the style attributes in the right order
    foreach(const QString & command, m_styleAttributes) {
        const QString &params = styles.value(command);
        if (params.isEmpty())
            continue;
        parsePA(gc, command, params);
    }

    if (!obj)
        return;

    if (!dynamic_cast<KoShapeGroup*>(obj)) {
        applyFillStyle(obj);
        applyStrokeStyle(obj);
    }
    applyFilter(obj);
    applyClipping(obj);

    if (! gc->display)
        obj->setVisible(false);
    obj->setTransparency(1.0 - gc->opacity);
}

void SvgParser_generic::applyFillStyle(KoShape *shape)
{
    SvgGraphicsContext *gc = m_context.currentGC();
    if (! gc)
        return;

    switch (gc->fillType) {
    case SvgGraphicsContext::None:
        shape->setBackground(0);
        break;
    case SvgGraphicsContext::Gradient: {
        SvgGradientHelper *gradient = findGradient(gc->fillId);
        if (gradient) {
            KoGradientBackground *bg = 0;
            if (gradient->gradientUnits() == SvgGradientHelper::ObjectBoundingBox) {
                bg = new KoGradientBackground(*gradient->gradient());
                bg->setTransform(gradient->transform());
            } else {
                QGradient *convertedGradient = SvgGradientHelper::convertGradient(gradient->gradient(), shape->size());
                bg = new KoGradientBackground(*convertedGradient);
                delete convertedGradient;
                QTransform invShapematrix = shape->transformation().inverted();
                bg->setTransform(gradient->transform() * gc->matrix * invShapematrix);
            }

            shape->setBackground(bg);
        }
    }
    break;
    case SvgGraphicsContext::Pattern: {
        SvgPatternHelper *pattern = findPattern(gc->fillId);
        KoImageCollection *imageCollection = m_documentResourceManager->imageCollection();
        if (pattern && imageCollection) {
            QRectF objectBound = QRectF(QPoint(), shape->size());
            QRectF currentBoundbox = gc->currentBoundbox;

            // properties from the object are not inherited
            // so we are creating a new context without copying
            SvgGraphicsContext *gc = m_context.pushGraphicsContext(pattern->content(), false);

            // the pattern establishes a new coordinate system with its
            // origin at the patterns x and y attributes
            gc->matrix = QTransform();
            // object bounding box units are relative to the object the pattern is applied
            if (pattern->patternContentUnits() == SvgPatternHelper::ObjectBoundingBox) {
                gc->currentBoundbox = objectBound;
                gc->forcePercentage = true;
            } else {
                // inherit the current bounding box
                gc->currentBoundbox = currentBoundbox;
            }

            parseStyle(0, pattern->content());

            // parse the pattern content elements
            QList<KoShape*> patternContent = parseContainer(pattern->content());

            // generate the pattern image from the shapes and the object bounding rect
            QImage image = pattern->generateImage(objectBound, patternContent);

            m_context.popGraphicsContext();

            // delete the shapes created from the pattern content
            qDeleteAll(patternContent);

            if (!image.isNull()) {
                KoPatternBackground *bg = new KoPatternBackground(imageCollection);
                bg->setPattern(image);

                QPointF refPoint = shape->documentToShape(pattern->position(objectBound));
                QSizeF tileSize = pattern->size(objectBound);

                bg->setPatternDisplaySize(tileSize);
                if (pattern->patternUnits() == SvgPatternHelper::ObjectBoundingBox) {
                    if (tileSize == objectBound.size())
                        bg->setRepeat(KoPatternBackground::Stretched);
                }

                // calculate pattern reference point offset in percent of tileSize
                // and relative to the topleft corner of the shape
                qreal fx = refPoint.x() / tileSize.width();
                qreal fy = refPoint.y() / tileSize.height();
                if (fx < 0.0)
                    fx = floor(fx);
                else if (fx > 1.0)
                    fx = ceil(fx);
                else
                    fx = 0.0;
                if (fy < 0.0)
                    fy = floor(fy);
                else if (fx > 1.0)
                    fy = ceil(fy);
                else
                    fy = 0.0;
                qreal offsetX = 100.0 * (refPoint.x() - fx * tileSize.width()) / tileSize.width();
                qreal offsetY = 100.0 * (refPoint.y() - fy * tileSize.height()) / tileSize.height();
                bg->setReferencePointOffset(QPointF(offsetX, offsetY));

                shape->setBackground(bg);
            }
        }
    }
    break;
    case SvgGraphicsContext::Solid:
    default:
        shape->setBackground(new KoColorBackground(gc->fillColor));
        break;
    }

    KoPathShape *path = dynamic_cast<KoPathShape*>(shape);
    if (path)
        path->setFillRule(gc->fillRule);
}

void SvgParser_generic::applyStrokeStyle(KoShape *shape)
{
    SvgGraphicsContext *gc = m_context.currentGC();
    if (! gc)
        return;

    switch (gc->strokeType) {
    case SvgGraphicsContext::Solid: {
        double lineWidth = gc->stroke.lineWidth();
        QVector<qreal> dashes = gc->stroke.lineDashes();

        KoLineBorder *border = new KoLineBorder(gc->stroke);

        // apply line width to dashes and dash offset
        if (dashes.count() && lineWidth > 0.0) {
            QVector<qreal> dashes = border->lineDashes();
            for (int i = 0; i < dashes.count(); ++i)
                dashes[i] /= lineWidth;
            double dashOffset = border->dashOffset();
            border->setLineStyle(Qt::CustomDashLine, dashes);
            border->setDashOffset(dashOffset / lineWidth);
        } else {
            border->setLineStyle(Qt::SolidLine, QVector<qreal>());
        }
        shape->setBorder(border);
    }
    break;
    case SvgGraphicsContext::Gradient: {
        SvgGradientHelper *gradient = findGradient(gc->strokeId);
        if (gradient) {
            QBrush brush;
            if (gradient->gradientUnits() == SvgGradientHelper::ObjectBoundingBox) {
                brush = *gradient->gradient();
                brush.setTransform(gradient->transform());
            } else {
                QGradient *convertedGradient(SvgGradientHelper::convertGradient(gradient->gradient(), shape->size()));
                brush = *convertedGradient;
                delete convertedGradient;
                brush.setTransform(gradient->transform() * gc->matrix * shape->transformation().inverted());
            }
            KoLineBorder *border = new KoLineBorder(gc->stroke);
            border->setLineBrush(brush);
            border->setLineStyle(Qt::SolidLine, QVector<qreal>());
            shape->setBorder(border);
        }
    }
    break;
    case SvgGraphicsContext::None:
    default:
        shape->setBorder(0);
        break;
    }
}

void SvgParser_generic::applyFilter(KoShape *shape)
{
    SvgGraphicsContext *gc = m_context.currentGC();
    if (! gc)
        return;

    if (gc->filterId.isEmpty())
        return;

    SvgFilterHelper *filter = findFilter(gc->filterId);
    if (! filter)
        return;

    KoXmlElement content = filter->content();

    // parse filter region
    QRectF bound(shape->position(), shape->size());
    // work on bounding box without viewbox tranformation applied
    // so user space coordinates of bounding box and filter region match up
    bound = gc->viewboxTransform.inverted().mapRect(bound);

    QRectF filterRegion(filter->position(bound), filter->size(bound));

    // convert filter region to boundingbox units
    QRectF objectFilterRegion;
    objectFilterRegion.setTopLeft(SvgUtil::userSpaceToObject(filterRegion.topLeft(), bound));
    objectFilterRegion.setSize(SvgUtil::userSpaceToObject(filterRegion.size(), bound));

    KoFilterEffectLoadingContext context(m_context.xmlBaseDir());
    context.setShapeBoundingBox(bound);
    // enable units conversion
    context.enableFilterUnitsConversion(filter->filterUnits() == SvgFilterHelper::UserSpaceOnUse);
    context.enableFilterPrimitiveUnitsConversion(filter->primitiveUnits() == SvgFilterHelper::UserSpaceOnUse);

    KoFilterEffectRegistry *registry = KoFilterEffectRegistry::instance();

    KoFilterEffectStack *filterStack = 0;

    QSet<QString> stdInputs;
    stdInputs << "SourceGraphic" << "SourceAlpha";
    stdInputs << "BackgroundImage" << "BackgroundAlpha";
    stdInputs << "FillPaint" << "StrokePaint";

    QMap<QString, KoFilterEffect*> inputs;

    // create the filter effects and add them to the shape
    for (KoXmlNode n = content.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement primitive = n.toElement();
        KoFilterEffect *filterEffect = registry->createFilterEffectFromXml(primitive, context);
        if (!filterEffect) {
            kWarning(30514) << "filter effect" << primitive.tagName() << "is not implemented yet";
            continue;
        }

        if (primitive.hasAttribute("in"))
            filterEffect->setInput(0, primitive.attribute("in"));
        if (primitive.hasAttribute("result"))
            filterEffect->setOutput(primitive.attribute("result"));

        QRectF subRegion;
        // parse subregion
        if (filter->primitiveUnits() == SvgFilterHelper::UserSpaceOnUse) {
            if (! primitive.hasAttribute("x") || ! primitive.hasAttribute("y") ||
                    ! primitive.hasAttribute("width") || ! primitive.hasAttribute("height")) {
                bool hasStdInput = false;
                bool isFirstEffect = filterStack == 0;
                // check if one of the inputs is a standard input
                foreach(const QString &input, filterEffect->inputs()) {
                    if ((isFirstEffect && input.isEmpty()) || stdInputs.contains(input)) {
                        hasStdInput = true;
                        break;
                    }
                }
                if (hasStdInput || primitive.tagName() == "feImage") {
                    // default to 0%, 0%, 100%, 100%
                    subRegion.setTopLeft(QPointF(0, 0));
                    subRegion.setSize(QSizeF(1, 1));
                } else {
                    // defaults to bounding rect of all referenced nodes
                    foreach(const QString &input, filterEffect->inputs()) {
                        if (!inputs.contains(input))
                            continue;

                        KoFilterEffect *inputFilter = inputs[input];
                        if (inputFilter)
                            subRegion |= inputFilter->filterRect();
                    }
                }
            } else {
                qreal x = parseUnitX(primitive.attribute("x"));
                qreal y = parseUnitY(primitive.attribute("y"));
                qreal w = parseUnitX(primitive.attribute("width"));
                qreal h = parseUnitY(primitive.attribute("height"));
                subRegion.setTopLeft(SvgUtil::userSpaceToObject(QPointF(x, y), bound));
                subRegion.setSize(SvgUtil::userSpaceToObject(QSizeF(w, h), bound));
            }
        } else {
            // x, y, width, height are in percentages of the object referencing the filter
            // so we just parse the percentages
            qreal x = SvgUtil::fromPercentage(primitive.attribute("x", "0"));
            qreal y = SvgUtil::fromPercentage(primitive.attribute("y", "0"));
            qreal w = SvgUtil::fromPercentage(primitive.attribute("width", "1"));
            qreal h = SvgUtil::fromPercentage(primitive.attribute("height", "1"));
            subRegion = QRectF(QPointF(x, y), QSizeF(w, h));
        }

        filterEffect->setFilterRect(subRegion);

        if (!filterStack)
            filterStack = new KoFilterEffectStack();

        filterStack->appendFilterEffect(filterEffect);
        inputs[filterEffect->output()] = filterEffect;
    }
    if (filterStack) {
        filterStack->setClipRect(objectFilterRegion);
        shape->setFilterEffectStack(filterStack);
    }
}

void SvgParser_generic::applyClipping(KoShape *shape)
{
    SvgGraphicsContext *gc = m_context.currentGC();
    if (! gc)
        return;

    if (gc->clipPathId.isEmpty())
        return;

    SvgClipPathHelper *clipPath = findClipPath(gc->clipPathId);
    if (! clipPath)
        return;

    kDebug(30514) << "applying clip path" << gc->clipPathId << "clip rule" << gc->clipRule;

    const bool boundingBoxUnits = clipPath->clipPathUnits() == SvgClipPathHelper::ObjectBoundingBox;
    kDebug(30514) << "using" << (boundingBoxUnits ? "boundingBoxUnits" : "userSpaceOnUse");

    QTransform shapeMatrix = shape->absoluteTransformation(0);
    // TODO:
    // clip path element can have a clip-path property
    // -> clip-path = intersection of children with referenced clip-path
    // any of its children can have a clip-path property
    // -> child element is clipped and the ORed with other children
    m_context.pushGraphicsContext();

    if (boundingBoxUnits) {
        SvgGraphicsContext *gc = m_context.currentGC();
        gc->matrix.reset();
        gc->viewboxTransform.reset();
        gc->currentBoundbox = shape->boundingRect();
        gc->forcePercentage = true;
    }

    QList<KoShape*> clipShapes = parseContainer(clipPath->content());
    QList<KoPathShape*> pathShapes;
    while (!clipShapes.isEmpty()) {
        KoShape *clipShape = clipShapes.first();
        clipShapes.removeFirst();
        // remove clip shape from list of all parsed shapes
        m_shapes.removeOne(clipShape);
        // check if we have a path shape
        KoPathShape *path = dynamic_cast<KoPathShape*>(clipShape);
        if (!path) {
            // if shape is a group, ungroup and add children to lits of clip shapes
            KoShapeGroup *group = dynamic_cast<KoShapeGroup*>(clipShape);
            if (group) {
                QList<KoShape*> groupedShapes = group->shapes();
                KoShapeUngroupCommand cmd(group, groupedShapes);
                cmd.redo();
                clipShapes.append(groupedShapes);
            } else {
                // shape is not a group shape, use its outline as clip path
                QPainterPath outline = clipShape->absoluteTransformation(0).map(clipShape->outline());
                path = KoPathShape::createShapeFromPainterPath(outline);
            }
            delete clipShape;
        }
        if (path) {
            kDebug(30514) << "using shape" << path->name() << "as clip path";
            pathShapes.append(path);
            if (boundingBoxUnits)
                path->applyAbsoluteTransformation(shapeMatrix);
        }
    }

    m_context.popGraphicsContext();

    if (pathShapes.count()) {
        QTransform transformToShape;
        if (!boundingBoxUnits)
            transformToShape = shape->absoluteTransformation(0).inverted();
        KoClipData *clipData = new KoClipData(pathShapes);
        KoClipPath *clipPath = new KoClipPath(shape, clipData);
        clipPath->setClipRule(gc->clipRule);
        shape->setClipPath(clipPath);
    }
}

void SvgParser_generic::parseFont(const SvgStyles &styles)
{
    SvgGraphicsContext *gc = m_context.currentGC();
    if (!gc)
        return;

    // make sure to only parse font attributes here
    foreach(const QString & command, m_fontAttributes) {
        const QString &params = styles.value(command);
        if (params.isEmpty())
            continue;
        parsePA(gc, command, params);
    }
}

QList<KoShape*> SvgParser_generic::parseUse(const KoXmlElement &e)
{
    QList<KoShape*> shapes;

    QString id = e.attribute("xlink:href");
    //
    if (!id.isEmpty()) {
        SvgGraphicsContext *gc = m_context.pushGraphicsContext(e);

        QString key = id.mid(1);

        if (e.hasAttribute("x"))
            gc->matrix.translate(parseUnitX(e.attribute("x")), 0.0);
        if (e.hasAttribute("y"))
            gc->matrix.translate(0.0, parseUnitX(e.attribute("y")));

        // TODO: use width and height attributes too

        if (m_defs.contains(key)) {
            const KoXmlElement &a = m_defs[key];
            SvgStyles styles = mergeStyles(collectStyles(e), collectStyles(a));
            if (a.tagName() == "g" || a.tagName() == "a" || a.tagName() == "symbol") {
                m_context.pushGraphicsContext(a);

                KoShapeGroup *group = new KoShapeGroup();
                group->setZIndex(m_context.nextZIndex());

                parseStyle(0, styles);
                parseFont(styles);

                QList<KoShape*> childShapes = parseContainer(a);

                // handle id
                if (!a.attribute("id").isEmpty())
                    group->setName(a.attribute("id"));

                addToGroup(childShapes, group);
                parseStyle(group, styles);   // apply style to group after size is set

                shapes.append(group);

                m_context.popGraphicsContext();
            } else {
                // Create the object with the merged styles.
                // The object inherits all style attributes from the use tag, but keeps it's own attributes.
                // So, not just use the style attributes of the use tag, but merge them first.
                KoShape *shape = createObject(a, styles);
                if (shape)
                    shapes.append(shape);
            }
        } else {
            // TODO: any named object can be referenced too
        }
        m_context.popGraphicsContext();
    }

    return shapes;
}

void SvgParser_generic::addToGroup(QList<KoShape*> shapes, KoShapeGroup *group)
{
    m_shapes += shapes;

    if (! group)
        return;

    KoShapeGroupCommand cmd(group, shapes);
    cmd.redo();
}

QList<KoShape*> SvgParser_generic::parseSvg(const KoXmlElement &e, QSizeF *fragmentSize)
{
    // check if we are the root svg element
    const bool isRootSvg = !m_context.currentGC();

    SvgGraphicsContext *gc = m_context.pushGraphicsContext();

    parseStyle(0, e);

    bool hasViewBox = e.hasAttribute("viewBox");
    QRectF viewBox;

    if (hasViewBox) {
        viewBox = SvgUtil::parseViewBox(e.attribute("viewBox"));
    }

    double width = 550.0;
    if (e.hasAttribute("width"))
        width = parseUnit(e.attribute("width"), true, false, viewBox);
    double height = 841.0;
    if (e.hasAttribute("height"))
        height = parseUnit(e.attribute("height"), false, true, viewBox);

    QSizeF svgFragmentSize(QSizeF(width, height));

    if (fragmentSize)
        *fragmentSize = svgFragmentSize;

    gc->currentBoundbox = QRectF(QPointF(0, 0), svgFragmentSize);

    if (! isRootSvg) {
        QTransform move;
        // x and y attribute has no meaning for outermost svg elements
        double x = e.hasAttribute("x") ? parseUnit(e.attribute("x")) : 0.0;
        double y = e.hasAttribute("y") ? parseUnit(e.attribute("y")) : 0.0;
        move.translate(x, y);
        gc->matrix = move * gc->matrix;
        gc->viewboxTransform = move *gc->viewboxTransform;
    }

    if (hasViewBox) {
        QTransform viewTransform;
        viewTransform.translate(viewBox.x(), viewBox.y());
        viewTransform.scale(width / viewBox.width() , height / viewBox.height());
        gc->matrix = viewTransform * gc->matrix;
        gc->viewboxTransform = viewTransform *gc->viewboxTransform;
        gc->currentBoundbox.setWidth(gc->currentBoundbox.width() * (viewBox.width() / width));
        gc->currentBoundbox.setHeight(gc->currentBoundbox.height() * (viewBox.height() / height));
    }

    QList<KoShape*> shapes = parseContainer(e);

    m_context.popGraphicsContext();

    return shapes;
}

QList<KoShape*> SvgParser_generic::parseContainer(const KoXmlElement &e)
{
    QList<KoShape*> shapes;

    // are we parsing a switch container
    bool isSwitch = e.tagName() == "switch";

    for (KoXmlNode n = e.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement b = n.toElement();
        if (b.isNull())
            continue;

        if (isSwitch) {
            // if we are parsing a switch check the requiredFeatures, requiredExtensions
            // and systemLanguage attributes
            if (b.hasAttribute("requiredFeatures")) {
                QString features = b.attribute("requiredFeatures");
                if (features.isEmpty() || features.simplified().isEmpty())
                    continue;
                // TODO: evaluate feature list
            }
            if (b.hasAttribute("requiredExtensions")) {
                // we do not support any extensions
                continue;
            }
            if (b.hasAttribute("systemLanguage")) {
                // not implemeted yet
            }
        }

                
        if (b.tagName() == "svg") {
           shapes += parseSvg(b);
        } else if (b.tagName() == "g" || b.tagName() == "a" || b.tagName() == "symbol") {
            // treat svg link <a> as group so we don't miss its child elements
            m_context.pushGraphicsContext(b);

            KoShapeGroup *group = new KoShapeGroup();
            group->setZIndex(m_context.nextZIndex());

            SvgStyles styles = collectStyles(b);
            parseStyle(0, styles);   // parse style for inheritance
            parseFont(styles);

            QList<KoShape*> childShapes = parseContainer(b);

            // handle id
            if (!b.attribute("id").isEmpty())
                group->setName(b.attribute("id"));

            addToGroup(childShapes, group);
            if (b.hasAttribute("viewBox")) {
                QRectF viewBox = SvgUtil::parseViewBox(b.attribute("viewBox"));
                QTransform viewTransform;
                viewTransform.translate(viewBox.x(), viewBox.y());
                viewTransform.scale(group->size().width() / viewBox.width() , group->size().height() / viewBox.height());
                group->applyAbsoluteTransformation(viewTransform);
            }
            parseStyle(group, styles);   // apply style to this group after size is set

            shapes.append(group);

            m_context.popGraphicsContext();
        } else if (b.tagName() == "switch") {
            m_context.pushGraphicsContext(b);
            shapes += parseContainer(b);
            m_context.popGraphicsContext();
        } else if (b.tagName() == "defs") {
            parseDefs(b);
        } else if (b.tagName() == "linearGradient" || b.tagName() == "radialGradient") {
            parseGradient(b);
        } else if (b.tagName() == "pattern") {
            QString id = b.attribute("id");
            if (!id.isEmpty() && !m_defs.contains(id))
                m_defs.insert(id, b);
        } else if (b.tagName() == "filter") {
            parseFilter(b);
        } else if (b.tagName() == "clipPath") {
            parseClipPath(b);
        } else if (b.tagName() == "style") {
            m_cssStyles.parseStylesheet(b);
        } else if (b.tagName() == "rect" ||
                   b.tagName() == "ellipse" ||
                   b.tagName() == "circle" ||
                   b.tagName() == "line" ||
                   b.tagName() == "polyline" ||
                   b.tagName() == "polygon" ||
                   b.tagName() == "path" ||
                   b.tagName() == "image") {
            KoShape *shape = createObject(b);
            if (shape)
                shapes.append(shape);
        } else if (b.tagName() == "text") {
            KoShape *shape = createText(b, shapes);
            if (shape)
                shapes.append(shape);
        } else if (b.tagName() == "use") {
            shapes += parseUse(b);
        }else if(b.tagName() == m_appData_tagName) {
            parseAppData(b);
	    } else {
            continue;
        }
        // if we are parsing a switch, stop after the first supported element
        if (isSwitch)
            break;
    }
   
    return shapes;
}

void SvgParser_generic::parseDefs(const KoXmlElement &e)
{
    for (KoXmlNode n = e.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement b = n.toElement();
        if (b.isNull())
            continue;

        if (b.tagName() == "style") {
            m_cssStyles.parseStylesheet(b);
        } else if (b.tagName() == "defs") {
            parseDefs(b);
        } else {
            QString definition = b.attribute("id");
            if (!definition.isEmpty() && !m_defs.contains(definition)) {
                m_defs.insert(definition, b);
            }
        }
    }
}

// Creating functions
// ---------------------------------------------------------------------------------------

ArtisticTextRange createTextRange(const QString &text, SvgTextHelper &context, SvgGraphicsContext *gc)
{
    ArtisticTextRange range(context.simplifyText(text, gc->preserveWhitespace), gc->font);

    const int textLength = range.text().length();
    switch(context.xOffsetType()) {
    case SvgTextHelper::Absolute:
        range.setXOffsets(context.xOffsets(textLength), ArtisticTextRange::AbsoluteOffset);
        break;
    case SvgTextHelper::Relative:
        range.setXOffsets(context.xOffsets(textLength), ArtisticTextRange::RelativeOffset);
        break;
    default:
        // no x-offsets
        break;
    }
    switch(context.yOffsetType()) {
    case SvgTextHelper::Absolute:
        range.setYOffsets(context.yOffsets(textLength), ArtisticTextRange::AbsoluteOffset);
        break;
    case SvgTextHelper::Relative:
        range.setYOffsets(context.yOffsets(textLength), ArtisticTextRange::RelativeOffset);
        break;
    default:
        // no y-offsets
        break;
    }

    range.setRotations(context.rotations(textLength));
    range.setLetterSpacing(gc->letterSpacing);
    range.setWordSpacing(gc->wordSpacing);
      if(gc->baselineShift == "sub") {
        range.setBaselineShift(ArtisticTextRange::Sub);
    } else if(gc->baselineShift == "super") {
        range.setBaselineShift(ArtisticTextRange::Super);
    } else if(gc->baselineShift.endsWith('%')) {
        range.setBaselineShift(ArtisticTextRange::Percent, SvgUtil::fromPercentage(gc->baselineShift));
    } else {
        qreal value = SvgUtil::parseUnitX(gc, gc->baselineShift);
        if (value != 0.0)
            range.setBaselineShift(ArtisticTextRange::Length, value);
    }

    //range.printDebug();

    return range;
}

void SvgParser_generic::parseTextRanges(const KoXmlElement &element, SvgTextHelper &textContext, KoShape *textShape, const QList<KoShape*> & shapes)
{
    ArtisticTextShape *text = dynamic_cast<ArtisticTextShape*>(textShape);
    if (!text)
        return;

    for (KoXmlNode n = element.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement e = n.toElement();
        if (e.isNull()) {
            ArtisticTextRange range = createTextRange(n.toText().data(), textContext, m_context.currentGC());
            text->appendText(range);
        }
        else if (e.tagName() == "tspan") {
            SvgGraphicsContext *gc = m_context.pushGraphicsContext(e);
            parseFont(collectStyles(e));
            textContext.pushCharacterTransforms();
            textContext.parseCharacterTransforms(e, gc);
            parseTextRanges(e, textContext, text, shapes);
            textContext.popCharacterTransforms();
            m_context.popGraphicsContext();
        }
        else if (e.tagName() == "tref") {
            if (e.attribute("xlink:href").isEmpty())
                continue;

            QString key = e.attribute("xlink:href").mid(1);
            if (! m_defs.contains(key)) {
                // try to find referenced object in document
                KoShape *obj = findObject(key);
                // try to find referenced object in actual group, which is not yet part of document
                if (! obj)
                    obj = findObject(key, shapes);
                if (obj) {
                    ArtisticTextShape *refText = dynamic_cast<ArtisticTextShape*>(obj);
                    if (refText) {
                        foreach (const ArtisticTextRange &range, refText->text()) {
                            text->appendText(range);
                        }
                    }
                }
            }
            else {
                KoXmlElement p = m_defs[key];
                SvgGraphicsContext *gc = m_context.currentGC();
                text->appendText(ArtisticTextRange(textContext.simplifyText(p.text(), gc->preserveWhitespace), gc->font));
            }
        }
        else {
            continue;
        }
    }
}

KoShape * SvgParser_generic::createText(const KoXmlElement &textElement, const QList<KoShape*> & shapes)
{
    QString anchor;

    ArtisticTextShape *text = static_cast<ArtisticTextShape*>(createShape(ArtisticTextShapeID));
    if (! text)
        return 0;
    text->clear();

    m_context.pushGraphicsContext(textElement);

    if (! textElement.attribute("text-anchor").isEmpty())
        anchor = textElement.attribute("text-anchor");

    SvgStyles elementStyles = collectStyles(textElement);
    parseFont(elementStyles);
    KoXmlElement styleElement = textElement;

    SvgTextHelper context;
    context.parseCharacterTransforms(textElement, m_context.currentGC());

    KoXmlElement parentElement = textElement;
    // first check if we have a "textPath" child element
    for (KoXmlNode n = textElement.firstChild(); !n.isNull(); n = n.nextSibling()) {
        KoXmlElement e = n.toElement();
        if (e.tagName() == "textPath") {
            parentElement = e;
            break;
        }
    }

    KoPathShape *path = 0;
    bool pathInDocument = false;
    double offset = 0.0;

    const bool hasTextPathElement = parentElement != textElement && parentElement.hasAttribute("xlink:href");
    if (hasTextPathElement) {
        // create the referenced path shape
        m_context.pushGraphicsContext(parentElement);
        parseFont(collectStyles(parentElement));
        context.pushCharacterTransforms();
        context.parseCharacterTransforms(parentElement, m_context.currentGC());

        QString key = parentElement.attribute("xlink:href").mid(1);
        if (! m_defs.contains(key)) {
            // try to find referenced object in document
            KoShape *obj = findObject(key);
            // try to find referenced object in actual group, which is not yet part of document
            if (! obj)
                obj = findObject(key, shapes);
            if (obj)
                path = dynamic_cast<KoPathShape*>(obj);
            if (path)
                pathInDocument = true;
        } else {
            KoXmlElement p = m_defs[key];
            path = dynamic_cast<KoPathShape*>(createObject(p));
            pathInDocument = false;
            path->applyAbsoluteTransformation(m_context.currentGC()->matrix.inverted());
        }
        // parse the start offset
        if (! parentElement.attribute("startOffset").isEmpty()) {
            QString start = parentElement.attribute("startOffset");
            if (start.endsWith('%'))
                offset = 0.01 * start.remove('%').toDouble();
            else {
                const float pathLength = path ? path->outline().length() : 0.0;
                if (pathLength > 0.0)
                    offset = start.toDouble() / pathLength;
            }
        }
    }

    if (parentElement.hasChildNodes()) {
        // parse child elements
        parseTextRanges(parentElement, context, text, shapes);
        if (!m_context.currentGC()->preserveWhitespace) {
            const QString plainText = text->plainText();
            if (plainText.endsWith(' '))
                text->removeText(plainText.length()-1, 1);
        }
        text->setPosition(context.textPosition());
    } else {
        // a single text range
        text->appendText(createTextRange(textElement.text(), context, m_context.currentGC()));
        text->setPosition(context.textPosition());
    }

    if (hasTextPathElement) {
        if (path) {
            if (pathInDocument)
                text->putOnPath(path);
            else
                text->putOnPath(path->absoluteTransformation(0).map(path->outline()));

            if (offset > 0.0)
                text->setStartOffset(offset);
        }

        context.popCharacterTransforms();
        m_context.popGraphicsContext();
    }

    // adjust position by baseline offset
    if (! text->isOnPath())
        text->setPosition(text->position() - QPointF(0, text->baselineOffset()));

    if (anchor == "middle")
        text->setTextAnchor(ArtisticTextShape::AnchorMiddle);
    else if (anchor == "end")
        text->setTextAnchor(ArtisticTextShape::AnchorEnd);

    if (!textElement.attribute("id").isEmpty())
        text->setName(textElement.attribute("id"));

    text->applyAbsoluteTransformation(m_context.currentGC()->matrix);
    text->setZIndex(m_context.nextZIndex());

    // apply the style merged from the text element and the last tspan element
    parseStyle(text, mergeStyles(collectStyles(styleElement), elementStyles));

    m_context.popGraphicsContext();

    return text;
}

KoShape * SvgParser_generic::createObject(const KoXmlElement &b, const SvgStyles &style)
{
    KoShape *obj = 0;

    m_context.pushGraphicsContext(b);

    if (b.tagName() == "rect") {
        double x = parseUnitX(b.attribute("x"));
        double y = parseUnitY(b.attribute("y"));
        double w = parseUnitX(b.attribute("width"));
        double h = parseUnitY(b.attribute("height"));
        bool hasRx = b.hasAttribute("rx");
        bool hasRy = b.hasAttribute("ry");
        double rx = hasRx ? parseUnitX(b.attribute("rx")) : 0.0;
        double ry = hasRy ? parseUnitY(b.attribute("ry")) : 0.0;
        if (hasRx && ! hasRy)
            ry = rx;
        if (! hasRx && hasRy)
            rx = ry;

	if(b.attribute("id").contains("ViewPort"))
	{
	  PresentationViewPortShape* shape = static_cast<PresentationViewPortShape*>(createShape(PresentationViewPortShapeId));
	  if(shape)
	  {
	    shape->setSize(QSizeF(w, h));
	    shape->setPosition(QPointF(x, y));
	  }
	  obj = shape;
	}
	else{
        RectangleShape *rect = static_cast<RectangleShape*>(createShape(RectangleShapeId));
        if (rect) {
            rect->setSize(QSizeF(w, h));
            rect->setPosition(QPointF(x, y));
            if (rx >= 0.0)
                rect->setCornerRadiusX(qMin(100.0, rx / (0.5 * w) * 100.0));
            rect->setPosition(QPointF(x, y));
            if (ry >= 0.0)
                rect->setCornerRadiusY(qMin(100.0, ry / (0.5 * h) * 100.0));
            obj = rect;
            if (w == 0.0 || h == 0.0)
                obj->setVisible(false);
        }
	}
    } else if (b.tagName() == "ellipse") {
        obj = createShape(EllipseShapeId);
        if (obj) {
            double rx = parseUnitX(b.attribute("rx"));
            double ry = parseUnitY(b.attribute("ry"));
            double cx = b.attribute("cx").isEmpty() ? 0.0 : parseUnitX(b.attribute("cx"));
            double cy = b.attribute("cy").isEmpty() ? 0.0 : parseUnitY(b.attribute("cy"));
            obj->setSize(QSizeF(2*rx, 2*ry));
            obj->setPosition(QPointF(cx - rx, cy - ry));
            if (rx == 0.0 || ry == 0.0)
                obj->setVisible(false);
        }
    } else if (b.tagName() == "circle") {
        obj = createShape(EllipseShapeId);
        if (obj) {
            double r  = parseUnitXY(b.attribute("r"));
            double cx = b.attribute("cx").isEmpty() ? 0.0 : parseUnitX(b.attribute("cx"));
            double cy = b.attribute("cy").isEmpty() ? 0.0 : parseUnitY(b.attribute("cy"));
            obj->setSize(QSizeF(2*r, 2*r));
            obj->setPosition(QPointF(cx - r, cy - r));
            if (r == 0.0)
                obj->setVisible(false);
        }
    } else if (b.tagName() == "line") {
        KoPathShape *path = static_cast<KoPathShape*>(createShape(KoPathShapeId));
        if (path) {
            double x1 = b.attribute("x1").isEmpty() ? 0.0 : parseUnitX(b.attribute("x1"));
            double y1 = b.attribute("y1").isEmpty() ? 0.0 : parseUnitY(b.attribute("y1"));
            double x2 = b.attribute("x2").isEmpty() ? 0.0 : parseUnitX(b.attribute("x2"));
            double y2 = b.attribute("y2").isEmpty() ? 0.0 : parseUnitY(b.attribute("y2"));
            path->clear();
            path->moveTo(QPointF(x1, y1));
            path->lineTo(QPointF(x2, y2));
            path->normalize();
            obj = path;
        }
    } else if (b.tagName() == "polyline" || b.tagName() == "polygon") {
        KoPathShape *path = static_cast<KoPathShape*>(createShape(KoPathShapeId));
        if (path) {
            path->clear();

            bool bFirst = true;
            QString points = b.attribute("points").simplified();
            points.replace(',', ' ');
            points.remove('\r');
            points.remove('\n');
            QStringList pointList = points.split(' ', QString::SkipEmptyParts);
            for (QStringList::Iterator it = pointList.begin(); it != pointList.end(); ++it) {
                QPointF point;
                point.setX(SvgUtil::fromUserSpace((*it).toDouble()));
                ++it;
                if (it == pointList.end())
                    break;
                point.setY(SvgUtil::fromUserSpace((*it).toDouble()));
                if (bFirst) {
                    path->moveTo(point);
                    bFirst = false;
                } else
                    path->lineTo(point);
            }
            if (b.tagName() == "polygon")
                path->close();

            path->setPosition(path->normalize());

            obj = path;
        }
    } else if (b.tagName() == "path") {
           KoPathShape *path = static_cast<KoPathShape*>(createShape(KoPathShapeId));
        if (path) {
            path->clear();

            KoPathShapeLoader loader(path);
            loader.parseSvg(b.attribute("d"), true);
            path->setPosition(path->normalize());

            QPointF newPosition = QPointF(SvgUtil::fromUserSpace(path->position().x()),
                                          SvgUtil::fromUserSpace(path->position().y()));
            QSizeF newSize = QSizeF(SvgUtil::fromUserSpace(path->size().width()),
                                    SvgUtil::fromUserSpace(path->size().height()));

            path->setSize(newSize);
            path->setPosition(newPosition);

	    obj = path;
	    
	  }
	 
    } else if (b.tagName() == "image") {
        double x = b.hasAttribute("x") ? parseUnitX(b.attribute("x")) : 0;
        double y = b.hasAttribute("x") ? parseUnitY(b.attribute("y")) : 0;
        double w = b.hasAttribute("width") ? parseUnitX(b.attribute("width")) : 0;
        double h = b.hasAttribute("height") ? parseUnitY(b.attribute("height")) : 0;

        // zero width of height disables rendering this image (see svg spec)
        if (w == 0.0 || h == 0.0)
            return 0;
        QString fname = b.attribute("xlink:href");
        QImage img;
        if (parseImage(fname, img)) {
            KoShape *picture = createShape("PictureShape");
            KoImageCollection *imageCollection = m_documentResourceManager->imageCollection();

            if (picture && imageCollection) {
                // TODO use it already for loading
                KoImageData *data = imageCollection->createImageData(img);

                picture->setUserData(data);
                picture->setSize(QSizeF(w, h));
                picture->setPosition(QPointF(x, y));

                obj = picture;
            }
        }
    }

    if (! obj) {
        m_context.popGraphicsContext();
        return 0;
    }

    obj->applyAbsoluteTransformation(m_context.currentGC()->matrix);

    if (!style.isEmpty())
        parseStyle(obj, style);
    else
        parseStyle(obj, collectStyles(b));

    // handle id
    if (!b.attribute("id").isEmpty())
        obj->setName(b.attribute("id"));

    m_context.popGraphicsContext();

    obj->setZIndex(m_context.nextZIndex());

    return obj;
}

KoShape * SvgParser_generic::createShape(const QString &shapeID)
{
  
   KoShapeFactoryBase *factory = KoShapeRegistry::instance()->get(shapeID);
    if (! factory) {
        kWarning(30514) << "Could not find factory for shape id" << shapeID;
        return 0;
    }

    KoShape *shape = factory->createDefaultShape(m_documentResourceManager);
    if (shape && shape->shapeId().isEmpty())
        shape->setShapeId(factory->id());

    // reset tranformation that might come from the default shape
    shape->setTransformation(QTransform());

    // reset border
    KoShapeBorderModel *oldBorder = shape->border();
    shape->setBorder(0);
    delete oldBorder;

    // reset fill
    KoShapeBackground *oldFill = shape->background();
    shape->setBackground(0);
    delete oldFill;

    return shape;
}

QString SvgParser_generic::inheritedAttribute(const QString &attributeName, const KoXmlElement &e)
{
    KoXmlNode parent = e.parentNode();
    while (!parent.isNull()) {
        KoXmlElement currentElement = parent.toElement();
        if (currentElement.hasAttribute(attributeName)) {
            return currentElement.attribute(attributeName);
        }
        parent = currentElement.parentNode();
    }
    return QString();
}
