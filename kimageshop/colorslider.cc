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

#include <qpainter.h>
#include <kdebug.h>

#include "colorslider.h"

ColorSlider::ColorSlider(QWidget *parent) : QWidget(parent)
{
  setBackgroundColor(white);
}
ColorSlider::~ColorSlider() {}

void ColorSlider::slotSetColor1(const QColor& c)
{
 
}

void ColorSlider::slotSetColor2(const QColor& c)
{
  
}

void ColorSlider::mousePressEvent (QMouseEvent *e)
{
  /*
	if (e->button() & LeftButton)
    {
      QColor c = colorAt(QPoint(e->pos().x() - contentsRect().left(), e->pos().y() - contentsRect().top()));
	  kdebug(KDEBUG_INFO, 0, "ColorSlider -> emit colorSelected()");
	  emit colorSelected(c);
	}
  else
	QFrame::mousePressEvent(e);
  */
}

#include "colorslider.moc"
