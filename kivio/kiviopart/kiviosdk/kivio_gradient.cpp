/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_gradient.h"
#include "kivio_point.h"
/**
 * Default constructor
 *
 * Allocates a new list of colors, and a new list of points for the
 * gradient.  Also, sets the gradient type to none.
 */
KivioGradient::KivioGradient()
    : m_pColors(NULL),
      m_pPoints(NULL)
{
    // Allocate the color list
    m_pColors = new QPtrList<QColor>;
    m_pColors->setAutoDelete(true);

    // Allocate the point list
    m_pPoints = new QPtrList<KivioPoint>;
    m_pPoints->setAutoDelete(true);

    m_gradientType = kgtNone;
}


/**
 * Destructor
 *
 * Deletes the color and point lists.
 */
KivioGradient::~KivioGradient()
{
    if( m_pColors )
    {
        delete m_pColors;
        m_pColors = NULL;
    }

    if( m_pPoints )
    {
        delete m_pPoints;
        m_pPoints = NULL;
    }
}


/**
 * Copy constructor
 *
 * @param source The source @ref KivioGradient to copy from
 *
 * This creates a new KivioGradient as a copy of source.  New colors
 * and points are allocated.
 */
KivioGradient::KivioGradient( const KivioGradient &source )
    : m_pColors(NULL),
      m_pPoints(NULL)
{

    m_gradientType = source.m_gradientType;

    // Duplicate the colors list
    QColor *pColor;
    m_pColors = new QPtrList<QColor>;
    pColor = source.m_pColors->first();
    while( pColor )
    {
        m_pColors->append( new QColor(*pColor) );

        pColor = source.m_pColors->next();
    }


    // Duplicate the point list
    KivioPoint *pPoint;
    m_pPoints = new QPtrList<KivioPoint>;
    pPoint = source.m_pPoints->first();
    while( pPoint )
    {
        m_pPoints->append( new KivioPoint( *pPoint ) );

        pPoint = source.m_pPoints->next();
    }
}


/**
 * Copy all attributes of this into pTarget
 *
 * @param pTarget The object to copy into
 *
 * This will copy all colors/points/everything-else into pTarget.
 */
void KivioGradient::copyInto( KivioGradient *pTarget ) const
{
    if( !pTarget )
        return;

    // Copy the gradient type
    pTarget->m_gradientType = m_gradientType;

    // Delete the old color array if we have one
    if( pTarget->m_pColors )
    {
        delete pTarget->m_pColors;
        pTarget->m_pColors = NULL;
    }

    // Allocate a new color array
    pTarget->m_pColors = new QPtrList<QColor>;
    pTarget->m_pColors->setAutoDelete(true);


    // Copy the colors
    QColor *pColor;
    pColor = m_pColors->first();
    while( pColor )
    {
        pTarget->m_pColors->append( new QColor(*pColor) );

        pColor = m_pColors->next();
    }

    if( pTarget->m_pPoints )
    {
        delete pTarget->m_pPoints;
        pTarget->m_pPoints = NULL;
    }

    pTarget->m_pPoints = new QPtrList<KivioPoint>;
    pTarget->m_pPoints->setAutoDelete(true);

    KivioPoint *pPoint;
    pPoint = m_pPoints->first();
    while( pPoint )
    {
        pTarget->m_pPoints->append( new KivioPoint( *pPoint ) );

        pPoint = m_pPoints->next();
    }
}


/**
 * Load this object from an XML element
 *
 * FIXME: Implement this
 */
bool KivioGradient::loadXML( const QDomElement & )
{
    return false;
}


/**
 * Save this object to an XML element
 *
 * FIXME: Implement this
 */
QDomElement KivioGradient::saveXML( QDomDocument &doc )
{

    return doc.createElement("KivioGradient");
}
