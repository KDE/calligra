/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo.nl)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

#ifndef __StylePanel_h__
#define __StylePanel_h__

#include <qtabwidget.h>
#include <qpen.h>

class KontourView;
class GStyle;
class QComboBox;
class KoColor;
class KoColorChooser;
class QCheckBox;
class QSpinBox;
class QButtonGroup;

class OutlinePanel : public QTabWidget
{
  Q_OBJECT
public:
  OutlinePanel(KontourView *aView, QWidget *parent = 0L, const char *name = 0L);

public slots:
  void slotUpdate();

private slots:
  void slotChangeStroked(bool b);
  void slotChangeStartArrow(int w);
  void slotChangeEndArrow(int w);
  void slotChangeOpacity(int o);
  void slotChangeLineWidth(int l);
  void slotChangeLineStyle(int w);
  void slotJoinPressed(int w);
  void slotCapPressed(int w);

private:
  KontourView     *mView;
  QCheckBox       *mStroked;
  QComboBox       *mStartArrowBox;
  QComboBox       *mEndArrowBox;
  QSpinBox        *mOpacityBox;
  QSpinBox        *mWidthBox;
  QComboBox       *mLineStyleBox;
  QButtonGroup    *mJoinBox;
  QButtonGroup    *mCapBox;
  KoColorChooser  *mOutlinePanel;
};

class PaintPanel : public QTabWidget
{
  Q_OBJECT
public:
  PaintPanel(KontourView *aView, QWidget *parent = 0L, const char *name = 0L);

public slots:
  void slotUpdate();

private slots:
  void slotChangeFilled(int f);

private:
  KontourView     *mView;
  QComboBox       *mPaintingBox;
  QSpinBox        *mOpacityBox;
  KoColorChooser  *mPaintPanel;
};

#endif
