/*
 *  brush.h - part of KImageShop
 *
 *  Contains the imformation needed to describe a brush.
 *  Inherits the layer class.
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
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

#ifndef __brush_h__
#define __brush_h__

#include "layer.h"
#include "qsize.h"
#include "qpoint.h"

#include "iconitem.h"

class QImage;
class QPixmap;

class Brush : public Layer, public IconItem
{
 public:
  Brush(QString file);

  void 	 setSpacing(int s) 		{ spacingVal = s;    }
  int    spacing()   		const	{ return spacingVal; }
  QSize  brushSize() 		const	{ return sizeVal;    }
  bool   isValid()   		const	{ return validVal;   }
  void   setHotSpot(QPoint pt) 		{ hotSpotVal=pt;     } // XXX check in brush
  QPoint hotSpot()   		const	{ return hotSpotVal; }
  const QPixmap&  pixmap() 	const 	{ return pixmapVal;  }

 private:
  void loadBrush(QString file);
  void createPixmap(const QImage& img, const QImage& alpha);

  bool validVal;;
  int spacingVal;
  QSize sizeVal;
  QPoint hotSpotVal;
  QPixmap pixmapVal;
};

#endif

