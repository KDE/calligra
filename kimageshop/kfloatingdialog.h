/*
 *  kfloatingdialog.h - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
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

#ifndef __kfloatingdialog_h__
#define __kfloatingdialog_h__

#include <qpoint.h>
#include <qframe.h>

class KFloatingDialog : public QFrame
{
  Q_OBJECT
 
 public:
  KFloatingDialog(QWidget *parent = 0L);
  ~KFloatingDialog();

 protected:
  virtual void paintEvent(QPaintEvent *);
  virtual void resizeEvent(QResizeEvent *);
  virtual void mousePressEvent(QMouseEvent *);
  virtual void mouseMoveEvent(QMouseEvent *);
  virtual void mouseReleaseEvent(QMouseEvent *);
  virtual void mouseDoubleClickEvent (QMouseEvent *); 

 protected:
  bool     m_dragging;
  bool     m_resizing;
  bool     m_shaded;

  QPoint   m_resizeStart;
  QPoint   m_dragStart;

  int      m_unshadedHeight;

  QWidget  *m_pParent;
};

#endif
