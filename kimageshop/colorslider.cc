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

  m_min = 0;
  m_max = 255;
  m_value = 0;

  connect(m_pSlider, SIGNAL(positionChanged(int)), this, SLOT(slotValueChanged(int)));
}

ColorSlider::~ColorSlider()
{
  delete m_pColorFrame;
  delete m_pSlider;
}

int ColorSlider::minValue()
{
  return  m_min;
}

int ColorSlider::maxValue()
{
  return  m_max;
}

void ColorSlider::slotSetRange(int min, int max)
{
  if (min >= max)
	return;
  
  m_min = min;
  m_max = max;
}

void ColorSlider::resizeEvent (QResizeEvent *e)
{
  QWidget::resizeEvent(e);
  m_pColorFrame->setGeometry(3, 0, width()-6, height()-8);
  slotSetValue(m_value);
}

void ColorSlider::slotSetColor1(const QColor& c)
{
  m_pColorFrame->slotSetColor1(c);
}

void ColorSlider::slotSetColor2(const QColor& c)
{
  m_pColorFrame->slotSetColor2(c);
}

void ColorSlider::slotSetValue(int value)
{
  if (value < m_min) value = m_min;
  if (value > m_max) value = m_max;

  m_value = value;

  int range = m_max - m_min +1;
  //kdebug(KDEBUG_INFO, 0, "range: %d", range);
  float v = value;
   if (m_min < 0)
	v += -m_min;

   //kdebug(KDEBUG_INFO, 0, "value: %f", v);
  
  float factor = v /range;
  int x = static_cast<int>(factor * m_pColorFrame->contentsWidth());
  //kdebug(KDEBUG_INFO, 0, "x: %d", x);

  m_pSlider->move(QPoint(x , height()-16));
}

void ColorSlider::slotValueChanged(int x)
{
  if (x < 0)
	x = 0;
  if (x > m_pColorFrame->contentsWidth())
	x = m_pColorFrame->contentsWidth();
  float factor = x;
  factor /= m_pColorFrame->contentsWidth();
  int range = m_max - m_min +1;
  
  m_value = static_cast<int>(factor * range);

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
	  emit positionChanged(pos().x());
    }
  else
	QFrame::mouseMoveEvent(e);
}


#include "colorslider.moc"
