/*
 *  colorframe.h - part of KImageShop
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

#ifndef __colorframe_h__
#define __colorframe_h__

#include <qrect.h>
#include <qframe.h>
#include <qimage.h>

#include "kpixmap.h"
#include "kpixmapeffect.h"

class ColorFrame : public QFrame
{
  Q_OBJECT
 
public:
  ColorFrame(QWidget *parent = 0L);

  const QColor colorAt(const QPoint &);
  
protected:
  void drawContents(QPainter *);
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  
public slots:
  void slotSetColor1(const QColor &);
  void slotSetColor2(const QColor &);

signals:
  void clicked(const QPoint &);
  void colorSelected(const QColor &);

protected:
  QColor    m_c1, m_c2;
  KPixmap   m_pm;
  QImage    m_pmImage;
  bool      m_colorChanged;
  bool      m_pixChanged;
  bool      m_dragging;
};

#endif
