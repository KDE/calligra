/* This file is part of the KDE project
 * Copyright (C) 2002-2005,2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002-2004 Nicolas Goutte <nicolasg@snafu.de>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005-2009 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005,2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006-2007 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2007-2008 Thorsten Zachmann <t.zachmann@zagge.de>

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

#include "SvgParser.h"
#include "color.h"
#include "SvgUtil.h"

#include <KarbonGlobal.h>
#include <KarbonPart.h>
#include <KarbonGradientHelper.h>

#include <KoShape.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoShapeLayer.h>
#include <KoShapeContainer.h>
#include <KoShapeGroup.h>
#include <KoPathShape.h>
#include <KoPathShapeLoader.h>
#include <commands/KoShapeGroupCommand.h>
#include <KoUnit.h>
#include <KoImageData.h>
#include <KoImageCollection.h>
#include <pictureshape/PictureShape.h>
#include <pathshapes/rectangle/KoRectangleShape.h>
#include <pathshapes/ellipse/KoEllipseShape.h>
#include <plugins/artistictextshape/ArtisticTextShape.h>
#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoPatternBackground.h>
#include <KoFilterEffectRegistry.h>
#include <KoFilterEffect.h>

#include <KDebug>

#include <QtGui/QColor>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


SvgParser::SvgParser( const QMap<QString, KoDataCenter*> & dataCenters )
: m_dataCenters( dataCenters )
{
    SETRGBCOLORS();
    m_fontAttributes << "font-family" << "font-size" << "font-weight" << "text-decoration";
    // the order of the style attributes is important, don't change without reason !!!
    m_styleAttributes << "color" << "opacity" << "display";
    m_styleAttributes << "fill" << "fill-rule" << "fill-opacity";
    m_styleAttributes << "stroke" << "stroke-width" << "stroke-linejoin" << "stroke-linecap";
    m_styleAttributes << "stroke-dasharray" << "stroke-dashoffset" << "stroke-opacity" << "stroke-miterlimit"; 
    m_styleAttributes << "filter";
}

SvgParser::~SvgParser()
{
    if( ! m_gc.isEmpty() )
        kWarning() << "the context stack is not empty (current count" << m_gc.size() << ", expected 0)";
    qDeleteAll( m_gc );
    m_gc.clear();
}

void SvgParser::setXmlBaseDir( const QString &baseDir )
{
    m_xmlBaseDir = baseDir;
}

QList<KoShape*> SvgParser::shapes() const
{
    return m_shapes;
}

// Helper functions
// ---------------------------------------------------------------------------------------

// parses the number into parameter number
const char * parseNumber( const char *ptr, double &number )
{
    int integer, exponent;
    double decimal, frac;
    int sign, expsign;

    exponent = 0;
    integer = 0;
    frac = 1.0;
    decimal = 0;
    sign = 1;
    expsign = 1;

    // read the sign
    if(*ptr == '+')
        ptr++;
    else if(*ptr == '-')
    {
        ptr++;
        sign = -1;
    }

    // read the integer part
    while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
        integer = (integer * 10) + *(ptr++) - '0';
    if(*ptr == '.') // read the decimals
    {
        ptr++;
        while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
            decimal += (*(ptr++) - '0') * (frac *= 0.1);
    }

    if(*ptr == 'e' || *ptr == 'E') // read the exponent part
    {
        ptr++;

        // read the sign of the exponent
        if(*ptr == '+')
            ptr++;
        else if(*ptr == '-')
        {
            ptr++;
            expsign = -1;
        }

        exponent = 0;
        while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
        {
            exponent *= 10;
            exponent += *ptr - '0';
            ptr++;
        }
    }
    number = integer + decimal;
    number *= sign * pow( (double)10, double( expsign * exponent ) );

    return ptr;
}

void SvgParser::addGraphicContext()
{
    SvgGraphicsContext *gc = new SvgGraphicsContext;
    // set as default
    if( ! m_gc.isEmpty() )
        *gc = *( m_gc.top() );

    gc->filterId = QString(); // filters are not inherited

    m_gc.push( gc );
}

void SvgParser::removeGraphicContext()
{
    delete( m_gc.pop() );
}

void SvgParser::updateContext( const QDomElement &e )
{
    SvgGraphicsContext *gc = m_gc.top();
    if( e.hasAttribute( "xml:base" ) )
        gc->xmlBaseDir = e.attribute( "xml:base" );
}

void SvgParser::setupTransform( const QDomElement &e )
{
    SvgGraphicsContext *gc = m_gc.top();

    if( e.hasAttribute( "transform" ) )
    {
        QMatrix mat = parseTransform( e.attribute( "transform" ) );
        gc->matrix = mat * gc->matrix;
    }
}

QMatrix SvgParser::parseTransform( const QString &transform )
{
    QMatrix result;

    // Split string for handling 1 transform statement at a time
    QStringList subtransforms = transform.split(')', QString::SkipEmptyParts);
    QStringList::ConstIterator it = subtransforms.constBegin();
    QStringList::ConstIterator end = subtransforms.constEnd();
    for(; it != end; ++it)
    {
        QStringList subtransform = (*it).simplified().split('(', QString::SkipEmptyParts);
        if( subtransform.count() < 2 )
            continue;

        subtransform[0] = subtransform[0].trimmed().toLower();
        subtransform[1] = subtransform[1].simplified();
        QRegExp reg("[,( ]");
        QStringList params = subtransform[1].split(reg, QString::SkipEmptyParts);

        if(subtransform[0].startsWith(';') || subtransform[0].startsWith(','))
            subtransform[0] = subtransform[0].right(subtransform[0].length() - 1);

        if(subtransform[0] == "rotate")
        {
            if(params.count() == 3)
            {
                double x = params[1].toDouble();
                double y = params[2].toDouble();

                result.translate(x, y);
                result.rotate(params[0].toDouble());
                result.translate(-x, -y);
            }
            else
                result.rotate(params[0].toDouble());
        }
        else if(subtransform[0] == "translate")
        {
            if(params.count() == 2)
                result.translate( SvgUtil::fromUserSpace(params[0].toDouble()),
                                  SvgUtil::fromUserSpace(params[1].toDouble()));
            else    // Spec : if only one param given, assume 2nd param to be 0
                result.translate( SvgUtil::fromUserSpace(params[0].toDouble()) , 0);
        }
        else if(subtransform[0] == "scale")
        {
            if(params.count() == 2)
                result.scale(params[0].toDouble(), params[1].toDouble());
            else    // Spec : if only one param given, assume uniform scaling
                result.scale(params[0].toDouble(), params[0].toDouble());
        }
        else if(subtransform[0].toLower() == "skewx")
            result.shear(tan(params[0].toDouble() * KarbonGlobal::pi_180), 0.0F);
        else if(subtransform[0].toLower() == "skewy")
            result.shear(0.0F, tan(params[0].toDouble() * KarbonGlobal::pi_180));
        else if(subtransform[0] == "matrix")
        {
            if(params.count() >= 6)
                result.setMatrix(params[0].toDouble(), params[1].toDouble(),
                                 params[2].toDouble(), params[3].toDouble(), 
                                 SvgUtil::fromUserSpace(params[4].toDouble()),
                                 SvgUtil::fromUserSpace(params[5].toDouble()));
        }
    }

    return result;
}

KoShape * SvgParser::findObject( const QString &name, const QList<KoShape*> & shapes )
{
    foreach( KoShape * shape, shapes )
    {
        if( shape->name() == name )
            return shape;

        KoShape * resultShape = findObject( name, dynamic_cast<KoShapeContainer*>( shape ) );
        if( resultShape )
            return resultShape;
    }

    return 0;
}

KoShape * SvgParser::findObject( const QString &name, KoShapeContainer * group )
{
    if( ! group )
        return 0L;

    foreach( KoShape * shape, group->childShapes() )
    {
        if( shape->name() == name )
            return shape;

        KoShapeContainer * container = dynamic_cast<KoShapeContainer*>( shape );
        if( container )
        {
            KoShape * resultShape = findObject( name, container );
            if( resultShape )
                return resultShape;
        }
    }

    return 0;
}

KoShape * SvgParser::findObject( const QString &name )
{
    return findObject( name, m_shapes );
}

SvgGradientHelper* SvgParser::findGradient( const QString &id, const QString &href)
{
    // check if gradient was already parsed, and return it
    if( m_gradients.contains( id ) )
        return &m_gradients[ id ];

    // check if gradient was stored for later parsing
    if( !m_defs.contains( id ) )
        return 0L;

    QDomElement e = m_defs[ id ];
    if(e.childNodes().count() == 0)
    {
        QString mhref = e.attribute("xlink:href").mid(1);

        if(m_defs.contains(mhref))
            return findGradient(mhref, id);
        else
            return 0L;
    }
    else
    {
        // ok parse gradient now
        if( ! parseGradient( m_defs[ id ], m_defs[ href ] ) )
            return 0L;
    }

    // return successfully parsed gradient or NULL
    QString n;
    if(href.isEmpty())
        n = id;
    else
        n = href;

    if( m_gradients.contains( n ) )
        return &m_gradients[ n ];
    else
        return 0L;
}

SvgPatternHelper* SvgParser::findPattern( const QString &id, const QString &href )
{
    // check if pattern was already parsed, and return it
    if( m_patterns.contains( id ) )
        return &m_patterns[ id ];

    // check if pattern was stored for later parsing
    if( !m_defs.contains( id ) )
        return 0L;

    QDomElement e = m_defs[ id ];
    if(e.childNodes().count() == 0)
    {
        QString mhref = e.attribute("xlink:href").mid(1);

        if(m_defs.contains(mhref))
            return findPattern(mhref, id);
        else
            return 0L;
    }
    else
    {
        // ok parse pattern now
        if( ! parsePattern( m_defs[ id ], m_defs[ href ] ) )
            return 0L;
    }

    // return successfully parsed pattern or NULL
    QString n;
    if(href.isEmpty())
        n = id;
    else
        n = href;

    if( m_patterns.contains( n ) )
        return &m_patterns[ n ];
    else
        return 0L;
}

SvgFilterHelper* SvgParser::findFilter( const QString &id, const QString &href )
{
    // check if filter was already parsed, and return it
    if( m_filters.contains( id ) )
        return &m_filters[ id ];

    // check if filter was stored for later parsing
    if( !m_defs.contains( id ) )
        return 0L;

    QDomElement e = m_defs[ id ];
    if(e.childNodes().count() == 0)
    {
        QString mhref = e.attribute("xlink:href").mid(1);

        if(m_defs.contains(mhref))
            return findFilter(mhref, id);
        else
            return 0L;
    }
    else
    {
        // ok parse filter now
        if( ! parseFilter( m_defs[ id ], m_defs[ href ] ) )
            return 0L;
    }

    // return successfully parsed filter or NULL
    QString n;
    if(href.isEmpty())
        n = id;
    else
        n = href;

    if( m_filters.contains( n ) )
        return &m_filters[ n ];
    else
        return 0L;
}

QDomElement SvgParser::mergeStyles( const QDomElement &referencedBy, const QDomElement &referencedElement )
{
    // First use all the style attributes of the element being referenced.
    QDomElement e = referencedElement;

    // Now go through the style attributes of the element that is referencing and substitute the original ones.
    if( !referencedBy.attribute( "color" ).isEmpty() )
        e.setAttribute( "color", referencedBy.attribute( "color" ) );
    if( !referencedBy.attribute( "fill" ).isEmpty() )
        e.setAttribute( "fill", referencedBy.attribute( "fill" ) );
    if( !referencedBy.attribute( "fill-rule" ).isEmpty() )
        e.setAttribute( "fill-rule", referencedBy.attribute( "fill-rule" ) );
    if( !referencedBy.attribute( "stroke" ).isEmpty() )
        e.setAttribute( "stroke", referencedBy.attribute( "stroke" ) );
    if( !referencedBy.attribute( "stroke-width" ).isEmpty() )
        e.setAttribute( "stroke-width", referencedBy.attribute( "stroke-width" ) );
    if( !referencedBy.attribute( "stroke-linejoin" ).isEmpty() )
        e.setAttribute( "stroke-linejoin", referencedBy.attribute( "stroke-linejoin" ) );
    if( !referencedBy.attribute( "stroke-linecap" ).isEmpty() )
        e.setAttribute( "stroke-linecap", referencedBy.attribute( "stroke-linecap" ) );
    if( !referencedBy.attribute( "stroke-dasharray" ).isEmpty() )
        e.setAttribute( "stroke-dasharray", referencedBy.attribute( "stroke-dasharray" ) );
    if( !referencedBy.attribute( "stroke-dashoffset" ).isEmpty() )
        e.setAttribute( "stroke-dashoffset", referencedBy.attribute( "stroke-dashoffset" ) );
    if( !referencedBy.attribute( "stroke-opacity" ).isEmpty() )
        e.setAttribute( "stroke-opacity", referencedBy.attribute( "stroke-opacity" ) );
    if( !referencedBy.attribute( "stroke-miterlimit" ).isEmpty() )
        e.setAttribute( "stroke-miterlimit", referencedBy.attribute( "stroke-miterlimit" ) );
    if( !referencedBy.attribute( "fill-opacity" ).isEmpty() )
        e.setAttribute( "fill-opacity", referencedBy.attribute( "fill-opacity" ) );
    if( !referencedBy.attribute( "opacity" ).isEmpty() )
        e.setAttribute( "opacity", referencedBy.attribute( "opacity" ) );
    if( !referencedBy.attribute( "filter" ).isEmpty() )
        e.setAttribute( "filter", referencedBy.attribute( "filter" ) );

    // build map of style attributes from the element being referenced (original)
    QString origStyle = e.attribute( "style" ).simplified();
    QStringList origSubstyles = origStyle.split( ';', QString::SkipEmptyParts );
    QMap<QString, QString> mergesStyles;
    for( QStringList::Iterator it = origSubstyles.begin(); it != origSubstyles.end(); ++it )
    {
        QStringList origSubstyle = it->split( ':' );
        QString command = origSubstyle[0].trimmed();
        QString params  = origSubstyle[1].trimmed();
        mergesStyles[command] = params;
    }

    // build map of style attributes from the referencing element and substitue the original style
    QString refStyle = referencedBy.attribute( "style" ).simplified();
    QStringList refSubstyles = refStyle.split( ';', QString::SkipEmptyParts );
    for( QStringList::Iterator it = refSubstyles.begin(); it != refSubstyles.end(); ++it )
    {
        QStringList refSubstyle = it->split( ':' );
        QString command = refSubstyle[0].trimmed();
        // do not parse font attributes here, this is done in parseFont
        if( m_fontAttributes.contains( command ) )
            continue;

        QString params  = refSubstyle[1].trimmed();
        mergesStyles[command] = params;
    }

    // rebuild the style attribute from the merged styleElement
    QString newStyleAttribute;
    QMap<QString, QString>::const_iterator it = mergesStyles.constBegin();
    for( ; it != mergesStyles.constEnd(); ++it )
        newStyleAttribute += it.key() + ':' + it.value() + ';';

    e.setAttribute( "style", newStyleAttribute );

    return e;
}


// Parsing functions
// ---------------------------------------------------------------------------------------

double SvgParser::parseUnit( const QString &unit, bool horiz, bool vert, QRectF bbox )
{
    if( unit.isEmpty() )
        return 0.0;
    // TODO : percentage?
    const char *start = unit.toLatin1();
    if(!start) {
        return 0.0;
    }
    double value = 0.0;
    const char *end = parseNumber( start, value );

    if( int( end - start ) < unit.length() )
    {
        if( unit.right( 2 ) == "px" )
            value = SvgUtil::fromUserSpace( value );
        else if( unit.right( 2 ) == "cm" )
            value = CM_TO_POINT( value );
        else if( unit.right( 2 ) == "pc" )
            value = PI_TO_POINT( value );
        else if( unit.right( 2 ) == "mm" )
            value = MM_TO_POINT( value );
        else if( unit.right( 2 ) == "in" )
            value = INCH_TO_POINT( value );
        else if( unit.right( 2 ) == "em" )
            value = value * m_gc.top()->font.pointSize();
        else if( unit.right( 2 ) == "ex" )
        {
            QFontMetrics metrics( m_gc.top()->font );
            value = value * metrics.xHeight();
        }
        else if( unit.right( 1 ) == "%" )
        {
            if( horiz && vert )
                value = ( value / 100.0 ) * (sqrt( pow( bbox.width(), 2 ) + pow( bbox.height(), 2 ) ) / sqrt( 2.0 ) );
            else if( horiz )
                value = ( value / 100.0 ) * bbox.width();
            else if( vert )
                value = ( value / 100.0 ) * bbox.height();
        }
    }
    else
    {
        value = SvgUtil::fromUserSpace( value );
    }
    /*else
    {
        if( m_gc.top() )
        {
            if( horiz && vert )
                value *= sqrt( pow( m_gc.top()->matrix.m11(), 2 ) + pow( m_gc.top()->matrix.m22(), 2 ) ) / sqrt( 2.0 );
            else if( horiz )
                value /= m_gc.top()->matrix.m11();
            else if( vert )
                value /= m_gc.top()->matrix.m22();
        }
    }*/
    //value *= 90.0 / DPI;

    return value;
}

double SvgParser::parseUnitX( const QString &unit )
{
    SvgGraphicsContext * gc = m_gc.top();
    if( gc->forcePercentage )
    {
        return SvgUtil::fromPercentage( unit ) * gc->currentBoundbox.width();
    }
    else
    {
        return parseUnit( unit, true, false, gc->currentBoundbox );
    }
}

double SvgParser::parseUnitY( const QString &unit )
{
    SvgGraphicsContext * gc = m_gc.top();
    if( gc->forcePercentage )
    {
        return SvgUtil::fromPercentage( unit ) * gc->currentBoundbox.height();
    }
    else
    {
        return parseUnit( unit, false, true, gc->currentBoundbox );
    }
}

double SvgParser::parseUnitXY( const QString &unit )
{
    SvgGraphicsContext * gc = m_gc.top();
    if( gc->forcePercentage )
    {
        qreal value = SvgUtil::fromPercentage( unit );
        value *=  sqrt( pow( gc->currentBoundbox.width(), 2 ) + pow( gc->currentBoundbox.height(), 2 ) ) / sqrt( 2.0 );
        return value;
    }
    else
    {
        return parseUnit( unit, true, true, gc->currentBoundbox );
    }
}

QColor SvgParser::stringToColor( const QString &rgbColor )
{
    return m_rgbcolors[ rgbColor.toLatin1() ];
}

void SvgParser::parseColor( QColor &color, const QString &s )
{
    if( s.startsWith( "rgb(" ) )
    {
        QString parse = s.trimmed();
        QStringList colors = parse.split( ',' );
        QString r = colors[0].right( ( colors[0].length() - 4 ) );
        QString g = colors[1];
        QString b = colors[2].left( ( colors[2].length() - 1 ) );

        if( r.contains( "%" ) )
        {
            r = r.left( r.length() - 1 );
            r = QString::number( int( ( double( 255 * r.toDouble() ) / 100.0 ) ) );
        }

        if( g.contains( "%" ) )
        {
            g = g.left( g.length() - 1 );
            g = QString::number( int( ( double( 255 * g.toDouble() ) / 100.0 ) ) );
        }

        if( b.contains( "%" ) )
        {
            b = b.left( b.length() - 1 );
            b = QString::number( int( ( double( 255 * b.toDouble() ) / 100.0 ) ) );
        }

        color = QColor( r.toInt(), g.toInt(), b.toInt() );
    }
    else if( s == "currentColor" )
    {
        SvgGraphicsContext *gc = m_gc.top();
        color = gc->currentColor;
    }
    else
    {
        QString rgbColor = s.trimmed();
        if( rgbColor.startsWith( '#' ) )
            color.setNamedColor( rgbColor );
        else
            color = stringToColor( rgbColor );
    }
}

void SvgParser::parseColorStops( QGradient *gradient, const QDomElement &e )
{
    QGradientStops stops;
    QColor c;
    for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement stop = n.toElement();
        if( stop.tagName() == "stop" )
        {
            float offset;
            QString temp = stop.attribute( "offset" );
            if( temp.contains( '%' ) )
            {
                temp = temp.left( temp.length() - 1 );
                offset = temp.toFloat() / 100.0;
            }
            else
                offset = temp.toFloat();

            if( !stop.attribute( "stop-color" ).isEmpty() )
                parseColor( c, stop.attribute( "stop-color" ) );
            else
            {
                // try style attr
                QString style = stop.attribute( "style" ).simplified();
                QStringList substyles = style.split( ';', QString::SkipEmptyParts );
                for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
                {
                    QStringList substyle = it->split( ':' );
                    QString command = substyle[0].trimmed();
                    QString params  = substyle[1].trimmed();
                    if( command == "stop-color" )
                        parseColor( c, params );
                    if( command == "stop-opacity" )
                        c.setAlphaF( params.toDouble() );
                }

            }
            if( !stop.attribute( "stop-opacity" ).isEmpty() )
                c.setAlphaF( stop.attribute( "stop-opacity" ).toDouble() );
            stops.append( QPair<qreal,QColor>(offset, c ) );
        }
    }
    if( stops.count() )
        gradient->setStops( stops );
}

bool SvgParser::parseGradient( const QDomElement &e, const QDomElement &referencedBy)
{
    // IMPROVEMENTS:
    // - Store the parsed colorstops in some sort of a cache so they don't need to be parsed again.
    // - A gradient inherits attributes it does not have from the referencing gradient.
    // - Gradients with no color stops have no fill or stroke.
    // - Gradients with one color stop have a solid color.

    SvgGraphicsContext *gc = m_gc.top();
    if( !gc )
        return false;

    SvgGradientHelper gradhelper;

    if( e.hasAttribute("xlink:href") )
    {
        QString href = e.attribute("xlink:href").mid(1);
        if( ! href.isEmpty())
        {
            // copy the referenced gradient if found
            SvgGradientHelper * pGrad = findGradient( href );
            if( pGrad )
                gradhelper = *pGrad;
        }
        else
        {
            //gc->fillType = SvgGraphicsContext::None; // <--- TODO Fill OR Stroke are none
            return false;
        }
    }

    // Use the gradient that is referencing, or if there isn't one, the original gradient.
    QDomElement b;
    if( !referencedBy.isNull() )
        b = referencedBy;
    else
        b = e;

    QString gradientId = b.attribute("id");

    if( ! gradientId.isEmpty() )
    {
        // check if we have this gradient already parsed
        // copy existing gradient if it exists
        if( m_gradients.find( gradientId ) != m_gradients.end() )
            gradhelper.copyGradient( m_gradients[ gradientId ].gradient() );
    }

    if( b.attribute( "gradientUnits" ) == "userSpaceOnUse" )
        gradhelper.setGradientUnits( SvgGradientHelper::UserSpaceOnUse );

    // parse color prop
    QColor c = m_gc.top()->currentColor;

    if( !b.attribute( "color" ).isEmpty() )
    {
        parseColor( c, b.attribute( "color" ) );
    }
    else
    {
        // try style attr
        QString style = b.attribute( "style" ).simplified();
        QStringList substyles = style.split( ';', QString::SkipEmptyParts );
        for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
        {
            QStringList substyle = it->split( ':' );
            QString command = substyle[0].trimmed();
            QString params  = substyle[1].trimmed();
            if( command == "color" )
                parseColor( c, params );
        }
    }
    m_gc.top()->currentColor = c;

    if( b.tagName() == "linearGradient" )
    {
        QLinearGradient * g = new QLinearGradient();
        if( gradhelper.gradientUnits() == SvgGradientHelper::ObjectBoundingBox )
        {
            g->setStart( QPointF( SvgUtil::toPercentage( b.attribute( "x1", "0%" ) ), 
                                  SvgUtil::toPercentage( b.attribute( "y1", "0%" ) ) ) );
            g->setFinalStop( QPointF( SvgUtil::toPercentage( b.attribute( "x2", "100%" ) ),
                                      SvgUtil::toPercentage( b.attribute( "y2", "0%" ) ) ) );
        }
        else
        {
            g->setStart( QPointF( SvgUtil::fromUserSpace(b.attribute( "x1" ).toDouble()),
                                  SvgUtil::fromUserSpace( b.attribute( "y1" ).toDouble() ) ) );
            g->setFinalStop( QPointF( SvgUtil::fromUserSpace( b.attribute( "x2" ).toDouble() ), 
                                      SvgUtil::fromUserSpace( b.attribute( "y2" ).toDouble() ) ) );
        }
        // preserve color stops
        if( gradhelper.gradient() )
            g->setStops( gradhelper.gradient()->stops() );
        gradhelper.setGradient( g );
    }
    else if( b.tagName() == "radialGradient" )
    {
        QRadialGradient * g = new QRadialGradient();
        if( gradhelper.gradientUnits() == SvgGradientHelper::ObjectBoundingBox )
        {
            g->setCenter( QPointF( SvgUtil::toPercentage( b.attribute( "cx", "50%" ) ), 
                                   SvgUtil::toPercentage( b.attribute( "cy", "50%" ) ) ) );
            g->setRadius( SvgUtil::toPercentage( b.attribute( "r", "50%" ) ) );
            g->setFocalPoint( QPointF( SvgUtil::toPercentage( b.attribute( "fx", "50%" ) ),
                                       SvgUtil::toPercentage( b.attribute( "fy", "50%" ) ) ) );
        }
        else
        {
            g->setCenter( QPointF( SvgUtil::fromUserSpace( b.attribute( "cx" ).toDouble() ), 
                                   SvgUtil::fromUserSpace( b.attribute( "cy" ).toDouble() ) ) );
            g->setFocalPoint( QPointF( SvgUtil::fromUserSpace( b.attribute( "fx" ).toDouble() ), 
                                       SvgUtil::fromUserSpace( b.attribute( "fy" ).toDouble() ) ) );
            g->setRadius( SvgUtil::fromUserSpace( b.attribute( "r" ).toDouble() ) );
        }
        // preserve color stops
        if( gradhelper.gradient() )
            g->setStops( gradhelper.gradient()->stops() );
        gradhelper.setGradient( g );
    }
    else
    {
        return false;
    }

    // handle spread method
    QString spreadMethod = b.attribute( "spreadMethod" );
    if( !spreadMethod.isEmpty() )
    {
        if( spreadMethod == "reflect" )
            gradhelper.gradient()->setSpread( QGradient::ReflectSpread );
        else if( spreadMethod == "repeat" )
            gradhelper.gradient()->setSpread( QGradient::RepeatSpread );
        else
            gradhelper.gradient()->setSpread( QGradient::PadSpread );
    }
    else
        gradhelper.gradient()->setSpread( QGradient::PadSpread );

    // Parse the color stops. The referencing gradient does not have colorstops, 
    // so use the stops from the gradient it references to (e in this case and not b)
    parseColorStops( gradhelper.gradient(), e );
    //gradient.setGradientTransform( parseTransform( e.attribute( "gradientTransform" ) ) );
    gradhelper.setTransform( parseTransform( b.attribute( "gradientTransform" ) ) );
    m_gradients.insert( gradientId, gradhelper );

    return true;
}

bool SvgParser::parsePattern( const QDomElement &e, const QDomElement &referencedBy )
{
    SvgGraphicsContext *gc = m_gc.top();
    if( !gc )
        return false;

    SvgPatternHelper pattern;

    // Use the pattern that is referencing, or if there isn't one, the original pattern
    QDomElement b;
    if( !referencedBy.isNull() )
        b = referencedBy;
    else
        b = e;

    // check if we are referencing another pattern
    if( e.hasAttribute("xlink:href")  )
    {
        QString href = e.attribute("xlink:href").mid(1);
        if( ! href.isEmpty() )
        {
            // copy the referenced pattern if found
            SvgPatternHelper * refPattern = findPattern( href );
            if( refPattern )
                pattern = *refPattern;
        }
    }
    else
    {
        pattern.setContent( b );
    }

    if( b.attribute( "patternUnits" ) == "userSpaceOnUse" )
        pattern.setPatternUnits( SvgPatternHelper::UserSpaceOnUse );
    if( b.attribute( "patternContentUnits" ) == "objectBoundingBox" )
        pattern.setPatternContentUnits( SvgPatternHelper::ObjectBoundingBox );
    if( b.hasAttribute( "viewBox" ) )
        pattern.setPatternContentViewbox( parseViewBox( b.attribute( "viewBox" ) ) );
    if( b.hasAttribute( "patternTransform" ) )
        pattern.setTransform( parseTransform( b.attribute( "patternTransform" ) ) );


    // parse tile reference rectangle
    if( pattern.patternUnits() == SvgPatternHelper::UserSpaceOnUse )
    {
        pattern.setPosition( QPointF( parseUnitX( b.attribute( "x" ) ),
                                      parseUnitY( b.attribute( "y" ) ) ) );
        pattern.setSize( QSizeF( parseUnitX( b.attribute( "width" ) ),
                                 parseUnitY( b.attribute( "height" ) ) ) );
    }
    else
    {
        // x, y, width, height are in percentages of the object referencing the pattern
        // so we just parse the percentages
        pattern.setPosition( QPointF( SvgUtil::fromPercentage( b.attribute( "x" ) ), 
                                      SvgUtil::fromPercentage( b.attribute( "y" ) ) ) );
        pattern.setSize( QSizeF( SvgUtil::fromPercentage( b.attribute( "width" ) ),
                                 SvgUtil::fromPercentage( b.attribute( "height" ) ) ) );
    }

    m_patterns.insert( b.attribute( "id" ), pattern );

    return true;
}

bool SvgParser::parseFilter( const QDomElement &e, const QDomElement &referencedBy )
{
    SvgFilterHelper filter;

    // Use the filter that is referencing, or if there isn't one, the original filter
    QDomElement b;
    if( !referencedBy.isNull() )
        b = referencedBy;
    else
        b = e;

    // check if we are referencing another filter
    if( e.hasAttribute("xlink:href")  )
    {
        QString href = e.attribute("xlink:href").mid(1);
        if( ! href.isEmpty() )
        {
            // copy the referenced filter if found
            SvgFilterHelper * refFilter = findFilter( href );
            if( refFilter )
                filter = *refFilter;
        }
    }
    else
    {
        filter.setContent( b );
    }

    if( b.attribute( "filterUnits" ) == "userSpaceOnUse" )
        filter.setFilterUnits( SvgFilterHelper::UserSpaceOnUse );
    if( b.attribute( "primitiveUnits" ) == "objectBoundingBox" )
        filter.setPrimitiveUnits( SvgFilterHelper::ObjectBoundingBox );

    // parse filter region rectangle
    if( filter.filterUnits() == SvgFilterHelper::UserSpaceOnUse )
    {
        filter.setPosition( QPointF( parseUnitX( b.attribute( "x" ) ),
                                      parseUnitY( b.attribute( "y" ) ) ) );
        filter.setSize( QSizeF( parseUnitX( b.attribute( "width" ) ),
                                 parseUnitY( b.attribute( "height" ) ) ) );
    }
    else
    {
        // x, y, width, height are in percentages of the object referencing the filter
        // so we just parse the percentages
        filter.setPosition( QPointF( SvgUtil::fromPercentage( b.attribute( "x", "-0.1" ) ), 
                                      SvgUtil::fromPercentage( b.attribute( "y", "-0.1" ) ) ) );
        filter.setSize( QSizeF( SvgUtil::fromPercentage( b.attribute( "width", "1.2" ) ),
                                 SvgUtil::fromPercentage( b.attribute( "height", "1.2" ) ) ) );
    }

    m_filters.insert( b.attribute( "id" ), filter );

    return true;
}

bool SvgParser::parseImage( const QString &attribute, QImage &image )
{
    if( attribute.startsWith( "data:" ) )
    {
        int start = attribute.indexOf( "base64," );
        if( start > 0 && image.loadFromData( QByteArray::fromBase64( attribute.mid( start + 7 ).toLatin1() ) ) )
            return true;
    }
    else if( image.load( absoluteFilePath( attribute, m_gc.top()->xmlBaseDir ) ) )
    {
        return true;
    }

    return false;
}

void SvgParser::parsePA( SvgGraphicsContext *gc, const QString &command, const QString &params )
{
    QColor fillcolor = gc->fillColor;
    QColor strokecolor = gc->stroke.color();

    if( params == "inherit" ) 
        return;

    if( command == "fill" )
    {
        if( params == "none" )
        {
            gc->fillType = SvgGraphicsContext::None;
        }
        else if( params.startsWith( "url(" ) )
        {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.lastIndexOf(')');
            QString key = params.mid( start, end - start );
            // try to find referenced gradient
            SvgGradientHelper * gradHelper = findGradient( key );
            if( gradHelper )
            {
                // great, we have a gradient fill
                gc->fillType = SvgGraphicsContext::Gradient;
                gc->fillId = key;
            }
            else 
            {
                // try to find referenced pattern
                SvgPatternHelper * pattern = findPattern( key );
                if( pattern )
                {
                    // great we have a pattern fill
                    gc->fillType = SvgGraphicsContext::Pattern;
                    gc->fillId = key;
                }
                else
                {
                    // no referenced fill found, reset fill
                    gc->fillType = SvgGraphicsContext::None;
                    gc->fillId.clear();
                }
            }
        }
        else
        {
            // great we have a solid fill
            gc->fillType = SvgGraphicsContext::Solid;
            parseColor( fillcolor,  params );
        }
    }
    else if( command == "fill-rule" )
    {
        if( params == "nonzero" )
            gc->fillRule = Qt::WindingFill;
        else if( params == "evenodd" )
            gc->fillRule = Qt::OddEvenFill;
    }
    else if( command == "stroke" )
    {
        if( params == "none" )
        {
            gc->strokeType = SvgGraphicsContext::None;
        }
        else if( params.startsWith( "url(" ) )
        {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.lastIndexOf(')');
            QString key = params.mid( start, end - start );
            // try to find referenced gradient
            SvgGradientHelper * gradHelper = findGradient( key );
            if( gradHelper ) 
            {
                // great, we have a gradient stroke
                gc->strokeType = SvgGraphicsContext::Gradient;
                gc->strokeId = key;
            }
            else
            {
                // no referenced stroke found, reset stroke
                gc->strokeType = SvgGraphicsContext::None;
                gc->strokeId.clear();
            }
        }
        else
        {
            // great we have a solid stroke
            gc->strokeType = SvgGraphicsContext::Solid;
            parseColor( strokecolor, params );
        }
    }
    else if( command == "stroke-width" )
    {
        gc->stroke.setLineWidth( parseUnitXY( params ) );
    }
    else if( command == "stroke-linejoin" )
    {
        if( params == "miter" )
            gc->stroke.setJoinStyle( Qt::MiterJoin );
        else if( params == "round" )
            gc->stroke.setJoinStyle( Qt::RoundJoin );
        else if( params == "bevel" )
            gc->stroke.setJoinStyle( Qt::BevelJoin );
    }
    else if( command == "stroke-linecap" )
    {
        if( params == "butt" )
            gc->stroke.setCapStyle( Qt::FlatCap );
        else if( params == "round" )
            gc->stroke.setCapStyle( Qt::RoundCap );
        else if( params == "square" )
            gc->stroke.setCapStyle( Qt::SquareCap );
    }
    else if( command == "stroke-miterlimit" )
    {
        gc->stroke.setMiterLimit( params.toFloat() );
    }
    else if( command == "stroke-dasharray" )
    {
        QVector<qreal> array;
        if(params != "none")
        {
            QString dashString = params;
            QStringList dashes = dashString.replace( ',', ' ').simplified().split( ' ' );
            for( QStringList::Iterator it = dashes.begin(); it != dashes.end(); ++it )
                array.append( (*it).toFloat() );
        }
        gc->stroke.setLineStyle( Qt::CustomDashLine, array );
    }
    else if( command == "stroke-dashoffset" )
    {
        gc->stroke.setDashOffset( params.toFloat() );
    }
    // handle opacity
    else if( command == "stroke-opacity" )
        strokecolor.setAlphaF( SvgUtil::fromPercentage( params ) );
    else if( command == "fill-opacity" )
    {
        float opacity = SvgUtil::fromPercentage( params );
        if( opacity < 0.0 )
            opacity = 0.0;
        if( opacity > 1.0 )
            opacity = 1.0;
        fillcolor.setAlphaF( opacity );
    }
    else if( command == "opacity" )
    {
        fillcolor.setAlphaF( SvgUtil::fromPercentage( params ) );
        strokecolor.setAlphaF( SvgUtil::fromPercentage( params ) );
    }
    else if( command == "font-family" )
    {
        QString family = params;
        family.replace( '\'' , ' ' );
        gc->font.setFamily( family );
    }
    else if( command == "font-size" )
    {
        float pointSize = parseUnitY( params );
        if( pointSize > 0.0f ) 
            gc->font.setPointSizeF( pointSize );
    }
    else if( command == "font-weight" )
    {
        int weight = QFont::Normal;

        // map svg weight to qt weight
        // svg value        qt value
        // 100,200,300      1, 17, 33
        // 400              50          (normal)
        // 500,600          58,66
        // 700              75          (bold)
        // 800,900          87,99

        if( params == "bold" )
            weight = QFont::Bold;
        else if( params == "lighter" )
        {
            weight = gc->font.weight();
            if( weight <= 17 ) 
                weight = 1;
            else if( weight <= 33 )
                weight = 17;
            else if( weight <= 50 )
                weight = 33;
            else if( weight <= 58 )
                weight = 50;
            else if( weight <= 66 )
                weight = 58;
            else if( weight <= 75 )
                weight = 66;
            else if( weight <= 87 )
                weight = 75;
            else if( weight <= 99 )
                weight = 87;
        }
        else if( params == "bolder" )
        {
            weight = gc->font.weight();
            if( weight >= 87 ) 
                weight = 99;
            else if( weight >= 75 )
                weight = 87;
            else if( weight >= 66 )
                weight = 75;
            else if( weight >= 58 )
                weight = 66;
            else if( weight >= 50 )
                weight = 58;
            else if( weight >= 33 )
                weight = 50;
            else if( weight >= 17 )
                weight = 50;
            else if( weight >= 1 )
                weight = 17;
        }
        else
        {
            bool ok;
            // try to read numerical weight value
            weight = params.toInt( &ok, 10 );

            if( !ok )
                return;

            switch( weight )
            {
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
        gc->font.setWeight( weight );
    }
    else if( command == "text-decoration" )
    {
        if( params == "line-through" )
            gc->font.setStrikeOut( true );
        else if( params == "underline" )
            gc->font.setUnderline( true );
    }
    else if( command == "color" )
    {
        QColor color;
        parseColor( color, params );
        gc->currentColor = color;
    }
    else if( command == "display" )
    {
        if( params == "none" )
            gc->display = false;
    }
    else if( command == "filter" )
    {
        if( params != "none" && params.startsWith( "url(" ) )
        {
            unsigned int start = params.indexOf('#') + 1;
            unsigned int end = params.lastIndexOf(')');
            gc->filterId = params.mid( start, end - start );
        }
    }

    gc->fillColor = fillcolor;
    gc->stroke.setColor( strokecolor );
}

void SvgParser::parseStyle( KoShape *obj, const QDomElement &e )
{
    SvgGraphicsContext *gc = m_gc.top();
    if( !gc ) return;

    QMap<QString, QString> styleMap;

    // first collect individual style attributes
    foreach( const QString & command, m_styleAttributes )
    {
        if( e.hasAttribute( command ) )
            styleMap[command] = e.attribute( command );
    }

    // now parse style attribute
    QString style = e.attribute( "style" ).simplified();
    QStringList substyles = style.split( ';', QString::SkipEmptyParts );
    for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
    {
        QStringList substyle = it->split( ':' );
        QString command = substyle[0].trimmed();
        QString params  = substyle[1].trimmed();
        // only use style attributes
        if( m_styleAttributes.contains( command ) )
            styleMap[command] = params;
    }

    // make sure we parse the style attributes in the right order
    foreach( const QString & command, m_styleAttributes )
    {
        QString params = styleMap.value( command );
        if( params.isEmpty() )
            continue;
        parsePA( gc, command, params );
    }

    if(!obj)
        return;

    applyFillStyle( obj );
    applyStrokeStyle( obj );
    applyFilter( obj );

    if( ! gc->display )
        obj->setVisible( false );
}

void SvgParser::applyFillStyle( KoShape * shape )
{
    SvgGraphicsContext *gc = m_gc.top();
    if( ! gc )
        return;

    switch( gc->fillType )
    {
    case SvgGraphicsContext::None:
        shape->setBackground( 0 );
        break;
    case SvgGraphicsContext::Gradient:
        {
            SvgGradientHelper * gradient = findGradient( gc->fillId );
            if( gradient )
            {
                KoGradientBackground * bg = 0;
                if( gradient->gradientUnits() == SvgGradientHelper::ObjectBoundingBox )
                {
                    // adjust to bounding box
                    QRectF bbox = QRectF( QPoint(), shape->size() );
                    bg = new KoGradientBackground( gradient->adjustedGradient( bbox ) );
                    bg->setMatrix( gradient->transform() );
                }
                else
                {
                    QMatrix invShapematrix = shape->transformation().inverted();
                    bg = new KoGradientBackground( *gradient->gradient() );
                    bg->setMatrix( gradient->transform() * gc->matrix * invShapematrix );
                }
            
                shape->setBackground( bg );
            }
        }
        break;
    case SvgGraphicsContext::Pattern:
        {
            SvgPatternHelper * pattern = findPattern( gc->fillId );
            KoImageCollection * imageCollection = dynamic_cast<KoImageCollection *>( m_dataCenters["ImageCollection"] );
            if( pattern && imageCollection )
            {
                QRectF objectBound = QRectF( QPoint(), shape->size() );
                QRectF currentBoundbox = gc->currentBoundbox;

                // properties from the object are not inherited
                // so we are creating a new context without copying
                m_gc.push( new SvgGraphicsContext() );

                // the pattern establishes a new coordinate system with its
                // origin at the patterns x and y attributes
                m_gc.top()->matrix = QMatrix();
                // object bounding box units are relative to the object the pattern is applied
                if( pattern->patternContentUnits() == SvgPatternHelper::ObjectBoundingBox )
                {
                    m_gc.top()->currentBoundbox = objectBound;
                    m_gc.top()->forcePercentage = true;
                }
                else
                {
                    // inherit the current bounding box
                    m_gc.top()->currentBoundbox = currentBoundbox;
                }

                updateContext( pattern->content() );
                parseStyle( 0, pattern->content() );

                // parse the pattern content elements
                QList<KoShape*> patternContent = parseContainer( pattern->content() );

                // generate the pattern image from the shapes and the object bounding rect
                QImage image = pattern->generateImage( objectBound, patternContent );
                
                removeGraphicContext();
                
                // delete the shapes created from the pattern content 
                qDeleteAll( patternContent );

                KoPatternBackground * bg = new KoPatternBackground( imageCollection );
                bg->setPattern( image );

                QPointF refPoint = shape->documentToShape( pattern->position( objectBound ) );
                QSizeF tileSize = pattern->size( objectBound );

                bg->setPatternDisplaySize( tileSize );
                if( pattern->patternUnits() == SvgPatternHelper::ObjectBoundingBox )
                {
                    if( tileSize == objectBound.size() )
                        bg->setRepeat( KoPatternBackground::Stretched );
                }
                
                // calculate pattern reference point offset in percent of tileSize
                // and relative to the topleft corner of the shape
                qreal fx = refPoint.x() / tileSize.width();
                qreal fy = refPoint.y() / tileSize.height();
                if( fx < 0.0 )
                    fx = floor(fx);
                else if( fx > 1.0 )
                    fx = ceil(fx);
                else
                    fx = 0.0;
                if( fy < 0.0 )
                    fy = floor(fy);
                else if( fx > 1.0 )
                    fy = ceil(fy);
                else
                    fy = 0.0;
                qreal offsetX = 100.0 * (refPoint.x()-fx*tileSize.width()) / tileSize.width();
                qreal offsetY = 100.0 * (refPoint.y()-fy*tileSize.height()) / tileSize.height();
                bg->setReferencePointOffset( QPointF(offsetX, offsetY) );
                
                shape->setBackground( bg );
            }
        }
        break;
    case SvgGraphicsContext::Solid:
    default:
        shape->setBackground( new KoColorBackground( gc->fillColor ) );
        break;
    }

    KoPathShape * path = dynamic_cast<KoPathShape*>( shape );
    if( path )
        path->setFillRule( gc->fillRule );
}

void SvgParser::applyStrokeStyle( KoShape * shape )
{
    SvgGraphicsContext *gc = m_gc.top();
    if( ! gc )
        return;

    switch( gc->strokeType )
    {
        case SvgGraphicsContext::Solid:
        {
            double lineWidth = gc->stroke.lineWidth();
            QVector<qreal> dashes = gc->stroke.lineDashes();

            KoLineBorder * border = new KoLineBorder( gc->stroke );

            // apply line width to dashes and dash offset
            if( dashes.count() && lineWidth > 0.0 )
            {
                QVector<qreal> dashes = border->lineDashes();
                for( int i = 0; i < dashes.count(); ++i )
                    dashes[i] /= lineWidth;
                double dashOffset = border->dashOffset();
                border->setLineStyle( Qt::CustomDashLine, dashes );
                border->setDashOffset( dashOffset / lineWidth );
            }
            else
            {
                border->setLineStyle( Qt::SolidLine, QVector<qreal>() );
            }
            shape->setBorder( border );
        }
        break;
        case SvgGraphicsContext::Gradient:
        {
            SvgGradientHelper * gradient = findGradient( gc->strokeId );
            if( gradient )
            {
                QBrush brush;
                if( gradient->gradientUnits() == SvgGradientHelper::ObjectBoundingBox )
                {
                    // adjust to bbox
                    QRectF bbox = QRectF( QPoint(), shape->size() );
                    brush = gradient->adjustedFill( bbox );
                    brush.setMatrix( gradient->transform() );
                }
                else
                {
                    brush = QBrush( *gradient->gradient() );
                    brush.setMatrix( gradient->transform() * gc->matrix * shape->transformation().inverted() );
                }
                KoLineBorder * border = new KoLineBorder( gc->stroke );
                border->setLineBrush( brush );
                border->setLineStyle( Qt::SolidLine, QVector<qreal>() );
                shape->setBorder( border );
            }
        }
        break;
        case SvgGraphicsContext::None:
        default:
            shape->setBorder( 0 );
        break;
    }
}

void SvgParser::applyFilter( KoShape * shape )
{
    SvgGraphicsContext *gc = m_gc.top();
    if( ! gc )
        return;

    if( gc->filterId.isEmpty() )
        return;

    SvgFilterHelper * filter = findFilter( gc->filterId );
    if( ! filter )
        return;

    QDomElement content = filter->content();

    // parse filter region
    QRectF bound( QPoint(), shape->size() );
    QRectF filterRegion( filter->position(bound), filter->size(bound) );

    // convert filter region to boundingbox units 
    QRectF objectFilterRegion;
    objectFilterRegion.setTopLeft(SvgUtil::userSpaceToObject(filterRegion.topLeft(), bound));
    objectFilterRegion.setSize(SvgUtil::userSpaceToObject(filterRegion.size(), bound));

    KoFilterEffectRegistry * registry = KoFilterEffectRegistry::instance();

    int existingFilterCount = shape->filterEffectStack().count();
    // create the filter effects and add them to the shape
    for( QDomNode n = content.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement primitive = n.toElement();
        KoFilterEffect * filterEffect = registry->createFilterEffectFromXml(primitive);
        if (!filterEffect) {
            kWarning(30514) << "filter effect" << primitive.tagName() << "is not implemented yet";
            continue;
        }

        QRectF subRegion = filterRegion;
        // parse subregion
        if( filter->primitiveUnits() == SvgFilterHelper::UserSpaceOnUse )
        {
            if( primitive.hasAttribute( "x" ) )
                subRegion.setX( parseUnitX( primitive.attribute( "x" ) ) ); 
            if( primitive.hasAttribute( "y" ) )
                subRegion.setY( parseUnitY( primitive.attribute( "y" ) ) );
            if( primitive.hasAttribute( "width" ) )
                subRegion.setWidth( parseUnitX( primitive.attribute( "width" ) ) );
            if( primitive.hasAttribute( "height" ) )
                subRegion.setHeight( parseUnitY( primitive.attribute( "height" ) ) );
            subRegion.setTopLeft(SvgUtil::userSpaceToObject(subRegion.topLeft(), filterRegion));
            subRegion.setSize(SvgUtil::userSpaceToObject(subRegion.size(), filterRegion));
        }
        else
        {
            // x, y, width, height are in percentages of the object referencing the filter
            // so we just parse the percentages
            qreal x = SvgUtil::fromPercentage( primitive.attribute( "x", "0" ) ); 
            qreal y = SvgUtil::fromPercentage( primitive.attribute( "y", "0" ) );
            qreal w = SvgUtil::fromPercentage( primitive.attribute( "width", "1" ) );
            qreal h = SvgUtil::fromPercentage( primitive.attribute( "height", "1" ) );
            subRegion = QRectF(QPointF(x, y), QSizeF(w, h));
        }
        if (primitive.hasAttribute("in"))
            filterEffect->addInput(primitive.attribute("in"));
        if (primitive.hasAttribute("result"))
            filterEffect->setOutput(primitive.attribute("result"));

        filterEffect->setClipRect(objectFilterRegion);
        filterEffect->setFilterRect(subRegion);
        shape->insertFilterEffect(existingFilterCount, filterEffect);
        existingFilterCount++;
    }
}

void SvgParser::parseFont( const QDomElement &e )
{
    SvgGraphicsContext * gc = m_gc.top();
    if( !gc ) return;

    foreach( const QString &attributeName, m_fontAttributes )
    {
        if( ! e.attribute( attributeName ).isEmpty() )
            parsePA( gc, attributeName, e.attribute( attributeName ) );
    }

    // now parse the style attribute for font specific parameters
    QString style = e.attribute( "style" ).simplified();
    QStringList substyles = style.split( ';', QString::SkipEmptyParts );
    for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
    {
        QStringList substyle = it->split( ':' );
        QString command = substyle[0].trimmed();
        QString params  = substyle[1].trimmed();
        // only parse font related parameters here
        if( m_fontAttributes.contains( command ) )
            parsePA( gc, command, params );
    }
}

QList<KoShape*> SvgParser::parseUse( const QDomElement &e )
{
    QList<KoShape*> shapes;

    QString id = e.attribute( "xlink:href" );
    // 
    if( !id.isEmpty() )
    {
        addGraphicContext();
        setupTransform( e );
        updateContext( e );

        QString key = id.mid( 1 );

        if( e.hasAttribute( "x" ) )
            m_gc.top()->matrix.translate( parseUnitX( e.attribute( "x" ) ), 0.0 );
        if( e.hasAttribute( "y" ) )
            m_gc.top()->matrix.translate( 0.0, parseUnitX( e.attribute( "y" ) ) );

        // TODO: use width and height attributes too
        
        if(m_defs.contains(key))
        {
            QDomElement a = mergeStyles( e, m_defs[key] );
            if(a.tagName() == "g" || a.tagName() == "a")
            {
                addGraphicContext();
                setupTransform( a );
                updateContext( a );

                KoShapeGroup * group = new KoShapeGroup();
                group->setZIndex( nextZIndex() );

                parseStyle( 0, a );
                parseFont( a );

                QList<KoShape*> childShapes = parseContainer( a );

                // handle id
                if( !a.attribute("id").isEmpty() )
                    group->setName( a.attribute("id") );

                addToGroup( childShapes, group );

                shapes.append( group );

                removeGraphicContext();
            }
            else
            {
                // Create the object with the merged styles.
                // The object inherits all style attributes from the use tag, but keeps it's own attributes.
                // So, not just use the style attributes of the use tag, but merge them first.
                KoShape * shape = createObject( a, mergeStyles(e, a) );
                if( shape )
                    shapes.append( shape );
            }
        }
        removeGraphicContext();
    }

    return shapes;
}

void SvgParser::addToGroup( QList<KoShape*> shapes, KoShapeGroup * group )
{
    m_shapes += shapes;

    if( ! group )
        return;

    KoShapeGroupCommand cmd( group, shapes );
    cmd.redo();
}

QList<KoShape*> SvgParser::parseSvg( const QDomElement &e, QSizeF * fragmentSize )
{
    // check if we are the root svg element
    bool isRootSvg = m_gc.isEmpty();

    addGraphicContext();

    SvgGraphicsContext *gc = m_gc.top();

    parseStyle( 0, e );

    bool hasViewBox = e.hasAttribute( "viewBox" );
    QRectF viewBox;

    if( hasViewBox )
    {
        viewBox = parseViewBox( e.attribute( "viewBox" ) );
    }

    double width = 550.0;
    if( e.hasAttribute( "width" ) )
        width = parseUnit( e.attribute( "width" ), true, false, viewBox );
    double height = 841.0;
    if( e.hasAttribute( "height" ) )
        height = parseUnit( e.attribute( "height" ), false, true, viewBox );

    QSizeF svgFragmentSize( QSizeF( width, height ) );

    if( fragmentSize )
        *fragmentSize = svgFragmentSize;

    gc->currentBoundbox = QRectF( QPointF(0,0), svgFragmentSize );

    if( ! isRootSvg )
    {
        QMatrix move;
        // x and y attribute has no meaning for outermost svg elements
        double x = e.hasAttribute( "x" ) ? parseUnit( e.attribute( "x" ) ) : 0.0;
        double y = e.hasAttribute( "y" ) ? parseUnit( e.attribute( "y" ) ) : 0.0;
        move.translate( x, y );
        gc->matrix = move * gc->matrix;
    }

    if( hasViewBox )
    {
        QMatrix viewTransform;
        viewTransform.translate( viewBox.x(), viewBox.y() );
        viewTransform.scale( width / viewBox.width() , height / viewBox.height() );
        gc->matrix = viewTransform * gc->matrix;
        gc->currentBoundbox.setWidth( gc->currentBoundbox.width() * ( viewBox.width() / width ) );
        gc->currentBoundbox.setHeight( gc->currentBoundbox.height() * ( viewBox.height() / height ) );
    }

    QList<KoShape*> shapes = parseContainer( e );

    removeGraphicContext();

    return shapes;
}

QList<KoShape*> SvgParser::parseContainer( const QDomElement &e )
{
    QList<KoShape*> shapes;

    // are we parsing a switch container
    bool isSwitch = e.tagName() == "switch";

    for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement b = n.toElement();
        if( b.isNull() )
            continue;

        if( isSwitch )
        {
            // if we are parsing a switch check the requiredFeatures, requiredExtensions
            // and systemLanguage attributes
            if( b.hasAttribute("requiredFeatures") )
            {
                QString features = b.attribute("requiredFeatures");
                if( features.isEmpty() || features.simplified().isEmpty() )
                    continue;
                // TODO: evaluate feature list
            }
            if( b.hasAttribute( "requiredExtensions" ) )
            {
                // we do not support any extensions
                continue;
            }
            if( b.hasAttribute( "systemLanguage" ) )
            {
                // not implemeted yet
            }
        }

        if( b.tagName() == "svg" )
        {
            shapes += parseSvg( b );
        }
        else if( b.tagName() == "g" || b.tagName() == "a" )
        {
            // treat svg link <a> as group so we don't miss its child elements
            addGraphicContext();
            setupTransform( b );
            updateContext( b );

            KoShapeGroup * group = new KoShapeGroup();
            group->setZIndex( nextZIndex() );

            parseStyle( 0, b );
            parseFont( b );

            QList<KoShape*> childShapes = parseContainer( b );

            // handle id
            if( !b.attribute("id").isEmpty() )
                group->setName( b.attribute("id") );

            addToGroup( childShapes, group );

            shapes.append( group );

            removeGraphicContext();
        }
        else if( b.tagName() == "switch" )
        {
            addGraphicContext();
            setupTransform( b );
            
            shapes += parseContainer( b );
            
            removeGraphicContext();
        }
        else if( b.tagName() == "defs" )
        {
            parseDefs( b );
        }
        else if( b.tagName() == "linearGradient" || b.tagName() == "radialGradient" )
        {
            parseGradient( b );
        }
        else if( b.tagName() == "pattern" )
        {
            parsePattern( b );
        }
        else if( b.tagName() == "filter" )
        {
            parseFilter( b );
        }
        else if( b.tagName() == "rect" ||
            b.tagName() == "ellipse" ||
            b.tagName() == "circle" ||
            b.tagName() == "line" ||
            b.tagName() == "polyline" ||
            b.tagName() == "polygon" ||
            b.tagName() == "path" ||
            b.tagName() == "image" )
        {
            KoShape * shape = createObject( b );
            if( shape )
                shapes.append( shape );
        }
        else if( b.tagName() == "text" )
        {
            KoShape * shape = createText( b, shapes );
            if( shape )
                shapes.append( shape );
        }
        else if( b.tagName() == "use" )
        {
            shapes += parseUse( b );
        }
        else
        {
            continue;
        }

        // if we are parsing a switch, stop after the first supported element
        if( isSwitch )
            break;
    }

    return shapes;
}

void SvgParser::parseDefs( const QDomElement &e )
{
    for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement b = n.toElement();
        if( b.isNull() ) continue;

        QString definition = b.attribute( "id" );
        if( !definition.isEmpty() )
        {
            if( !m_defs.contains( definition ) )
                m_defs.insert( definition, b );
        }
    }
}

QRectF SvgParser::parseViewBox( QString viewbox )
{
    QRectF viewboxRect;

    QStringList points = viewbox.replace( ',', ' ').simplified().split( ' ' );
    if( points.count() == 4 )
    {
        viewboxRect.setX( SvgUtil::fromUserSpace( points[0].toFloat() ) );
        viewboxRect.setY( SvgUtil::fromUserSpace( points[1].toFloat() ) );
        viewboxRect.setWidth( SvgUtil::fromUserSpace( points[2].toFloat() ) );
        viewboxRect.setHeight( SvgUtil::fromUserSpace( points[3].toFloat() ) );
    }

    return viewboxRect;
}

// Creating functions
// ---------------------------------------------------------------------------------------

KoShape * SvgParser::createText( const QDomElement &b, const QList<KoShape*> & shapes )
{
    QString content;
    QString anchor;
    double offset = 0.0;

    QPointF textPosition;
    ArtisticTextShape * text = 0;

    addGraphicContext();
    setupTransform( b );
    updateContext( b );

    if( ! b.attribute( "text-anchor" ).isEmpty() )
        anchor = b.attribute( "text-anchor" );

    parseFont( b );
    QDomElement styleElement = b;

    if( b.hasChildNodes() )
    {
        if( textPosition.isNull() )
        {
            if( b.hasAttribute( "x" ) ) 
                textPosition.setX( parseUnitX( b.attribute( "x" ) ) );
            if( b.hasAttribute( "y" ) )
                textPosition.setY( parseUnitY( b.attribute( "y" ) ) );
        }

        text = static_cast<ArtisticTextShape*>( createShape( ArtisticTextShapeID ) );
        if( ! text )
            return 0;

        KoPathShape * path = 0;
        bool pathInDocument = false;

        for( QDomNode n = b.firstChild(); !n.isNull(); n = n.nextSibling() )
        {
            QDomElement e = n.toElement();
            if( e.isNull() )
            {
                content += n.toCharacterData().data();
            }
            else if( e.tagName() == "textPath" )
            {
                if( e.attribute( "xlink:href" ).isEmpty() )
                    continue;

                QString key = e.attribute( "xlink:href" ).mid( 1 );
                if( ! m_defs.contains(key) )
                {
                    // try to find referenced object in document
                    KoShape * obj = findObject( key );
                    // try to find referenced object in actual group, which is not yet part of document
                    if( ! obj )
                        obj = findObject( key, shapes );
                    if( obj )
                        path = dynamic_cast<KoPathShape*>( obj );
                    if( path )
                        pathInDocument = true;
                }
                else
                {
                    QDomElement p = m_defs[key];
                    path = dynamic_cast<KoPathShape*>( createObject( p ) );
                    pathInDocument = false;
                    path->applyAbsoluteTransformation( m_gc.top()->matrix.inverted() );
                }
                if( ! path )
                    continue;

                content += e.text();

                if( ! e.attribute( "startOffset" ).isEmpty() )
                {
                    QString start = e.attribute( "startOffset" );
                    if( start.endsWith( '%' ) )
                        offset = 0.01 * start.remove( '%' ).toDouble();
                    else
                    {
                        float pathLength = path->outline().length();
                        if( pathLength > 0.0 )
                            offset = start.toDouble() / pathLength;
                    }
                }
            }
            else if( e.tagName() == "tspan" )
            {
                // only use text of tspan element, as we are not supporting text 
                // with different styles yet
                content += e.text();
                if( textPosition.isNull() && ! e.attribute( "x" ).isEmpty() && ! e.attribute( "y" ).isEmpty() )
                {
                    QStringList posX = e.attribute( "x" ).split( ", " );
                    QStringList posY = e.attribute( "y" ).split( ", " );
                    if( posX.count() && posY.count() )
                    {
                        textPosition.setX( parseUnitX( posX.first() ) );
                        textPosition.setY( parseUnitY( posY.first() ) );
                    }
                }
                styleElement = e;
                // this overrides the font of the text element or of previous tspan elements
                // TODO we probably have to create separate shapes per tspan element later
                parseFont( e );
            }
            else if( e.tagName() == "tref" )
            {
                if( e.attribute( "xlink:href" ).isEmpty() )
                    continue;

                QString key = e.attribute( "xlink:href" ).mid( 1 );
                if( ! m_defs.contains(key) )
                {
                    // try to find referenced object in document
                    KoShape * obj = findObject( key );
                    // try to find referenced object in actual group, which is not yet part of document
                    if( ! obj )
                        obj = findObject( key, shapes );
                    if( obj ) 
                        content += dynamic_cast<ArtisticTextShape*>( obj )->text();
                }
                else
                {
                    QDomElement p = m_defs[key];
                    content += p.text();
                }
            }
            else
                continue;

            if( ! e.attribute( "text-anchor" ).isEmpty() )
                anchor = e.attribute( "text-anchor" );
        }

        text->setText( content.simplified() );
        text->setPosition( textPosition );

        if( path )
        {
            if( pathInDocument )
                text->putOnPath( path );
            else
                text->putOnPath( path->absoluteTransformation(0).map( path->outline() ) );

            if( offset > 0.0 )
                text->setStartOffset( offset );
        }
    }
    else
    {
        // a single text line
        textPosition.setX( parseUnitX( b.attribute( "x" ) ) );
        textPosition.setY( parseUnitY( b.attribute( "y" ) ) );

        text = static_cast<ArtisticTextShape*>( createShape( ArtisticTextShapeID ) );
        if( ! text )
            return 0;

        text->setText( b.text().simplified() );
        text->setPosition( textPosition );
    }

    if( ! text )
    {
        removeGraphicContext();
        return 0;
    }

    // first set the font for the right size and offsets
    text->setFont( m_gc.top()->font );
    // adjust position by baseline offset
    if( ! text->isOnPath() )
        text->setPosition( text->position() - QPointF( 0, text->baselineOffset() ) );

    if( anchor == "middle" )
        text->setTextAnchor( ArtisticTextShape::AnchorMiddle );
    else if( anchor == "end" )
        text->setTextAnchor( ArtisticTextShape::AnchorEnd );

    if( !b.attribute("id").isEmpty() )
        text->setName( b.attribute("id") );

    text->applyAbsoluteTransformation( m_gc.top()->matrix );
    text->setZIndex( nextZIndex() );

    // apply the style of the text element
    parseStyle( text, b );
    // apply the style of the last tspan element
    parseStyle( text, styleElement );

    removeGraphicContext();

    return text;
}

KoShape * SvgParser::createObject( const QDomElement &b, const QDomElement &style )
{
    KoShape *obj = 0L;

    addGraphicContext();
    setupTransform( b );
    updateContext( b );

    if( b.tagName() == "rect" )
    {
        double x = parseUnitX( b.attribute( "x" ) );
        double y = parseUnitY( b.attribute( "y" ) );
        double w = parseUnitX( b.attribute( "width" ) );
        double h = parseUnitY( b.attribute( "height" ) );
        bool hasRx = b.hasAttribute( "rx" );
        bool hasRy = b.hasAttribute( "ry" );
        double rx = hasRx ? parseUnitX( b.attribute( "rx" ) ) : 0.0;
        double ry = hasRy ? parseUnitY( b.attribute( "ry" ) ) : 0.0;
        if( hasRx && ! hasRy )
            ry = rx;
        if( ! hasRx && hasRy )
            rx = ry;

        KoRectangleShape * rect = static_cast<KoRectangleShape*>( createShape( KoRectangleShapeId ) );
        if( rect )
        {
            rect->setSize( QSizeF(w,h) );
            rect->setPosition( QPointF(x,y) );
            if( rx >= 0.0 )
                rect->setCornerRadiusX( qMin( 100.0, rx / (0.5 * w) * 100.0 ) );
            rect->setPosition( QPointF(x,y) );
            if( ry >= 0.0 )
                rect->setCornerRadiusY( qMin( 100.0, ry / (0.5 * h) * 100.0 ) );
            obj = rect;
            if( w == 0.0 || h == 0.0 )
                obj->setVisible( false );
        }
    }
    else if( b.tagName() == "ellipse" )
    {
        obj = createShape( KoEllipseShapeId );
        if( obj )
        {
            double rx = parseUnitX( b.attribute( "rx" ) );
            double ry = parseUnitY( b.attribute( "ry" ) );
            double cx = b.attribute( "cx" ).isEmpty() ? 0.0 : parseUnitX( b.attribute( "cx" ) );
            double cy = b.attribute( "cy" ).isEmpty() ? 0.0 : parseUnitY( b.attribute( "cy" ) );
            obj->setSize( QSizeF(2*rx, 2*ry) );
            obj->setPosition( QPointF(cx-rx,cy-ry) );
            if( rx == 0.0 || ry == 0.0 )
                obj->setVisible( false );
        }
    }
    else if( b.tagName() == "circle" )
    {
        obj = createShape( KoEllipseShapeId );
        if( obj )
        {
            double r  = parseUnitXY( b.attribute( "r" ) );
            double cx = b.attribute( "cx" ).isEmpty() ? 0.0 : parseUnitX( b.attribute( "cx" ) );
            double cy = b.attribute( "cy" ).isEmpty() ? 0.0 : parseUnitY( b.attribute( "cy" ) );
            obj->setSize( QSizeF(2*r, 2*r) );
            obj->setPosition( QPointF(cx-r,cy-r) );
            if( r == 0.0 )
                obj->setVisible( false );
        }
    }
    else if( b.tagName() == "line" )
    {
        KoPathShape * path = static_cast<KoPathShape*>( createShape( KoPathShapeId ) );
        if( path )
        {
            double x1 = b.attribute( "x1" ).isEmpty() ? 0.0 : parseUnitX( b.attribute( "x1" ) );
            double y1 = b.attribute( "y1" ).isEmpty() ? 0.0 : parseUnitY( b.attribute( "y1" ) );
            double x2 = b.attribute( "x2" ).isEmpty() ? 0.0 : parseUnitX( b.attribute( "x2" ) );
            double y2 = b.attribute( "y2" ).isEmpty() ? 0.0 : parseUnitY( b.attribute( "y2" ) );
            path->clear();
            path->moveTo( QPointF( x1, y1 ) );
            path->lineTo( QPointF( x2, y2 ) );
            path->normalize();
            obj = path;
        }
    }
    else if( b.tagName() == "polyline" || b.tagName() == "polygon" )
    {
        KoPathShape * path = static_cast<KoPathShape*>( createShape( KoPathShapeId ) );
        if( path )
        {
            path->clear();

            bool bFirst = true;
            QString points = b.attribute( "points" ).simplified();
            points.replace( ',', ' ' );
            points.remove( '\r' );
            points.remove( '\n' );
            QStringList pointList = points.split( ' ' );
            for( QStringList::Iterator it = pointList.begin(); it != pointList.end(); ++it)
            {
                QPointF point;
                point.setX( SvgUtil::fromUserSpace( (*it).toDouble() ) );
                ++it;
                if( it == pointList.end() )
                    break;
                point.setY( SvgUtil::fromUserSpace( (*it).toDouble() ) );
                if( bFirst )
                {
                    path->moveTo( point );
                    bFirst = false;
                }
                else
                    path->lineTo( point );
            }
            if( b.tagName() == "polygon" ) 
                path->close();

            path->setPosition( path->normalize() );

            obj = path;
        }
    }
    else if( b.tagName() == "path" )
    {
        KoPathShape * path = static_cast<KoPathShape*>( createShape( KoPathShapeId ) );
        if( path )
        {
            path->clear();

            KoPathShapeLoader loader( path );
            loader.parseSvg( b.attribute( "d" ), true );
            path->setPosition( path->normalize() );

            QPointF newPosition = QPointF( SvgUtil::fromUserSpace( path->position().x() ),
                                           SvgUtil::fromUserSpace( path->position().y() ) );
            QSizeF newSize = QSizeF( SvgUtil::fromUserSpace( path->size().width() ), 
                                     SvgUtil::fromUserSpace( path->size().height() ) );

            path->setSize( newSize );
            path->setPosition( newPosition );

            obj = path;
        }
    }
    else if( b.tagName() == "image" )
    {
        double x = b.hasAttribute( "x" ) ? parseUnitX( b.attribute( "x" ) ) : 0;
        double y = b.hasAttribute( "x" ) ? parseUnitY( b.attribute( "y" ) ) : 0;
        double w = b.hasAttribute( "width" ) ? parseUnitX( b.attribute( "width" ) ) : 0;
        double h = b.hasAttribute( "height" ) ? parseUnitY( b.attribute( "height" ) ) : 0;

        // zero width of height disables rendering this image (see svg spec)
        if( w == 0.0 || h == 0.0 )
            return 0;
        QString fname = b.attribute("xlink:href");
        QImage img;
        if( parseImage( fname, img ) )
        {
            KoShape * picture = createShape( PICTURESHAPEID );
            KoImageCollection * imageCollection = dynamic_cast<KoImageCollection *>( m_dataCenters["ImageCollection"] );
            
            if( picture && imageCollection )
            {
                // TODO use it already for loading
                KoImageData * data = imageCollection->getImage(img);

                picture->setUserData( data );
                picture->setSize( QSizeF(w,h) );
                picture->setPosition( QPointF(x,y) );

                obj = picture;
            }
        }
    }

    if( ! obj )
    {
        removeGraphicContext();
        return 0;
    }

    obj->applyAbsoluteTransformation( m_gc.top()->matrix );

    if( !style.isNull() )
        parseStyle( obj, style );
    else
        parseStyle( obj, b );

    // handle id
    if( !b.attribute("id").isEmpty() )
        obj->setName( b.attribute("id") );

    removeGraphicContext();

    obj->setZIndex( nextZIndex() );

    return obj;
}

int SvgParser::nextZIndex()
{
    static int zIndex = 0;

    return zIndex++;
}

QString SvgParser::absoluteFilePath( const QString &href, const QString &xmlBase )
{
    QFileInfo info( href );
    if( ! info.isRelative() )
        return href;

    QString baseDir = m_xmlBaseDir;
    if( ! xmlBase.isEmpty() )
        baseDir = absoluteFilePath( xmlBase, QString() );

    QFileInfo pathInfo( QFileInfo( baseDir ).filePath() );

    QString relFile = href;
    while( relFile.startsWith( "../" ) )
    {
        relFile = relFile.mid( 3 );
        pathInfo.setFile( pathInfo.dir(), QString() );
    }

    QString absFile = pathInfo.absolutePath() + '/' + relFile;

    return absFile;
}

KoShape * SvgParser::createShape( const QString &shapeID )
{
    KoShapeFactory * factory = KoShapeRegistry::instance()->get( shapeID );
    if( ! factory )
    {
        kWarning(30514) << "Could not find factory for shape id" << shapeID;
        return 0;
    }

    KoShape * shape = factory->createDefaultShapeAndInit( m_dataCenters );
    if( shape && shape->shapeId().isEmpty() )
        shape->setShapeId( factory->id() );

    // reset tranformation that might come from the default shape
    shape->setTransformation( QMatrix() );

    return shape;
}
