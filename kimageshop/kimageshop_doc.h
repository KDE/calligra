/*
 *  kimageshop_doc.h - part of KImageShop
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

#ifndef __kimageshop_doc_h__
#define __kimageshop_doc_h__

#include <koDocument.h>
#include <koUndo.h>

#include <qimage.h>
#include <qlist.h>

#include <stdlib.h>
#include <X11/Xlib.h>

#include "layer.h"

class Brush;

struct canvasTileDescriptor
{
  QPixmap pix;
};

class KImageShopDoc : public KoDocument
{
    Q_OBJECT
public:
    KImageShopDoc( int w = 510, int h = 510, KoDocument* parent = 0, const char* name = 0 );
    ~KImageShopDoc();

    virtual bool loadFromURL( const QString& );
    
    // document
    virtual View* createView( QWidget* parent = 0, const char* name = 0 );
    virtual Shell* createShell();
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );
    virtual bool initDoc();
    virtual QCString mimeType() const;
    KoCommandHistory* commandHistory() { return &m_commands; };

    void paintPixmap( QPainter *painter, QRect area );
    int height() { return h; }
    int width() { return w; }

    Layer* getCurrentLayer() { return currentLayer; }
    int getCurrentLayerIndex() { return layers.find( currentLayer ); }
    void setCurrentLayer( int _layer );
    
    void upperLayer( unsigned int _layer );
    void lowerLayer( unsigned int _layer );
    void setFrontLayer( unsigned int _layer );
    void setBackgroundLayer( unsigned int _layer );

    void addRGBLayer( QString _file );
    void removeLayer( unsigned int _layer );
    void compositeImage( QRect _rect );
    Layer* layerPtr( Layer *_layer );
    void setLayerOpacity( uchar _opacity, Layer *_layer = 0 );
    
    void renderLayerIntoTile( QRect tileBoundary, const Layer *srcLay, 
			      Layer *dstLay, int dstTile );
    void moveLayer( int dx, int dy, Layer *lay = 0 );
    void moveLayerTo( int x, int y, Layer *lay = 0 );
    void renderTileQuadrant( const Layer *srcLay, int srcTile, Layer *dstLay,
			     int dstTile, int srcX, int srcY, int dstX, int dstY, int w, int h );
    LayerList layerList() { return layers; };
    
    void rotateLayer180(Layer *_layer);
    void rotateLayerLeft90(Layer *_layer);
    void rotateLayerRight90(Layer *_layer);
    void mirrorLayerX(Layer *_layer);
    void mirrorLayerY(Layer *_layer);

    void mergeAllLayers();
    void mergeVisibleLayers();
    void mergeLinkedLayers();
    void mergeLayers(QList<Layer>);
       
public slots:
  void slotUndoRedoChanged( QString _undo, QString _redo );
  void slotUndoRedoChanged( QStringList _undo, QStringList _redo );
  void setCurrentLayerOpacity( double opacity )
    {  setLayerOpacity( (uchar) ( opacity * 255 / 100 ) ); };
    
protected:
  virtual QString configFile() const;
  KoCommandHistory m_commands;
  
  void compositeTile( int x, int y, Layer *dstLay = 0, int dstTile = -1 );
  void convertTileToPixmap( Layer *lay, int tileNo, QPixmap *pix );

signals:
  void docUpdated();
  void docUpdated( const QRect& rect);
  void layersUpdated();

private:

  void setUpVisual();
  void convertImageToPixmap( QImage *img, QPixmap *pix );

  enum dispVisual { unknown, rgb565, rgb888x } visual;

  int       w;
  int       h;
  int       channels;
  QRect     viewportRect;
  int       xTiles;
  int       yTiles;
  LayerList layers;
  Layer     *compose;
  QImage    img;
  Layer     *currentLayer;
  QPixmap   **tiles;
  bool      dragging;
  QPoint    dragStart;
  uchar     *background;
  char      *imageData;
  XImage    *xi;
};

#endif
