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

#include <kdualcolorbtn.h>

#include "color.h"
#include "kpixmap.h"
#include "kpixmapeffect.h"
#include "kfloatingdialog.h"

class KDualColorButton;
class QPushButton;
class QFrame;
class QLabel;
class QSpinBox;
class QSpinBox;
class RGBWidget;
class HSBWidget;
class CMYKWidget;
class LABWidget;
class GreyWidget;
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

 signals:
  void fgColorChanged(const KColor&);
  void bgColorChanged(const KColor&);

 public slots:
  void slotFGColorChanged(const QColor&);
  void slotBGColorChanged(const QColor&);

private:
  ColorChooserWidget   *m_pBase;
};

class ColorChooserWidget : public QWidget
{
  Q_OBJECT
 
 public:
  ColorChooserWidget(QWidget *parent = 0L);
  ~ColorChooserWidget();

 public slots:
  void slotShowGrey();
  void slotShowRGB();
  void slotShowHSB();
  void slotShowCMYK();
  void slotShowLAB();
  void slotSetFGColor(const QColor&);
  void slotSetBGColor(const QColor&);

 protected slots:
  void slotRGBWidgetChanged(const QColor&);
  void slotGreyWidgetChanged(const QColor&);
  void slotColorFrameChanged(const QColor&);
  void slotColorButtonFGChanged(const QColor&);
  void slotColorButtonBGChanged(const QColor&);
  void slotColorButtonCurrentChanged(KDualColorButton::DualColor s);

 protected:
  virtual void resizeEvent(QResizeEvent *);

 signals:
  void fgColorChanged(const QColor&);
  void bgColorChanged(const QColor&);

 protected:
  QPushButton      *m_pGreyButton, *m_pRGBButton, *m_pHSBButton, *m_pCMYKButton, *m_pLABButton;
  ColorFrame       *m_pColorFrame;
  RGBWidget        *m_pRGBWidget;
  HSBWidget        *m_pHSBWidget;
  CMYKWidget       *m_pCMYKWidget;
  LABWidget        *m_pLABWidget;
  GreyWidget       *m_pGreyWidget;
  KDualColorButton *m_pColorButton;
  KColor            m_fg, m_bg;
};

class RGBWidget : public QWidget
{
  Q_OBJECT
 
 public:
  RGBWidget(QWidget *parent = 0L);
  ~RGBWidget();

 public slots:
  void slotSetColor(const QColor&);

 protected slots:
  void slotRSliderChanged(int);
  void slotGSliderChanged(int);
  void slotBSliderChanged(int);

  void slotRInChanged(int);
  void slotGInChanged(int);
  void slotBInChanged(int);

 signals:
  void colorChanged(const QColor&);

 protected:
  virtual void resizeEvent(QResizeEvent *);

 protected:
  ColorSlider *m_pRSlider, *m_pGSlider, *m_pBSlider;
  QLabel      *m_pRLabel, *m_pGLabel, *m_pBLabel;
  QSpinBox    *m_pRIn, *m_pGIn, *m_pBIn;
  QColor       m_c;
};
/*
class HSBWidget : public QWidget
{
  Q_OBJECT
 
 public:
  HSBWidget(QWidget *parent = 0L);
  ~HSBWidget();

 public slots:
  void slotSetColor(const QColor&);

 protected slots:
  void slotHueChanged(const QColor&);
  void slotSaturationChanged(const QColor&);
  void slotBrightnessChanged(const QColor&);

 signals:
  void colorChanged(const QColor&);

 protected:
  virtual void resizeEvent(QResizeEvent *);

 protected:
  ColorSlider *m_pHSlider, *m_pSSlider, *m_pBSlider;
  QLabel      *m_pHLabel, *m_pSLabel, *m_pBLabel;
  QSpinBox    *m_pHIn, *m_pSIn, *m_pBIn;
};

class CMYKWidget : public QWidget
{
  Q_OBJECT
 
 public:
  CMYKWidget(QWidget *parent = 0L);
  ~CMYKWidget();

 public slots:
  void slotSetColor(const QColor&);

 protected slots:
  void slotCyanChanged(const QColor&);
  void slotMagentaChanged(const QColor&);
  void slotYellowChanged(const QColor&);
  void slotBlackChanged(const QColor&);

 signals:
  void colorChanged(const QColor&);

 protected:
  virtual void resizeEvent(QResizeEvent *);

 protected:
  ColorSlider *m_pCSlider, *m_pMSlider, *m_pYSlider, *m_pKSlider;
  QLabel      *m_pCLabel, *m_pMLabel, *m_pYLabel, *m_pKLabel;
  QSpinBox    *m_pCIn, *m_pMIn, *m_pYIn, *m_pKIn;
};

class LABWidget : public QWidget
{
  Q_OBJECT
 
 public:
  LABWidget(QWidget *parent = 0L);
  ~LABWidget();

 public slots:
  void slotSetColor(const QColor&);

 protected slots:
  void slotLChanged(const QColor&);
  void slotAChanged(const QColor&);
  void slotBChanged(const QColor&);

 signals:
  void colorChanged(const QColor&);

 protected:
  virtual void resizeEvent(QResizeEvent *);

 protected:
  ColorSlider *m_pLSlider,*m_pASlider, *m_pBSlider;
  QLabel      *m_pLLabel, *m_pALabel, *m_pBLabel;
  QSpinBox    *m_pLIn, *m_pAIn, *m_pBIn;
};
*/

class GreyWidget : public QWidget
{
  Q_OBJECT
 
 public:
  GreyWidget(QWidget *parent = 0L);
  ~GreyWidget();

 public slots:
  void slotSetColor(const QColor&);

 protected slots:
  void slotVSliderChanged(int);
  void slotVInChanged(int);

 signals:
  void colorChanged(const QColor&);

 protected:
  virtual void resizeEvent(QResizeEvent *);

 protected:
  ColorSlider *m_pVSlider;
  QLabel      *m_pVLabel;
  QSpinBox    *m_pVIn;
  QColor       m_c;
};

#endif
