/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo.nl)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#include <qdockwindow.h>

class KontourView;
class QTabWidget;
class KComboBox;
class KoColor;
class KoColorChooser;
class KoIconChooser;
class QCheckBox;
class QSpinBox;
class QButtonGroup;

namespace Kontour {
class GStyle;
class OutlinePanel : public QDockWindow
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
  void slotChangeColor(const KoColor &c);
  void slotChangeLineWidth(int l);
  void slotChangeLineStyle(int w);
  void slotJoinPressed(int w);
  void slotCapPressed(int w);

private:
  KontourView     *mView;

  QTabWidget      *mTab;
  QCheckBox       *mStroked;
  KComboBox       *mStartArrowBox;
  KComboBox       *mEndArrowBox;
  QSpinBox        *mOpacityBox;
  QSpinBox        *mWidthBox;
  KComboBox       *mLineStyleBox;
  QButtonGroup    *mJoinBox;
  QButtonGroup    *mCapBox;
  KoColorChooser  *mOutlinePanel;
};

class PaintPanel : public QDockWindow
{
  Q_OBJECT
public:
  PaintPanel(KontourView *aView, QWidget *parent = 0L, const char *name = 0L);

public slots:
  void slotUpdate(bool pages = true);

private slots:
  void slotChangeFilled(int f);
  void slotChangeOpacity(int o);
  void slotChangeColor(const KoColor &c);

private:
  KontourView     *mView;

  QTabWidget      *mTab;
  KComboBox       *mPaintingBox;
  QSpinBox        *mOpacityBox;
  KoColorChooser  *mPaintPanel;
  KoIconChooser   *mBitmapPanel;
};
};
using namespace Kontour;

#endif
