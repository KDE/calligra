/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#ifndef XfigImport_h_
#define XfigImport_h_

#include <qintdict.h>
#include <qmap.h>
#include <ImportFilter.h>

class GDocument;
class GObject;
class QColor;
#ifndef __DECCXX
class istream;
#endif

class XfigImport : public ImportFilter {
public:
  XfigImport ();
  ~XfigImport ();

  bool setup (GDocument* doc, const char* format);
  bool importFromFile (GDocument *doc);

private:
  void parseColorObject (istream& fin);
  void parseArc (istream& fin, GDocument* doc);
  void parseEllipse (istream& fin, GDocument* doc);
  void parsePolyline (istream& fin, GDocument* doc);
  void parseSpline (istream& fin, GDocument* doc);
  void parseText (istream& fin, GDocument* doc);
  void parseCompoundObject (istream& fin, GDocument* doc);
  void buildDocument (GDocument *doc);

  void setProperties (GObject* obj, int pen_color, int style, int thickness,
                      int area_fill, int fill_color);

  float fig_resolution;
  int coordinate_system;
  int version;
  QIntDict<QColor> colorTable;
  QMap<int, GObject*> objList;
};

#endif
