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

#ifndef __OptionDialog_h__
#define __OptionDialog_h__

#include <kdialogbase.h>

class KComboBox;
class QColor;
class KColorButton;
class QCheckBox;
class KoPageLayoutDia;
class KontourView;

namespace Kontour {
class GDocument;
class UnitBox;

class OptionsDialog : public KDialogBase
{
  Q_OBJECT
public:
  OptionsDialog(KontourView *aView, GDocument *aGDoc, QWidget* parent = 0L, const char* name = 0L);

protected:
  void createGeneralWidget(QWidget *parent);
  void createEditWidget(QWidget *parent);
  void createBGWidget(QWidget *parent);
  void createPageLayoutWidget(QWidget *parent);
  void createGridWidget(QWidget *parent);
  void createHorizLineWidget(QWidget *parent);
  void createVertLineWidget(QWidget *parent);

  void initHelplinesLists();

protected slots:
  void slotApply();
  void slotOk();

  void slotSetDocModified();
  void slotSetViewUpdate();

  void addHorizLine();
  void updateHorizLine();
  void deleteHorizLine();
  void horizLineSelected(int idx);

  void addVertLine();
  void updateVertLine();
  void deleteVertLine();
  void vertLineSelected(int idx);

private:
  KontourView *mView;
  GDocument *mGDoc;
  bool mDocModified;
  bool mViewUpdate;

  // General
  KComboBox *unit;
  KColorButton *mWSColorButton;

  UnitBox *horiz, *vert;
  UnitBox *smallStep, *bigStep;

  // Background
  KColorButton *bgbutton;

  KoPageLayoutDia *mPageLayout;

  // Helplines
  UnitBox *horizValue;
  UnitBox *vertValue;
  QListBox *horizList;
  QListBox *vertList;
  QValueList<double> horizLines;
  QValueList<double> vertLines;
  QPushButton *addHorizHelpLine,*updateHorizHelpLine,*delHorizHelpLine;
  QPushButton *delVertHelpLine, *updateVertHelpLine,*addVertHelpLine;

  // Grid
  QCheckBox *gbutton, *sbutton;
  UnitBox *hspinbox, *vspinbox;
  KColorButton *cbutton;
};
};
using namespace Kontour;

#endif
