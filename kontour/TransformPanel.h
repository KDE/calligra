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

class QWMatrix;
class QTabWidget;
class QSpinBox;
class QGroupBox;
class KIntNumInput;
class GPage;
class Handle;
class KCommand;
class TransformPanel : public QDockWindow
{
  Q_OBJECT
public:
  TransformPanel(QWidget *parent = 0L, const char *name = 0L);

public slots:
  void setContext(const QWMatrix &, GPage *);

private slots:
  void slotDupPressed();
  void slotApplyPressed();
  void slotRelativeToggled(bool);

signals:
  void changeTransform(KCommand *);

private:
  QTabWidget    *mTab;
  QWidget       *mTranslate;
  QSpinBox      *mHorizBox;
  QSpinBox      *mVertBox;
  QWidget       *mRotate;
  QSpinBox      *mAngleBox;
  QWidget       *mShear;
  QSpinBox      *mShearAngleXBox;
  QSpinBox      *mShearAngleYBox;
  GPage         *mPage;
  Handle        *mHandle; // not strictly necessary, so convenience
  bool          mTRelative : 1;
  bool          mRRelative : 1;
};

#endif
