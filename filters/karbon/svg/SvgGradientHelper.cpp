/* This file is part of the KDE project
 * Copyright (C) 2007,2009 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgGradientHelper.h"

SvgGradientHelper::SvgGradientHelper()
: m_gradient(0), m_gradientUnits(ObjectBoundingBox)
{
}

SvgGradientHelper::~SvgGradientHelper()
{
    delete m_gradient;
}

SvgGradientHelper::SvgGradientHelper( const SvgGradientHelper &other )
: m_gradient(0), m_gradientUnits(ObjectBoundingBox)
{
    m_gradientUnits = other.m_gradientUnits;
    m_gradientTransform = other.m_gradientTransform;
    copyGradient( other.m_gradient );
}

SvgGradientHelper & SvgGradientHelper::operator = ( const SvgGradientHelper & rhs )
{
    if( this == &rhs )
        return *this;

    m_gradientUnits = rhs.m_gradientUnits;
    m_gradientTransform = rhs.m_gradientTransform;
    copyGradient( rhs.m_gradient );

    return *this;
}

void SvgGradientHelper::setGradientUnits( Units units )
{
    m_gradientUnits = units;
}

SvgGradientHelper::Units SvgGradientHelper::gradientUnits() const
{
    return m_gradientUnits;
}


QGradient * SvgGradientHelper::gradient()
{
    return m_gradient;
}

void SvgGradientHelper::setGradient( QGradient * g )
{
    delete m_gradient;
    m_gradient = g;
}

void SvgGradientHelper::copyGradient( QGradient * other )
{
    delete m_gradient;
    m_gradient = 0;
    if( other )
    {
        switch( other->type() )
        {
        case QGradient::ConicalGradient:
            {
                QConicalGradient * o = static_cast<QConicalGradient*>( other );
                QConicalGradient * g = new QConicalGradient();
                g->setAngle( o->angle() );
                g->setCenter( o->center() );
                m_gradient = g;
                break;
            }
        case QGradient::LinearGradient:
            {
                QLinearGradient * o = static_cast<QLinearGradient*>( other );
                QLinearGradient * g = new QLinearGradient();
                g->setStart( o->start() );
                g->setFinalStop( o->finalStop() );
                m_gradient = g;
                break;
            }
        case QGradient::RadialGradient:
            {
                QRadialGradient * o = static_cast<QRadialGradient*>( other );
                QRadialGradient * g = new QRadialGradient();
                g->setCenter( o->center() );
                g->setFocalPoint( o->focalPoint() );
                g->setRadius( o->radius() );
                m_gradient = g;
                break;
            }
        default:
            break;
        }
        m_gradient->setStops( other->stops() );
    }
}

QBrush SvgGradientHelper::adjustedFill( const QRectF &bound )
{
    QBrush brush;

    QGradient * g = adjustedGradient( bound );
    if( g )
    {
        brush = QBrush( *g );
        delete g;
    }

    return brush;
}

QMatrix SvgGradientHelper::transform() const
{
    return m_gradientTransform;
}

void SvgGradientHelper::setTransform( const QMatrix &transform )
{
    m_gradientTransform = transform;
}

QGradient * SvgGradientHelper::adjustedGradient( const QRectF &bound ) const
{
    QMatrix matrix;
    matrix.scale( 0.01 * bound.width(), 0.01 * bound.height() );

    if( ! m_gradient )
        return 0;

    switch( m_gradient->type() )
    {
    case QGradient::ConicalGradient:
        {
            QConicalGradient * o = static_cast<QConicalGradient*>( m_gradient );
            QConicalGradient * g = new QConicalGradient();
            g->setStops( m_gradient->stops() );
            g->setAngle( o->angle() );
            g->setCenter( matrix.map( o->center() ) );
            return g;
        }
    case QGradient::LinearGradient:
        {
            QLinearGradient * o = static_cast<QLinearGradient*>( m_gradient );
            QLinearGradient * g = new QLinearGradient();
            g->setStops( m_gradient->stops() );
            g->setStart( matrix.map( o->start() ) );
            g->setFinalStop( matrix.map( o->finalStop() ) );
            return g;
        }
    case QGradient::RadialGradient:
        {
            QRadialGradient * o = static_cast<QRadialGradient*>( m_gradient );
            QRadialGradient * g = new QRadialGradient();
            g->setStops( m_gradient->stops() );
            g->setCenter( matrix.map( o->center() ) );
            g->setFocalPoint( matrix.map( o->focalPoint() ) );
            g->setRadius( matrix.map( QPointF(o->radius(),0.0) ).x() );
            return g;
        }
    default:
        return 0;
    }
}
