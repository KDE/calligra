//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef __canvas_h__
#define __canvas_h__

#include <qimage.h>
#include <qobject.h>
#include <qlist.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "layer.h"
#include "brush.h"

struct canvasTileDescriptor
{
  QPixmap pix;
};


class Canvas : public QObject
{
  Q_OBJECT

 public:
  Canvas(int width, int height);
  ~Canvas();

  // Paint a area of image data on widget w.
  // Handle zoomFactor and offset.
  void paintPixmap(QWidget *w, QRect area, QPoint offset = QPoint(0,0), int zoomFactor = 0);
 
  // return current layer
  layer* getCurrentLayer() { return currentLayer; }

  // return size
  int height();
  int width();

  void addRGBLayer(QString file);
  void compositeImage(QRect r);
  void setCurrentLayer(int l);
  layer *layerPtr(layer *lay);
  void setLayerOpacity(uchar o, layer *lay=0);
  
  void renderLayerIntoTile(QRect tileBoundary, layer *srcLay, layer *dstLay, int dstTile);
  void moveLayer(int dx, int dy, layer *lay=0);
  void renderTileQuadrant(layer *srcLay, int srcTile, layer *dstLay, int dstTile,
			  int srcX, int srcY, int dstX, int dstY, int w, int h);
  void paintBrush(QPoint pt, brush *brsh);
  QList<layer> layerList() { return layers; };

public slots:
  void setCurrentLayerOpacity(double o) { setLayerOpacity((uchar)(o*255/100)); };
  
protected:
  void compositeTile(int x, int y, layer *dstLay=0, int dstTile=-1);
  void convertTileToPixmap(layer *lay, int tileNo, QPixmap *pix);

private:
  void setUpVisual();
  void convertImageToPixmap(QImage *img, QPixmap *pix);

  int w,h, channels;
  QRect viewportRect;
  int xTiles, yTiles;
  QList<layer> layers;
  layer *compose;
  QImage img;
  layer *currentLayer;
  QPixmap **tiles;
  bool dragging;
  QPoint dragStart;
  uchar *background[5];
  
  enum dispVisual {unknown, rgb565, rgb888x} visual;
  char *imageData;
  XImage *xi;
};

#endif
