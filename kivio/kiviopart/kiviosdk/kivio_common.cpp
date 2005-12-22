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
#include "kivio_common.h"
#include "kivio_connector_point.h"
#include <kdebug.h>
#include <qstringlist.h>
#include <math.h>
#include <KoPoint.h>
#include <KoRect.h>

/**
 * Read a floating point value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read a floating point attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
float XmlReadFloat( const QDomElement &e, const QString &att, const float &def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;
    
    // Read the attribute
    QString val = e.attribute( att );
    bool ok=false;
    
    // Make sure it is a floating point value.  If not, return the default
    float fVal = val.toFloat( &ok );
    if( !ok )
    {
	kdDebug(43000) << "Invalid XML-value read for " << att.ascii() << ", expected float\n" << endl;
	return 1.0;
    }
    
    // Return the value
    return fVal;
}


/**
 * Write a floating point value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write a floating point value to a @ref QDomElement.
 */
void XmlWriteFloat( QDomElement &e, const QString &att, const float &val )
{
    e.setAttribute( att, (double)val );
}


/**
 * Read an int value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read an int attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
int XmlReadInt( const QDomElement &e, const QString &att, const int &def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att, "1" );
    bool ok=false;

    // Make sure it is a floating point value.  If not, return the default
    int iVal = val.toInt( &ok );
    if( !ok )
    {
       kdDebug(43000) << "Invalid XML-value read for " << att << " expected int\n" << endl;
	  return 1;
    }

    // Return the value
    return iVal;
}


/**
 * Write a int value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write an int value to a @ref QDomElement.
 */
void XmlWriteInt( QDomElement &e, const QString &att, const int &val )
{
    e.setAttribute( att, (int)val );
}


/**
 * Read an uint value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read an uint attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
uint XmlReadUInt( const QDomElement &e, const QString &att, const uint &def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att, "1" );
    bool ok=false;

    // Make sure it is a floating point value.  If not, return the default
    uint iVal = val.toUInt( &ok );
    if( !ok )
    {
       kdDebug(43000) << "Invalid XML-value read for " << att.ascii() << ", expected uint\n" << endl;
        return 1;
    }

    // Return the value
    return iVal;
}


/**
 * Write an uint value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write an uint value to a @ref QDomElement.
 */
void XmlWriteUInt( QDomElement &e, const QString &att, const uint &val )
{
    e.setAttribute( att, (uint)val );
}


/**
 * Read a @ref QString from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read a QString attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
QString XmlReadString( const QDomElement &e, const QString &att, const QString &def )
{
    // Check if the attribute exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return QString(def);
    // Otherwise return the attribute
    else return e.attribute( att );
}


/**
 * Write a QString to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write a QString to a @ref QDomElement.
 */
void XmlWriteString( QDomElement &e,  const QString &att,  const QString &val )
{
    e.setAttribute( att, val );
}


/**
 * Read a QColor value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read a QColor attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
QColor XmlReadColor( const QDomElement &e, const QString &att, const QColor &def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att, "1" );
    bool ok=false;
    QColor newColor;

    if( val.contains("#") ) // Is it #RRGGBB format?
    {
        newColor.setNamedColor(val);
        return newColor;
    }

    // Otherwise it is a #xxxxxxxx color (rgb format)
    // Make sure it is a uint value.  If not, return the default
    uint iVal = val.toUInt( &ok );
    if( !ok )
    {
       kdDebug(43000) << "Invalid XML-value read for " << att.ascii() << ", expected QColor" << endl;
        return 1;
    }

    // Return the value
    return QColor(iVal);
}


/**
 * Write a QColor value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write a QColor value to a @ref QDomElement.
 */
void XmlWriteColor( QDomElement &e, const QString &att, const QColor &val )
{
    // Write it out in #RRGGBB format
    e.setAttribute( att, val.name() );
}


/**
 * Read a double value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read a double attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
double XmlReadDouble( const QDomElement &e, const QString &att, const double &def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att, "1.0" );
    bool ok=false;

    // Make sure it is a floating point value.  If not, return the default
    double dVal = val.toDouble( &ok );
    if( !ok )
    {
       kdDebug(43000) << "Invalid XML-value read for ," << att.ascii() << " expected double" << endl;
        return 1.0;
    }

    // Return the value
    return dVal;
}


/**
 * Write a double value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write a double value to a @ref QDomElement.
 */
void XmlWriteDouble( QDomElement &e, const QString &att, const double &val )
{
    e.setAttribute( att, (double)val );
}


#define WHICH_QUAD( vertex, hitPos ) \
	( (vertex.x() > hitPos->x()) ? ((vertex.y() > hitPos->y()) ? 1 : 4 ) : ((vertex.y() > hitPos->y())?2:3))

#define X_INTERCEPT( point1, point2, hitY ) \
	(point2.x() - (((point2.y()-hitY)*(point1.x()-point2.x()))/(point1.y()-point2.y())))

/**
 * Determines if a point is inside a given polygon
 * @param points An array of points representing the polygon
 * @param numPoints The number of points in the array
 * @param hitPos The point we are to check
 *
 * Code taken from Game Developer magazine page 22, January 1999 issue
 * Explaination:
 *
 * A better strategy is to divide the polygon into quadrants centered on the test point.
 * Start at the first vertex in the polygon and set a counter to 0.  Anytime an edge crosses
 * from one quadrant to the next, add one to the counter if it crosses clockwise around the
 * test point and subtract one if it crosses counter-clockwise.  If the edge crosses diagonally
 * across two quadrants, you need to determine which side of the test point it crossed, and then
 * either add or subtract 2.
 *
 * Quad layout:
 *   1 2
 *   4 3
 */
bool PointInPoly( KoPoint *points, int numPoints, KoPoint *hitPos )
{
	int edge,  next;
	int quad, next_quad, delta, total;

	edge = 0;

	quad = WHICH_QUAD( points[ edge ], hitPos );
	total = 0; // count of absolute sectors crossed

	// Loop through all the vertices
	do {
		next = (edge + 1) % numPoints;
		next_quad = WHICH_QUAD( points[ next ], hitPos );

		// Calculate how many quads have been crossed
		delta = next_quad - quad;

		// Special case to handle crossings of more than one quad
		switch( delta )
		{
			case 2:		// If we crossed the middle, figure out if it was clockwise or counter clockwise
			case -2:	// Use the X-position at the hit point to determine which way around
				if( X_INTERCEPT( points[edge], points[next], hitPos->y() ) > hitPos->x() )
					delta = -delta;
				break;

			case 3:		// Moving 3 quads is like moving back 1
				delta = -1;
				break;

			case -3:	// Moving back 3 is like moving forward 1
				delta = 1;
				break;
		}

		// Add in the delta
		total += delta;
		quad = next_quad;
		edge = next;
	} while( edge != 0 );
	

	// After everything, if the total is 4, then we are inside
	if((total==4) || (total==-4))
		return true;
	else
		return false;
}

KoRect XmlReadRect( const QDomElement &e, const QString &att, const KoRect &def )
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att );

    if (val.find("[") == 0 && val.find("]") == (int)val.length()-1) {
      val.remove(0,1);
      val.remove(val.length()-1,1);
      QStringList vlist = QStringList::split(",",val);
      if (vlist.count() == 4) {
        bool allOk = true;
        bool ok = false;

        double x = vlist[0].toDouble(&ok);
        allOk = allOk & ok;

        double y = vlist[1].toDouble(&ok);
        allOk = allOk & ok;

        double w = vlist[2].toDouble(&ok);
        allOk = allOk & ok;

        double h = vlist[3].toDouble(&ok);
        allOk = allOk & ok;

        if (allOk)
          return KoRect(x, y, w, h);
      }
    }

    return def;
}

void  XmlWriteRect( QDomElement &e, const QString &att, const KoRect &val )
{
    e.setAttribute( att, QString("[%1,%2,%3,%4]").arg(val.x()).arg(val.y()).arg(val.width()).arg(val.height()) );
}


float shortestDistance( KivioConnectorPoint *pStart, KivioConnectorPoint *pEnd, KivioConnectorPoint *q )
{
   float uX, uY;
   float pqX, pqY;

   uX = pStart->x() - pEnd->x();
   uY = pStart->y() - pEnd->y();

   pqX = pStart->x() - q->x();
   pqY = pStart->y() - q->y();

   float magTop = fabs(pqX*uY - (pqY*uX));

   float magU = sqrt( uX*uX + uY*uY );
   
   if( magU == 0.0f )
   {
      kdDebug(43000) << "shortestDistance() - SERIOUS ERROR: magU is 0.0f!\n";
      return 10.0f;
   }

   return magTop / magU;
}
