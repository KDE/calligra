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

#ifndef Command_h_
#define Command_h_

#include <GObject.h>
#include <qvector.h>

class GDocument;

class Command
{
protected:
  Command (const QString& n) { name = n; }

public:
  virtual ~Command () {}

  // every subclass has to implement this method
  virtual void execute () = 0;

  // only undoable commands have to implement this
  virtual void unexecute () {}

  QString getName() const { return name; }

private:
  QString name;

};

class ObjectManipCmd : public Command
{
protected:
  ObjectManipCmd (GDocument* doc, const QString& name);
  ObjectManipCmd (GObject* obj, const QString& name);

public:
  ~ObjectManipCmd ();

  void execute ();
  void unexecute ();

protected:
  GDocument *document;
  QVector<GObject> objects;
  QVector<GOState> states;
};

#endif
