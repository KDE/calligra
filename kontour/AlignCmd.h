/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2002 Igor Jansen (rm@kde.org)

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

#ifndef __AlignCmd_h__
#define __AlignCmd_h__

#include "Command.h"

namespace Kontour {

enum HorizAlignment
{
  HAlign_None, HAlign_Left, HAlign_Center, HAlign_Right
};

enum VertAlignment
{
  VAlign_None, VAlign_Top, VAlign_Center, VAlign_Bottom
};

class AlignCmd : public TransformationCmd
{
public:
  AlignCmd(GDocument *aGDoc, HorizAlignment halign, VertAlignment valign, bool centerToPage = false, bool snapToGrid = false);

  void execute ();

private:
  HorizAlignment horizAlign;
  VertAlignment vertAlign;
  bool center;
  bool snap;
};

};
using namespace Kontour;

#endif
