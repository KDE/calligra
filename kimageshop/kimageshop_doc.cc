/*
 *  kimageshop_doc.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
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


#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include <sys/time.h>
#include <unistd.h>

#include <qpainter.h>
#include <qwidget.h>
#include <qregexp.h>
#include <qfileinfo.h>

#include <kstddirs.h>
#include <kglobal.h>

#include "kimageshop_doc.h"
#include "kimageshop_view.h"
#include "kimageshop_shell.h"
#include "kimageshop_factory.h"
#include "layer.h"
#include "misc.h"
#include "brush.h"

#define KIS_DEBUG(AREA, CMD)

KImageShopDoc::KImageShopDoc( int width, int height, KoDocument* parent, const char* name )
  : KoDocument( parent, name )
  , m_commands()
{
  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QString, QString ) ),
                    this, SLOT( slotUndoRedoChanged( QString, QString ) ) );
  QObject::connect( &m_commands, SIGNAL( undoRedoChanged( QStringList, QStringList ) ),
                    this, SLOT( slotUndoRedoChanged( QStringList, QStringList ) ) );

  w=width;
  h=height;
  viewportRect=QRect(0,0,w,h);
  // SHOW_RECT_COORDS(viewportRect);

  QRect tileExtents=::findTileExtents(viewportRect);
  // SHOW_RECT(tileExtents);
  xTiles=tileExtents.width()/TILE_SIZE;	
  yTiles=tileExtents.height()/TILE_SIZE;

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

  compose=new Layer(channels);
  compose->allocateRect(QRect(0,0, TILE_SIZE,TILE_SIZE));
  
  // XXX make this like the compose layer
  // make it work with other colour spaces
  background=new uchar[channels*TILE_SIZE*TILE_SIZE];
  for(int y=0;y<TILE_SIZE;y++)
    for(int x=0;x<TILE_SIZE;x++) {
      *(background+channels*(y*TILE_SIZE+x))  =128+63*((x/16+y/16)%2);
      *(background+channels*(y*TILE_SIZE+x)+1)=128+63*((x/16+y/16)%2);
      *(background+channels*(y*TILE_SIZE+x)+2)=128+63*((x/16+y/16)%2);
    }
}

KImageShopDoc::~KImageShopDoc()
{
  qDebug("~KImageShopDoc()");

  // XXX delete individual tiles
  delete tiles;

  if ((visual!=unknown) && (visual!=rgb888x))
    free(imageData);
}

bool KImageShopDoc::initDoc()
{
  // load some test layers
  QString _image = locate("kis_images", "cam9b.jpg", KImageShopFactory::global());
  addRGBLayer(_image);
  setLayerOpacity(255);

  _image = locate("kis_images", "cambw12.jpg", KImageShopFactory::global());
  addRGBLayer(_image);
  moveLayer(256,384);
  setLayerOpacity(180);

  _image = locate("kis_images", "cam05.jpg", KImageShopFactory::global());
  addRGBLayer(_image);
  setLayerOpacity(255);

  _image = locate("kis_images", "cam6.jpg", KImageShopFactory::global());
  addRGBLayer(_image);
  moveLayer(240,280);
  setLayerOpacity(255);

  _image = locate("kis_images", "img2.jpg", KImageShopFactory::global());
  addRGBLayer(_image);
   setLayerOpacity(80);

  compositeImage(QRect());
  return true;
}

QCString KImageShopDoc::mimeType() const
{
    return "application/x-kimageshop";
}

View* KImageShopDoc::createView( QWidget* parent, const char* name )
{
    KImageShopView* view = new KImageShopView( this, parent, name );
    addView( view );

    return view;
}

Shell* KImageShopDoc::createShell()
{
    Shell* shell = new KImageShopShell;
    shell->setRootPart( this );
    shell->show();

    return shell;}

void KImageShopDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/)
{
  //debug("w, h: %i, %i", rect.width(), rect.height());
  //paintPixmap( &painter, rect, QPoint(0,0), QPoint(0,0), 1.0 );
  paintPixmap( &painter, rect);
}

QString KImageShopDoc::configFile() const
{
    return readConfigFile( locate("kis", "kimageshop.rc", KImageShopFactory::global()) );
}


void KImageShopDoc::slotUndoRedoChanged( QString /*_undo*/, QString /*_redo*/ )
{
  //####### FIXME
}

void KImageShopDoc::slotUndoRedoChanged( QStringList /*_undo*/, QStringList /*_redo*/ )
{
  //####### FIXME
}

void KImageShopDoc::paintPixmap(QPainter *p, QRect area)
{
  if (!w)
    return;

 int startX, startY, pixX, pixY, clipX, clipY = 0;

 int l = area.left();
 int t = area.top();
 int r = area.right();
 int b = area.bottom();

 //qDebug("left: %d; top: %d; right: %d; bottom: %d", l, t, r, b);

  // XXX clip it correctly
  // XXX paint only what is needed

 for(int y=0;y<yTiles;y++)
    {
      for(int x=0;x<xTiles;x++)
	{
	  QRect tileRect(x*TILE_SIZE, y*TILE_SIZE,TILE_SIZE,TILE_SIZE);
	  if (viewportRect.intersects(tileRect) && tileRect.intersects(area))
	    {
	      if (tiles[y*xTiles+x]->isNull())
		continue;
	      
	      //qDebug("tile: %d,%d", x, y);

	      if (x*TILE_SIZE < l)
		{
		  startX = 0;
		  pixX = l - x*TILE_SIZE;
		  clipX = -1;
		}
	      else
		{
		  startX = x*TILE_SIZE - l;
		  pixX = 0;
		  clipX =r - x*TILE_SIZE;
		}
	      
	      if (y*TILE_SIZE < t)
		{
		  startY = 0;
		  pixY = t - y*TILE_SIZE;
		  clipY=-1;
		}
	      else
		{
		  startY = y*TILE_SIZE - t;
		  pixY = 0;
		  clipY = b - y*TILE_SIZE;
		}

	      if (clipX == 0)
		clipX = -1;
	      if (clipY == 0)
		clipY = -1;
	      
	      p->drawPixmap(startX, startY, *tiles[y*xTiles+x], pixX, pixY, clipX, clipY);
	    }
	}
    }
}

void KImageShopDoc::setUpVisual()
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

void KImageShopDoc::addRGBLayer(QString file)
{
  printf("KImageShopDoc::addRGBLayer: %s\n",file.latin1());

  QImage img(file);
  if (img.isNull()) {
    KIS_DEBUG("Unable to load image: %s\n",file.latin1());
    return;
  }
  img=img.convertDepth(32);

  // XXX currently assumes the alpha image IS a greyscale and the same size as
  // the other channels
  QString alphaName=file;
  alphaName.replace(QRegExp("\\.jpg$"),"-alpha.jpg");

	printf("alphaname=%s\n",alphaName.latin1());
  QImage alpha(alphaName);
  if (!alpha.isNull() && (img.size()!=alpha.size())) {
    puts("Incorrect sized alpha channel - not loaded");
    alpha=QImage();
  }

  Layer *lay=new Layer(3);
  lay->setName(QFileInfo(file).fileName());

  lay->loadRGBImage(img, alpha);
  layers.append(lay);
  currentLayer=lay;
}

void KImageShopDoc::removeLayer( unsigned int _layer )
{
  if( _layer >= layers.count() )
    return;

  Layer* lay = layers.take( _layer );

  if( currentLayer == lay )
  {
    if( layers.count() != 0 )
      currentLayer = layers.at( 0 );
    else
      currentLayer = NULL;
  }

  delete lay;
}


// Constructs the composite image in the tile at x,y and updates the relevant
// pixmap
void KImageShopDoc::compositeTile(int x, int y, Layer *dstLay, int dstTile)
{
  // work out which tile to render into unless directed to a specific tile
  if (dstTile==-1)
    dstTile=y*xTiles+x;
  if (dstLay==0)
    dstLay=compose;

  KIS_DEBUG(tile, printf("\n*** compositeTile %d,%d\n",x,y); );

  //printf("compositeTile: dstLay=%p dstTile=%d\n",dstLay, dstTile);

  // Set the background
	memcpy(dstLay->channelMem(dstTile,0,0), background,
				 channels*TILE_SIZE*TILE_SIZE);

  // Find the tiles boundary in KImageShopDoc coordinates
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

void KImageShopDoc::compositeImage(QRect r)
{
  TIME_START;
  for(int y=0;y<yTiles;y++)
    for(int x=0;x<xTiles;x++)
      if (r.isNull() ||
	  r.intersects(QRect(x*TILE_SIZE, y*TILE_SIZE,TILE_SIZE,TILE_SIZE))) {
				// set the alpha channel to opaque
	memset(compose->channelMem(0, 0,0, true),255, 
	       TILE_SIZE*TILE_SIZE);
	compositeTile(x,y, compose, 0);
	
	convertTileToPixmap(compose, 0, tiles[y*xTiles+x]);
      }
  TIME_END("compositeImage");

  emit docUpdated(r);
}


// Renders the part of srcLay which resides in dstTile of dstLay

void KImageShopDoc::renderLayerIntoTile(QRect tileBoundary, const Layer *srcLay,
																 Layer *dstLay, int dstTile)
{
  int tileNo, tileOffsetX, tileOffsetY, xTile, yTile;

	//puts("renderLayerIntoTile");

  srcLay->findTileNumberAndPos(tileBoundary.topLeft(), &tileNo,
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

void KImageShopDoc::renderTileQuadrant(const Layer *srcLay, int srcTile,
				Layer *dstLay, int dstTile,
				int srcX, int srcY,
				int dstX, int dstY, int w, int h)
{
  if (srcLay->channelMem(srcTile,0,0)==0) return;

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

  //printf("renderTileQuadrant: srcTile=%d src=(%d,%d) dstTile=%d dst=(%d,%d) size=(%d,%d)\n", srcTile, srcX, srcY, dstTile, dstX, dstY, w,h);


  uchar one=255;
  int leadIn=(TILE_SIZE-w);

	uchar *composeD=dstLay->channelMem(dstTile, dstX, dstY);
	uchar *composeA=dstLay->channelMem(dstTile, dstX, dstY, true);
	uchar *channelD=srcLay->channelMem(srcTile, srcX, srcY);
	uchar *alpha   =srcLay->channelMem(srcTile, srcX, srcY, true);

	int alphaInc, alphaLeadIn;

	if (alpha) {
		alphaInc=1;
		alphaLeadIn=leadIn;
	} else {
		puts("************* no alpha");
		alpha=&one;
		alphaInc=0;
		alphaLeadIn=0;
	}

	leadIn*=3;

	uchar opac,invOpac;
	for(int y=h; y; y--) {
		for(int x=w; x; x--) {
			// for prepultiply => invOpac=255-(*alpha*opacity)/255;
			opac=(*alpha*opacity)/255;
			invOpac=255-opac;
			// fix this (in a fast way for colour spaces

			*composeD++ = (((*composeD * *composeA)/255) * invOpac + 
										 *channelD++ * opac)/255;
			*composeD++ = (((*composeD * *composeA)/255) * invOpac + 
										 *channelD++ * opac)/255;
			*composeD++ = (((*composeD * *composeA)/255) * invOpac + 
										 *channelD++ * opac)/255;
			*composeA=*alpha + *composeA - (*alpha * *composeA)/255;
			composeA+=alphaInc;
			alpha+=alphaInc;
		}
		composeD+=leadIn;
		composeA+=alphaLeadIn;
		channelD+=leadIn;
		alpha+=alphaLeadIn;
	}
}

Layer* KImageShopDoc::layerPtr( Layer *_layer )
{
  if( _layer == 0 )
    return( currentLayer );
  return( _layer );
}

void KImageShopDoc::setCurrentLayer( int _layer )
{
  currentLayer = layers.at( _layer );
}

void KImageShopDoc::setLayerOpacity( uchar _opacity, Layer *_layer )
{
  _layer = layerPtr( _layer );
  _layer->setOpacity( _opacity );

  // FIXME: repaint
}

void KImageShopDoc::moveLayer( int _dx, int _dy, Layer *_lay )
{
  _lay = layerPtr( _lay );
  _lay->moveBy( _dx, _dy );
}

void KImageShopDoc::moveLayerTo( int _x, int _y, Layer *_lay )
{
  _lay = layerPtr( _lay );
  _lay->moveTo( _x, _y );
}

void KImageShopDoc::convertImageToPixmap(QImage *image, QPixmap *pix)
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

void KImageShopDoc::convertTileToPixmap(Layer *lay, int tileNo, QPixmap *pix)
{
  // Copy the composite image into a QImage so it can be converted to a
  // QPixmap.
  // Note: surprisingly it is not quicker to render directly into a QImage
  // probably due to the CPU cache, it's also useless wrt to other colour
  // spaces

  // For RGB images XXX
	uchar *comp=lay->channelMem(tileNo, 0,0);
	for(int yy=0;yy<TILE_SIZE;yy++) {
		uchar *ptr=img.scanLine(yy);
		for(int xx=TILE_SIZE;xx;xx--) {
			*ptr++=*comp++;
			*ptr++=*comp++;
			*ptr++=*comp++;
			ptr++;
		}
	}

  // Construct the relevant pixmap
  convertImageToPixmap(&img, pix);
}

// NB: painting is only allowed within the rectangle of the canvas
// everything else is clipped. However, layers can extend outside the canvas

void KImageShopDoc::paintBrush(QPoint /*pt*/, const Brush* /*brsh*/)
{
  /*
  Layer *lay=layerPtr(0);

	// find the brush rect in canvas coords and clip the brush to the canvas
	QRect brushCanvasRect=QRect(pt-brsh->hotSpot(), brsh->imageExtents().size());
	brushCanvasRect=brushCanvasRect.intersect(QRect(0,0, w,h));
	
	if (brushCanvasRect.isEmpty())
		return;
	
	// allocate out tiles if required
	lay->allocateRect(brushCanvasRect);	
	
	// move the brust to the correct part of the canvas
  QPoint brushTL=pt-brsh->hotSpot();
  brsh->moveTo(brushTL.x(), brushTL.y());

	// layerRect is in layer coords (offset from tileExtents.topLeft())
	QRect layerRect=brushCanvasRect;
	layerRect.moveTopLeft(brushCanvasRect.topLeft()-
												lay->tileExtents().topLeft());

 	// workout which tiles in the layer need to be updated
  int minYTile=layerRect.top()/TILE_SIZE;
  int maxYTile=layerRect.bottom()/TILE_SIZE;
  int minXTile=layerRect.left()/TILE_SIZE;
  int maxXTile=layerRect.right()/TILE_SIZE;

  printf("paintBrush:: pt(%d,%d)\n",pt.x(),pt.y());
  printf("paintBrush:: brushTL(%d,%d)\n",brushTL.x(),brushTL.y());
  printf("paintBrush:: tiles y->(%d,%d) x->(%d,%d)\n",minYTile,maxYTile,
				 minXTile, maxXTile);
	QRect tileBoundary;
	
	for(int y=minYTile; y<=maxYTile; y++)
		for(int x=minXTile; x<=maxXTile; x++) {
			printf("updating layer %d,%d\n",x,y);
			int dstTile=y*lay->xTiles()+x;
			tileBoundary=lay->tileRect(dstTile);
			SHOW_RECT(tileBoundary);
			renderLayerIntoTile(tileBoundary, brsh, lay, dstTile);
		}

	// should really just set a dirty flag on the canvas
	compositeImage(brushCanvasRect);
  */
}

void KImageShopDoc::mergeAllLayers()
{
  QList<Layer> l;

  Layer *lay = layers.first();

  while(lay)
    {
      l.append(lay);
      lay = layers.next();
    }
  mergeLayers(l);
}

void KImageShopDoc::mergeVisibleLayers()
{
  QList<Layer> l;

  Layer *lay = layers.first();

  while(lay)
    {
      if(lay->isVisible())
	l.append(lay);
      lay = layers.next();
    }
  mergeLayers(l);
}

void KImageShopDoc::mergeLinkedLayers()
{
  QList<Layer> l;

  Layer *lay = layers.first();

  while(lay)
    {
      if (lay->isLinked())
	l.append(lay);
      lay = layers.next();
    }
  mergeLayers(l);
}

void KImageShopDoc::mergeLayers(QList<Layer> list)
{
  list.setAutoDelete(false);

  Layer *a, *b;
  QRect newRect;

  a = list.first();
  while(a)
    {
      newRect.unite(a->imageExtents());
      a->renderOpacityToAlpha();
      a = list.next();
    }

  while((a = list.first()) && (b = list.next()))
    {
      if (!a || !b)
	break;
      QRect ar = a->imageExtents();
      QRect br = b->imageExtents();
      
      QRect urect = ar.unite(br);
      
      // allocate out tiles if required
      a->allocateRect(urect);
      b->allocateRect(urect);
      
      // rect in layer coords (offset from tileExtents.topLeft())
      QRect rect = urect;
      rect.moveTopLeft(urect.topLeft() - a->tileExtents().topLeft());
      
      // workout which tiles in the layer need to be updated
      int minYTile=rect.top() / TILE_SIZE;
      int maxYTile=rect.bottom() / TILE_SIZE;
      int minXTile=rect.left() / TILE_SIZE;
      int maxXTile=rect.right() / TILE_SIZE;
      
      QRect tileBoundary;
  
      for(int y=minYTile; y<=maxYTile; y++)
	{
	  for(int x=minXTile; x<=maxXTile; x++)
	    {
	      int dstTile = y * a->xTiles() + x;
	      tileBoundary = a->tileRect(dstTile);
	      renderLayerIntoTile(tileBoundary, b, a, dstTile);
	    }
	}
      
      list.remove(b);
      layers.remove(b);
      if( currentLayer == b )
	{
	  if(layers.count() != 0)
	    currentLayer = layers.at(0);
	  else
	    currentLayer = NULL;
	}
      delete b;
    }
  emit layersUpdated();
  compositeImage(newRect);
}

void KImageShopDoc::upperLayer( unsigned int _layer )
{
  ASSERT( _layer < layers.count() );

  if( _layer > 0 )
  {
    Layer *pLayer = layers.take( _layer );
    layers.insert( _layer - 1, pLayer );
  }
}

void KImageShopDoc::lowerLayer( unsigned int _layer )
{
  ASSERT( _layer < layers.count() );

  if( _layer < ( layers.count() - 1 ) )
  {
    Layer *pLayer = layers.take( _layer );
    layers.insert( _layer + 1, pLayer );
  }
}

void KImageShopDoc::setFrontLayer( unsigned int _layer )
{
  ASSERT( _layer < layers.count() );

  if( _layer < ( layers.count() - 1 ) )
  {
    Layer *pLayer = layers.take( _layer );
    layers.append( pLayer );
  }
}

void KImageShopDoc::setBackgroundLayer( unsigned int _layer )
{
  ASSERT( _layer < layers.count() );

  if( _layer > 0 )
  {
    Layer *pLayer = layers.take( _layer );
    layers.insert( 0, pLayer );
  }
}

void KImageShopDoc::rotateLayer180(Layer *_layer)
{
  _layer = layerPtr( _layer );
  _layer->rotate180();
  compositeImage(_layer->imageExtents());
}

void KImageShopDoc::rotateLayerLeft90(Layer *_layer)
{
  _layer = layerPtr( _layer );
  _layer->rotateLeft90();
  compositeImage(_layer->imageExtents());
}

void KImageShopDoc::rotateLayerRight90(Layer *_layer)
{
  _layer = layerPtr( _layer );
  _layer->rotateRight90();
  compositeImage(_layer->imageExtents());
}

void KImageShopDoc::mirrorLayerX(Layer *_layer)
{
  _layer = layerPtr( _layer );
  _layer->mirrorX();
  compositeImage(_layer->imageExtents());
}

void KImageShopDoc::mirrorLayerY(Layer *_layer)
{
  _layer = layerPtr( _layer );
  _layer->mirrorY();
  compositeImage(_layer->imageExtents());
}


#include "kimageshop_doc.moc"
