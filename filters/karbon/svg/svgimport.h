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

#include <core/vdocument.h>

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

    QList<KoShape*> parseGroup( const QDomElement & );
    void parseDefs( const QDomElement & );
    QList<KoShape*> parseUse( const QDomElement & );
    void parseStyle( KoShape *, const QDomElement & );
    void parsePA( KoShape *, SvgGraphicsContext *, const QString &, const QString & );
    void parseGradient( const QDomElement &, const QDomElement &referencedBy = QDomElement() );
    void parseColorStops( QGradient *, const QDomElement & );
    double parseUnit( const QString &, bool horiz = false, bool vert = false, QRectF bbox = QRectF() );
    void parseColor( QColor &, const QString & );
    QColor parseColor( const QString & );
    QMatrix parseTransform( const QString &transform );

    double toPercentage( QString );
    double fromPercentage( QString );
    void setupTransform( const QDomElement & );
    void addGraphicContext();
    void removeGraphicContext();
    void convert();
    KoShape * createObject( const QDomElement &, const QDomElement &style = QDomElement() );
    KoShape * createText( const QDomElement & );
    void parseFont( const QDomElement & );
    // find object with given id in document
    KoShape * findObject( const QString &name );
    // find object with given id in given group
    KoShape * findObject( const QString &name, KoShapeContainer * );
    // find gradient with given id in gradient map
    SvgGradientHelper* findGradient( const QString &id, const QString &href = 0 );

    // Determine scaling factor from given matrix
    double getScalingFromMatrix( QMatrix &matrix );

    QDomElement mergeStyles( const QDomElement &, const QDomElement & );

    void addToGroup( QList<KoShape*> shapes, KoShapeGroup * group );

    int nextZIndex();
    QString absoluteFilePath( const QString &href );

private:
    VDocument                      m_document;
    QStack<SvgGraphicsContext*>    m_gc;
    QMap<QString, SvgGradientHelper>  m_gradients;
    QMap<QString, QDomElement>     m_defs;
    QRectF                         m_outerRect;
    QDomDocument inpdoc;
};

#endif
