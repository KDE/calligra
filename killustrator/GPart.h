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

#ifndef GPart_h_
#define GPart_h_

#include <Coord.h>
#include <GObject.h>

class KIllustratorFrame;
class KIllustratorChild;
class KIllustratorView;
class QPainter;

class GPart : public GObject
{
  Q_OBJECT
   private:
      GPart ():GObject(0) {cout<<"GPart ctor"<<endl; exit(1);};
public:
  GPart (GDocument* doc);
  GPart (GDocument* doc, KIllustratorChild *c);
  GPart (GDocument* doc, KIllustratorDocument *kidoc, const QDomElement &element);
  GPart (const GPart& p);

  ~GPart ();

  virtual void draw (QPainter& p, bool withBasePoints = false,
                     bool outline = false, bool withEditMarks=true);

  virtual QString typeName () const;

  virtual GObject* copy ();
  //virtual GObject* clone (const QDomElement &element);

  virtual QDomElement writeToXml (QDomDocument &document);

  KIllustratorChild *getChild () { return child; }
  
  virtual void activate(KIllustratorView *view);
  virtual void deactivate();

protected:
  void calcBoundingBox ();

private:
  KIllustratorChild *child;
  QRect initialGeom, oldGeom;
};

#endif
