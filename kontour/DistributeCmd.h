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

#ifndef DistributeCmd_h_
#define DistributeCmd_h_

#include <Command.h>

class GDocument;

enum HorizDistribution {
  HDistrib_None, HDistrib_Left, HDistrib_Center,
  HDistrib_Distance, HDistrib_Right
};

enum VertDistribution {
  VDistrib_None, VDistrib_Top, VDistrib_Center,
  VDistrib_Distance, VDistrib_Bottom
};

enum DistributionMode {
  DMode_AtSelection, DMode_AtPage
};

class DistributeCmd : public ObjectManipCmd {
public:
  DistributeCmd (GDocument* doc, HorizDistribution hdistrib,
                 VertDistribution vdistrib, DistributionMode dmode);

  void execute ();

private:
  HorizDistribution hDistrib;
  VertDistribution vDistrib;
  DistributionMode mode;
};

#endif
