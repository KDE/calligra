/*
 *  canvas.h - part of KImageShop
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

#ifndef __canvas_h__
#define __canvas_h__

#include <stdlib.h>
#include <X11/Xlib.h>

#include <qimage.h>
#include <qobject.h>

#include "layer.h"
#include "brush.h"

struct canvasTileDescriptor
{
  QPixmap pix;
};

/**
 *  The canvas class.
 */
class Canvas : public QObject
{
  Q_OBJECT

public:

  /**
   *  Constructor
   *
   *  @param width The width of the canvas.
   *  @param width The height of the canvas.
   */
  Canvas( int width, int height );

  /**
   *  Destructor.
   */
  ~Canvas();

  /**
   *  Paint a area of image data on widget w.
   *  Handle zoomFactor and offset.
   */
  void paintPixmap( QWidget *widget, QRect area, QPoint offset = QPoint( 0, 0 ), QPoint paintOffset = QPoint( 0, 0 ), float zoomFactor = 1 );

  /**
   *  Returns current layer.
   */
  Layer* getCurrentLayer()
  { return currentLayer; }

  int getCurrentLayerIndex()
  { return layers.find( currentLayer ); }

  /**
   *  Defines the current layedr.
   */
  void setCurrentLayer( int _layer );

  void upperLayer( int _layer );
  void lowerLayer( int _layer );
  void setFrontLayer( int _layer );
  void setBackgroundLayer( int _layer );

  // return size
  int height();
  int width();

  void addRGBLayer( QString _file );
  void removeLayer( int _layer );
  void compositeImage( QRect _rect );
  Layer* layerPtr( Layer *_layer );
  void setLayerOpacity( uchar _opacity, Layer *_layer = 0 );

  void renderLayerIntoTile( QRect tileBoundary, Layer *srcLay, Layer *dstLay, int dstTile );
  void moveLayer( int dx, int dy, Layer *lay = 0 );
  void renderTileQuadrant( Layer *srcLay, int srcTile, Layer *dstLay, int dstTile,
			   int srcX, int srcY, int dstX, int dstY, int w, int h );
  void paintBrush( QPoint pt, const Brush *brush );
  LayerList layerList() { return layers; };

public slots:

  void setCurrentLayerOpacity( double opacity )
  {  setLayerOpacity( (uchar) ( opacity * 255 / 100 ) ); };

protected:

  void compositeTile( int x, int y, Layer *dstLay = 0, int dstTile = -1 );
  void convertTileToPixmap( Layer *lay, int tileNo, QPixmap *pix );

private:

  void setUpVisual();
  void convertImageToPixmap( QImage *img, QPixmap *pix );

  int w,h, channels;
  QRect viewportRect;
  int xTiles, yTiles;
  LayerList layers;
  Layer *compose;
  QImage img;
  Layer *currentLayer;
  QPixmap **tiles;
  bool dragging;
  QPoint dragStart;
  uchar *background[5];

  enum dispVisual { unknown, rgb565, rgb888x } visual;
  char *imageData;
  XImage *xi;
};

#endif


