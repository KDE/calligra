/* This file is part of the KDE project
 * Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005,2007-2009 Jan Hambrecht <jaham@gmx.net>
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

#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "SvgGradientHelper.h"
#include "SvgPatternHelper.h"
#include "SvgFilterHelper.h"
#include "SvgGraphicContext.h"
#include "SvgCssHelper.h"

#include <KoXmlReader.h>

#include <QtGui/QGradient>
#include <QtCore/QMap>
#include <QtCore/QStack>

class KoShape;
class KoShapeContainer;
class KoShapeGroup;
class KoResourceManager;

class SvgParser
{
public:
    SvgParser(KoResourceManager *documentResourceManager);
    virtual ~SvgParser();

    /// Parses a svg fragment, returning the list of top level child shapes
    QList<KoShape*> parseSvg(const KoXmlElement &e, QSizeF * fragmentSize = 0);

    /// Sets the initial xml base directory (the directory form where the file is read)
    void setXmlBaseDir(const QString &baseDir);

    /// Returns the list of all shapes of the svg document
    QList<KoShape*> shapes() const;

protected:

    typedef QMap<QString, QString> SvgStyles;

    /// Parses a container element, returning a list of child shapes
    QList<KoShape*> parseContainer(const KoXmlElement &);
    /// Parses a use element, returning a list of child shapes
    QList<KoShape*> parseUse(const KoXmlElement &);
    /// Parses definitions for later use
    void parseDefs(const KoXmlElement &);
    /// Parses style attributes, applying them to the given shape
    void parseStyle(KoShape *, const KoXmlElement &);
    void parseStyle(KoShape *, const SvgStyles &);
    /// Parses a single style attribute
    void parsePA(SvgGraphicsContext *, const QString &, const QString &);
    /// Parses a gradient element
    bool parseGradient(const KoXmlElement &, const KoXmlElement &referencedBy = KoXmlElement());
    /// Parses gradient color stops
    void parseColorStops(QGradient *, const KoXmlElement &);
    /// Parses a pattern element
    void parsePattern(SvgPatternHelper &pattern, const KoXmlElement &);
    /// Parses a filter element
    bool parseFilter(const KoXmlElement &, const KoXmlElement &referencedBy = KoXmlElement());
    /// Parses a length attribute
    double parseUnit(const QString &, bool horiz = false, bool vert = false, QRectF bbox = QRectF());
    /// parses a length attribute in x-direction
    double parseUnitX(const QString &unit);
    /// parses a length attribute in y-direction
    double parseUnitY(const QString &unit);
    /// parses a length attribute in xy-direction
    double parseUnitXY(const QString &unit);
    /// Parses a color attribute
    bool parseColor(QColor &, const QString &);
    /// Parse a image
    bool parseImage(const QString &imageAttribute, QImage &image);
    /// Parses a viewbox attribute into an rectangle
    QRectF parseViewBox(QString viewbox);

    void setupTransform(const KoXmlElement &);
    void updateContext(const KoXmlElement &);
    void addGraphicContext();
    void removeGraphicContext();

    /// Creates an object from the given xml element
    KoShape * createObject(const KoXmlElement &, const SvgStyles &style = SvgStyles());
    /// Create text object from the given xml element
    KoShape * createText(const KoXmlElement &, const QList<KoShape*> & shapes);
    /// Parses font attributes
    void parseFont(const SvgStyles &styles);
    /// find object with given id in document
    KoShape * findObject(const QString &name);
    /// find object with given id in given group
    KoShape * findObject(const QString &name, KoShapeContainer *);
    /// find object with given if in given shape list
    KoShape * findObject(const QString &name, const QList<KoShape*> & shapes);
    /// find gradient with given id in gradient map
    SvgGradientHelper* findGradient(const QString &id, const QString &href = 0);
    /// find pattern with given id in pattern map
    SvgPatternHelper* findPattern(const QString &id);
    /// find filter with given id in filter map
    SvgFilterHelper* findFilter(const QString &id, const QString &href = 0);

    /// Creates style map from given xml element
    SvgStyles collectStyles(const KoXmlElement &);
    /// Merges two style elements, returning the merged style
    SvgStyles mergeStyles(const SvgStyles &, const SvgStyles &);

    /// Adds list of shapes to the given group shape
    void addToGroup(QList<KoShape*> shapes, KoShapeGroup * group);

    /// Returns the next z-index
    int nextZIndex();

    /// Constructs an absolute file path from the fiven href and base directory
    QString absoluteFilePath(const QString &href, const QString &xmlBase);

    /// creates a shape from the given shape id
    KoShape * createShape(const QString &shapeID);

    /// Builds the document from the given shapes list
    void buildDocument(QList<KoShape*> shapes);

    /// Applies the current fill style to the object
    void applyFillStyle(KoShape * shape);

    /// Applies the current stroke style to the object
    void applyStrokeStyle(KoShape * shape);

    /// Applies the current filter to the object
    void applyFilter(KoShape * shape);

    /// Returns inherited attribute value for specified element
    QString inheritedAttribute(const QString &attributeName, const KoXmlElement &e);

private:
    QSizeF m_documentSize;
    QStack<SvgGraphicsContext*>    m_gc;
    QMap<QString, SvgGradientHelper>  m_gradients;
    QMap<QString, SvgPatternHelper> m_patterns;
    QMap<QString, SvgFilterHelper> m_filters;
    QMap<QString, KoXmlElement>     m_defs;
    QStringList m_fontAttributes; ///< font related attributes
    QStringList m_styleAttributes; ///< style related attributes
    KoResourceManager *m_documentResourceManager;
    QList<KoShape*> m_shapes;
    QList<KoShape*> m_toplevelShapes;
    QString m_xmlBaseDir;
    SvgCssHelper m_cssStyles;
};

#endif
