/*
 *  kis_layer.h - part of KImageShop
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

#ifndef __layer_h__
#define __layer_h__

#include <qimage.h>
#include <qobject.h>
#include <qlist.h>

#include "kis_channeldata.h"

class Layer;

typedef QList<Layer> LayerList;

class Layer : public QObject {
	Q_OBJECT;

 public:
	Layer(int ch);
	~Layer();
	
	void    loadRGBImage(QImage img, QImage alpha);
	void    loadGrayImage(QImage img, QImage alpha);

	uchar   opacity() const { return(opacityVal); };
	void    setOpacity(uchar o);
	QString name() const { return nameVal; };
	void    setName(QString name) { nameVal=name; };
	void    findTileNumberAndOffset(QPoint pt, int *tileNo, int *offset) const;
	void    findTileNumberAndPos(QPoint pt, int *tileNo, int *x, int *y) const;
	QRect   tileRect(int tileNo);
	uchar   *channelMem(int tileNo, int ox, int oy, bool alpha=false) const;
	void    moveBy(int dx, int dy);
	void    moveTo(int x, int y) const;
	bool    hasAlphaChannel() const { return alphaChannel; }
	bool    isVisible() const { return visible; }
	void    setVisible(bool vis) { visible=vis; }
	bool    isLinked() const { return linked; }
	void    setLinked(bool l) { linked=l; }

	QRect   imageExtents() const;  // Extents of the image in canvas coords
	QRect   tileExtents() const;   // Extents of the layers tiles in canv coords
	QPoint  channelOffset() const; // TopLeft of the image in the channel
	int     xTiles() const;
	int     yTiles() const;
	int     channelLastTileOffsetX() const;
	int     channelLastTileOffsetY() const;
	bool    boundryTileX(int tile) const;
	bool    boundryTileY(int tile) const;
	void    allocateRect(QRect _r);
	void    setPixel(int x, int y, uint val);
	uint    getPixel(int x, int y);
	void    setAlpha(int x, int y, uint val);
	uint    getAlpha(int x, int y);

	void    clear(const QColor& c);

	void    rotate180();
	void    rotateLeft90();
	void    rotateRight90();
	void    mirrorX();
	void    mirrorY();
	void    renderOpacityToAlpha();

 signals:
	void layerPropertiesChanged();

 private:
	uchar opacityVal;
	ChannelData *alphaChannel, *dataChannels;
	int channels;
	QString nameVal;
	bool visible, linked;
	bool alpha;
};

#endif
