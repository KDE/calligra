/*
 *  kis_doc.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
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

#ifndef __kis_doc_h__
#define __kis_doc_h__

#include <qstring.h>
#include <qlist.h>
#include <qstringlist.h>

#include <koDocument.h>
#include <koUndo.h>

#include "kis_dlg_new.h"
#include "kis_layer.h"
#include "kis_image.h"

class Brush;
class NewDialog;

class kisDoc : public KoDocument
{
    Q_OBJECT

public:
    enum {RGB=0, CMYK, LAB, GREYSCALE } ColorModel;
    enum {BACKGROUND=0, FOREGROUND, WHITE, TRANSPARENT } BgColor;

    kisDoc( KoDocument* parent = 0, const char* name = 0 );
    ~kisDoc();

    virtual bool loadFromURL( const QString& );
    
    // document
    virtual View* createView( QWidget* parent = 0, const char* name = 0 );
    virtual Shell* createShell();
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );
    virtual bool initDoc();
    virtual QCString mimeType() const;
    KoCommandHistory* commandHistory() { return &m_commands; };

    void paintPixmap( QPainter *painter, QRect area );

    int height();
    int width();
    QRect imageExtents();
    QSize size();

    Layer* getCurrentLayer();
    int getCurrentLayerIndex();
    void setCurrentLayer( int _layer );

    void upperLayer( unsigned int _layer );
    void lowerLayer( unsigned int _layer );
    void setFrontLayer( unsigned int _layer );
    void setBackgroundLayer( unsigned int _layer );

    void addRGBLayer( QString _file );
    void addRGBLayer(const QRect& r, const QColor& c, const QString& name);
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
    LayerList layerList();
    
    void rotateLayer180(Layer *_layer);
    void rotateLayerLeft90(Layer *_layer);
    void rotateLayerRight90(Layer *_layer);
    void mirrorLayerX(Layer *_layer);
    void mirrorLayerY(Layer *_layer);

    void mergeAllLayers();
    void mergeVisibleLayers();
    void mergeLinkedLayers();
    void mergeLayers(QList<Layer>);

    kisImage* newImage(const QString& _name, int w, int h, int colorModel = RGB, int backgroundMode = WHITE);
    bool saveImage( const QString& file, kisImage *img );
    bool saveCurrentImage( const QString& file );
    bool loadImage( const QString& file );
    void removeImage( kisImage *img );
    
    QString currentImage();

    void setCurrentImage(kisImage *img);
    void setCurrentImage(const QString& _name);

    QStringList images();
    
public slots:
  void slotUndoRedoChanged( QString _undo, QString _redo );
  void slotUndoRedoChanged( QStringList _undo, QStringList _redo );
  void setCurrentLayerOpacity( double opacity )
    {  setLayerOpacity( (uchar) ( opacity * 255 / 100 ) ); };
  void slotImageUpdated();
  void slotImageUpdated( const QRect& rect );
  void slotLayersUpdated();

  void slotNewImage();

  void slotRemoveImage( const QString& name );
 
signals:
  void docUpdated();
  void docUpdated( const QRect& rect );
  void layersUpdated();
  void imageAdded(const QString& name);
    
protected:
  virtual QString configFile() const;
  KoCommandHistory m_commands;

private:
  QList <kisImage> m_Images;
  kisImage *m_pCurrent;
  NewDialog       *m_pNewDialog;
};

#endif
