/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef SVGIMPORT_H
#define SVGIMPORT_H

#include "SvgGradientHelper.h"
#include "svggraphiccontext.h"

#include <core/KarbonDocument.h>

#include <KoFilter.h>

#include <QtGui/QGradient>
#include <QtCore/QMap>
#include <QtCore/QStack>
#include <QtCore/QVariant>

class KoShape;
class KoShapeContainer;
class KoShapeGroup;

class SvgImport : public KoFilter
{
    Q_OBJECT

public:
    SvgImport(QObject* parent, const QVariantList&);
    virtual ~SvgImport();

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

protected:

    /// The main entry point for the conversion
    void convert();

    /// Parses a group element, returning a list of child shapes
    QList<KoShape*> parseGroup( const QDomElement & );
    /// Parses a use element, returning a list of child shapes
    QList<KoShape*> parseUse( const QDomElement & );
    /// Parses definitions for later use
    void parseDefs( const QDomElement & );
    /// Parses style attributes, applying them to the given shape
    void parseStyle( KoShape *, const QDomElement & );
    /// Parses a single style attribute
    void parsePA( KoShape *, SvgGraphicsContext *, const QString &, const QString & );
    /// Parses a gradient element
    void parseGradient( const QDomElement &, const QDomElement &referencedBy = QDomElement() );
    /// Parses gradient color stops
    void parseColorStops( QGradient *, const QDomElement & );
    /// Parses a length attribute
    double parseUnit( const QString &, bool horiz = false, bool vert = false, QRectF bbox = QRectF() );
    /// Parses a color attribute
    void parseColor( QColor &, const QString & );
    /// Converts given string into a color
    QColor stringToColor( const QString & );
    /// Parses a transform attribute
    QMatrix parseTransform( const QString &transform );

    double toPercentage( QString );
    double fromPercentage( QString );
    double fromUserSpace( double value );

    void setupTransform( const QDomElement & );
    void updateContext( const QDomElement & );
    void addGraphicContext();
    void removeGraphicContext();

    /// Creates an object from the given xml element
    KoShape * createObject( const QDomElement &, const QDomElement &style = QDomElement() );
    /// Create text object from the given xml element
    KoShape * createText( const QDomElement &, const QList<KoShape*> & shapes );
    void parseFont( const QDomElement & );
    /// find object with given id in document
    KoShape * findObject( const QString &name );
    /// find object with given id in given group
    KoShape * findObject( const QString &name, KoShapeContainer * );
    /// find object with given if in given shape list
    KoShape * findObject( const QString &name, const QList<KoShape*> & shapes );
    /// find gradient with given id in gradient map
    SvgGradientHelper* findGradient( const QString &id, const QString &href = 0 );

    /// Determine scaling factor from given matrix
    double getScalingFromMatrix( QMatrix &matrix );

    /// Merges two style elements, returning the merged style
    QDomElement mergeStyles( const QDomElement &, const QDomElement & );

    /// Adds list of shapes to the given group shape
    void addToGroup( QList<KoShape*> shapes, KoShapeGroup * group );

    /// Returns the next z-index
    int nextZIndex();

    /// Constructs an absolute file path from the fiven href and base directory
    QString absoluteFilePath( const QString &href, const QString &xmlBase );

    /// creates a shape from the given shape id
    KoShape * createShape( const QString &shapeID );

private:
    KarbonDocument * m_document;
    QStack<SvgGraphicsContext*>    m_gc;
    QMap<QString, SvgGradientHelper>  m_gradients;
    QMap<QString, QDomElement>     m_defs;
    QRectF                         m_outerRect;
    QDomDocument                   m_inpdoc;
};

#endif
