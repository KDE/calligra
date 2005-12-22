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
#ifndef KIVIO_COMMON_H
#define KIVIO_COMMON_H

#include <qdom.h>
#include <qcolor.h>

class KivioConnectorPoint;
class KoPoint;
class KoRect;

extern "C" {

   KoRect XmlReadRect( const QDomElement &, const QString &, const KoRect & );
   void  XmlWriteRect( QDomElement &, const QString &, const KoRect & );
   
   QColor XmlReadColor( const QDomElement &, const QString &, const QColor & );
   void  XmlWriteColor( QDomElement &, const QString &, const QColor & );
   
   int XmlReadInt( const QDomElement &, const QString &, const int & );
   void  XmlWriteInt( QDomElement &, const QString &, const int & );
   
   uint XmlReadUInt( const QDomElement &, const QString &, const uint & );
   void  XmlWriteUInt( QDomElement &, const QString &, const uint & );
   
   double XmlReadDouble( const QDomElement &, const QString &, const double & );
   void  XmlWriteDouble( QDomElement &, const QString &, const double & );
   
   float XmlReadFloat( const QDomElement &, const QString &, const float & );
   void  XmlWriteFloat( QDomElement &, const QString &, const float & );
   
   QString XmlReadString( const QDomElement &, const QString &, const QString & );
   void    XmlWriteString( QDomElement &,  const QString &, const QString & );
   
   bool PointInPoly( KoPoint *points, int numPoints, KoPoint *hitPos );
   
   float shortestDistance( KivioConnectorPoint *pStart, KivioConnectorPoint *pEnd, KivioConnectorPoint *q );

}

#endif


