/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 2000 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef SVGExport_h_
#define SVGExport_h_

#include "ExportFilter.h"

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

/**
 * An export filter for SVG.
 */
class SVGExport : public ExportFilter {
public:
  SVGExport ();
  ~SVGExport ();

  bool setup (GDocument *doc, const char* fmt);
  bool exportToFile (GDocument *doc);
private:
  QDomElement exportObject (QDomDocument &document, GObject* obj);
  QDomElement exportPolygon (QDomDocument &document, GPolygon* obj);
  QDomElement exportPolyline (QDomDocument &document, GPolyline* obj);
  QDomElement exportEllipse (QDomDocument &document, GOval* obj);
  QDomElement exportText (QDomDocument &document, GText* obj);
  QDomElement exportTextLine (QDomDocument &document, GText* obj, int line, float xoff,
		       float yoff);
  QDomElement exportCurve (QDomDocument &document, GCurve* obj);
  QDomElement exportBezier (QDomDocument &document, GBezier* obj);
  QDomElement exportGroup (QDomDocument &document, GGroup* obj);
  QDomElement exportPixmap (QDomDocument &document, GPixmap* obj);

  QDomElement addTransformationAttribute (QDomDocument &document, GObject* obj);
  QDomElement addStyleAttribute (QDomDocument &document, GObject* obj);
  QDomElement addTextStyleAttribute (QDomDocument &document, GText* obj);

  QString format;
};

#endif
