/* This file is part of the KDE project
   Copyright (C) 2001 Rob Buis <rwlbuis@wanadoo.nl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef SVGEXPORT_TEST_H
#define SVGEXPORT_TEST_H

#include <koFilter.h>
#include <qlist.h>

class QDomElement;
class Coord;
class SVGExport : public KoFilter {

    Q_OBJECT

public:
    SVGExport( KoFilter *parent, const char *name );
    virtual ~SVGExport();
    virtual bool filter( const QString &fileIn, const QString &fileOut,
                         const QString &from,   const QString &to,
                         const QString &config=QString::null );	
private:
  void exportDocument( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportPage( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportLayout( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportLayer( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportRect( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportPolygon( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportGObject( QDomElement &docNodeOut, const QDomElement &docNodeIn,
                      bool bClosed = true );
  void exportTransform( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportPolyline( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportEllipse( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportBezier( QDomElement &docNodeOut, const QDomElement &docNodeIn );  
  void exportCurve( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  QList<Coord> getSegments( const QDomElement &docNodeIn );
  QList<Coord> getPoints( const QDomElement &docNodeIn );
  Coord *createPoint( const QDomElement &docNodeIn );
  void exportGroup( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportPixmap( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportText( QDomElement &docNodeOut, const QDomElement &docNodeIn );
  void exportTextLine( QDomElement &docNodeOut, const QDomElement &docNodeIn, const QString &text,
		       QFont &font, float xoff, float yoff );
  void addTextStyleAttribute( QDomElement &element, 
                              const QDomElement &docNodeIn, QFont &font );
};

#endif

