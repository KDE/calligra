/*
 *  kfloatingdialog.cc - part of KImageShop
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

#include <qcursor.h>
#include <kpixmap.h>
#include <qpainter.h>
#include <qrect.h>
#include <kpixmapeffect.h>
#include <kapp.h>
#include <kdebug.h>

#include "kfloatingdialog.h"

KFloatingDialog::KFloatingDialog(QWidget *parent) : QFrame(parent)
{
  m_pParent = parent;
  m_shaded = false;
  m_dragging = false;
  m_resizing = false;

  setMouseTracking(true);
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(2);
}
KFloatingDialog::~KFloatingDialog() {}

void KFloatingDialog::paintEvent(QPaintEvent *e)
{
  QPainter p;
  p.begin(this);

  KPixmap pm;
  pm.resize(width()-6, 14);
  QRect gradient(3,3, width()-6, 14);

  KPixmapEffect::gradient(pm, Qt::blue, Qt::black, KPixmapEffect::HorizontalGradient);
  
  p.drawPixmap(3, 3, pm);
  p.end();

  QFrame::paintEvent(e);
}

void KFloatingDialog::mouseDoubleClickEvent (QMouseEvent *e)
{
  if (e->button() & LeftButton)
    {
      QRect title(0,0, width(), 20);
      if(!title.contains(e->pos()))
	return;
      
      if (m_shaded)
	{
	  resize(width(), m_unshadedHeight);
	  m_shaded = false;
	}
      else
	{
	  m_shaded = true;
	  m_unshadedHeight = height();
	  resize(width(), 20);
	}
    }
}

void KFloatingDialog::mousePressEvent(QMouseEvent *e)
{
  if (e->button() & LeftButton)
    {
      QRect title(0,0, width(), 20);
      QRect bottom(0, height()-2, width(), 2);

      if(title.contains(e->pos()))
	{
	  m_dragging = true;
	  m_dragStart = e->pos();
	}
      else if(!m_shaded && bottom.contains(e->pos()))
	{
	  m_resizing = true;
	  m_resizeStart = e->globalPos();
	}
    }
}
	
void KFloatingDialog::mouseMoveEvent(QMouseEvent *e)
{
  if (m_dragging)
    {
      QPoint dist = m_dragStart - e->pos();
      QPoint newPos = pos() - dist;

      if (newPos.x() < 0)
	newPos.setX(0);

      if (newPos.y() < 0)
	newPos.setY(0);
      
      if(m_pParent)
	{
	  if (newPos.x() + width() > m_pParent->width())
	    newPos.setX(m_pParent->width() - width());

	  if (newPos.y()+ height() > m_pParent->height())
	    newPos.setY(m_pParent->height() - height());
	}
      
      move(newPos);
    }
  else if (m_resizing)
    {
      QPoint dist = m_resizeStart - e->globalPos();
      QPoint newSize = QPoint(width(), height()) - dist;
      
      resize(newSize.x(), newSize.y());
    }
}

void KFloatingDialog::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() & LeftButton)
    {
      m_dragging = false;
      m_resizing = false;
    }
}

void KFloatingDialog::resizeEvent(QResizeEvent *e)
{
  kdebug(KDEBUG_INFO, 0, "ColorDialog::resizeEvent");
}

#include "kfloatingdialog.moc"
