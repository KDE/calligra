/*
    Copyright (C) 1998 Kai-Uwe Sattler <kus@iti.cs.uni-magdeburg.de>
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>
    Copyright (C) 2003, Rob Buis <buis@kde.org>
    This file is part of the KDE project

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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION
*/

#ifndef XFIGIMPORT_H
#define XFIGIMPORT_H

#include <qintdict.h>
#include <qvaluelist.h>
#include <iostream.h>

class GDocument;
class GObject;
class QColor;

#include <KoFilter.h>
#include <qobject.h>
#include <qstring.h>

class XFIGImport : public KoFilter
{
    Q_OBJECT

public:
    XFIGImport( KoFilter *parent, const char *name );
    virtual ~XFIGImport();

    virtual bool filterImport( const QString &file, KoDocument *,
                               const QString &from, const QString &to,
                               const QString &config=QString::null );

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

  // An object and the depth. Used for sorting objects
  // in the object list
  struct GObjectListItem
  {
      GObjectListItem() : object(0L) {} // for QValueList

      GObjectListItem( int d, GObject * obj ) :
          object(obj), depth(d) {}

      GObject * object;
      int depth;
      bool operator < (const GObjectListItem & item ) const
      {
          // We want to sort by decreasing depths
          return depth > item.depth;
      }
      bool operator == (const GObjectListItem & item ) const
      {
          return depth == item.depth;
      }
  };
  QValueList<GObjectListItem> objList;
};

#endif
