/*
 *  colorslider.cc - part of KImageShop
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

#include <qframe.h>
#include <kdebug.h>

#include "colorframe.h"
#include "colorslider.h"

ColorSlider::ColorSlider(QWidget *parent) : QWidget(parent)
{
  m_pColorFrame = new ColorFrame(this);
  m_pSlider = new SliderWidget(this);

  connect(m_pSlider, SIGNAL(positionChanged(int)), this, SLOT(slotSetValue(int)));
}

ColorSlider::~ColorSlider()
{
  delete m_pColorFrame;
  delete m_pSlider;
}

void ColorSlider::resizeEvent (QResizeEvent *)
{
  m_pColorFrame->setGeometry(3, 0, width()-6, height()-8);
  m_pSlider->move(width()/2, height()-16);
}

void ColorSlider::slotSetColor1(const QColor& c)
{
  m_pColorFrame->slotSetColor1(c);
}

void ColorSlider::slotSetColor2(const QColor& c)
{
  m_pColorFrame->slotSetColor2(c);
}

void ColorSlider::slotSetValue(int x)
{
  if (x < 0)
	x = 0;
  if (x > m_pColorFrame->contentsWidth())
	x = m_pColorFrame->contentsWidth();

  emit colorSelected(m_pColorFrame->colorAt(QPoint(x, m_pColorFrame->contentsHeight()/2)));
}

SliderWidget::SliderWidget(QWidget *parent) : QFrame(parent)
{
  m_dragging = false;
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(2);
  setFixedHeight(16);
  setFixedWidth(6);
}

void SliderWidget::mousePressEvent (QMouseEvent *e)
{
  if (e->button() & LeftButton)
    {
      m_myPos = e->pos();
	  m_dragging = true;
	}
  else
    QFrame::mousePressEvent(e);
}

void SliderWidget::mouseReleaseEvent (QMouseEvent *e)
{
  if (e->button() & LeftButton)
	m_dragging = false;
  else
    QFrame::mouseReleaseEvent(e);
}

void SliderWidget::mouseMoveEvent (QMouseEvent *e)
{
  if (m_dragging)
    {
	  QWidget *p = parentWidget();

	  if (!p) return;
	  
	  QPoint newPos = p->mapFromGlobal(QCursor::pos()) - m_myPos;
	  
	  // don't drag vertically :-)
	  newPos.setY(pos().y());
	  
	  if (newPos.x() < 0)
		newPos.setX(0);
	  if (newPos.x() > p->width()- width())
		newPos.setX(p->width()- width());
	  
	  move(newPos);
	  emit positionChanged(pos().x() + width()/2);
    }
  else
	QFrame::mouseMoveEvent(e);
}


#include "colorslider.moc"
