/*
 *  kis_gradient.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
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

#ifndef __kis_painter_h__
#define __kis_painter_h__

#include <qobject.h>
#include <qimage.h>
#include <qpixmap.h>
#include "kis_doc.h"

class KisDoc;

class KisPainter : public QObject 
{
  Q_OBJECT

public:
  	KisPainter(KisDoc *doc);
  	~KisPainter();
  
protected:
	void toLayer();	
    void clear();

private:
  	QImage painterImage;
  	QPixmap painterPixmap;
  	QRect updateRect;

  	KisDoc *pDoc;
};

#endif

