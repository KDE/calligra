/*
 *  brush.cc - part of KImageShop
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

#include "brush.h"
#include <qbitmap.h>
#include <qpixmap.h>
#include <qregexp.h>
#include <stdio.h>

Brush::Brush(QString file) : Layer(3), IconItem()
{
  validVal=true;
  loadBrush(file);
  spacingVal=5;
	// default hotSpot in the centre
  hotSpotVal=QPoint(imageExtents().width()/2, imageExtents().height()/2);
}

void Brush::loadBrush(QString file)
{
  printf("brush::loadBrush: %s\n",file.latin1());
  QImage img(file);
  if (img.isNull())
    {
      printf("Unable to load image: %s\n",file.latin1());
      validVal=false;
      return;
    }

  // XXX currently assumes the alpha image IS a greyscale and the same size as
  // the other channels
  QString alphaName=file;
  alphaName.replace(QRegExp("\\.jpg$"),"-alpha.jpg");
  printf("brush::loadBrushAlpha: %s\n",alphaName.latin1());
  QImage alpha(alphaName);
  if (!alpha.isNull() && (img.size()!=alpha.size()))
    {
      puts("Incorrect sized alpha channel - not loaded");
      alpha=QImage();
      validVal=false;
      return;
    }
  loadRGBImage(img, alpha);
  sizeVal=img.size();
  createPixmap(img, alpha);
}


void Brush::createPixmap(const QImage& img, const QImage& alpha)
{
  pixmapVal = img;
  QBitmap bm;
  bm = alpha;
  pixmapVal.setMask( bm );
}
