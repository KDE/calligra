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

#include "htmwidget.h"
#include "htmwidget.moc"

#include <iostream.h>
#include <qapplication.h>

KMyHTMLWidget::KMyHTMLWidget(QWidget *parent, const char *name ,
                      const char *pixDir)
:KHTMLWidget(parent, name, pixDir)		
{
}		
		
KMyHTMLWidget::~KMyHTMLWidget()
{
}

void KMyHTMLWidget::draw(QPaintDevice *dev, int width, int height)
{
  cerr << "drawinggggggg" << endl;

//  if (painter) delete painter

  QPainter::redirect(this, dev);
  QPaintEvent pe(QRect(x_offset, y_offset, x_offset+width, y_offset+height));
  QApplication::sendEvent(this, &pe);
  QPainter::redirect(this, 0);

  cerr << "done :-))))))" << endl;
}
