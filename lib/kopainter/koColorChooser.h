/* -*- C++ -*-


  This file is part of KIllustrator.
  Copyright (C) 2000 Igor Janssen (rm@linux.ru.net)

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

#ifndef __kis_colorchooser_h__
#define __kis_colorchooser_h__

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
  KoColorChooser(QWidget* parent = 0L, const char* name = 0L);

signals:
  void colorChanged(const KoColor&);

protected slots:
  void slotChangeColor(const KoColor& c);

private:
  QGridLayout *mGrid;
  KoFrameButton *btn_RGB;
  KoFrameButton *btn_CMYK;
  KoFrameButton *btn_Grey;
  KoFrameButton *btn_HSB;
  KoFrameButton *btn_LAB;
  RGBWidget *clr;
  KColorPatch *clr_patch;
};

class RGBWidget : public QWidget
{
  Q_OBJECT
 
 public:
  RGBWidget(QWidget *parent = 0L);

 public slots:
  void slotSetColor(const KoColor&);

 protected slots:
  void slotRSliderChanged(int);
  void slotGSliderChanged(int);
  void slotBSliderChanged(int);

  void slotRInChanged(int);
  void slotGInChanged(int);
  void slotBInChanged(int);

 signals:
  void colorChanged(const KoColor&);

 protected:
  virtual void resizeEvent(QResizeEvent *);

 protected:
  KoColor     m_c;
  KoColorSlider *m_pRSlider, *m_pGSlider, *m_pBSlider;
  QLabel      *m_pRLabel, *m_pGLabel, *m_pBLabel;
  QSpinBox    *m_pRIn, *m_pGIn, *m_pBIn;
};

#endif
