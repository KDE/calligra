/*
 *  kis_layer.cc - part of KImageShop
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

#include "kis_layer.h"
#include "kis_util.h"

KisLayer::KisLayer(int ch)
	: QObject()
{
  channels=ch;
  opacityVal=255;
  visible=true;
  linked=false;
  
  dataChannels=new KisChannelData(ch, KisChannelData::RGB);
  alphaChannel=new KisChannelData( 1, KisChannelData::ALPHA);
}

KisLayer::~KisLayer()
{
  delete dataChannels;
  delete alphaChannel;
}

void KisLayer::clear(const QColor& c)
{
  uchar r = static_cast<uchar>(c.red());
  uchar g = static_cast<uchar>(c.green());
  uchar b = static_cast<uchar>(c.blue());

  for(int y = 0; y < yTiles(); y++)
    for(int x = 0; x < xTiles(); x++)
      {
	// set the alpha channel to opaque
	memset(channelMem(y * xTiles() + x, 0,0, true),255, TILE_SIZE*TILE_SIZE);
	
	uchar* data = channelMem(y * xTiles() + x, 0,0, false);
	
	// set data channels to color
	for(int y = 0; y < TILE_SIZE; y++)
	  for(int x = 0; x < TILE_SIZE; x++)
	    {
	      *(data + 3 * (y*TILE_SIZE+x)) = b;
	      *(data + 3 * (y*TILE_SIZE+x)+1) = g;
	      *(data + 3 * (y*TILE_SIZE+x)+2) = r;
	    }
	
      }
}

void KisLayer::setOpacity(uchar o)
{
  opacityVal=o;
  //	emit layerPropertiesChanged();
}


void KisLayer::loadRGBImage(QImage img, QImage alpha)
{
  qDebug("loadRGBImage img=(%d,%d) alpha=(%d,%d)\n",img.width(),img.height(),
	 alpha.width(),alpha.height());
  if (img.depth()!=32)
    img=img.convertDepth(32);
  
  if (!alpha.isNull()) {
    if (alpha.depth()!=32)
      alpha=alpha.convertDepth(32);
    alphaChannel->loadViaQImage(alpha);
  } else {
    alphaChannel->allocateRect(QRect(QPoint(0,0),img.size()));
    for(int y=0;y<img.height();y++)
      for(int x=0;x<img.width();x++)
	alphaChannel->setPixel(x,y,255);
  }
  dataChannels->loadViaQImage(img);
}


void KisLayer::loadGrayImage(QImage img, QImage alpha)
{
  if (img.depth()!=32)
    img=img.convertDepth(32);
  
  if (!alpha.isNull()) {
    if (alpha.depth()!=32)
      alpha=alpha.convertDepth(32);
    alphaChannel->loadViaQImage(alpha);  // ie assume R=B=G as above
  }
  dataChannels->loadViaQImage(img);
}


void KisLayer::findTileNumberAndOffset(QPoint pt, int *tileNo, int *offset) const
{
  pt=pt-dataChannels->tileExtents().topLeft();
  *tileNo=(pt.y()/TILE_SIZE)*xTiles() + pt.x()/TILE_SIZE;
  *offset=(pt.y()%TILE_SIZE)*TILE_SIZE + pt.x()%TILE_SIZE;
}

void KisLayer::findTileNumberAndPos(QPoint pt, int *tileNo, int *x, int *y) const
{
  pt=pt-dataChannels->tileExtents().topLeft();
  *tileNo=(pt.y()/TILE_SIZE)*xTiles() + pt.x()/TILE_SIZE;
  *y=pt.y()%TILE_SIZE;
  *x=pt.x()%TILE_SIZE;
}

QRect KisLayer::tileRect(int tileNo)
{
  return(dataChannels->tileRect(tileNo));
}

uchar* KisLayer::channelMem(int tileNo, int ox, int oy, bool alpha) const
{
  if (alpha)
    return alphaChannel->tileBlock()[tileNo]+(oy*TILE_SIZE+ox);
  
  return dataChannels->tileBlock()[tileNo]+(oy*TILE_SIZE+ox)*channels;
}


QRect KisLayer::imageExtents() const // Extents of the image in canvas coords
{
  return dataChannels->imageExtents();
}

QRect KisLayer::tileExtents() const// Extents of the image in canvas coords
{
  return dataChannels->tileExtents();
}

// TopLeft of the image in the channel (not always 0,0)
QPoint KisLayer::channelOffset() const 
{
  return dataChannels->offset();
}

int KisLayer::xTiles() const
{
  return dataChannels->xTiles();
}

int KisLayer::yTiles() const
{
  return dataChannels->yTiles();
}


void KisLayer::moveBy(int dx, int dy)
{
  alphaChannel->moveBy(dx, dy);	
  dataChannels->moveBy(dx, dy);	
}

void KisLayer::moveTo(int x, int y) const
{
  alphaChannel->moveTo(x, y);	
  dataChannels->moveTo(x, y);	
}

int KisLayer::channelLastTileOffsetX() const
{
  return dataChannels->lastTileOffsetX();
}

int KisLayer::channelLastTileOffsetY() const
{
  return dataChannels->lastTileOffsetY();
}

bool KisLayer::boundryTileX(int tile) const
{
  return(((tile % xTiles())+1)==xTiles());
}

bool KisLayer::boundryTileY(int tile) const
{
  return(((tile/xTiles())+1)==yTiles());
}

void KisLayer::allocateRect(QRect _r)
{
  alphaChannel->allocateRect(_r);
  dataChannels->allocateRect(_r);
}

void KisLayer::setPixel(int x, int y, uint pixel)
{
  dataChannels->setPixel(x,y, pixel);
}

uint KisLayer::getPixel(int x, int y)
{
  return dataChannels->getPixel(x,y);
}

void KisLayer::setAlpha(int x, int y, uint pixel)
{
  alphaChannel->setPixel(x,y, pixel);
}

uint KisLayer::getAlpha(int x, int y)
{
  return alphaChannel->getPixel(x,y);
}

void KisLayer::rotate180()
{
  alphaChannel->rotate180();
  dataChannels->rotate180();
}

void KisLayer::rotateLeft90()
{
  alphaChannel->rotateLeft90();
  dataChannels->rotateLeft90();
}

void KisLayer::rotateRight90()
{
  alphaChannel->rotateRight90();
  dataChannels->rotateRight90();
}

void KisLayer::mirrorX()
{
  alphaChannel->mirrorX();
  dataChannels->mirrorX();
}

void KisLayer::mirrorY()
{
  alphaChannel->mirrorY();
  dataChannels->mirrorY();
}

void KisLayer::renderOpacityToAlpha()
{
  uchar *alpha;
  int xt = xTiles();
  int yt = yTiles();
  
  for(int y = 0; y < yt; y++)
    {
      for(int x = 0; x < xt; x++)
	{
	  alpha = channelMem(y * xt + x, 0, 0, true);
	  
	  for(int y = TILE_SIZE; y; y--)
	    {
	      for(int x = TILE_SIZE; x; x--)
		{
		  *alpha++ = static_cast<uchar>(*alpha - (255 - opacityVal));
		}
	    }
	}
    }
  opacityVal = 255;
}


#include "kis_layer.moc"
