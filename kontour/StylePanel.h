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

#ifndef __StylePanel_h__
#define __StylePanel_h__

#include <qtabwidget.h>

class GStyle;
class KoColor;
class QCheckBox;
class QSpinBox;
class PaintPanel : public QTabWidget
{
  Q_OBJECT
public:
  PaintPanel(QWidget *parent = 0L, const char *name = 0L);

public slots:
  void slotStyleChanged(const GStyle &);

signals:
  void changePaintColor(const KoColor &);
  void changeFilled(bool);

private:
  QCheckBox *mFilled;
};

class OutlinePanel : public QTabWidget
{
  Q_OBJECT
public:
  OutlinePanel(QWidget *parent = 0L, const char *name = 0L);

public slots:
  void slotStyleChanged(const GStyle &);

signals:
  void changeOutlineColor(const KoColor &);
  void changeStroked(bool);
  void changeLinewidth(unsigned int);

private slots:
  void slotChangeLinewidth(int lwidth);

private:
  QCheckBox *mStroked;
  QSpinBox *mlwidthBox;
};

#endif
