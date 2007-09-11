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

#include "SvgGradientHelper.h"

SvgGradientHelper::SvgGradientHelper()
: m_gradient(0), m_bbox(true)
{
}

SvgGradientHelper::~SvgGradientHelper()
{
    delete m_gradient;
}

SvgGradientHelper::SvgGradientHelper( const SvgGradientHelper &other )
: m_gradient(0), m_bbox(true)
{
    m_bbox = other.m_bbox;
    m_gradientTransform = other.m_gradientTransform;
    copyGradient( other.m_gradient );
}

SvgGradientHelper & SvgGradientHelper::operator = ( const SvgGradientHelper & rhs )
{
    if( this == &rhs )
        return *this;

    m_bbox = rhs.m_bbox;
    m_gradientTransform = rhs.m_gradientTransform;
    copyGradient( rhs.m_gradient );

    return *this;
}

void SvgGradientHelper::setBoundboxUnits( bool on )
{
    m_bbox = on;
}

bool SvgGradientHelper::boundboxUnits() const
{
    return m_bbox;
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

    QMatrix matrix;
    matrix.scale( bound.width()/100.0, bound.height()/100.0);

    if( m_gradient )
    {
        switch( m_gradient->type() )
        {
        case QGradient::ConicalGradient:
            {
                QConicalGradient * o = static_cast<QConicalGradient*>( m_gradient );
                QConicalGradient * g = new QConicalGradient();
                g->setStops( m_gradient->stops() );
                g->setAngle( o->angle() );
                g->setCenter( matrix.map( o->center() ) );
                brush = QBrush( *g );
                delete g;
                break;
            }
        case QGradient::LinearGradient:
            {
                QLinearGradient * o = static_cast<QLinearGradient*>( m_gradient );
                QLinearGradient * g = new QLinearGradient();
                g->setStops( m_gradient->stops() );
                g->setStart( matrix.map( o->start() ) );
                g->setFinalStop( matrix.map( o->finalStop() ) );
                brush = QBrush( *g );
                delete g;
                break;
            }
        case QGradient::RadialGradient:
            {
                QRadialGradient * o = static_cast<QRadialGradient*>( m_gradient );
                QRadialGradient * g = new QRadialGradient();
                g->setStops( m_gradient->stops() );
                g->setCenter( matrix.map( o->center() ) );
                g->setFocalPoint( matrix.map( o->focalPoint() ) );
                g->setRadius( matrix.map( QPointF(o->radius(),0.0) ).x() );
                brush = QBrush( *g );
                delete g;
                break;
            }
        default:
            break;
        }
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
