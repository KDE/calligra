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

#ifndef GGroup_h_
#define GGroup_h_

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>
#include <qwmatrix.h>
#include <qpainter.h>
#include <qdstream.h>
#include <qdict.h>
#include <qlist.h>

#include "Coord.h"
#include "GObject.h"

class GGroup : public GObject {
  Q_OBJECT
public:
  GGroup ();
  GGroup (const list<XmlAttribute>& attribs);
  GGroup (const GGroup& obj);
  ~GGroup ();

  void addObject (GObject* obj);

  virtual void draw (Painter& p, bool withBasePoints = false);
  virtual bool contains (const Coord& p);

  virtual const char* typeName ();

  virtual GObject* copy ();

  virtual void writeToPS (ostream& os);
  virtual void writeToXml (XmlWriter&);

  QListIterator<GObject> getMembers() { 
    return QListIterator<GObject> (members); }

protected slots:
  void propagateProperties ();

protected:
  void calcBoundingBox ();
    
private:
  QList<GObject> members;
};

#endif

