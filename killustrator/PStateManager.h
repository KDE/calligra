/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#ifndef PStateManager_h_
#define PStateManager_h_

#include <units.h>

class PStateManager
{
protected:
  PStateManager ();

public:
  static PStateManager* instance ();

  void readDefaultSettings ();

  MeasurementUnit defaultMeasurementUnit () const {return defaultUnit;};
  float smallStepSize () const                    {return smallStep;};
  float bigStepSize () const                      {return bigStep;};
  float duplicateXOffset () const                 {return dupXOff;};
  float duplicateYOffset () const                 {return dupYOff;};
  bool showSplashScreen () const                  {return showSplash;};

  void setDuplicateOffsets (float x, float y);
  void setDefaultMeasurementUnit (MeasurementUnit unit);
  void setStepSizes (float small, float big);

//signals:
//  void settingsChanged ();

//public slots:
  void saveDefaultSettings ();

private:
  static PStateManager* managerInstance;
  MeasurementUnit defaultUnit;
  float dupXOff, dupYOff;
  float smallStep, bigStep;
  bool showSplash;
};

#endif
