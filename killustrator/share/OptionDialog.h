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

#include <qdialog.h>
#include <qtabdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <ktabctl.h>

#include "UnitBox.h"

class QComboBox;

class OptionDialog : public QTabDialog {
  Q_OBJECT
public:
  OptionDialog (QWidget* parent = 0L, const char* name = 0L);

  static int setup ();

protected:
  QWidget* createGeneralWidget (QWidget* parent);
  QWidget* createEditWidget (QWidget* parent);

private slots:
  void applyPressed ();
  void helpPressed ();

private:
  QComboBox* unit;
  UnitBox *horiz, *vert;
  UnitBox *smallStep, *bigStep;
};

#endif
