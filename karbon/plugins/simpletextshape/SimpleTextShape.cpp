/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "SimpleTextShape.h"

#include <KoPathShape.h>
#include <KoGlobal.h>

#include <KLocale>

#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtGui/QFont>

#define FROM_PS_SIZE(x) (72.0 / KoGlobal::dpiY() * x)
#define TO_PS_SIZE(x) ( KoGlobal::dpiY() / 72.0 * x)

SimpleTextShape::SimpleTextShape()
    : m_text( i18n( "Simple Text" ) ), m_font( "ComicSans", FROM_PS_SIZE(20) )
    , m_path(0), m_startOffset(0.0), m_baselineOffset(0.0)
    , m_textAnchor( AnchorStart ), m_textCursor( - 1 )
{
    setShapeId( SimpleTextShapeID );
    cacheGlyphOutlines();
    updateSizeAndPosition();
}

SimpleTextShape::~SimpleTextShape()
{
    if( m_path )
        m_path->removeDependee( this );
}

KoShape * SimpleTextShape::cloneShape() const
{
    // TODO implement cloning
    return 0;
}

void SimpleTextShape::paint(QPainter &painter, const KoViewConverter &converter)
{
    applyConversion( painter, converter );
    painter.setFont( m_font );
    painter.setBrush( background() );
    painter.drawPath( outline() );
    if ( m_textCursor > -1 ) {
        if( isOnPath() ) {
        } else {
            QFontMetrics metrics( m_font );
	    int x = 1;
	    if ( m_text.length() == m_textCursor ) {
                x = outline().boundingRect().width();
	    } else if ( m_textCursor > 0 ) {
                int w = metrics.width( m_text.left( m_textCursor + 1 ) );
	        int w2 = metrics.charWidth( m_text, m_textCursor );
	        x = w - w2;
	    }
            painter.fillRect( x - 1, 1, 1, size().height() - 1, Qt::black );
	}
    }
}

void SimpleTextShape::paintDecorations(QPainter &/*painter*/, const KoViewConverter &/*converter*/, const KoCanvasBase * /*canvas*/)
{
}

void SimpleTextShape::saveOdf(KoShapeSavingContext &/*context*/) const
{
    // TODO
}

bool SimpleTextShape::loadOdf( const KoXmlElement & /*element*/, KoShapeLoadingContext &/*context*/ )
{
    // TODO
    return false;
}

QSizeF SimpleTextShape::size() const
{
    return outline().boundingRect().size();
}

void SimpleTextShape::setSize( const QSizeF &newSize )
{
    QSizeF oldSize = size();
    if ( !oldSize.isNull() ) {
        double zoomX = newSize.width() / oldSize.width(); 
        double zoomY = newSize.height() / oldSize.height(); 
        QMatrix matrix( zoomX, 0, 0, zoomY, 0, 0 );

        update();
        applyTransformation( matrix );
        update();
    }
}

const QPainterPath SimpleTextShape::outline() const
{
    return m_outline;
}

void SimpleTextShape::createOutline()
{
    m_outline = QPainterPath();

    if( isOnPath() )
    {
        QFontMetricsF metrics( m_font );
        int textLength = m_text.length();
        qreal charPos = m_startOffset * m_baseline.length();

        qreal anchorPoint = 0.0;
        if( m_textAnchor == AnchorMiddle )
            anchorPoint = 0.5 * metrics.width( m_text );
        else if( m_textAnchor == AnchorEnd )
            anchorPoint = metrics.width( m_text );

        charPos -= anchorPoint;

        QPointF pathPoint;

        for( int charIdx = 0; charIdx < textLength; ++charIdx )
        {
            QString actChar( m_text[charIdx] );
            // get the percent value of the actual char position
            qreal t = m_baseline.percentAtLength( charPos );
            if( t >= 1.0 )
                break;

            if( t >= 0.0 )
            {
                // get the path point of the given path position
                pathPoint = m_baseline.pointAtPercent( t );

                t = m_baseline.percentAtLength( charPos + 0.5 * metrics.width( actChar ) );
            }

            charPos += metrics.width( actChar );
            if( t < 0.0 )
                continue;

            // get the angle at the given path position
            if( t >= 1.0 )
                break;
            qreal angle = m_baseline.angleAtPercent( t );

            QMatrix m;
            m.translate( pathPoint.x(), pathPoint.y() );
            m.rotate( angle );
            m_outline.addPath( m.map( m_charOutlines[charIdx] ) );
        }
    }
    else
    {
        m_outline.addText( QPointF(), m_font, m_text );
    }
}

void SimpleTextShape::setText( const QString & text )
{
    if( m_text == text )
        return;

    update();
    m_text = text;
    cacheGlyphOutlines();
    updateSizeAndPosition();
    update();
}

QString SimpleTextShape::text() const
{
    return m_text;
}

void SimpleTextShape::setFont( const QFont & font )
{
    if( m_font == font )
        return;

    update();
    m_font = font;
    m_font.setPointSizeF( FROM_PS_SIZE( font.pointSizeF() ) );
    cacheGlyphOutlines();
    updateSizeAndPosition();
    update();
}

QFont SimpleTextShape::font() const
{
    QFont font( m_font );
    font.setPointSizeF( TO_PS_SIZE( font.pointSizeF() ) );
    return font;
}

void SimpleTextShape::setStartOffset( qreal offset )
{
    if( m_startOffset == offset )
        return;

    update();
    m_startOffset = offset;
    m_startOffset = qMin( 1.0, m_startOffset );
    m_startOffset = qMax( 0.0, m_startOffset );
    updateSizeAndPosition();
    update();
}

qreal SimpleTextShape::startOffset() const
{
    return m_startOffset;
}

qreal SimpleTextShape::baselineOffset() const
{
    return m_baselineOffset;
}

void SimpleTextShape::setTextAnchor( TextAnchor anchor )
{
    m_textAnchor = anchor;
    update();
    updateSizeAndPosition();
    update();
}

SimpleTextShape::TextAnchor SimpleTextShape::textAnchor() const
{
    return m_textAnchor;
}

bool SimpleTextShape::putOnPath( KoPathShape * path )
{
    if( ! path )
        return false;

    if( path->outline().isEmpty() )
        return false;

    update();

    m_path = path;
    m_path->addDependee( this );
    // use the paths outline converted to document coordinates as the baseline
    m_baseline = m_path->absoluteTransformation(0).map( m_path->outline() );

    updateSizeAndPosition();
    update();

    return true;
}

bool SimpleTextShape::putOnPath( const QPainterPath &path )
{
    if( path.isEmpty() )
        return false;

    update();
    if( m_path )
        m_path->removeDependee( this );
    m_path = 0;
    m_baseline = path;
    updateSizeAndPosition();
    update();

    return true;
}

void SimpleTextShape::removeFromPath()
{
    update();
    if( m_path )
        m_path->removeDependee( this );
    m_path = 0;
    m_baseline = QPainterPath();
    updateSizeAndPosition();
    update();
}

bool SimpleTextShape::isOnPath() const
{
    return (m_path != 0 || ! m_baseline.isEmpty() );
}

SimpleTextShape::LayoutMode SimpleTextShape::layout() const
{
    if( m_path )
        return OnPathShape;
    else if( ! m_baseline.isEmpty() )
        return OnPath;
    else
        return Straight;
}


QPainterPath SimpleTextShape::baseline() const
{
    return m_baseline;
}

const KoPathShape * SimpleTextShape::baselineShape() const
{
    return m_path;
}

void SimpleTextShape::enableTextCursor( bool enable )
{
    if ( enable ) {
        setTextCursorInternal( text().length() );
    } else {
        setTextCursorInternal( -1 );
    }
}

void SimpleTextShape::setTextCursor( int textCursor )
{
    if ( m_textCursor == textCursor || textCursor < 0 || textCursor > m_text.length() )
        return;

    setTextCursorInternal( textCursor);
}

void SimpleTextShape::setTextCursorInternal( int textCursor )
{
    update();
    m_textCursor = textCursor;
    update();
}

void SimpleTextShape::removeFromTextCursor( unsigned int nr )
{
    if ( m_textCursor > 0 && m_textCursor >= nr ) {
        update();
	m_textCursor -= nr;
        m_text.remove( m_textCursor, nr );
        cacheGlyphOutlines();
        updateSizeAndPosition();
        update();
    }
}

void SimpleTextShape::addToTextCursor( const QString &str )
{
    if ( !str.isEmpty() && m_textCursor > -1 ) {
        int oldTextCursor = m_textCursor;
        for ( int i = 0;i < str.length();i++ ) {
            if ( str[i].isPrint() )
                m_text.insert( m_textCursor++, str[i] );
        }
        if ( oldTextCursor < m_textCursor ) {
            update();
            cacheGlyphOutlines();
            updateSizeAndPosition();
            update();
        }
    }
}

void SimpleTextShape::updateSizeAndPosition()
{
    // the actual position
//    QPointF position = absolutePosition( KoFlake::TopLeftCorner );

    createOutline();

    QRectF bbox = m_outline.boundingRect();

    if( isOnPath() )
    {
        // the outline position is in document coordinates
        // so we adjust our position
        setAbsolutePosition( bbox.topLeft(), KoFlake::TopLeftCorner );
    }
    else
    {
        // the text outlines baseline is at 0,0
        m_baselineOffset = -bbox.topLeft().y();
    }

    setSize( bbox.size() );

    // map outline to shape coordinate system
    QMatrix normalizeMatrix;
    normalizeMatrix.translate( -bbox.topLeft().x(), -bbox.topLeft().y() );
    m_outline = normalizeMatrix.map( m_outline );
}

void SimpleTextShape::cacheGlyphOutlines()
{
    m_charOutlines.clear();

    int textLength = m_text.length();
    for( int charIdx = 0; charIdx < textLength; ++charIdx )
    {
        QString actChar( m_text[charIdx] );
        QPainterPath charOutline;
        charOutline.addText( QPointF(), m_font, actChar );
        m_charOutlines.append( charOutline );
    }
}

void SimpleTextShape::notifyShapeChanged( KoShape * shape, ChangeType type )
{
    if( shape == m_path )
    {
        if( type == KoShape::Deleted )
        {
            m_path = 0;
        }
        else
        {
            update();
            // use the paths outline converted to document coordinates as the baseline
            m_baseline = m_path->absoluteTransformation(0).map( m_path->outline() );
            updateSizeAndPosition();
            update();
        }
    }
}
