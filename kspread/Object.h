/* This file is part of the KDE project
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_OBJECT_H
#define KSPREAD_OBJECT_H

#include "Sheet.h"

#include <klocale.h>
#include <KoBrush.h>
#include <KoChild.h>
#include <KoDom.h>
#include <KoOasisLoadingContext.h>
#include <KoPen.h>
#include <KoPictureCollection.h>
#include <KoRect.h>
#include <KoSize.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlReader.h>
//Added by qt3to4:
#include <QPixmap>

namespace KoChart
{
class Part;
}

namespace KSpread
{

class Doc;
class EmbeddedKOfficeObject;
class EmbeddedObject;
class View;
class ChartBinding;

enum ObjType {
  OBJECT_GENERAL,
  OBJECT_KOFFICE_PART,
  OBJECT_CHART,
  OBJECT_PICTURE
};

enum SelectionMode {
    SM_NONE = 0,
    SM_MOVERESIZE = 1,
    SM_ROTATE = 2,
    SM_PROTECT = 3
};

enum PictureMirrorType {
  PM_NORMAL = 0,
  PM_HORIZONTAL = 1,
  PM_VERTICAL = 2,
  PM_HORIZONTALANDVERTICAL = 3
};

enum ImageEffect {
  IE_NONE = -1,
  IE_CHANNEL_INTENSITY = 0,
  IE_FADE = 1,
  IE_FLATTEN = 2,
  IE_INTENSITY = 3,
  IE_DESATURATE = 4,
  IE_CONTRAST = 5,
  IE_NORMALIZE = 6,
  IE_EQUALIZE = 7,
  IE_THRESHOLD = 8,
  IE_SOLARIZE = 9,
  IE_EMBOSS = 10,
  IE_DESPECKLE = 11,
  IE_CHARCOAL = 12,
  IE_NOISE = 13,
  IE_BLUR = 14,
  IE_EDGE = 15,
  IE_IMPLODE = 16,
  IE_OIL_PAINT = 17,
  IE_SHARPEN = 18,
  IE_SPREAD = 19,
  IE_SHADE = 20,
  IE_SWIRL = 21,
  IE_WAVE = 22
};

enum AlignType {
  AT_PAGE,
  AT_CELL
};

/**
 * Contains basic information for every object.
 */
class EmbeddedObject
{
  public:
    EmbeddedObject( Sheet *_sheet, const KoRect& _geometry );
    virtual ~EmbeddedObject();
    virtual ObjType getType() const { return OBJECT_GENERAL; }
    virtual QString getTypeString() const
        { return QString(); }

    KoRect geometry();
    void setGeometry( const KoRect &rect );
    virtual void moveBy( const KoPoint &_point );
    virtual void moveBy( double _dx, double _dy );
    virtual void resizeBy( const KoSize & _size );
    virtual void resizeBy( double _dx, double _dy );

    Sheet* sheet() const { return m_sheet; }

    void setSelected(bool s) { m_selected = s; }
    bool isSelected() const { return m_selected; }

    void setObjectName( const QString &_objectName )
        { m_objectName = _objectName; }
    QString getObjectName() const
        { return m_objectName; }

    virtual void setProtect( bool b ) { m_protect = b; }
    bool isProtect() const { return m_protect; }

    virtual void setKeepRatio( bool b ) { m_keepRatio = b; }
    bool isKeepRatio() const { return m_keepRatio; }

    virtual void rotate( float _angle ) { angle = _angle; }
    virtual float getAngle() const { return angle; }

    struct KSpreadOasisSaveContext
    {
      KSpreadOasisSaveContext( KoXmlWriter &_xmlWriter, KoGenStyles &_context,
                               int &_indexObj, int &_partIndexObj )
        : xmlWriter( _xmlWriter )
          , context( _context )
          , indexObj( _indexObj )
          , partIndexObj( _partIndexObj )
          {};

          KoXmlWriter &xmlWriter;
          KoGenStyles &context;
          int &indexObj;
          int &partIndexObj;
    };

    virtual bool load( const KoXmlElement& element );
    virtual void loadOasis(const KoXmlElement &element, KoOasisLoadingContext & context );
    virtual QDomElement save( QDomDocument& doc );
    virtual bool saveOasisObject( KSpreadOasisSaveContext &sc ) const;
    virtual void draw( QPainter *_painter );

    /**
     * Renders the embedded object to a pixmap and returns the result.
     * This is a convenience function which calculates the necessary x and y zoom factors to render
     * the pixmap at the given size and calls toPixmap(double,double)
     *
     * @param size Specifies the desired size of the returned pixmap.
     */
    QPixmap toPixmap( QSize size );

    /**
     * Renders the embedded object to a pixmap at 100% scale.  Equivalent to calling toPixmap( 1.0, 1.0 )
     */
    QPixmap toPixmap();

    /**
     * Renders the embedded object to a pixmap at the specified x and y scale and returns the result.
     */
    virtual QPixmap toPixmap( double xZoom , double yZoom );


    void paintSelection( QPainter *_painter, SelectionMode selectionMode );
    virtual QCursor getCursor( const QPoint &_point, ModifyType &_modType, QRect &geometry ) const;

    virtual void removeFromObjList()
        { inObjList = false; }
    virtual void addToObjList()
        { inObjList = true; }
    virtual void incCmdRef()
        { cmds++; }
    virtual void decCmdRef()
        { cmds--; doDelete(); }

    //TEMP:
    virtual QBrush getBrush() const { return m_brush.getBrush(); }


  protected:
    /**
     * Get the element name for saving the object
     */
    virtual const char * getOasisElementName() const = 0;
    virtual void saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj ) const;
    virtual bool saveOasisObjectAttributes( KSpreadOasisSaveContext &sc ) const;

    virtual void doDelete();

    /**
     * Calculates the X and Y zoom factors required to render the embedded object at the given size
     *
     * @param desiredSize The desired size for the embedded object to be drawn at
     * @param xZoom This will be set to the required X zoom factor
     * @param yZoom This will be set to the required Y zoom factor
     */
    void calculateRequiredZoom( QSize desiredSize , double& xZoom, double& yZoom );

    KoRect m_geometry;
    Sheet *m_sheet;
    Canvas *m_canvas;
    QString m_objectName;
    bool m_selected:1;
    bool m_protect:1;
    bool m_keepRatio:1;
    bool inObjList:1;
    int cmds;
    float angle;

    KoPen pen;
    KoBrush m_brush;
};

/**
 * Holds an embedded koffice object.
 */
class EmbeddedKOfficeObject : public EmbeddedObject
{
  public:
    EmbeddedKOfficeObject( Doc *parent, Sheet *_sheet, KoDocument* doc, const KoRect& geometry );
    EmbeddedKOfficeObject( Doc *parent, Sheet *_sheet );
    virtual ~EmbeddedKOfficeObject();
    virtual ObjType getType() const { return OBJECT_KOFFICE_PART; }
    virtual QString getTypeString() const
        { return i18n("Embedded Object"); }

    Doc* parent();
    KoDocumentChild *embeddedObject();

    bool load( const KoXmlElement& element );
    virtual void loadOasis(const KoXmlElement &element, KoOasisLoadingContext & context );
    QDomElement save( QDomDocument& doc );
    virtual void draw( QPainter *_painter );

    /**
     * See EmbeddedObject::toPixmap(double,double)
     */
    virtual QPixmap toPixmap(double xZoom , double yZoom);

    void activate( View *_view, Canvas *_canvas );
    void deactivate();

  protected:
    void updateChildGeometry();
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KSpreadOasisSaveContext &sc ) const;

    KoDocumentChild *m_embeddedObject;
    Doc *m_parent;
};


/**
 * Holds an embedded chart object.
 */
class EmbeddedChart : public EmbeddedKOfficeObject
{
  public:
    EmbeddedChart( Doc *_spread, Sheet *_sheet, KoDocument* doc, const KoRect& _rect );
    EmbeddedChart( Doc *_spread, Sheet *_sheet );
    virtual ~EmbeddedChart();
    virtual ObjType getType() const { return OBJECT_CHART; }
    virtual QString getTypeString() const
        { return i18n("Chart"); }

    void setDataArea( const QRect& _data );
    void update();

    bool load( const KoXmlElement& element );
    virtual void loadOasis(const KoXmlElement &element, KoOasisLoadingContext & context );
    QDomElement save( QDomDocument& doc );
    virtual void draw( QPainter *_painter );

/**
     * @reimp
 */
    bool loadDocument( KoStore* _store );

    KoChart::Part* chart();

  protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KSpreadOasisSaveContext &sc ) const;

  private:
    ChartBinding *m_pBinding;
};


/**
 * Holds a picture object.
 */
class EmbeddedPictureObject : public EmbeddedObject
{
  public:
    EmbeddedPictureObject(Sheet *_sheet, const KoRect& _geometry, KoPictureCollection *_imageCollection );
    EmbeddedPictureObject(Sheet *_sheet, const KoRect& _geometry, KoPictureCollection *_imageCollection, const KoPictureKey & key );
    EmbeddedPictureObject( Sheet *_sheet, KoPictureCollection *_imageCollection );
    virtual ~EmbeddedPictureObject();
    EmbeddedPictureObject &operator=( const EmbeddedPictureObject & );

    virtual ObjType getType() const { return OBJECT_PICTURE; }
    virtual QString getTypeString() const
        { return i18n("Picture"); }
    bool load( const KoXmlElement& element );
    virtual void loadOasis(const KoXmlElement &element, KoOasisLoadingContext & context );
    QDomElement save( QDomDocument& doc );
    virtual void draw( QPainter *_painter );

    /**
     * See EmbeddedObject::toPixmap(double,double)
     */
    virtual QPixmap toPixmap(double xZoom , double yZoom);

        /**
     * Only used as a default value in the filedialog, in changePicture
     * \warning Do not use for anything else
         */
    QString getFileName() const
    { return image.getKey().filename(); }

    KoPictureKey getKey() const
    { return image.getKey(); }

    QSize originalSize() const
    { return image.getOriginalSize(); }

    void setPicture( const KoPictureKey & key );

    void reload( void );

    //virtual QDomDocumentFragment save( QDomDocument& doc, double offset );

    //virtual double load(const KoXmlElement &element);
    //virtual void loadOasis(const KoXmlElement &element, KoOasisLoadingContext & context, KPRLoadingInfo *info);

//     virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
//                        int /* page */, SelectionMode selectionMode, bool drawContour = false );

    QPixmap getOriginalPixmap();
    PictureMirrorType getPictureMirrorType() const { return mirrorType; }
    int getPictureDepth() const { return depth; }
    bool getPictureSwapRGB() const { return swapRGB; }
    bool getPictureGrayscal() const { return grayscal; }
    int getPictureBright() const { return bright; }

    ImageEffect getImageEffect() const {return m_effect;}
    QVariant getIEParam1() const {return m_ie_par1;}
    QVariant getIEParam2() const {return m_ie_par2;}
    QVariant getIEParam3() const {return m_ie_par3;}
    void setImageEffect(ImageEffect eff) { m_effect = eff; }
    void setIEParams(QVariant p1, QVariant p2, QVariant p3) {
      m_ie_par1=p1;
      m_ie_par2=p2;
      m_ie_par3=p3;
    }

    void setPictureMirrorType(const PictureMirrorType &_mirrorType) { mirrorType = _mirrorType; }
    void setPictureDepth(int _depth) { depth = _depth; }
    void setPictureSwapRGB(bool _swapRGB) { swapRGB = _swapRGB; }
    void setPictureGrayscal(bool _grayscal) { grayscal = _grayscal; }
    void setPictureBright(int _bright) { bright = _bright; }

    KoPicture picture() const { return image;}

    void loadPicture( const QString & fileName );

    virtual void flip(bool horizontal );

  protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KSpreadOasisSaveContext &sc ) const;

    QPixmap changePictureSettings( QPixmap _tmpPixmap );
    virtual void saveOasisPictureElement( KoGenStyle &styleobjectauto ) const;
    void loadOasisPictureEffect(KoOasisLoadingContext & context );
    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;

    /**
     * @internal
     * Draws the shadow
     */
    void drawShadow( QPainter* _painter,  KoZoomHandler* _zoomHandler);

    QPixmap generatePixmap(KoZoomHandler*_zoomHandler);
    QString convertValueToPercent( int val ) const;

    KoPictureCollection *imageCollection;
    KoPicture image;

    PictureMirrorType mirrorType, m_cachedMirrorType;
    int depth, m_cachedDepth;
    bool swapRGB, m_cachedSwapRGB;
    bool grayscal, m_cachedGrayscal;
    int bright, m_cachedBright;

    //image effect and its params
    ImageEffect m_effect, m_cachedEffect;
    QVariant m_ie_par1, m_cachedPar1;
    QVariant m_ie_par2, m_cachedPar2;
    QVariant m_ie_par3, m_cachedPar3;

    QPixmap m_cachedPixmap;
    QRect m_cachedRect;
};

 } // namespace KSpread
#endif // KSPREAD_OBJECT_H
