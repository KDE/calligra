/*
 *  layer.cc - part of KImageShop
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

#include "layer.h"
#include "misc.h"

Layer::Layer(int ch, bool hasAlpha)
	: QObject()
{
	channels=ch;
	opacityVal=255;
	visible=true;
	linked=false;
  alphaChannel=hasAlpha;

     for(int c=(alphaChannel ? 0 : 1); c<=channels; c++) {
		channelPtrs[c]=new Channel(0,0);
     }
}

Layer::~Layer()
{

}

void
Layer::setOpacity(uchar o)
{
	opacityVal=o;
	//	emit layerPropertiesChanged();
}

void
Layer::loadRGBImage(QImage img, QImage alpha)
{
	if (img.depth()!=32)
		img=img.convertDepth(32);

	if (!alpha.isNull()) {
		if (alpha.depth()!=32)
			alpha=alpha.convertDepth(32);
		channelPtrs[0]->loadViaQImage(alpha,0);  // ie assume R=B=G as above
		alphaChannel=true;
	}
	channelPtrs[1]->loadViaQImage(img,0);
	channelPtrs[2]->loadViaQImage(img,1);
	channelPtrs[3]->loadViaQImage(img,2);

// 	if (alphaChannel) {puts("premultiplying alpha");
// 		for(int y=0;y<alpha.height();y++) {
// 			for(int x=0;x<alpha.width();x++) {
// 				uchar alpha=channelPtrs[0]->getPixel(x,y);
// 				channelPtrs[1]->setPixel(x,y, channelPtrs[1]->getPixel(x,y)*alpha/255);
// 				channelPtrs[2]->setPixel(x,y, channelPtrs[2]->getPixel(x,y)*alpha/255);
// 				channelPtrs[3]->setPixel(x,y, channelPtrs[3]->getPixel(x,y)*alpha/255);
// 			}
// 		}
// 	}

}

void
Layer::loadGrayImage(QImage img, QImage alpha)
{
	if (img.depth()!=32)
		img=img.convertDepth(32);

	if (!alpha.isNull()) {
		if (alpha.depth()!=32)
			alpha=alpha.convertDepth(32);
		channelPtrs[0]->loadViaQImage(alpha,0);  // ie assume R=B=G as above
	}
	channelPtrs[1]->loadViaQImage(img,0);
}


void
Layer::findTileNumberAndOffset(QPoint pt, int *tileNo, int *offset)
{
	pt=pt-channelPtrs[1]->tileExtents().topLeft();
	*tileNo=(pt.y()/TILE_SIZE)*xTiles() + pt.x()/TILE_SIZE;
	*offset=(pt.y()%TILE_SIZE)*TILE_SIZE + pt.x()%TILE_SIZE;
}

void
Layer::findTileNumberAndPos(QPoint pt, int *tileNo, int *x, int *y)
{
	pt=pt-channelPtrs[1]->tileExtents().topLeft();
	*tileNo=(pt.y()/TILE_SIZE)*xTiles() + pt.x()/TILE_SIZE;
	*y=pt.y()%TILE_SIZE;
	*x=pt.x()%TILE_SIZE;
}

uchar*
Layer::channelMem(int channel, int tileNo, int ox, int oy)
{
	return channelPtrs[channel]->tileBlock()[tileNo]+oy*TILE_SIZE+ox;
}


QRect
Layer::imageExtents() // Extents of the image in canvas coords
{
	return channelPtrs[1]->imageExtents();
}

QRect
Layer::tileExtents() // Extents of the image in canvas coords
{
	return channelPtrs[1]->tileExtents();
}

QPoint
Layer::channelOffset() // TopLeft of the image in the channel (not always 0,0)
{
	return channelPtrs[1]->offset();
}

int
Layer::xTiles()
{
	return channelPtrs[1]->xTiles();
}

int
Layer::yTiles()
{
	return channelPtrs[1]->yTiles();
}


void
Layer::moveBy(int dx, int dy)
{
	for(int c=(alphaChannel ? 0 : 1); c<=channels; c++)
		channelPtrs[c]->moveBy(dx, dy);	
}

void
Layer::moveTo(int x, int y)
{
	for(int c=(alphaChannel ? 0 : 1); c<=channels; c++)
		channelPtrs[c]->moveTo(x, y);	
}

int
Layer::channelLastTileOffsetX()
{
	return channelPtrs[1]->lastTileOffsetX();
}

int
Layer::channelLastTileOffsetY()
{
	return channelPtrs[1]->lastTileOffsetY();
}

bool
Layer::boundryTileX(int tile)
{
	return(((tile % xTiles())+1)==xTiles());
}

bool
Layer::boundryTileY(int tile)
{
	return(((tile/xTiles())+1)==yTiles());
}

void
Layer::resizeToIncludePoint(QPoint p)
{
	for(int c=(alphaChannel ? 0 : 1); c<=channels; c++)
		channelPtrs[c]->resizeChannel(p);
}

void
Layer::setPixel(int x, int y, uchar val)
{
	printf("layer::setPixel(%d,%d, %d)\n",x,y,val);
	for(int c=1; c<=channels; c++)
		channelPtrs[c]->setPixel(x,y, val);
}


#include "layer.moc"
