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

#ifndef OptionDialog_h_
#define OptionDialog_h_

#include <kdialogbase.h>

class QComboBox;
class KColorButton;
class QCheckBox;
class UnitBox;

class OptionDialog : public KDialogBase {
  Q_OBJECT
public:
  OptionDialog (QWidget* parent = 0L, const char* name = 0L);

  static int setup ();

protected:
  void createGeneralWidget (QWidget* parent);
  void createEditWidget (QWidget* parent);
  void createGridWidget (QWidget* parent);
  void createHorizLineWidget (QWidget* parent);
  void createVertLineWidget (QWidget* parent);

private:
  QComboBox* unit;
  UnitBox *horiz, *vert;
  UnitBox *smallStep, *bigStep;
  
  QCheckBox *gbutton, *sbutton;
  UnitBox *hspinbox, *vspinbox;
  KColorButton *cbutton;
  
  UnitBox *horizValue, *vertValue;
  QListBox *horizList, *vertList;
};

#endif
