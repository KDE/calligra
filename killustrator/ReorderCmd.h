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

#ifndef ReorderCmd_h_
#define ReorderCmd_h_

#include <Command.h>
#include <qvector.h>
#include <qarray.h>

class GDocument;

enum ReorderPosition {
  RP_ToFront, RP_ToBack, RP_ForwardOne, RP_BackwardOne
};

class ReorderCmd : public Command {
public:
  ReorderCmd (GDocument* doc, ReorderPosition pos);
  ~ReorderCmd ();

  void execute ();
  void unexecute ();

private:
  QVector<GObject> objects;
  QArray<unsigned int> oldpos;
  GDocument* document;
  ReorderPosition position;
};

#endif
