/*
 *  kis_doc.h - part of Krayon
 *
 *  Copyright (c) 1999-2000 Matthias Elter  <me@kde.org>
 *  Copyright (c) 2001 Toshitaka Fujioka  <fujioka@kde.org>
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
#include "koUndo.h"

#include <iostream.h>
#include "kis_global.h"

#include "kis_view.h"
#include "kis_selection.h"
#include "kis_framebuffer.h"

class NewDialog;
class KisImage;

//class KisView;
//class KisSelection;
//class KisFrameBuffer;

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

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

	/*
	 *  Current shell or frame window for this doc.
	 */
    KoMainWindow* currentShell();

	/*
	 *  Force a redraw of scrollbars and other widgets in all
     *  shell windows for this document.
	 */
    void resetShells();

	/*
	 * KOffice undo/redo.
	 */
    KoCommandHistory* commandHistory() { return &m_commands; };

	/*
	 * Use QPainter p to paint a rectangular are of the current image.
	 */
    void paintPixmap( QPainter* p, QRect area );

	/*
	 * Create new KisImage, add it to our KisImage list and make it the current Image.
	 */
    KisImage* newImage(const QString& name, int width, int height, cMode cm = cm_RGBA, uchar bitDepth = 8);

	/*
	 * Remove img from our list and delete it.
	 */
    void removeImage( KisImage *img );

	/*
	 * Return apointer to the current view.
	 */
    KisView* currentView();

	/*
	 * Return apointer to the current image.
	 */
    KisImage* current();

	/*
	 * Return the name of the current image.
	 */
    QString currentImage();

	/*
	 * Make img the current image.
	 */
    void setCurrentImage(KisImage *img);

	/*
	 * Does the doc contain any images?
	 */
    bool isEmpty() const;

	/*
	 * Return a list of image names.
	 */
    QStringList images();

	/*
	 * Rename an image
	 */
    void renameImage(QString & oldName, QString & newName);

	/*
	 *  save current image as Qt image (standard image formats)
	 */
    bool saveAsQtImage(QString file, bool wholeImage);

	/*
	 *  needs to go in kis_framebuffer.cc
	 */
    bool QtImageToLayer(QImage *qimage, KisView *pView);

	/*
	 *  copy rectangular area of layer to Qt Image
	 */
    bool LayerToQtImage(QImage *qimage, KisView *pView, QRect & clipRect);

	/*
	 *  set selection or clip rectangle for the document
	 */
    bool setClipImage();

	/*
	 *  get selection or clip image for the document
	 */
    QImage *getClipImage() { return m_pClipImage; }

	/*
	 *  delete clip image for the document
	 */
    void removeClipImage();

	/*
	 *  get currrent selection for document
	 */
    KisSelection *getSelection() { return m_pSelection; }

    /*
	 *  set selection for document
	 */
    void setSelection(QRect & r);

    /*
	 *  clear selection for document -
	 */
    void clearSelection();

    /*
	 *  does the document have a selection ?
	 */
    bool hasSelection();

	/*
	 *  get FrameBuffer
	 */
    KisFrameBuffer *frameBuffer() { return m_pFrameBuffer; }

    QRect getImageRect();

    void setImage( QString imageName ); // for print, save file and load file.

    /* 
     * Pen tool settings 
     */
    struct PenToolSettings {
        PenToolSettings() {
            opacity = 255;
            paintThreshold = 128;
            paintWithPattern = false;
            paintWithGradient = false;
        }

        uint opacity, paintThreshold;
        bool paintWithPattern, paintWithGradient;
    };

    PenToolSettings getPenToolSettings() const { return penToolSettings; }
    void setPenToolSettings( PenToolSettings s );
    
    /* 
     * Brush tool settings
     */
    struct BrushToolSettings {
        BrushToolSettings() {
            opacity = 255;
            blendWithCurrentGradient = false;
            blendWithCurrentPattern = false;
        }

        uint opacity;
        bool blendWithCurrentGradient, blendWithCurrentPattern;
    };

    BrushToolSettings getBrushToolSettings() const { return brushToolSettings; }
    void setBrushToolSettings( BrushToolSettings s );

    /* 
     * Airbrush tool settings
     */
    struct AirbrushToolSettings {
        AirbrushToolSettings() {
            opacity = 255;
            useCurrentGradient = false;
            useCurrentPattern = false;
        }

        uint opacity;
        bool useCurrentGradient, useCurrentPattern;
    };

    AirbrushToolSettings getAirbrushToolSettings() const { return airbrushToolSettings; }
    void setAirbrushToolSettings( AirbrushToolSettings s );

    /* 
     * Eraser tool settings 
     */
    struct EraserToolSettings {
        EraserToolSettings() {
            opacity = 255;
            blendWithCurrentGradient = true;
            blendWithCurrentPattern = false;
        }

        uint opacity;
        bool blendWithCurrentGradient, blendWithCurrentPattern;
    };

    EraserToolSettings getEraserToolSettings() const { return eraserToolSettings; }
    void setEraserToolSettings( EraserToolSettings s );

    /* 
     * Line tool settings 
     */
    struct LineToolSettings {
        LineToolSettings() {
            thickness = 4;
            opacity = 255;
            fillInteriorRegions = false;
            useCurrentPattern = false;
            fillWithGradient = false;
        }

        uint thickness, opacity;
        bool fillInteriorRegions, useCurrentPattern, fillWithGradient;
    };

    LineToolSettings getLineToolSettings() const { return lineToolSettings; }
    void setLineToolSettings( LineToolSettings s );

    /* 
     * Polyline tool settings 
     */
    struct PolylineToolSettings {
        PolylineToolSettings() {
            thickness = 4;
            opacity = 255;
            fillInteriorRegions = false;
            useCurrentPattern = false;
            fillWithGradient = false;
        }

        uint thickness, opacity;
        bool fillInteriorRegions, useCurrentPattern, fillWithGradient;
    };

    PolylineToolSettings getPolyLineToolSettings() const { return polylineToolSettings; }
    void setPolylineToolSettings( PolylineToolSettings s );

    /* 
     * Rectangle tool settings 
     */
    struct RectangleToolSettings {
        RectangleToolSettings() {
            thickness = 4;
            opacity = 255;
            fillInteriorRegions = false;
            useCurrentPattern = false;
            fillWithGradient = false;
        }

        uint thickness, opacity;
        bool fillInteriorRegions, useCurrentPattern, fillWithGradient;
    };

    RectangleToolSettings getRectangleToolSettings() const { return rectangleToolSettings; }
    void setRectangleToolSettings( RectangleToolSettings s );

    /* 
     * Ellipse tool settings 
     */
    struct EllipseToolSettings {
        EllipseToolSettings() {
            thickness = 4;
            opacity = 255;
            fillInteriorRegions = false;
            useCurrentPattern = false;
            fillWithGradient = false;
        }

        uint thickness, opacity;
        bool fillInteriorRegions, useCurrentPattern, fillWithGradient;
    };

    EllipseToolSettings getEllipseToolSettings() const { return ellipseToolSettings; }
    void setEllipseToolSettings( EllipseToolSettings s );

    /* 
     * Filler tool settings 
     */
    struct FillerToolSettings {
        FillerToolSettings() {
            opacity = 255;
            fillWithPattern = false;
            fillWithGradient = false;
        }

        uint opacity;
        bool fillWithPattern, fillWithGradient;
    };

    FillerToolSettings getFillerToolSettings() const { return fillerToolSettings; }
    void setFillerToolSettings( FillerToolSettings s );

    /* 
     * Color changer settings 
     */
    struct ColorChangerSettings {
        ColorChangerSettings() {
            opacity = 255;
            fillWithPattern = false;
            fillWithGradient = false;
        }

        uint opacity;
        bool fillWithPattern, fillWithGradient;
    };

    ColorChangerSettings getColorChangerSettings() const { return colorChangerSettings; }
    void setColorChangerSettings( ColorChangerSettings s );

    /* 
     * Stamp (Pattern) tool settings
     */
    struct StampToolSettings {
        StampToolSettings() {
            opacity = 255;
            blendWithCurrentGradient = false;
        }

        uint opacity;
        bool blendWithCurrentGradient;
    };

    StampToolSettings getStampToolSettings() const { return stampToolSettings; }
    void setStampToolSettings( StampToolSettings s );

    /* 
     * Gradients settings 
     */
    struct GradientsSettings {
        GradientsSettings() {
            opacity = 100;
            offset = 0;
            mode = i18n( "Normal" );
            blend = i18n( "FG to BG (RGB)" );
            gradient = i18n( "Vertical" );
            repeat = i18n( "None" );
        }

        uint opacity, offset;
        QString mode, blend, gradient, repeat;
    };

    GradientsSettings getGradientsToolSettings() const { return gradientsSettings; }
    void setGradientsSettings( GradientsSettings s );

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


    /* save images */
    QDomElement saveImages( QDomDocument &doc );

    /* save layers */
    QDomElement saveLayers( QDomDocument &doc, KisImage *img );

    /* save channels */
    QDomElement saveChannels( QDomDocument &doc, KisLayer *lay );

    /* save tool settings */
    QDomElement saveToolSettings( QDomDocument &doc );

    /* save pen tool settings */
    QDomElement savePenToolSettings( QDomDocument &doc );

    /* save brush tool settings */
    QDomElement saveBrushToolSettings( QDomDocument &doc );

    /* save airbrush tool settings */
    QDomElement saveAirbrushToolSettings( QDomDocument &doc );

    /* save eraser tool settings */
    QDomElement saveEraserToolSettings( QDomDocument &doc );

    /* save line tool settings */
    QDomElement saveLineToolSettings( QDomDocument &doc );

    /* save polyline tool settings */
    QDomElement savePolylineToolSettings( QDomDocument &doc );

    /* save rectangle tool settings */
    QDomElement saveRectangleToolSettings( QDomDocument &doc );

    /* save ellipse tool settings */
    QDomElement saveEllipseToolSettings( QDomDocument &doc );

    /* save filler tool settings */
    QDomElement saveFillerToolSettings( QDomDocument &doc );

    /* save Color changer settings */
    QDomElement saveColorChangerSettings( QDomDocument &doc );

    /* save Stamp (Pattern) tool settings */
    QDomElement saveStampToolSettings( QDomDocument &doc );

    /* save Gradients settings */
    QDomElement saveGradientsSettings( QDomDocument &doc );


    /* load images */
    bool loadImages( QDomElement &elem );

    /* load layers */
    bool loadLayers( QDomElement &elem, KisImage *img );

    /* load channels */
    void loadChannels( QDomElement &elem, KisLayer *lay );

    /* load tool settings */
    void loadToolSettings( QDomElement &elem );

    /* load pen tool settings */
    void loadPenToolSettings( QDomElement &elem );

    /* load brush tool settings */
    void loadBrushToolSettings( QDomElement &elem );

    /* load airbrush tool settings */
    void loadAirbrushToolSettings( QDomElement &elem );

    /* load eraser tool settings */
    void loadEraserToolSettings( QDomElement &elem );

    /* load line tool settings */
    void loadLineToolSettings( QDomElement &elem );

    /* load polyline tool settings */
    void loadPolylineToolSettings( QDomElement &elem );

    /* load rectangle tool settings */
    void loadRectangleToolSettings( QDomElement &elem );

    /* load ellipse tool settings */
    void loadEllipseToolSettings( QDomElement &elem );

    /* load filler tool settings */
    void loadFillerToolSettings( QDomElement &elem );

    /* load Color changer settings */
    void loadColorChangerSettings( QDomElement &elem );

    /* load Stamp (Pattern) tool settings */
    void loadStampToolSettings( QDomElement &elem );

    /* load Gradients settings */
    void loadGradientsSettings( QDomElement &elem );

    /* load old file format */
    bool loadXMLOldFileFormat( QDomElement &image );
    void completeLoadingOldFileFormat( KoStore *store );
    bool oldFileFormat;


    /* undo/redo */
    KoCommandHistory m_commands;

    /* list of images for the document - each document can have multiple
    images and each image must have at least one layer. however, a document
    can only have one current image, which is what is loaded and saved -
    the permanent data associated with it. This coresponds to an
    image, but that image is interchangeable */
    QList <KisImage> m_Images;

    KisImage  * m_pCurrent;
    NewDialog * m_pNewDialog;
    QImage    * m_pClipImage;

    KisSelection *m_pSelection;
    KisFrameBuffer *m_pFrameBuffer;

private:
    PenToolSettings         penToolSettings;
    BrushToolSettings       brushToolSettings;
    AirbrushToolSettings    airbrushToolSettings;
    EraserToolSettings      eraserToolSettings;
    LineToolSettings        lineToolSettings;
    PolylineToolSettings    polylineToolSettings;
    RectangleToolSettings   rectangleToolSettings;
    EllipseToolSettings     ellipseToolSettings;
    FillerToolSettings      fillerToolSettings;
    ColorChangerSettings    colorChangerSettings;
    StampToolSettings       stampToolSettings;
    GradientsSettings       gradientsSettings;
};

#endif // __kis_doc_h__
