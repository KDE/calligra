/*
 *  colorslider.h - part of KImageShop
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

#ifndef __colorslider_h__
#define __colorslider_h__

#include <qpoint.h>
#include <qwidget.h>
#include <qframe.h>

class ColorFrame;
class SliderWidget;

class ColorSlider : public QWidget
{
  Q_OBJECT
 
 public:
  ColorSlider(QWidget *parent = 0L);
  virtual ~ColorSlider();

 protected:
  virtual void resizeEvent (QResizeEvent *);
  
 public slots:
  void slotSetColor1(const QColor&);
  void slotSetColor2(const QColor&);
  
  // for internal use
  void slotSetValue(int);

 signals:
  void  colorSelected(const QColor&);

 protected:
  SliderWidget *m_pSlider;
  ColorFrame   *m_pColorFrame;
};

class SliderWidget : public QFrame
{
  Q_OBJECT
 
 public:
  SliderWidget(QWidget *parent = 0L);

 protected:
  virtual void mousePressEvent (QMouseEvent *); 
  virtual void mouseReleaseEvent (QMouseEvent *); 
  virtual void mouseMoveEvent (QMouseEvent *); 
  
 signals:
  void  positionChanged(int);

 protected:
  bool   m_dragging;
  QPoint m_myPos;
};

#endif
