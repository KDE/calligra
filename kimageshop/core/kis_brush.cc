/*
 *  kis_brush.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
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

#include <qpoint.h>
#include <qsize.h>
#include <qimage.h>
#include <qpixmap.h>

#include "kis_brush.h"

Brush::Brush(QString file)
  : IconItem()
{
  m_valid    = false;
  m_spacing  = 3;
  loadViaQImage(file);

  // default hotSpot in the centre
  m_hotSpot = QPoint( width()/2, height()/2 );
}

Brush::~Brush()
{
  delete [] m_pData;
}

void Brush::loadViaQImage(QString file)
{
  // load via QImage
  QImage img(file);

  if (img.isNull())
    {
      m_valid = false;
      qDebug("Unable to load brush: %s.", file.latin1());
    }

  // create pixmap for preview dialog
  m_pPixmap = new QPixmap;
  m_pPixmap->convertFromImage(img, QPixmap::AutoColor);

  m_w = img.width()-2; // ##### FIXME !!!!!
  m_h = img.height();

  m_pData = new uchar[m_h * m_w];

  uchar *p;

  for (int h = 0; h < m_h; h++)
    {
      p = img.scanLine(h);
      for (int w = 0; w < m_w; w++)
	{
	  m_pData[m_w * h + w] = 255 - qRed(*((QRgb*)(p+w)));
	  //qDebug ("%d", static_cast<int>(value(w,h)));
	}
    }
 
  m_valid = true;
  qDebug("Brush: %s loaded.",file.latin1());
}

QPixmap& Brush::pixmap(){
  return *m_pPixmap;
}

void Brush::setHotSpot(QPoint pt)
{
  int x = pt.x();
  int y = pt.y();

  if (x < 0)
    x = 0;
  else if (x >= m_w)
    x = m_w-1;

  if (y < 0)
    y = 0;
  else if (y >= m_h)
    y = m_h-1;
  
  m_hotSpot = QPoint(x,y);
}

uchar Brush::value(int x, int y) const
{
  return m_pData[m_w * y + x];
}

uchar* Brush::scanline(int i) const
{
  if (i < 0)
    i = 0;
  if (i >= m_h)
    i = m_h-1;
  return (m_pData + m_w * i);
}

uchar* Brush::bits() const
{
  return m_pData;
}

void Brush::dump() const
{
  qDebug("Brush data:\n");

  for (int h = 0; h < m_h; h++) {
    for (int w = 0; w < m_w; w++) {
	  qDebug("%d", m_pData[m_w * h + w]);
	}
    }
}

