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

#ifndef __htmwidget_h_
#define __htmwidget_h_

#include <qpaintdevice.h>
#include <qstrlist.h>

#include <khtml.h>

class KMyHTMLWidget : public KHTMLWidget
{
  Q_OBJECT
public:
  KMyHTMLWidget(QWidget *parent = 0L, const char *name = 0L,
                      const char *pixDir = 0L);
  ~KMyHTMLWidget();

  void draw(QPaintDevice *dev, int width, int height);
};  		        

#endif
