//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef CANVAS_H
#define CANVAS_H

#include <qimage.h>
#include <qlist.h>
#include <qwidget.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "layer.h"
#include "brush.h"

struct canvasTileDescriptor {
	QPixmap pix;
};


class canvas : public QWidget {
	Q_OBJECT;
 public:
	canvas(int width, int height, QWidget *parent=0);
	~canvas();

	void addRGBLayer(QString file);
	void compositeImage(QRect r);
	void setCurrentLayer(int l);
	layer *layerPtr(layer *lay);
	void setLayerOpacity(uchar o, layer *lay=0);
	void renderLayerIntoTile(QRect tileBoundary, layer *srcLay, layer *dstLay,
													 int dstTile);
	void moveLayer(int dx, int dy, layer *lay=0);
	void renderTileQuadrant(layer *srcLay, int srcTile, 
													layer *dstLay, int dstTile,
													int srcX, int srcY,
													int dstX, int dstY, int w, int h);
	void paintBrush(QPoint pt, brush *brsh);
	QList<layer> layerList() { return layers; };


	brush *currentBrush;

 public slots:
	void setCurrentLayerOpacity(double o) { setLayerOpacity((uchar)(o*255/100)); };

 protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void mousePressEvent ( QMouseEvent * );
	virtual void mouseMoveEvent ( QMouseEvent * );
	virtual void mouseReleaseEvent ( QMouseEvent * );
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
