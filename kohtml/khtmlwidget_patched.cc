/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#include "khtmlwidget_patched.h"
#include "khtmlwidget_patched.moc"

#include <iostream.h>


KHTMLWidget_Patched::KHTMLWidget_Patched(QWidget *parent = 0L, const char *name = 0L,
                      const char *pixDir = 0L)
:KHTMLWidget(parent, name, pixDir)		      
{
  cout << "hey, we're using our patched KHTMLWidget :-D" << endl;
  
  m_bMouseLockHack = false;
}		      
		      
KHTMLWidget_Patched::~KHTMLWidget_Patched()
{
}

void KHTMLWidget_Patched::draw(QPaintDevice *dev, int width, int height)
{
  cerr << "drawinggggggg" << endl;

//  if (painter) delete painter
  
  QPainter::redirect(this, dev);
  QPaintEvent pe(QRect(x_offset, y_offset, x_offset+width, y_offset+height));
  QApplication::sendEvent(this, &pe);
  QPainter::redirect(this, 0);
  
  cerr << "done :-))))))" << endl;
}

void KHTMLWidget_Patched::mousePressEvent(QMouseEvent *ev)
{
  cerr << "void KHTMLWidget_Patched::mousePressEvent(QMouseEvent *ev)" << endl;
  
  if (m_bMouseLockHack) m_bMouseLockHack = false;
  else KHTMLWidget::mousePressEvent(ev);

}