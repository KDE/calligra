/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#ifndef __Command_h__
#define __Command_h__

#include <kcommand.h>
#include <qptrvector.h>
#include <qmemarray.h>

class QWMatrix;

namespace Kontour {
class GDocument;
class GObject;

class Command : public KNamedCommand
{
public:
  Command(GDocument *aGDoc, const QString &name);
  GDocument *document() const {return mGDoc; }

private:
  GDocument *mGDoc;
};

class TransformationCmd : public Command
{
protected:
  TransformationCmd(GDocument *aGDoc, const QString &name);

public:
  ~TransformationCmd();

  void execute();
  void unexecute();

protected:
  QPtrVector<GObject> objects;
  QMemArray<QWMatrix> states;
};
};
using namespace Kontour;

#endif
