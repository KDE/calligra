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
#include "kivio_point.h"
#include "kivio_common.h"

#include <kdebug.h>


/*
 * Names for the different point types. invalid
 * and last are not used for any real point type, 
 * and only serve as bounds checking devices.
 */
static const char *KivioPointTypeNames[]={
    "invalid", "normal", "bezier", "arc", "last"
};


/**
 * Constructor
 *
 * Sets this point to (0,0) and of type normal.
 */
KivioPoint::KivioPoint()
{
    m_x = 0.0f;
    m_y = 0.0f;
    m_pointType = kptNormal;
}


/**
 * Copy constructor
 *
 * @param copy The KivioPoint to make a copy of.
 *
 * Copies copy into this object.
 */
KivioPoint::KivioPoint( const KivioPoint &copy )
{
    m_x = copy.m_x;
    m_y = copy.m_y;
    m_pointType = copy.m_pointType;
}


/**
 * Alternate constructor.
 *
 * @param newX The x value of the point
 * @param newY The y value of the point
 * @param pt The point type
 *
 * Creates a new point with values.
 */
KivioPoint::KivioPoint( double newX, double newY, KivioPointType pt )
{
    m_x = newX;
    m_y = newY;
    m_pointType = pt;
}


/**
 * Destructor
 */
KivioPoint::~KivioPoint()
{
}


/**
 * Copies this object's data into pTarget
 *
 * @param pTarget The destination of the copy
 */
void KivioPoint::copyInto( KivioPoint *pTarget ) const
{
    if( !pTarget )
        return;

    pTarget->m_x = m_x;
    pTarget->m_y = m_y;
    pTarget->m_pointType = m_pointType;
}


/**
 * Figure out the KivioPointType from a string
 *
 * @param str The string to search with
 *
 * This will figure out the KivioPointType from a string.  For example,
 * "normal" will return kptNormal.
 */
KivioPoint::KivioPointType KivioPoint::pointTypeFromString( const QString &str )
{
    int i;
    
    // Iterate through all the possible enums
    for( i=(int)kptNone+1; i<(int)kptLast; i++ )
    {
        // If we find it, return it
        if( str.compare( KivioPointTypeNames[i] )==0 )
        {
            return (KivioPointType)i;
        }
    }
    
    // Otherwise return an invalid type
    return kptNone;
}



/**
 * Load this object from an XML element
 *
 * @param e The element to load from
 * @returns true on success, false on failure.
 */
bool KivioPoint::loadXML( const QDomElement &e )
{
    if( e.tagName().compare( "KivioPoint" ) != 0 )
    {
       kdDebug(43000) << "Attempted to load KivioPoint from non-KivioPoint element" << endl;
        return false;
    }

    m_x = XmlReadFloat( e, "x", 1.0f );
    m_y = XmlReadFloat( e, "y", 1.0f );
    m_pointType = (KivioPointType)pointTypeFromString( XmlReadString( e, "type", "normal" ) );
    

    return true;
}



/**
 * Save this object to an XML element
 *
 * @param doc The document we are saving to
 * @returns QDomElement
 */
QDomElement KivioPoint::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioPoint");
    

    XmlWriteFloat( e, QString("x"), m_x );
    XmlWriteFloat( e, QString("y"), m_y );
    XmlWriteString( e, QString("type"), QString(KivioPointTypeNames[m_pointType]) );
    return e;
}
