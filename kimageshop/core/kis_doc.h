/*
 *  kis_doc.h - part of KImageShop
 *
 *  Copyright (c) 1999-2000 Matthias Elter  <me@kde.org>
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
//#include <koUndo.h> //jwc

#include <iostream.h>
#include "kis_image.h"
#include "kis_global.h"
#include "kis_view.h"
#include "kis_selection.h"

class KisBrush;
class KisSelection;
class NewDialog;

/*
 * A KisDoc can hold multiple KisImages.
 *
 * KisDoc->current() returns a Pointer to the currently active KisImage.
 */

class KisDoc : public KoDocument
{
    Q_OBJECT

public:
    KisDoc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    virtual ~KisDoc();

	/*
	 * Reimplemented from KoDocument.
	 * See koDocument.h.
	 */

    virtual KoMainWindow* createShell();

    virtual QCString mimeType() const;

    virtual bool initDoc();

    virtual QDomDocument saveXML();
    
    virtual bool loadXML( QIODevice *, const QDomDocument & doc ); 

    virtual bool completeLoading( KoStore* store );
    
    virtual bool completeSaving( KoStore* );

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

	/*
	 * KOffice undo/redo.
	 */
    //KoCommandHistory* commandHistory() { return &m_commands; }; //jwc

	/*
	 * Use QPainter p to paint a rectangular are of the current image.
	 */
    void paintPixmap( QPainter *p, QRect area );

	/*
	 * Create new KisImage, add it to our KisImage list and make it the current Image.
	 */
    KisImage* newImage(const QString& name, int width, int height, cMode cm = cm_RGBA, uchar bitDepth = 8);

	/*
	 * Remove img from our list and delete it.
	 */
    void removeImage( KisImage *img );

	/*
	 * Return apointer to the current image.
	 */
    KisImage* current();

	/*
	 * Return the name of the current image.
	 */
    QString  currentImage();

	/*
	 * Make img the current image.
	 */
    void setCurrentImage(KisImage *img);

	/*
	 * Does the doc contain any images?
	 */
    bool isEmpty();

	/*
	 * Return a list of image names.
	 */
    QStringList images();

	/*
	 *  save current image as Qt image (standard image formats)
	 */    
    bool saveAsQtImage(QString file);

	/*
	 *  write curren screen contents into layer
	 */    
    void CopyToLayer(KisView *pView);

	/*
	 *  auxillary to above
	 */    
    bool QtImageToLayer(QImage *qimage, KisView *pView);

	/*
	 *  copy rectangular area of layer to Qt Image
	 */    
    bool LayerToQtImage(QImage *qimage, KisView *pView, QRect & clipRect);

	/*
	 *  set selection rectangle for the document
	 */    
    void setSelectRect(QRect & rect);    

	/*
	 *  set selection rectangle for the document
	 */    
    QRect getSelectRect() { return selectRect; }    

	/*
	 *  set selection or clip rectangle for the document
	 */    
    bool setClipImage(); 
    
	/*
	 *  get selection or clip image for the document
	 */    
    QImage *getClipImage() { return m_pClipImage; }

	/*
	 *  currrent selection for document
	 */    

    KisSelection *m_pSelection;

public slots:
    void slotImageUpdated();
    void slotImageUpdated( const QRect& rect);
    void slotLayersUpdated();

    bool slotNewImage();
    void setCurrentImage(const QString& _name);
    void slotRemoveImage( const QString& name );

signals:
    void docUpdated();
    void docUpdated( const QRect& rect );
    void layersUpdated();
    void imageListUpdated();

protected:

    /* reimplemented from koDocument - a document can have multiple
    views of the same data */
    virtual KoView* createViewInstance( QWidget* parent, const char* name );

    /* undo/redo - currently nonfunctional */
    //KoCommandHistory  m_commands; 
    
    /* list of images for the document - each document can have multiple 
    images and each image must have at least one layer. however, a document
    can only have one current image, which is what is loaded and saved -
    the permanent data associated with it. This coresponds to an
    image, but that image is interchangeable */
    QList <KisImage> m_Images;

    KisImage  * m_pCurrent;
    NewDialog * m_pNewDialog;
    QImage    * m_pClipImage;
    
    /* selection rectangle for the document - there can only be one
    no matter how many views or images - possible list of selections 
    may be useful later, but list is questionable because current 
    selection is tied to gui focus in current layer */
    QRect     selectRect;    
};

#endif // __kis_doc_h__
