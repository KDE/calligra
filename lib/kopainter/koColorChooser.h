/* This file is part of the KDE project
  Copyright (c) 1999 Matthias Elter <me@kde.org>
  Copyright (c) 2001 Igor Janssen <rm@linux.ru.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __ko_ColorChooser_h__
#define __ko_ColorChooser_h__

#include <qwidget.h>
#include "koColor.h"

class KoFrameButton;
class QGridLayout;
class RGBWidget;
class KoColor;
class KoColorSlider;
class QLabel;
class QSpinBox;
class KColorPatch;

class KoColorChooser : public QWidget
{
  Q_OBJECT
public:
  KoColorChooser(QWidget *parent = 0L, const char *name = 0L);

  const KoColor &color() const {return mColor; }

signals:
  void colorChanged(const KoColor &);

protected slots:
  void slotChangeColor(const KoColor &c);
  void slotChangeColor(const QColor &c);

private:
  KoColor     mColor;

  QGridLayout *mGrid;
  KoFrameButton *btn_RGB;
  KoFrameButton *btn_CMYK;
  KoFrameButton *btn_Grey;
  KoFrameButton *btn_HSB;
  KoFrameButton *btn_LAB;
  RGBWidget *mRGBWidget;
  KColorPatch *clr_patch;
};

class RGBWidget : public QWidget
{
  Q_OBJECT
public:
  RGBWidget(KoColorChooser *aCC, QWidget *parent = 0L);

public slots:
  void slotChangeColor();

protected slots:
  void slotRSliderChanged(int r);
  void slotGSliderChanged(int g);
  void slotBSliderChanged(int b);

  void slotRInChanged(int r);
  void slotGInChanged(int g);
  void slotBInChanged(int b);

signals:
  void colorChanged(const KoColor &);

private:
  KoColorChooser *mCC;
  KoColorSlider *mRSlider, *mGSlider, *mBSlider;
  QLabel      *mRLabel, *mGLabel, *mBLabel;
  QSpinBox    *mRIn, *mGIn, *mBIn;
};

#endif
