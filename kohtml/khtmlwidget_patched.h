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

#ifndef __khtmlwidget_patched_h_
#define __khtmlwidget_patched_h_

#include <qpaintdevice.h>
#include <qstrlist.h>

#include <khtml.h>

class KHTMLWidget_Patched : public KHTMLWidget
{
  Q_OBJECT
public:
  KHTMLWidget_Patched(QWidget *parent = 0L, const char *name = 0L,
                      const char *pixDir = 0L);
  ~KHTMLWidget_Patched();

  void draw(QPaintDevice *dev, int width, int height);
  
  void setMouseLock(bool flag) { m_bMouseLockHack = flag; }
  
protected:
  virtual void mousePressEvent(QMouseEvent *ev);

private:
  bool m_bMouseLockHack;    
};  		        

#endif
