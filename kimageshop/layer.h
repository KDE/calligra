//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef LAYER_H
#define LAYER_H

#include <qimage.h>
#include <qobject.h>
#include <qlist.h>

#include "channel.h"

class Layer;

typedef QList<Layer> LayerList;

class Layer : public QObject {
	Q_OBJECT;

 public:
	Layer(int ch, bool hasAlpha=true);
	~Layer();
	
	void    loadRGBImage(QImage img, QImage alpha);
	void    loadGrayImage(QImage img, QImage alpha);

	uchar   opacity() { return(opacityVal); };
	void    setOpacity(uchar o);
	QString name() { return nameVal; };
	void    setName(QString name) { nameVal=name; };
	void    findTileNumberAndOffset(QPoint pt, int *tileNo, int *offset);
	void    findTileNumberAndPos(QPoint pt, int *tileNo, int *x, int *y);
	uchar   *channelMem(int channel, int tileNo, int ox, int oy);
	void    moveBy(int dx, int dy);
	void    moveTo(int x, int y);
	bool    hasAlphaChannel() { return alphaChannel; }
	bool    isVisible() { return visible; }
	void    setVisible(bool vis) { visible=vis; }
	bool    isLinked() { return linked; }
	void    setLinked(bool l) { linked=l; }

	QRect   imageExtents();  // Extents of the image in canvas coords
	QRect   tileExtents();   // Extents of the layers tiles in channel coords
	QPoint  channelOffset(); // TopLeft of the image in the channel
	int     xTiles();
	int     yTiles();
	int     channelLastTileOffsetX();
	int     channelLastTileOffsetY();
	bool    boundryTileX(int tile);
	bool    boundryTileY(int tile);
	void    resizeToIncludePoint(QPoint p);
	void    setPixel(int x, int y, uchar val);

 signals:
	void layerPropertiesChanged();

 private:
	uchar opacityVal;
	Channel *channelPtrs[5];
	int channels;
	QString nameVal;
	bool visible, linked;
	bool alphaChannel;
};

#endif

