/*
 *  channeldata.cc - part of KImageShop
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

#include "channeldata.h"
#include <qstring.h>
#include <qimage.h>
#include <stdio.h>
#include "misc.h"


ChannelData::ChannelData(int _numChannels, enum cSpace _cspace)
  : colorSpace (_cspace)
  , channels (_numChannels)
{
  xTilesNo = 0;
  yTilesNo = 0;
  tileInfo = 0;
}

void ChannelData::loadViaQImage(QImage img)
{
  qDebug("loadViaQImage %dx%d c=%d\n",img.width(),img.height(), channels);
  
  // slow but simple - speed it up.
  int w = img.width();
  int h = img.height();
  
  allocateRect(QRect(0,0, w,h));
  
  uint *imgPtr;
  for(int y=0;y<h;y++)
    {
      imgPtr=(uint*)img.scanLine(y);
      for(int x=0;x<w;x++)
	{
	  setPixel(x,y, *imgPtr++);
	}
    }
}

void ChannelData::setPixel(int x, int y, uint pixel)
{
  // assumes imageRect.contains(QPoint(x,y)) for speed

  // find the point in tile coordinates
  x = x - tilesRect.x();
  y = y - tilesRect.y();
  
  // find the tile
  int tileNo = (y / TILE_SIZE) * xTilesNo + x / TILE_SIZE;
  
  // get a pointer to the points tile data
  uchar *ptr = tileInfo[tileNo] + ((y % TILE_SIZE) * TILE_SIZE + x % TILE_SIZE) * channels;
  
  // we support 1 to 4 channels
  switch (channels)
    {
    case 1:
      *ptr = pixel;
      break;
    case 2:
      *((short*)ptr) = pixel;
      break;
    case 3:
      {
	uchar *pptr = (uchar*)&pixel;
	*ptr++ = *pptr++;
	*ptr++ = *pptr++;
	*ptr++ = *pptr++;
	break;
      }
    case 4:
      *((uint*)ptr)=pixel;
      break;
    }
}

uint ChannelData::getPixel(int x, int y)
{
  uint pixel;

  // assumes imageRect.contains(QPoint(x,y)) for speed
  
  // find the point in tile coordinates
  x = x - tilesRect.x();
  y = y - tilesRect.y();

  // find the tile
  int tileNo = (y / TILE_SIZE) * xTilesNo + x / TILE_SIZE;

  if (tileInfo[tileNo] == 0)
    return(0); // XXX fix this return some sort of undef (or bg) via KColor
  
  // get a pointer to the points tile data
  uchar *ptr = tileInfo[tileNo] + ((y % TILE_SIZE) * TILE_SIZE + x % TILE_SIZE) * channels;

  switch (channels)
    {
    case 1:
      pixel = (int)*ptr;
      break;
    case 2:
      pixel = *((short*)ptr);
      break;
    case 3:
      {
	uchar *pptr = (uchar*)&pixel;
	*pptr++ = *ptr++;
	*pptr++ = *ptr++;
	*pptr++ = *ptr++;
	break;
      }
    case 4:
      pixel = *((uint*)ptr);
      break;
    }
  return(pixel);
}

int ChannelData::lastTileOffsetX()
{
  int lastTileXOffset = TILE_SIZE - ( tilesRect.right() - imageRect.right());
  return((lastTileXOffset) ? lastTileXOffset :  TILE_SIZE);
}

int ChannelData::lastTileOffsetY()
{
  int lastTileYOffset = TILE_SIZE - (tilesRect.bottom() - imageRect.bottom());
  return((lastTileYOffset) ? lastTileYOffset :  TILE_SIZE);
}


// Resize the channel so that it includes the rectangle newRect (canvasCoords)
// and allocates space for all the pixels in newRect

void ChannelData::allocateRect(QRect newRect)
{
  if (newRect.isNull())
    return;
  
  if (!imageRect.contains(newRect))
    {
      if (!tilesRect.contains(newRect))
	{
	  puts("\nnew resizeChannel");
	  
	  // make a newImageExtents rect which contains imageRect and newRect
	  // now make it fall on the closest multiple of TILE_SIZE which contains it
	  if (tilesRect.isNull())
	    tilesRect=QRect(newRect.topLeft(),QSize(1,1));
	  QRect newTileExtents=tilesRect;
	  newTileExtents=newTileExtents.unite(newRect);
	  //SHOW_RECT(newTileExtents);
	  //SHOW_RECT_COORDS(newTileExtents);
	  
	  if (newTileExtents.left()<tilesRect.left())
	    newTileExtents.setLeft(tilesRect.left()-((tilesRect.left()-
						      newTileExtents.left()+TILE_SIZE-1)/TILE_SIZE)*TILE_SIZE);
	  if (newTileExtents.top()<tilesRect.top())
	    newTileExtents.setTop(tilesRect.top()-((tilesRect.top()-
						    newTileExtents.top()+TILE_SIZE-1)/TILE_SIZE)*TILE_SIZE);
	  
	  newTileExtents.setWidth(((newTileExtents.width()+TILE_SIZE-1)/
				   TILE_SIZE)*TILE_SIZE);
	  newTileExtents.setHeight(((newTileExtents.height()+TILE_SIZE-1)/
				    TILE_SIZE)*TILE_SIZE);
	  
	  //SHOW_RECT(newTileExtents);
	  //SHOW_RECT_COORDS(newTileExtents);
	  
	  int newXTiles=newTileExtents.width()/TILE_SIZE;
	  int newYTiles=newTileExtents.height()/TILE_SIZE;
	  
	  //showi(newXTiles);
	  //showi(newYTiles);
	  
	  // Allocate new tileInfo block an zero it
	  uchar **newTileInfo=new uchar* [newXTiles*newYTiles];
	  for(int yTile=0;yTile<newYTiles;yTile++) {
	    for(int xTile=0;xTile<newXTiles;xTile++) {
	      newTileInfo[yTile*newXTiles+xTile]=0;
	    }
	  }
	  
	  // copy the old descriptors to the new block and resize the image
	  // these are where the old tiles start in the new tile block
	  int oldXTilePos=(tilesRect.left()-newTileExtents.left())/TILE_SIZE;
	  int oldYTilePos=(tilesRect.top()-newTileExtents.top())/TILE_SIZE;
	  
	  // 	  int oldXTilePos=0, oldYTilePos=0;
	  // 	  if (newRect.x() < imageRect.x())
	  // 			oldXTilePos=newXTiles-xTilesNo;
	  // 	  if (newRect.y() < imageRect.y())
	  // 			oldYTilePos=newYTiles-yTilesNo;
	  
	  //puts("copying old descriptors");
	  //showi(oldXTilePos);
	  //showi(oldYTilePos);
			
	  // 			showi(xTilesNo);
	  // 			showi(yTilesNo);
	  
	  // Copy the old tile descriptors into the new array
	  for(int y=0;y<yTilesNo;y++)
	    for(int x=0;x<xTilesNo;x++)
	      newTileInfo[(y+oldYTilePos)*newXTiles+(x+oldXTilePos)]=
		tileInfo[y*xTilesNo+x];
	  
	  // 			SHOW_RECT_COORDS(imageRect);
	  
	  delete tileInfo;
	  
	  imageRect=imageRect.unite(newRect);
	  tileInfo=newTileInfo;
	  xTilesNo=newXTiles;
	  yTilesNo=newYTiles;
	  tilesRect=newTileExtents;
	  
	  //dumpTileBlock();
	  //SHOW_RECT(imageRect);
	  //SHOW_RECT(tilesRect);
	  //puts("resizeChannel2: fin");
	}
      else {
	imageRect=imageRect.unite(newRect);
      }
	}
  
  // Allocate any unallocated tiles in the newRect
  QRect allocRect=newRect;
  allocRect.moveBy(-tilesRect.x(), -tilesRect.y());
  int minYTile=allocRect.top()/TILE_SIZE;
  int maxYTile=allocRect.bottom()/TILE_SIZE;
  int minXTile=allocRect.left()/TILE_SIZE;
  int maxXTile=allocRect.right()/TILE_SIZE;
  
  for(int y=minYTile; y<=maxYTile; y++)
    for(int x=minXTile; x<=maxXTile; x++)
      if (tileInfo[(y*xTilesNo)+x]==0) {
	tileInfo[(y*xTilesNo)+x]=new uchar [channels*TILE_SIZE*TILE_SIZE];
	if (colorSpace==ALPHA)
	  memset(tileInfo[(y*xTilesNo)+x], 0, channels*TILE_SIZE*TILE_SIZE);
	else
	  memset(tileInfo[(y*xTilesNo)+x], 255, channels*TILE_SIZE*TILE_SIZE);
      }
}

void ChannelData::dumpTileBlock()
{
  puts("====dumpTileBlock====");
  SHOW_RECT(imageRect);
  SHOW_RECT(tilesRect);
  showi(xTilesNo);
  showi(yTilesNo);
  for(int y=0;y<yTilesNo;y++)
    {
      for(int x=0;x<xTilesNo;x++)
	printf("%08x ",(unsigned int)tileInfo[(y*xTilesNo)+x]);
      puts("");
    }
  puts("");
}

void ChannelData::moveBy(int dx, int dy)
{
  imageRect.moveBy(dx, dy);
  tilesRect.moveBy(dx, dy);
}

void ChannelData::moveTo(int x, int y)
{
  int dx=x-imageRect.x();
  int dy=y-imageRect.y();
  imageRect.moveTopLeft(QPoint(x, y));
  tilesRect.moveBy(dx,dy);
}

QRect ChannelData::tileRect(int tileNo)
{
  int xTile=tileNo%xTilesNo;
  int yTile=tileNo/xTilesNo;
  QRect tr(xTile*TILE_SIZE,yTile*TILE_SIZE, TILE_SIZE, TILE_SIZE);
  tr.moveBy(tilesRect.x(), tilesRect.y());
  return(tr);
}


void ChannelData::rotate180()
{
  uchar *tmp, tmpC;
  uchar **newTileInfo=new uchar* [xTilesNo*yTilesNo];
  
  int top=0, bot=xTilesNo*yTilesNo-1;	
  for(int tile=xTilesNo*yTilesNo; tile; tile--)
    newTileInfo[top++]=tileInfo[bot--];
  delete tileInfo;
  tileInfo=newTileInfo;
  
  for(int y=0; y<yTilesNo; y++) {
    for(int x=0; x<xTilesNo; x++) {
      tmp=tileInfo[y*xTilesNo+x];
      if (tmp) {
	uchar *top=tmp;
	uchar *bot=tmp+TILE_SIZE*TILE_SIZE*channels-channels;
	for(int pix=TILE_SIZE*TILE_SIZE/2; pix; pix--) {
	  for(int c=channels; c; c--) {
	    tmpC=*top;
	    *top++=*bot;
	    *bot++=tmpC;
	  }
	  bot-=2*channels;
	}
      }
    }
  }
  
  tilesRect.moveBy(imageRect.left()-tilesRect.left() -
		   (tilesRect.right()-imageRect.right()),
		   imageRect.top()-tilesRect.top() -
		   (tilesRect.bottom()-imageRect.bottom()));
}

void ChannelData::rotateRight90()
{
  uchar **newTileInfo=new uchar* [xTilesNo*yTilesNo];
  int tmpI;

  for(int y=0; y<yTilesNo; y++)
    for(int x=0; x<xTilesNo; x++)
      newTileInfo[x*yTilesNo + yTilesNo-1-y] = tileInfo[y*xTilesNo+x];
  tmpI=xTilesNo;
  xTilesNo=yTilesNo;
  yTilesNo=tmpI;
  delete tileInfo;
  tileInfo=newTileInfo;
  
  uchar *newBlk=new uchar[TILE_SIZE*TILE_SIZE*channels];
  
  for(int y=0; y<yTilesNo; y++) {
    for(int x=0; x<xTilesNo; x++) {
      uchar *tmp=tileInfo[y*xTilesNo+x];
      if (tmp) {
	for(int yp=0; yp<TILE_SIZE; yp++)
	  for(int xp=0; xp<TILE_SIZE; xp++)
	    for(int c=0; c<channels; c++) {
	      *(newBlk+((xp+1)*TILE_SIZE-1-yp)*channels+c) = 
		*(tmp+(yp*TILE_SIZE+xp)*channels+c);
						}
	tileInfo[y*xTilesNo+x]=newBlk;
				newBlk=tmp;
      }
    }
  }
  delete newBlk;
  
  int newOffX=tilesRect.bottom()-imageRect.bottom();
  int newOffY=imageRect.left()-tilesRect.left();
  
  imageRect.setSize(QSize(imageRect.height(), imageRect.width()));
  
  tilesRect=QRect(imageRect.x()-newOffX, imageRect.y()-newOffY,
		  tilesRect.height(), tilesRect.width());
}

void ChannelData::rotateLeft90()
{
  uchar **newTileInfo=new uchar* [xTilesNo*yTilesNo];
  int tmpI;
  
  for(int y=0; y<yTilesNo; y++)
    for(int x=0; x<xTilesNo; x++)
      newTileInfo[(xTilesNo-1-x)*yTilesNo + y] = tileInfo[y*xTilesNo+x];
  tmpI=xTilesNo;
  xTilesNo=yTilesNo;
  yTilesNo=tmpI;
  delete tileInfo;
  tileInfo=newTileInfo;
  
  uchar *newBlk=new uchar[TILE_SIZE*TILE_SIZE*channels];
  
  for(int y=0; y<yTilesNo; y++) {
    for(int x=0; x<xTilesNo; x++) {
      uchar *tmp=tileInfo[y*xTilesNo+x];
      if (tmp) {
	for(int yp=0; yp<TILE_SIZE; yp++)
	  for(int xp=0; xp<TILE_SIZE; xp++)
	    for(int c=0; c<channels; c++) {
	      *(newBlk+((TILE_SIZE-1-xp)*TILE_SIZE+yp)*channels+c) = 
		*(tmp+(yp*TILE_SIZE+xp)*channels+c);
	    }
	tileInfo[y*xTilesNo+x]=newBlk;
	newBlk=tmp;
      }
    }
	}
  delete newBlk;
  
  int newOffX=imageRect.top()-tilesRect.top();
  int newOffY=tilesRect.right()-imageRect.right();
  
  imageRect.setSize(QSize(imageRect.height(), imageRect.width()));
  
  tilesRect=QRect(imageRect.x()-newOffX, imageRect.y()-newOffY,
		  tilesRect.height(), tilesRect.width());
}

void ChannelData::mirrorX()
{
  uchar *tmp, tmpC;
  
  
  for(int y=0; y<yTilesNo; y++) {
    for(int x=0; x<=(xTilesNo-1)/2; x++) {
      tmp=tileInfo[y*xTilesNo+x];
      tileInfo[y*xTilesNo+x]=tileInfo[(y+1)*xTilesNo-x-1];
      tileInfo[(y+1)*xTilesNo-x-1]=tmp;
    }
  }
  
  for(int y=0; y<yTilesNo; y++) {
    for(int x=0; x<xTilesNo; x++) {
      tmp=tileInfo[y*xTilesNo+x];
      if (tmp) {
	for(int line=0; line<TILE_SIZE; line++) {
	  uchar *left =tmp+line*TILE_SIZE*channels;
	  uchar *right=left+(TILE_SIZE-1)*channels;
	  for(int i=TILE_SIZE/2; i; i--) {
	    for(int c=channels; c ; c--) {
	      tmpC=*left;
	      *left++=*right;
	      *right++=tmpC;
	    }
	    right-=2*channels;
	  }
	}
      }
    }
  }
  tilesRect.moveBy(imageRect.left()-tilesRect.left() -
		   (tilesRect.right()-imageRect.right()) , 0);
}

void ChannelData::mirrorY()
{
  uchar *tmp, buf[TILE_SIZE*channels];
  
  for(int y=0; y<=(yTilesNo-1)/2; y++) {
    for(int x=0; x<xTilesNo; x++) {
      tmp=tileInfo[y*xTilesNo+x];
      tileInfo[y*xTilesNo+x]=tileInfo[(yTilesNo-y-1)*xTilesNo+x];
      tileInfo[(yTilesNo-y-1)*xTilesNo+x]=tmp;
    }
  }
  
  for(int y=0; y<yTilesNo; y++) {
    for(int x=0; x<xTilesNo; x++) {
      tmp=tileInfo[y*xTilesNo+x];
      if (tmp) {
	for(int line=0; line<TILE_SIZE/2; line++) {
	  uchar *top=tmp+line*TILE_SIZE*channels;
	  uchar *bot=tmp+(TILE_SIZE-line-1)*TILE_SIZE*channels;
	  memcpy(buf, top, TILE_SIZE*channels);
	  memcpy(top, bot, TILE_SIZE*channels);
	  memcpy(bot, buf, TILE_SIZE*channels);
	}
      }
    }
  }
  
  tilesRect.moveBy(0, imageRect.top()-tilesRect.top() -
		   (tilesRect.bottom()-imageRect.bottom()));
}
