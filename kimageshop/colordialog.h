/*
 *  colordialog.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __colordialog_h__
#define __colordialog_h__

#include "kcolor.h"
#include "kpixmap.h"
#include "kpixmapeffect.h"
#include "kfloatingdialog.h"

class KDualColorButton;
class QPushButton;
class QFrame;
class QSpinBox;
class RGBWidget;
class ColorFrame;
class ColorSlider;
class ColorChooserWidget;
class QVBoxLayout;

class ColorDialog : public KFloatingDialog
{
  Q_OBJECT
 
 public:
  ColorDialog(QWidget *parent = 0L);
  ~ColorDialog();

private:
  ColorChooserWidget   *m_pBase;
};

class ColorChooserWidget : public QWidget
{
  Q_OBJECT
 
 public:
  ColorChooserWidget(QWidget *parent = 0L);
  ~ColorChooserWidget();

 protected:
  virtual void resizeEvent(QResizeEvent *);

 protected:
  QPushButton      *m_pGrayButton, *m_pRGBButton, *m_pHSBButton, *m_pCMYKButton, *m_pLABButton;
  ColorFrame       *m_pColorFrame;
  RGBWidget        *m_pRGBWidget;
  KDualColorButton *m_pColorButton;
  KColor            m_fg, m_bg;

};

class RGBWidget : public QWidget
{
  Q_OBJECT
 
 public:
  RGBWidget(QWidget *parent = 0L);
  ~RGBWidget();

 protected:
  ColorSlider *m_pRSlider;
  ColorSlider *m_pGSlider;
  ColorSlider *m_pBSlider;
  QVBoxLayout *m_pVLayout;
};

#endif
