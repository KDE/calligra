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

#ifndef __khtmlview_patched_h_
#define __khtmlview_patched_h_

#include <qpainter.h>

#include <khtmlview.h>

#include <khtmlsavedpage.h>

class KHTMLView_Patched : public KHTMLView
{
  Q_OBJECT
public:
  KHTMLView_Patched(QWidget *parent = 0L, const char *name = 0L, int flags = 0,
                    KHTMLView_Patched *parent_view = 0L);
  ~KHTMLView_Patched();

  void draw(QPainter *painter, int width, int height);

  void drawWidget( QWidget *widget );

  virtual KHTMLView *newView(QWidget *parent, const char *name = 0L, int flags = 0L);

  void setMouseLock(bool flag);  
  
private:
  QPixmap *pixmap;
};

#endif
