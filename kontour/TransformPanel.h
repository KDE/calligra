/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo.nl)

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

#ifndef __TransformPanel_h__
#define __TransformPanel_h__

#include <qdockwindow.h>

class KontourView;
class QPushButton;
class QTabWidget;
class QSpinBox;

namespace Kontour {
class TransformPanel : public QDockWindow
{
  Q_OBJECT
public:
  TransformPanel(KontourView *aView, QWidget *parent = 0L, const char *name = 0L);

public slots:
  void slotUpdate();

private slots:
  void slotApplyPressed();
  void slotDupPressed();
  void slotRelativeToggled(bool);

private:
  KontourView   *mView;
  QWidget       *mTransformPanel;
  QTabWidget    *mTab;
  QPushButton   *mApplyBtn;
  QPushButton   *mDuplicateBtn;

  QWidget       *mTranslate;
  QSpinBox      *mHorizBox;
  QSpinBox      *mVertBox;
  QWidget       *mRotate;
  QSpinBox      *mAngleBox;
  QWidget       *mShear;
  QSpinBox      *mShearAngleXBox;
  QSpinBox      *mShearAngleYBox;
  QWidget       *mScale;
  QSpinBox      *mScaleXBox;
  QSpinBox      *mScaleYBox;
  bool          mTRelative : 1;
  bool          mRRelative : 1;
};
};
using namespace Kontour;

#endif
