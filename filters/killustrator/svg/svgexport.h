/* This file is part of the KDE project
   Copyright (C) 2000 Kai-Uwe Sattler <kus@iti.cs.uni-magdeburg.de>
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

class QCString;
class QString;
class KoDocument;
class GPolygon;
class GObject;
class GPolyline;
class GOval;
class GText;
class GBezier;
class GCurve;
class GGroup;
class GPixmap;
class QDomElement;
class QDomDocumentFragment;
class SVGExport : public KoFilter {

    Q_OBJECT

public:
    SVGExport( KoFilter *parent, const char*name );
    virtual ~SVGExport();
    /** do the filtering :) */
    virtual bool filterExport( const QString &file, KoDocument * document,
                               const QString &from, const QString &to,
                               const QString &config=QString::null );
				
private:
  QDomDocumentFragment exportObject( QDomDocument &document, GObject* obj );
  QDomDocumentFragment exportPolygon( QDomDocument &document, GPolygon* obj );
  QDomDocumentFragment exportPolyline( QDomDocument &document, GPolyline* obj );
  QDomDocumentFragment exportEllipse( QDomDocument &document, GOval* obj );
  QDomDocumentFragment exportText( QDomDocument &document, GText* obj );
  QDomDocumentFragment exportTextLine( QDomDocument &document, GText* obj, 
		                       int line, float xoff, float yoff );
  QDomDocumentFragment exportCurve( QDomDocument &document, GCurve* obj );
  QDomDocumentFragment exportBezier( QDomDocument &document, GBezier* obj );
  QDomDocumentFragment exportGroup( QDomDocument &document, GGroup* obj );
  QDomDocumentFragment exportPixmap( QDomDocument &document, GPixmap* obj );
 
  void addTransformationAttribute( QDomElement &element, GObject* obj );
  void addStyleAttribute( QDomElement &element, GObject* obj );
  void addTextStyleAttribute( QDomElement &element, GText* obj );
 
  QString format;
};
#endif

