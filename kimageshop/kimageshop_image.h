/*
 *  kimageshop_image.h - part of KImageShop
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

#ifndef __kimageshop_image_h__
#define __kimageshop_image_h__

#include <qimage.h>
#include <qlist.h>
#include <qobject.h>

#include <stdlib.h>
#include <X11/Xlib.h>

#include "layer.h"

class Brush;

struct canvasTileDescriptor
{
  QPixmap pix;
};

class KImageShopImage : public QObject
{
    Q_OBJECT

 public:
    KImageShopImage( const QString& _name, int w = 512, int h = 512 );
    ~KImageShopImage();

    QString name()                { return m_name; }
    void setName(const QString& _name) { m_name = _name; }
    
    void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );
    void paintPixmap( QPainter *painter, QRect area );

    int height()          { return h; }
    int width()           { return w; }
    QSize size()          { return QSize( w, h); }
    QRect imageExtents()  { return QRect(0, 0, w, h); }

    Layer* getCurrentLayer() { return currentLayer; }
    int getCurrentLayerIndex() { return layers.find( currentLayer ); }
    void setCurrentLayer( int _layer );

    void upperLayer( unsigned int _layer );
    void lowerLayer( unsigned int _layer );
    void setFrontLayer( unsigned int _layer );
    void setBackgroundLayer( unsigned int _layer );
    void moveLayer( int dx, int dy, Layer *lay = 0 );
    void moveLayerTo( int x, int y, Layer *lay = 0 );
    void setLayerOpacity( uchar _opacity, Layer *_layer = 0 );

    void addRGBLayer( QString _file );
    void addRGBLayer(const QRect& r, const QColor& c, const QString& name);
    void removeLayer( unsigned int _layer );

    Layer* layerPtr( Layer *_layer );
    LayerList layerList() { return layers; };
 
    void compositeImage( QRect _rect );
     
    void rotateLayer180(Layer *_layer);
    void rotateLayerLeft90(Layer *_layer);
    void rotateLayerRight90(Layer *_layer);
    void mirrorLayerX(Layer *_layer);
    void mirrorLayerY(Layer *_layer);

    void mergeAllLayers();
    void mergeVisibleLayers();
    void mergeLinkedLayers();
    void mergeLayers(QList<Layer>);
    
 signals:
    void updated();
    void updated( const QRect& rect );
    void layersUpdated();
    
 protected:
    void compositeTile( int x, int y, Layer *dstLay = 0, int dstTile = -1 );
    void convertTileToPixmap( Layer *lay, int tileNo, QPixmap *pix );
    void renderLayerIntoTile( QRect tileBoundary, const Layer *srcLay, Layer *dstLay, int dstTile );
    void renderTileQuadrant( const Layer *srcLay, int srcTile, Layer *dstLay,
			     int dstTile, int srcX, int srcY, int dstX, int dstY, int w, int h );
    void setUpVisual();
    void convertImageToPixmap( QImage *img, QPixmap *pix );
        
 private:
    enum dispVisual { unknown, rgb565, rgb888x } visual;

    int         w;
    int         h;
    int         channels;
    QRect       viewportRect;
    int         xTiles;
    int         yTiles;
    LayerList   layers;
    Layer       *compose;
    QImage      img;
    Layer       *currentLayer;
    QPixmap     **tiles;
    bool        dragging;
    QPoint      dragStart;
    uchar       *background;
    char        *imageData;
    XImage      *xi;
    QString     m_name;
};

#endif
