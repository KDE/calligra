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

#ifndef TransformationDialog_h
#define TransformationDialog_h

#include <qdialog.h>
#include "GDocument.h"
#include "GObject.h"
#include "CommandHistory.h"
#include "FloatSpinBox.h"
#include "UnitBox.h"
#include "MyTabCtl.h"

class QRadioButton;
class QPushButton;
class QCheckBox;

class TransformationDialog : public QDialog {
  Q_OBJECT
public:
  TransformationDialog (CommandHistory* cmdHist,
			QWidget* parent = 0L, const char* name = 0L);

  void setDocument (GDocument* doc);
  void showTab (int id);

public slots:
  void update ();

protected:
  QWidget* createPositionWidget (QWidget* parent);
  QWidget* createDimensionWidget (QWidget* parent);
  QWidget* createRotationWidget (QWidget* parent);
  QWidget* createMirrorWidget (QWidget* parent);

private slots:
  void applyPressed ();
  void applyToDuplicatePressed ();
  void updateProportionalDimension (float value);
  void slotAbsScale ();
  void slotPercentScale ();

private:
  void translate (bool onDuplicate);
  void rotate (bool onDuplicate);
  void scale (bool onDuplicate);
  void mirror (bool onDuplicate);

  MyTabCtl *tabCtl;
  QWidget *widgets[4];
  GDocument* document;
  CommandHistory *history;

  QPushButton *applyBttn[4], *applyToDupBttn[4];

  UnitBox *horizPosition;
  UnitBox *vertPosition;
  QCheckBox *relativePosition;

  FloatSpinBox *rotAngle;
  UnitBox *horizRotCenter;
  UnitBox *vertRotCenter;
  QCheckBox *relativeRotCenter;

  QPushButton *horizMirror, *vertMirror;

  QRadioButton *absolute, *percent;
  UnitBox *horizDim, *vertDim;
  QCheckBox *proportional;
  float dimRatio, selWidth, selHeight;
};

#endif
