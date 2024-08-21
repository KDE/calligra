/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005, 2007-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "SvgClipPathHelper.h"
#include "SvgFilterHelper.h"
#include "SvgGradientHelper.h"
#include "SvgLoadingContext.h"
#include "SvgPatternHelper.h"
#include "SvgStyleParser.h"
#include "flake_export.h"

#include <KoXmlReader.h>

#include <QMap>

class KoShape;
class KoShapeGroup;
class KoDocumentResourceManager;

class FLAKE_EXPORT SvgParser
{
public:
    explicit SvgParser(KoDocumentResourceManager *documentResourceManager);
    virtual ~SvgParser();

    /// Parses a svg fragment, returning the list of top level child shapes
    QList<KoShape *> parseSvg(const KoXmlElement &e, QSizeF *fragmentSize = nullptr);

    /// Sets the initial xml base directory (the directory form where the file is read)
    void setXmlBaseDir(const QString &baseDir);

    /// Returns the list of all shapes of the svg document
    QList<KoShape *> shapes() const;

protected:
    /// Parses a container element, returning a list of child shapes
    QList<KoShape *> parseContainer(const KoXmlElement &);
    /// Parses a use element, returning a list of child shapes
    QList<KoShape *> parseUse(const KoXmlElement &);
    /// Parses definitions for later use
    void parseDefs(const KoXmlElement &);
    /// Parses a gradient element
    bool parseGradient(const KoXmlElement &, const KoXmlElement &referencedBy = KoXmlElement());
    /// Parses a pattern element
    void parsePattern(SvgPatternHelper &pattern, const KoXmlElement &);
    /// Parses a filter element
    bool parseFilter(const KoXmlElement &, const KoXmlElement &referencedBy = KoXmlElement());
    /// Parses a clip path element
    bool parseClipPath(const KoXmlElement &, const KoXmlElement &referencedBy = KoXmlElement());
    /// parses a length attribute
    qreal parseUnit(const QString &, bool horiz = false, bool vert = false, const QRectF &bbox = QRectF());
    /// parses a length attribute in x-direction
    qreal parseUnitX(const QString &unit);
    /// parses a length attribute in y-direction
    qreal parseUnitY(const QString &unit);
    /// parses a length attribute in xy-direction
    qreal parseUnitXY(const QString &unit);

    /// Creates an object from the given xml element
    KoShape *createObject(const KoXmlElement &, const SvgStyles &style = SvgStyles());
    /// Create path object from the given xml element
    KoShape *createPath(const KoXmlElement &);
    /// find gradient with given id in gradient map
    SvgGradientHelper *findGradient(const QString &id, const QString &href = QString());
    /// find pattern with given id in pattern map
    SvgPatternHelper *findPattern(const QString &id);
    /// find filter with given id in filter map
    SvgFilterHelper *findFilter(const QString &id, const QString &href = QString());
    /// find clip path with given id in clip path map
    SvgClipPathHelper *findClipPath(const QString &id, const QString &href = QString());

    /// Adds list of shapes to the given group shape
    void addToGroup(QList<KoShape *> shapes, KoShapeGroup *group);

    /// creates a shape from the given shape id
    KoShape *createShape(const QString &shapeID);
    /// Creates shape from specified svg element
    KoShape *createShapeFromElement(const KoXmlElement &element, SvgLoadingContext &context);

    /// Builds the document from the given shapes list
    void buildDocument(QList<KoShape *> shapes);

    /// Applies styles to the given shape
    void applyStyle(KoShape *, const KoXmlElement &);

    /// Applies styles to the given shape
    void applyStyle(KoShape *, const SvgStyles &);

    /// Applies the current fill style to the object
    void applyFillStyle(KoShape *shape);

    /// Applies the current stroke style to the object
    void applyStrokeStyle(KoShape *shape);

    /// Applies the current filter to the object
    void applyFilter(KoShape *shape);

    /// Applies the current clip path to the object
    void applyClipping(KoShape *shape);

    /// Applies id to specified shape
    void applyId(const QString &id, KoShape *shape);

private:
    QSizeF m_documentSize;
    SvgLoadingContext m_context;
    QMap<QString, SvgGradientHelper> m_gradients;
    QMap<QString, SvgPatternHelper> m_patterns;
    QMap<QString, SvgFilterHelper> m_filters;
    QMap<QString, SvgClipPathHelper> m_clipPaths;
    KoDocumentResourceManager *m_documentResourceManager;
    QList<KoShape *> m_shapes;
    QList<KoShape *> m_toplevelShapes;
};

#endif
