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

#ifndef HelplineDialog_h_
#define HelplineDialog_h_

#include <vector>

#include <qdialog.h>
#include <qtabdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qlistbox.h>
#include <ktabctl.h>
#include <kcombo.h>

#include "Canvas.h"
#include "UnitBox.h"

class HelplineDialog : public QTabDialog {
  Q_OBJECT
public:
  HelplineDialog (Canvas* canvas, QWidget* parent = 0L, 
		  const char* name = 0L);

  static void setup (Canvas *c);

protected:
  QWidget* createHorizLineWidget (QWidget* parent);
  QWidget* createVertLineWidget (QWidget* parent);

private slots:
  void applyPressed ();
  void helpPressed ();

  void addHorizLine ();
  void updateHorizLine ();
  void deleteHorizLine ();

  void addVertLine ();
  void updateVertLine ();
  void deleteVertLine ();

private:
  void initLists ();

  Canvas *canvas;
  UnitBox *horizValue, *vertValue;
  QListBox *horizList, *vertList;
  vector<float> horizLines, vertLines;
};

#endif
