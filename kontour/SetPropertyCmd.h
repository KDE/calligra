/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

#ifndef __SetPropertyCmd_h__
#define __SetPropertyCmd_h__

#include "Command.h"
#include "GStyle.h"
#include <qptrvector.h>
#include <qmemarray.h>

namespace Kontour {
class SetPropertyCmd : public Command
{
public:
  SetPropertyCmd(GDocument *aGDoc, const QString &name);
  ~SetPropertyCmd();

  void unexecute();

protected:
  QPtrVector<GObject> objects;
  QMemArray<GStyle> states;
};

class SetOutlineCmd : public SetPropertyCmd
{
public:
  SetOutlineCmd(GDocument *aGDoc, bool b);
  void execute();

private:
  bool outline;
};

class SetOutlineOpacityCmd : public SetPropertyCmd
{
public:
  SetOutlineOpacityCmd(GDocument *aGDoc, int o);
  void execute();

private:
  int opacity;
};

class SetOutlineColorCmd : public SetPropertyCmd
{
public:
  SetOutlineColorCmd(GDocument *aGDoc, const KoColor &c);
  void execute();

private:
  KoColor color;
};

class SetOutlineWidthCmd : public SetPropertyCmd
{
public:
  SetOutlineWidthCmd(GDocument *aGDoc, int w);
  void execute();

private:
  int width;
};

class SetJoinStyleCmd : public SetPropertyCmd
{
public:
  SetJoinStyleCmd(GDocument *aGDoc, KoOutline::Join j);
  void execute();

private:
  KoOutline::Join join;
};

class SetCapStyleCmd : public SetPropertyCmd
{
public:
  SetCapStyleCmd(GDocument *aGDoc, KoOutline::Cap c);
  void execute();

private:
  KoOutline::Cap cap;
};

class SetFillCmd : public SetPropertyCmd
{
public:
  SetFillCmd(GDocument *aGDoc, int f);
  void execute();

private:
  int fill;
};

class SetFillOpacityCmd : public SetPropertyCmd
{
public:
  SetFillOpacityCmd(GDocument *aGDoc, int o);
  void execute();

private:
  int opacity;
};

class SetFillColorCmd : public SetPropertyCmd
{
public:
  SetFillColorCmd(GDocument *aGDoc, const KoColor &c);
  void execute();

private:
  KoColor color;
};
};
using namespace Kontour;

#endif
