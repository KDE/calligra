/*
 *  colorframe.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
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

#include <qpainter.h>
#include <kdebug.h>

#include "colorframe.h"

ColorFrame::ColorFrame(QWidget *parent) : QFrame(parent)
{
  setFrameStyle(Panel | Sunken);
  setBackgroundMode(NoBackground);

  m_c1 = QColor(255,255,255);
  m_c2 = QColor(0,0,0);

  m_colorChanged = false;
}
ColorFrame::~ColorFrame() {}

void ColorFrame::slotSetColor1(const QColor& c)
{
  m_c1 = c;
  m_colorChanged = true;
  m_pixChanged = true;
  repaint();
}

void ColorFrame::slotSetColor2(const QColor& c)
{
  m_c2 = c;
  m_colorChanged = true;
  repaint();
}

int ColorFrame::contentsWidth()
{
  return contentsRect().width();
}

int ColorFrame::contentsHeight()
{
  return contentsRect().height();
}

void ColorFrame::drawContents(QPainter *p)
{
  QRect r = contentsRect();

  if ((m_pm.size() != r.size()) || m_colorChanged)
	{
	  m_pm.resize(r.width(), r.height());
	  KPixmapEffect::gradient(m_pm, m_c1, m_c2, KPixmapEffect::HorizontalGradient);
	  m_colorChanged = false;
	  m_pixChanged = true;
	}
  
  p->drawPixmap(r.left(), r.top(), m_pm);
}

void ColorFrame::mousePressEvent (QMouseEvent *e)
{
  if (e->button() & LeftButton)
    {
	  QPoint pos = QPoint(e->pos().x() - contentsRect().left(), e->pos().y() - contentsRect().top());

	  if (pos.x() < 0
		  || pos.y() < 0
		  || pos.x() >= contentsRect().width()
		  || pos.y() >= contentsRect().height())
		return ;
	  
      QColor c = colorAt(pos);
	  kdebug(KDEBUG_INFO, 0, "ColorFrame -> emit colorSelected()");
	  emit colorSelected(c);
	}
  else
	QFrame::mousePressEvent(e);
}

const QColor ColorFrame::colorAt (const QPoint& p)
{
  if (m_pixChanged)
	{
	  m_pmImage = m_pm.convertToImage();
	  m_pixChanged = false;
	}

  if (p.x() >= m_pm.width()
	  || p.y() >= m_pm.height())
	return QColor(255,255,255);

  return QColor(m_pmImage.pixel(p.x(), p.y()));
}

#include "colorframe.moc"
