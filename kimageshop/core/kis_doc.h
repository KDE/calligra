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
#include "kis_global.h"

class KisBrush;
class NewDialog;

class KisDoc : public KoDocument
{
    Q_OBJECT

public:

    KisDoc( QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KisDoc();


	virtual bool save( ostream&, const char *_format );
	virtual bool load( istream& in, KoStore* _store );
	virtual bool loadXML( const QDomDocument& doc, KoStore* store );

	virtual bool hasToWriteMultipart();

    virtual KoView* createView( QWidget* parent = 0, const char* name = 0 );
    virtual KoMainWindow* createShell();
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );
    virtual bool initDoc();
    virtual QCString mimeType() const;
    KoCommandHistory* commandHistory() { return &m_commands; };

    void paintPixmap( QPainter *painter, QRect area );

    int height();
    int width();
    QRect imageExtents();
    QSize size();

    KisLayer* getCurrentLayer();
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
    KisLayer* layerPtr( KisLayer *_layer );
    void setLayerOpacity( uchar _opacity, KisLayer *_layer = 0 );

    void renderLayerIntoTile( QRect tileBoundary, const KisLayer *srcLay,
			      KisLayer *dstLay, int dstTile );
    void moveLayer( int dx, int dy, KisLayer *lay = 0 );
    void moveLayerTo( int x, int y, KisLayer *lay = 0 );
    void renderTileQuadrant( const KisLayer *srcLay, int srcTile, KisLayer *dstLay,
			     int dstTile, int srcX, int srcY, int dstX, int dstY, int w, int h );
    LayerList layerList();

    void rotateLayer180(KisLayer *_layer);
    void rotateLayerLeft90(KisLayer *_layer);
    void rotateLayerRight90(KisLayer *_layer);
    void mirrorLayerX(KisLayer *_layer);
    void mirrorLayerY(KisLayer *_layer);

    void mergeAllLayers();
    void mergeVisibleLayers();
    void mergeLinkedLayers();
    void mergeLayers(QList<KisLayer>);

    KisImage* newImage(const QString& _name, int w, int h, cMode cm = CM_RGBA, bgMode bgm = BM_WHITE);
    bool saveImage( const QString& file, KisImage *img );
    bool saveCurrentImage( const QString& file );
    bool loadImage( const QString& file );
    void removeImage( KisImage *img );

    QString currentImage();

    void setCurrentImage(KisImage *img);

    bool isEmpty();


    QStringList images();

    void renameImage(const QString& oldname, const QString &newname);

public slots:

  void setCurrentLayerOpacity( double opacity )
    {  setLayerOpacity( (uchar) ( opacity * 255 / 100 ) ); };
  void slotImageUpdated();
  void slotImageUpdated( const QRect& rect );
  void slotLayersUpdated();

  void slotNewImage();
  void setCurrentImage(const QString& _name);

  void slotRemoveImage( const QString& name );

signals:

  void docUpdated();
  void docUpdated( const QRect& rect );
  void layersUpdated();
  void imageListUpdated();

protected:
  KoCommandHistory m_commands;

private:

  QList <KisImage> m_Images;
  KisImage *m_pCurrent;
  NewDialog       *m_pNewDialog;
};

#endif // __kis_doc_h__
