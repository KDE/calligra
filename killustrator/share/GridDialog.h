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

#ifndef GridDialog_h
#define GridDialog_h

#include <qdialog.h>
#include <qradiobt.h>
#include <qchkbox.h>
#include "Canvas.h"
#include "FloatSpinBox.h"
#include "UnitBox.h"

class GridDialog : public QDialog {
  Q_OBJECT
public:
  GridDialog (QWidget* parent = 0L, const char* name = 0L);

  float horizontalDistance ();
  float verticalDistance ();
  bool showGrid ();
  bool snapToGrid ();

  void setDistances (float h, float v);
  void setSnapToGridOn (bool flag);
  void setShowGridOn (bool flag);



  static void setupGrid (Canvas* canvas);

protected:
  QWidget* createGridWidget (QWidget* parent);

private slots:
  void helpPressed ();

private:
  QCheckBox *gbutton, *sbutton;
  UnitBox *hspinbox, *vspinbox;
};

#endif
