/*
 *  canvas.cc - part of KImageShop
 *
 *  Handles compositing the image and drawing to layers.
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *                1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <mkoch@kde.org>
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

#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include <sys/time.h>
#include <unistd.h>                                                      

#include <qpainter.h>
#include <qwidget.h>
#include <qregexp.h>
#include <qfileinfo.h>

#include "layer.h"
#include "misc.h"
#include "canvas.h"

#define KIS_DEBUG(AREA, CMD)

Canvas::Canvas(int width, int height)
{
  w=width;
  h=height;
  viewportRect=QRect(0,0,w,h);
  // SHOW_RECT_COORDS(viewportRect);
  
  QRect tileExtents=::findTileExtents(viewportRect);
  // SHOW_RECT(tileExtents);
  xTiles=tileExtents.width()/TILE_SIZE;	
  yTiles=tileExtents.height()/TILE_SIZE;
  // showi(xTiles);
  // showi(yTiles);
  
  setUpVisual();
  QPixmap::setDefaultOptimization(QPixmap::NoOptim);
  
  tiles=new QPixmap* [xTiles*yTiles];
  
  for(int y=0;y<yTiles;y++)
    for(int x=0;x<xTiles;x++) {
      tiles[y*xTiles+x]=new QPixmap(TILE_SIZE,TILE_SIZE);
      tiles[y*xTiles+x]->fill();
    }
  img.create(TILE_SIZE, TILE_SIZE, 32);
  
  channels=3;
  currentLayer=0;
  
  compose=new Layer(channels, false);
  compose->resizeToIncludePoint(QPoint(TILE_SIZE-1,TILE_SIZE-1));
  compose->setPixel(1,1, 0);
  
  // make this like the compose layer
  for(int c=1;c<=channels;c++) {
    background[c]=new uchar[TILE_SIZE*TILE_SIZE];
    for(int y=0;y<TILE_SIZE;y++)
      for(int x=0;x<TILE_SIZE;x++)
	*(background[c]+y*TILE_SIZE+x)=128+63*((x/16+y/16)%2);
  }
}

Canvas::~Canvas()
{
  // XXX delete individual tiles
  delete tiles;
  
  if ((visual!=unknown) && (visual!=rgb888x))
    free(imageData);
}

int Canvas::height()
{
  return h;
}

int Canvas::width()
{
  return w;
}

void Canvas::paintPixmap(QWidget *w, QRect area, QPoint offset, QPoint paintOffset, float zoomFactor)
{
  if (!w)
    return;

 int startX, startY, pixX, pixY = 0;
 int pixW, pixH = -1;
  
 QPainter p;
 p.begin(w);
 p.scale(zoomFactor,zoomFactor);

  // XXX clip it correctly
  // XXX paint only what is needed
  
 for(int y=0;y<yTiles;y++)
    {
      for(int x=0;x<xTiles;x++)
	{
	  QRect tileRect(x*TILE_SIZE, y*TILE_SIZE,TILE_SIZE,TILE_SIZE);
	  if (viewportRect.intersects(tileRect) && tileRect.intersects(area))
	    {
	      if (!tiles[y*xTiles+x]->isNull())
		{
		  if (x*TILE_SIZE < offset.x())
		    {
		      startX = 0;
		      pixX = offset.x() - x*TILE_SIZE;
		    }
		  else
		    {
		      startX = x*TILE_SIZE - offset.x();
		      pixX = 0;
		    }

		  if (y*TILE_SIZE < offset.y())
		    {
		      startY = 0;
		      pixY = offset.y() - y*TILE_SIZE;
		    }
		  else
		    {
		      startY = y*TILE_SIZE - offset.y();
		      pixY = 0;
		    }

		  /*if (x*TILE_SIZE > (offset.x() + area.width()))
		    pixW = x*TILE_SIZE - offset.x() - area.width();
		  else
		    pixW = -1;

		  if (y*TILE_SIZE > (offset.y() + area.height()))
		    pixH = y*TILE_SIZE - offset.y() - area.height();
		  else
		    pixH = -1;
		  */
		  startX += static_cast<int>(paintOffset.x()/zoomFactor);
		  startY += static_cast<int>(paintOffset.y()/zoomFactor);

		  p.drawPixmap(startX, startY, *tiles[y*xTiles+x], pixX, pixY);//, pixW, pixH);
		}
	      else
		puts("Null: not rendering");
	    }
	  //p.drawRect(tileRect);
	}
    }
  p.end();
}

void Canvas::setUpVisual()
{
  QPixmap p;
  Display *dpy    =p.x11Display();
  int displayDepth=p.x11Depth();
  Visual *vis     =(Visual*)p.x11Visual();
  bool trueColour = (vis->c_class == TrueColor);
  
  // change the false to true to test the faster image converters
  visual=unknown;
  if (true && trueColour) { // do they have a worthy display
    uint red_mask  =(uint)vis->red_mask;
    uint green_mask=(uint)vis->green_mask;
    uint blue_mask =(uint)vis->blue_mask;
    
    if ((red_mask==0xf800) && (green_mask==0x7e0) && (blue_mask==0x1f))
      visual=rgb565;
    if ((red_mask==0xff0000) && (green_mask==0xff00) && (blue_mask==0xff))
      visual=rgb888x;
    
    if (visual==unknown) {
      puts("Unoptimized visual - want to write an optimised routine?");
      printf("red=%8x green=%8x blue=%8x\n",red_mask,green_mask,blue_mask);
    } else {
      puts("Using optimized visual");
      xi=XCreateImage( dpy, vis, displayDepth, ZPixmap, 0,0, TILE_SIZE,TILE_SIZE, 32, 0 );
      printf("ximage: bytes_per_line=%d\n",xi->bytes_per_line);
      if (visual!=rgb888x) {
	imageData=new char[xi->bytes_per_line*TILE_SIZE];
	xi->data=imageData;
      }
    }
  }
}	

void Canvas::addRGBLayer(QString file)
{
  DEBUG("Canvas::addRGBLayer: %s\n",file.latin1());

  QImage img(file);
  if (img.isNull()) {
    DEBUG("Unable to load image: %s\n",file.latin1());
    return;
  }
  img=img.convertDepth(32);
  
  // XXX currently assumes the alpha image IS a greyscale and the same size as
  // the other channels
  QString alphaName=file;
  alphaName.replace(QRegExp("\\.jpg$"),"-alpha.jpg");

  DEBUG("Canvas::addRGBLayerAlpha: %s\n",alphaName.latin1());

  QImage alpha(alphaName);
  if (!alpha.isNull() && (img.size()!=alpha.size())) {
    puts("Incorrect sized alpha channel - not loaded");
    alpha=QImage();
  }
  
  Layer *lay=new Layer(3, !alpha.isNull());
  lay->setName(QFileInfo(file).fileName());
  
  lay->loadRGBImage(img, alpha);
  layers.append(lay);
  currentLayer=lay;
}


// Constructs the composite image in the tile at x,y and updates the relevant
// pixmap
void Canvas::compositeTile(int x, int y, Layer *dstLay, int dstTile)
{
  // work out which tile to render into unless directed to a specific tile
  if (dstTile==-1)
    dstTile=y*xTiles+x;
  if (dstLay==0)
    dstLay=compose;
  
  KIS_DEBUG(tile, printf("\n*** compositeTile %d,%d\n",x,y); );
  
  //printf("compositeTile: dstLay=%p dstTile=%d\n",dstLay, dstTile);
  
  // Set the background
  for(int c=1; c<=channels; c++)
    memcpy(dstLay->channelMem(c,dstTile,0,0), background[c], 
	   TILE_SIZE*TILE_SIZE);
  
  // Find the tiles boundary in Canvas coordinates
  QRect tileBoundary(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
  
  int l=0;
  Layer *lay=layers.first();
  while(lay) { // Go through each layer and find its contribution to this tile
    l++;
    //printf("layer: %s opacity=%d\n",lay->name().data(), lay->opacity());
    if ((lay->isVisible()) && 
	(tileBoundary.intersects(lay->imageExtents()))) {
      // The layer is part of the tile. Find out the 1-4 tiles of the channel
      // which are in it and render the appropriate proportions of each
      //TIME_START;
      //printf("*** compositeTile %d,%d\n",x,y);
      renderLayerIntoTile(tileBoundary, lay, dstLay, dstTile);
      //TIME_END("renderLayerIntoTile");
    }
    lay=layers.next();
  }
}

void Canvas::compositeImage(QRect r)
{
  TIME_START;
  for(int y=0;y<yTiles;y++)
    for(int x=0;x<xTiles;x++)
      if (r.isNull() || 
	  r.intersects(QRect(x*TILE_SIZE, y*TILE_SIZE,TILE_SIZE,TILE_SIZE))) {
	compositeTile(x,y, compose, 0);
	
	convertTileToPixmap(compose, 0, tiles[y*xTiles+x]);
      }
  TIME_END("compositeImage");
}

void Canvas::renderLayerIntoTile(QRect tileBoundary, Layer *srcLay, Layer *dstLay,
				 int dstTile)
{
  // calculate the position in the layer of the topLeft of the drawing tile
  QPoint layerPoint=tileBoundary.topLeft()-srcLay->imageExtents().topLeft()
    + srcLay->channelOffset();
  
  int tileNo, tileOffsetX, tileOffsetY, xTile, yTile;
  srcLay->findTileNumberAndPos(layerPoint, &tileNo, 
			       &tileOffsetX, &tileOffsetY);
  xTile=tileNo%srcLay->xTiles();
  yTile=tileNo/srcLay->xTiles();
  KIS_DEBUG(render, showi(tileNo); );
  bool renderQ1=true, renderQ2=true, renderQ3=true, renderQ4=true;
  if (tileOffsetX<0)
    renderQ1=renderQ3=false;
  if (tileOffsetY<0)
    renderQ2=renderQ1=false;
  
  KIS_DEBUG(render, showi(tileOffsetX); );
  KIS_DEBUG(render, showi(tileOffsetY); );
  
  int maxLayerX=TILE_SIZE, maxLayerY=TILE_SIZE;
  if (srcLay->boundryTileX(tileNo)) {
    maxLayerX=srcLay->channelLastTileOffsetX();
    if (tileOffsetX>=0)
      renderQ2=false;
    KIS_DEBUG(render, showi(maxLayerX); );
  }
  if (tileOffsetX==0)
    renderQ4=false;
  
  if (srcLay->boundryTileY(tileNo)) {
    maxLayerY=srcLay->channelLastTileOffsetY();
		if (tileOffsetY>=0)
		  renderQ3=false;
		KIS_DEBUG(render, showi(maxLayerX); );
  }
  if (tileOffsetY==0)
    renderQ4=false;
  
  
  KIS_DEBUG(render, showi(renderQ1); );
  KIS_DEBUG(render, showi(renderQ2); );
  KIS_DEBUG(render, showi(renderQ3); );
  KIS_DEBUG(render, showi(renderQ4); );
  
  // Render quadrants of each tile (either 1, 2 or 4 quadrants get rendered)
  // 
  //  ---------
  //  | 1 | 2 |
  //  ---------
  //  | 3 | 4 |
  //  ---------
  //
  
  dbg=false;
  
  KIS_DEBUG(render, {
    SHOW_POINT(tileBoundary.topLeft());
    SHOW_POINT(layerPoint);
    printf("tileNo %d, tileOffsetX %d, tileOffsetY %d\n",
	   tileNo, tileOffsetX, tileOffsetY);
  });

  int renderedToX, renderedToY;
  
  KIS_DEBUG(render, printf("Test 1: "); );
  if (renderQ1) {
    // true => render 1
    renderTileQuadrant(srcLay, tileNo, dstLay, dstTile, 
		       tileOffsetX, tileOffsetY, 0, 0,
		       TILE_SIZE, TILE_SIZE);
    renderedToX=maxLayerX-tileOffsetX;
    renderedToY=maxLayerY-tileOffsetY;
  } else
    KIS_DEBUG(render, puts("ignore"); );
  
  KIS_DEBUG(render, printf("Test 2:"); );
  if (renderQ2) {
    // true => render 2
    if (renderQ1)
      renderTileQuadrant(srcLay, tileNo+1, dstLay, dstTile, 
			 0, tileOffsetY, maxLayerX-tileOffsetX, 0,
			 TILE_SIZE, TILE_SIZE);
    else
      renderTileQuadrant(srcLay, tileNo, dstLay, dstTile, 
			 0, tileOffsetY, -tileOffsetX,0,
			 TILE_SIZE, TILE_SIZE);
  } else
    KIS_DEBUG(render, puts("ignore"));
  
  KIS_DEBUG(render, printf("Test 3:"); );
  if (renderQ3) {
    // true => render 3
    if (renderQ1)
      renderTileQuadrant(srcLay, tileNo+srcLay->xTiles(), dstLay, dstTile, 
			 tileOffsetX, 0, 0, maxLayerY-tileOffsetY, 
			 TILE_SIZE, TILE_SIZE);
    else
      renderTileQuadrant(srcLay, tileNo, dstLay, dstTile, 
			 tileOffsetX, 0, 0, -tileOffsetY, 
			 TILE_SIZE, TILE_SIZE);
  } else
    KIS_DEBUG(render, puts("ignore"); );
  
  KIS_DEBUG(render, printf("Test 4:"); );
  // true => render 4
  if (renderQ4) {
    int newTile=tileNo;
    KIS_DEBUG(render, showi(xTile); );
    KIS_DEBUG(render, showi(yTile); );
    if (renderQ1) {
      xTile++; yTile++; newTile+=srcLay->xTiles()+1;
    } else {
      if (renderQ2) { yTile++; newTile+=srcLay->xTiles(); }
			if (renderQ3) { xTile++; newTile+=1; }
    }
    KIS_DEBUG(render, showi(xTile); );
    KIS_DEBUG(render, showi(yTile); );
    if ((xTile<srcLay->xTiles()) && (yTile<srcLay->yTiles())) {
      KIS_DEBUG(render, showi(newTile); );
      if (!(renderQ1 && !renderQ2 && !renderQ3)) {
	if (tileOffsetX>0) tileOffsetX=tileOffsetX-TILE_SIZE;
	if (tileOffsetY>0) tileOffsetY=tileOffsetY-TILE_SIZE;
	renderTileQuadrant(srcLay, newTile, dstLay, dstTile, 
			   0, 0, -tileOffsetX, -tileOffsetY,
			   TILE_SIZE, TILE_SIZE);
      }
    }	else
      KIS_DEBUG(render, puts("ignore"); );
  }	else
    KIS_DEBUG(render, puts("ignore"); );
  
  dbg=false;
}

void Canvas::renderTileQuadrant(Layer *srcLay, int srcTile, 
				Layer *dstLay, int dstTile,
				int srcX, int srcY, 
				int dstX, int dstY, int w, int h)
{
  uchar *channelData[channels+1];
  if (srcLay->hasAlphaChannel())
    channelData[0]=srcLay->channelMem(0,srcTile,0,0);
  channelData[1]=srcLay->channelMem(1,srcTile,0,0);
  channelData[2]=srcLay->channelMem(2,srcTile,0,0);
  channelData[3]=srcLay->channelMem(3,srcTile,0,0);
  
  uchar opacity=srcLay->opacity();
  
  // Constrain the width so that the copy is clipped to the overlap
  w=MIN(MIN(w, TILE_SIZE-srcX), TILE_SIZE-dstX);
  h=MIN(MIN(h, TILE_SIZE-srcY), TILE_SIZE-dstY);
  // now constrain if on the boundry of the layer
  if (srcLay->boundryTileX(srcTile))
    w=MIN(w, srcLay->channelLastTileOffsetX()-srcX);
  if (srcLay->boundryTileY(srcTile))
    h=MIN(h, srcLay->channelLastTileOffsetY()-srcY);
  // XXX now constrain for the boundry of the Canvas
  
  KIS_DEBUG(render, printf("renderTileQuadrant: srcTile=%d src=(%d,%d) dstTile=%d dst=(%d,%d) size=(%d,%d)\n", srcTile, srcX, srcY, dstTile, dstX, dstY, w,h); );
  
  
  
  uchar one=255;
  int leadIn=TILE_SIZE-w;
  
  for(int c=1;c<=channels;c++) {
    uchar *composeD=dstLay->channelMem(c,dstTile, dstX, dstY);
    uchar *channelD=srcLay->channelMem(c,srcTile, srcX, srcY);
    uchar *alpha;
    int alphaInc, alphaLeadIn;
    if (srcLay->hasAlphaChannel()) {
      alpha=srcLay->channelMem(0,srcTile, srcX, srcY);
      alphaInc=1;
      alphaLeadIn=leadIn;
    } else {
      alpha=&one;
      alphaInc=0;
      alphaLeadIn=0;
    }
    uchar opac,invOpac;
    for(int y=h; y; y--) {
      for(int x=w; x; x--) {
	// for prepultiply => invOpac=255-(*alpha*opacity)/255;
	opac=(*alpha*opacity)/255;
	invOpac=255-opac;
	*composeD++ = (*composeD * invOpac + *channelD++ * opac)/255;
	alpha+=alphaInc;
      }
      composeD+=leadIn;
      channelD+=leadIn;
      alpha+=alphaLeadIn;
    }
  }
}

Layer* Canvas::layerPtr( Layer *_layer )
{
  if( _layer == 0 )
    return( currentLayer );
  return( _layer );
}

void Canvas::setCurrentLayer( int _layer )
{
  currentLayer = layers.at( _layer );
}

void Canvas::setLayerOpacity( uchar _opacity, Layer *_layer )
{
  _layer = layerPtr( _layer );
  _layer->setOpacity( _opacity );

  DEBUG("set layer: %s opacity to %d\n", _layer->name().data(), _opacity );
  
  // FIXME: repaint
}

void Canvas::moveLayer( int _dx, int _dy, Layer *_lay ) 
{
  _lay = layerPtr( _lay );
  _lay->moveBy( _dx, _dy );
}

void Canvas::convertImageToPixmap(QImage *image, QPixmap *pix)
{
  if (visual==unknown) {
    //TIME_START;
    pix->convertFromImage(*image);
    //TIME_END("convertFromImage");
  } else {
    //TIME_START;
    switch(visual) {
    case rgb565: {
      ushort s;
      ushort *ptr=(ushort *)imageData;
      uchar *qimg=image->bits();
      for(int y=0;y<TILE_SIZE;y++)
	for(int x=0;x<TILE_SIZE;x++) { 
	  s =(*qimg++)>>3;
	  s|=(*qimg++ & 252)<<3;
	  s|=(*qimg++ & 248)<<8; 
	  qimg++;
	  *ptr++=s;
	}
    }
    break;
    
    case rgb888x:
      xi->data=(char*)image->bits();
      break;
      
    default: break;
    }
    XPutImage(pix->x11Display(), pix->handle(), qt_xget_readonly_gc(),
	      xi, 0,0, 0,0, TILE_SIZE, TILE_SIZE);
    //TIME_END("fast convertImageToPixmap");
  }
} 

void Canvas::convertTileToPixmap(Layer *lay, int tileNo, QPixmap *pix)
{
  // Copy the composite image into a QImage so it can be converted to a
  // QPixmap. 
  // Note: surprisingly it is not quicker to render directly into a QImage
  // probably due to the CPU cache, it's also useless wrt to other colour
  // spaces
  
  // For RGB images XXX
  for(int c=1;c<=channels;c++) {
    uchar *comp=lay->channelMem(c,tileNo, 0,0);
    for(int yy=0;yy<TILE_SIZE;yy++) {
      uchar *ptr=img.scanLine(yy)+c-1;
      for(int xx=TILE_SIZE;xx;xx--) {
	*ptr=*comp++;
	ptr+=4;
      }
    }
  }
  // Construct the relevant pixmap
  convertImageToPixmap(&img, pix);
}

void Canvas::paintBrush(QPoint pt, Brush *brsh)
{
  Layer *lay=layerPtr(0);
  QPoint layPt=pt-brsh->hotSpot()-lay->imageExtents().topLeft();
  QRect brushRect(layPt, brsh->brushSize());
  
  int minYTile=brushRect.top()/TILE_SIZE;
  int maxYTile=brushRect.bottom()/TILE_SIZE;
  int minXTile=brushRect.left()/TILE_SIZE;
  int maxXTile=brushRect.right()/TILE_SIZE;
  
  printf("paintBrush:: pt(%d,%d)\n",pt.x(),pt.y());
  printf("paintBrush:: layPt(%d,%d)\n",layPt.x(),layPt.y());
  printf("paintBrush:: y->(%d,%d) x->(%d,%d)\n",minYTile,maxYTile,minXTile,
	 maxXTile);
  
  SHOW_POINT(lay->channelOffset());
  QRect tileImage(layPt+lay->channelOffset(), brsh->imageExtents().size());
  SHOW_RECT(tileImage);
  
  brsh->moveTo(layPt.x(), layPt.y());
  for(int y=minYTile; y<=maxYTile; y++)
    for(int x=minXTile; x<=maxXTile; x++) {
      printf("testing %d,%d\n",x,y);
      if ((x>=0) && (y>=0) && (x<lay->xTiles()) && (y<lay->yTiles())) {
	QRect tileBoundary(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
	SHOW_RECT(tileBoundary);
	if (tileBoundary.intersects(tileImage)) {
	  renderLayerIntoTile(tileBoundary, brsh, lay, y*lay->xTiles()+x);
	  compositeTile(x,y, compose, 0);
	  convertTileToPixmap(compose, 0, tiles[y*xTiles+x]);
	}
      }
    }
}

void Canvas::upperLayer( int _layer )
{
  ASSERT( ( _layer >= 0 ) && ( _layer < layers.count() ) );

  if( _layer > 0 )
  {
    cerr << "Michael : move layer " << _layer << " to " << _layer - 1 << endl;
 
    Layer *pLayer = layers.take( _layer );
    layers.insert( _layer - 1, pLayer );
  }
}

void Canvas::lowerLayer( int _layer )
{
  ASSERT( ( _layer >= 0 ) && ( _layer < layers.count() ) );

  if( _layer < ( layers.count() - 1 ) )
  {
    cerr << "Michael : move layer " << _layer << " to " << _layer + 1 << endl;

    Layer *pLayer = layers.take( _layer );
    layers.insert( _layer + 1, pLayer );
  }
}

// TODO: remove this two methods

void Canvas::frontLayer( int _layer )
{
}

void Canvas::backgroundLayer( int _layer )
{
}

void Canvas::setFrontLayer( int _layer )
{
  ASSERT( ( _layer >= 0 ) && ( _layer < layers.count() ) );
 
  if( _layer < ( layers.count() - 1 ) )
  {
    Layer *pLayer = layers.take( _layer );
    layers.append( pLayer );
  }
}

void Canvas::setBackgroundLayer( int _layer )
{
  ASSERT( ( _layer >= 0 ) && ( _layer < layers.count() ) );
 
  if( _layer > 0 )
  {
    Layer *pLayer = layers.take( _layer );
    layers.insert( 0, pLayer );
  }
}

#include "canvas.moc"
