//   layer widget  - handles the proterties and channels of a layer
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include "layer.h"
#include "misc.h"


layer::layer(int ch, bool hasAlpha=true)
	: QObject()
{
	channels=ch;
	opacityVal=255;
	visible=true;
	linked=false;
  alphaChannel=hasAlpha;

	for(int c=(alphaChannel ? 0 : 1); c<=channels; c++)
		channelPtrs[c]=new channel(0,0);
}

layer::~layer()
{

}

void
layer::setOpacity(uchar o)
{
	opacityVal=o;
	//	emit layerPropertiesChanged();
}

void
layer::loadRGBImage(QImage img, QImage alpha)
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
layer::loadGrayImage(QImage img, QImage alpha)
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
layer::findTileNumberAndOffset(QPoint pt, int *tileNo, int *offset)
{
	pt=pt-channelPtrs[1]->tileExtents().topLeft();
	*tileNo=(pt.y()/TILE_SIZE)*xTiles() + pt.x()/TILE_SIZE;
	*offset=(pt.y()%TILE_SIZE)*TILE_SIZE + pt.x()%TILE_SIZE;
}

void
layer::findTileNumberAndPos(QPoint pt, int *tileNo, int *x, int *y)
{
	pt=pt-channelPtrs[1]->tileExtents().topLeft();
	*tileNo=(pt.y()/TILE_SIZE)*xTiles() + pt.x()/TILE_SIZE;
	*y=pt.y()%TILE_SIZE;
	*x=pt.x()%TILE_SIZE;
}

uchar*
layer::channelMem(int channel, int tileNo, int ox, int oy)
{
	return channelPtrs[channel]->tileBlock()[tileNo]+oy*TILE_SIZE+ox;
}


QRect
layer::imageExtents() // Extents of the image in canvas coords
{
	return channelPtrs[1]->imageExtents();
}

QRect
layer::tileExtents() // Extents of the image in canvas coords
{
	return channelPtrs[1]->tileExtents();
}

QPoint
layer::channelOffset() // TopLeft of the image in the channel (not always 0,0)
{
	return channelPtrs[1]->offset();
}

int   
layer::xTiles() 
{
	return channelPtrs[1]->xTiles();
}

int   
layer::yTiles() 
{
	return channelPtrs[1]->yTiles();
}


void
layer::moveBy(int dx, int dy)
{
	for(int c=(alphaChannel ? 0 : 1); c<=channels; c++)
		channelPtrs[c]->moveBy(dx, dy);	
}

void
layer::moveTo(int x, int y)
{
	for(int c=(alphaChannel ? 0 : 1); c<=channels; c++)
		channelPtrs[c]->moveTo(x, y);	
}

int
layer::channelLastTileOffsetX()
{
	return channelPtrs[1]->lastTileOffsetX();
}

int
layer::channelLastTileOffsetY()
{
	return channelPtrs[1]->lastTileOffsetY();
}

bool
layer::boundryTileX(int tile)
{
	return(((tile % xTiles())+1)==xTiles());
}

bool
layer::boundryTileY(int tile)
{
	return(((tile/xTiles())+1)==yTiles());
}

void
layer::resizeToIncludePoint(QPoint p)
{
	for(int c=(alphaChannel ? 0 : 1); c<=channels; c++)
		channelPtrs[c]->resizeChannel(p);
}

void
layer::setPixel(int x, int y, uchar val)
{
	printf("layer::setPixel(%d,%d, %d)\n",x,y,val);
	for(int c=1; c<=channels; c++)
		channelPtrs[c]->setPixel(x,y, val);
}


#include "layer.mcc"
