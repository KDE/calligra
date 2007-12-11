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

#include <KLocale>
#include <KDebug>

#include <QtGui/QPen>
#include <QtGui/QPainter>

SimpleTextShape::SimpleTextShape()
    : m_text( i18n( "Simple Text" ) ), m_font( "ComicSans", 20 )
    , m_path(0), m_startOffset(0.0)
{
    setShapeId( SimpleTextShapeID );
    cacheOutlines();
}

SimpleTextShape::~SimpleTextShape()
{
    if( m_path )
        m_path->KoShape::removeDependee( this );
}

void SimpleTextShape::paint(QPainter &painter, const KoViewConverter &converter)
{
    applyConversion( painter, converter );
    painter.setFont( m_font );
    painter.setBrush( background() );
    painter.drawPath( outline() );
}

void SimpleTextShape::paintDecorations(QPainter &painter, const KoViewConverter &converter, const KoCanvasBase *canvas)
{
}

void SimpleTextShape::saveOdf(KoShapeSavingContext & context) const
{
    // TODO
}

bool SimpleTextShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context )
{
    // TODO
    return false;
}

QSizeF SimpleTextShape::size() const
{
    return outline().boundingRect().size();
}

void SimpleTextShape::setSize( const QSizeF & )
{
    // TODO alter font size here ?
}

const QPainterPath SimpleTextShape::outline() const
{
    QPainterPath p = textOutline();
    QRectF bb = p.boundingRect();
    QMatrix m;
    m.translate( -bb.topLeft().x(), -bb.topLeft().y() );

    return m.map( p );
}

QPainterPath SimpleTextShape::textOutline() const
{
    QFontMetricsF metrics( m_font );
    QPainterPath textOutline;
    if( m_path )
    {
        QPainterPath pathOutline = m_path->absoluteTransformation(0).map( m_path->outline() );
        int textLength = m_text.length();
        qreal charPos = m_startOffset * pathOutline.length();
        for( int charIdx = 0; charIdx < textLength; ++charIdx )
        {
            QString actChar( m_text[charIdx] );
            // get the percent value of the actual char position
            qreal t = pathOutline.percentAtLength( charPos );
            if( t >= 1.0 )
                break;
            // get the path point of the given path position
            QPointF pathPoint = pathOutline.pointAtPercent( t );

            t = pathOutline.percentAtLength( charPos + 0.5 * metrics.width( actChar ) );
            // get the angle at the given path position
            if( t >= 1.0 )
                break;
            qreal angle = pathOutline.angleAtPercent( t );

            QMatrix m;
            m.translate( pathPoint.x(), pathPoint.y() );
            m.rotate( angle );
            textOutline.addPath( m.map( m_charOutlines[charIdx] ) );

            charPos += metrics.width( actChar );
        }
    }
    else
    {
        textOutline.addText( QPointF(), m_font, m_text );
        QPointF translate = absolutePosition( KoFlake::TopLeftCorner ) - textOutline.boundingRect().topLeft();
        QMatrix m;
        m.translate( translate.x(), translate.y() );
        textOutline = m.map( textOutline );
    }

    return textOutline;
}

void SimpleTextShape::setText( const QString & text )
{
    if( m_text == text )
        return;

    update();
    m_text = text;
    cacheOutlines();
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
    cacheOutlines();
    updateSizeAndPosition();
    update();
}

QFont SimpleTextShape::font() const
{
    return m_font;
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

bool SimpleTextShape::attach( KoPathShape * path )
{
    if( ! path )
        return false;

    if( path->outline().isEmpty() )
        return false;

    update();
    m_path = path;
    m_path->addDependee( this );
    updateSizeAndPosition();
    update();

    return true;
}

void SimpleTextShape::detach()
{
    update();
    if( m_path )
        m_path->removeDependee( this );
    m_path = 0;
    updateSizeAndPosition();
    update();
}

bool SimpleTextShape::isAttached() const
{
    return (m_path != 0);
}

void SimpleTextShape::updateSizeAndPosition()
{
    if( ! m_path )
        return;

    QRectF bbox = textOutline().boundingRect();
    setAbsolutePosition( bbox.topLeft(), KoFlake::TopLeftCorner );
    setSize( bbox.size() );
}

void SimpleTextShape::cacheOutlines()
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
        update();
        updateSizeAndPosition();
        update();
    }
}
