//   channel class - handles properties and memory allocation for channels
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include "channel.h"
#include <qstring.h>
#include <qimage.h>
#include <stdio.h>
#include "misc.h"

Channel::Channel(int w, int h)
{
// 	puts("new channel");
	xTilesNo=0;
	yTilesNo=0;
	tileInfo=0;
	imageRect=QRect(0,0, w,h);
	tilesRect=QRect(0,0, 0,0);
	offsetPoint=QPoint(0,0);
	//resizeChannel(QPoint(),true);
}

void
Channel::loadViaQImage(QImage img, int channel)
{
	// printf("loadViaQImage %dx%d\n",img.width(),img.height());

	// XXX assumes the channel is already correctly setup
	// slow but simple - speed it up.
 	int w=img.width();
 	int h=img.height();
	
	resizeChannel(QPoint(w-1,h-1));
	
	//	SHOW_RECT(imageRect);
	
	uchar *imgPtr;
 	for(int y=0;y<h;y++) {
		imgPtr=img.scanLine(y)+channel;
 		for(int x=0;x<w;x++) {
 			setPixel(x,y, *imgPtr);
			imgPtr+=4;
		}
	}
}


void
Channel::setPixel(int x, int y, uchar val)
{
	// assumes offset of 0,0 for top left 
	// XXX and that x,y is inside the imageRect
	int tileNo=(y/TILE_SIZE)*xTilesNo + x/TILE_SIZE;
	if (tileInfo[tileNo]==0)
		tileInfo[tileNo]=new uchar[TILE_SIZE*TILE_SIZE];
	*(tileInfo[tileNo]+(y%TILE_SIZE)*TILE_SIZE + x%TILE_SIZE)=val;
}

uchar
Channel::getPixel(int x, int y)
{
	// assumes offset of 0,0 for top left 
	// XXX and that x,y is inside the imageRect
	int tileNo=(y/TILE_SIZE)*xTilesNo + x/TILE_SIZE;
	if (tileInfo[tileNo]==0)
		return(0); // XXX fix this
	return(*(tileInfo[tileNo]+(y%TILE_SIZE)*TILE_SIZE + x%TILE_SIZE));
}


// Resize the channel so that it includes the point newPoint
//    init - is used only when the channel is created

void
Channel::resizeChannel(QPoint newPoint, bool init)
{
	if ((!init) && imageRect.contains(newPoint))
		return;

//   	puts("resizeChannel");
// 		showi(init);
//   	SHOW_POINT(newPoint);
//   	SHOW_RECT_COORDS(imageRect);
//   	SHOW_RECT_COORDS(tilesRect);
// 	showi(xTilesNo);
// 	showi(yTilesNo);

	if (imageRect.isNull())
		imageRect=QRect(0,0,1,1);
	if (tilesRect.isNull())
		tilesRect=QRect(0,0,1,1);

	if (!tilesRect.contains(newPoint)) {
		// make a newImageExtents rect which contains imageRect and newPoint
		// now make it fall on the closest multiple of TILE_SIZE which contains it
		QRect newTileExtents=tilesRect;
		if (init)
			newTileExtents=imageRect;
		else
			newTileExtents=newTileExtents.unite(QRect(newPoint,newPoint));
		newTileExtents=::findTileExtents(newTileExtents);		
 		//SHOW_RECT_COORDS(newTileExtents);

		int newXTiles=newTileExtents.width()/TILE_SIZE;
		int newYTiles=newTileExtents.height()/TILE_SIZE;

// 		showi(newXTiles);
// 		showi(newYTiles);
		
		// Allocate new tileInfo block an zero it
		uchar **newTileInfo=new uchar* [newXTiles*newYTiles];
		for(int yTile=0;yTile<newYTiles;yTile++) {
			for(int xTile=0;xTile<newXTiles;xTile++) {
				newTileInfo[yTile*newXTiles+xTile]=0;
			}
		}

		// If not an initialisation then copy the old descriptors to the new
		// descriptor block and resize the image
		if (!init) {
			// These are where the old tiles start in the new tile block
			int oldXTilePos=0, oldYTilePos=0;
			if (newPoint.x() < imageRect.x())
				oldXTilePos=newXTiles-xTilesNo;
			if (newPoint.y() < imageRect.y())
				oldYTilePos=newYTiles-yTilesNo;
			
// 			puts("copying old descriptors");
// 			showi(oldXTilePos);
// 			showi(oldYTilePos);
			
// 			showi(xTilesNo);
// 			showi(yTilesNo);
			// Copy the old tile descriptors into the new array
			for(int y=0;y<yTilesNo;y++)
				for(int x=0;x<xTilesNo;x++)
					newTileInfo[(y+oldYTilePos)*xTilesNo+(x+oldXTilePos)]=
						tileInfo[y*xTilesNo+x];
			
// 			SHOW_RECT_COORDS(imageRect);
			
			delete tileInfo;
		}
		imageRect=imageRect.unite(QRect(newPoint,newPoint));
		tileInfo=newTileInfo;
		xTilesNo=newXTiles;
		yTilesNo=newYTiles;
		tilesRect=newTileExtents;
		lastTileXOffset=(imageRect.width()+offsetPoint.x())%TILE_SIZE;
		lastTileYOffset=(imageRect.height()+offsetPoint.y())%TILE_SIZE;
 	} else {
		puts("point already within tile enlarging image");
		imageRect=imageRect.unite(QRect(newPoint,newPoint));
	}
		

// 	SHOW_RECT(imageRect);
// 	SHOW_RECT(tilesRect);
	//	dumpTileBlock();
}


void
Channel::dumpTileBlock()
{
	puts("====dumpTileBlock====");
	SHOW_RECT_COORDS(imageRect);
	SHOW_RECT_COORDS(tilesRect);
	showi(xTilesNo);
	showi(yTilesNo);
	for(int y=0;y<yTilesNo;y++) {
		for(int x=0;x<xTilesNo;x++)
			printf("%08x ",(unsigned int)tileInfo[(y*xTilesNo)+x]);
		puts("");
	}
	puts("");
	printf("mx=%d my=%d\n", lastTileXOffset, lastTileYOffset);
}

void
Channel::moveBy(int dx, int dy)
{
	imageRect.moveBy(dx, dy);
	//puts("channel::moveBy");
	//SHOW_RECT_COORDS(imageRect);
}

void
Channel::moveTo(int x, int y)
{
	// XXX adapt for the case where the image starts at (>0,>0)
	imageRect.moveTopLeft(QPoint(x, y));
}

