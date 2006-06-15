// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kpobject_h
#define kpobject_h

#include <qbrush.h>
#include <qcursor.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>
#include <QPixmap>

#include "global.h"

#include "KPrGradient.h"
#include "KoPointArray.h"
#include <klocale.h>
#include <KoPoint.h>
#include <KoSize.h>
#include <KoRect.h>
#include <KoStyleStack.h>
#include <KoOasisStyles.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include "KPrLoadingInfo.h"
#include "KPrBrush.h"
#include <KoPen.h>

class KoSavingContext;
class QPainter;
class KPrObjectAdaptor;
class QDomDocumentFragment;
class QDomDocument;
class QDomElement;
class KoTextZoomHandler;
class KPrDocument;
class KPrTextObject;
class KoTextObject;
class KoOasisContext;

#define RAD_FACTOR 180.0 / M_PI
#define MIN_SIZE 10

class KPrObject
{
public:
    KPrObject();
    virtual ~KPrObject();


    virtual KPrObjectAdaptor* dbusObject();

    virtual void setSelected( bool _selected )
        { selected = _selected; }
    virtual void rotate( float _angle )
        { angle = _angle; }
    virtual void setSize( const KoSize & _size )
        { setSize( _size.width(), _size.height() ); }
    virtual void setSize(double _width, double _height )
        { ext = KoSize( qMax( (double)MIN_SIZE, _width ), qMax( (double)MIN_SIZE, _height ) ); } // no object is smaller than 10x10
    virtual void setOrig( const KoPoint & _point )
        { orig = _point; }
    virtual void setOrig( double _x, double _y )
        { orig = KoPoint( _x, _y ); }
    virtual void moveBy( const KoPoint &_point )
        { orig = orig + _point; }
    virtual void moveBy( double _dx, double _dy )
        { orig = orig + KoPoint( _dx, _dy ); }
    virtual void resizeBy( const KoSize & _size )
        { resizeBy( _size.width(), _size.height() ); }
    virtual void resizeBy( double _dx, double _dy )
        { setSize( ext.width() + _dx, ext.height() + _dy ); } // call (possibly reimplemented) setSize

    virtual void setShadowParameter(int _distance,ShadowDirection _direction,const QColor &_color)
        {
            shadowDistance = _distance;
            shadowDirection = _direction;
            shadowColor = _color;
        }

    virtual void setShadowDistance( int _distance )
        { shadowDistance = _distance; }
    virtual void setShadowDirection( ShadowDirection _direction )
        { shadowDirection = _direction; }
    virtual void setShadowColor( const QColor & _color )
        { shadowColor = _color; }
    virtual void setEffect( Effect _effect )
        { effect = _effect; }
    virtual void setEffect2( Effect2 _effect2 )
        { effect2 = _effect2; }
    virtual void setAppearStep( int _appearStep )
        { appearStep = _appearStep; }
    virtual void setDisappear( bool b )
        { disappear = b; }
    virtual void setDisappearStep( int num )
        { disappearStep = num; }
    virtual void setEffect3( Effect3 _effect3)
        { effect3 = _effect3; }
    virtual void setAppearSpeed( EffectSpeed _speed )
        { m_appearSpeed = _speed; }
    virtual void setDisappearSpeed( EffectSpeed _speed )
        { m_disappearSpeed = _speed; }
    virtual void setAppearTimer( int _appearTimer )
        { appearTimer = _appearTimer; }
    virtual void setDisappearTimer( int _disappearTimer )
        { disappearTimer = _disappearTimer; }
    virtual void setAppearSoundEffect( bool b )
        { appearSoundEffect = b; }
    virtual void setDisappearSoundEffect( bool b )
        { disappearSoundEffect = b; }
    virtual void setAppearSoundEffectFileName( const QString & _a_fileName )
        { a_fileName = _a_fileName; }
    virtual void setDisappearSoundEffectFileName( const QString &_d_fileName )
        { d_fileName = _d_fileName; }

    virtual void setObjectName( const QString &_objectName )
        { m_objectName = _objectName; }
    virtual QString getObjectName() const
        { return m_objectName; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context,  KPrLoadingInfo *info);

    struct KPOasisSaveContext
    {
        KPOasisSaveContext( KoXmlWriter &_xmlWriter, KoSavingContext &_context,
                            int &_indexObj, int &_partIndexObj, bool _onMaster )
            : xmlWriter( _xmlWriter )
            , context( _context )
            , indexObj( _indexObj )
            , partIndexObj( _partIndexObj )
            , onMaster( _onMaster ) {};

        KoXmlWriter &xmlWriter;
        KoSavingContext &context;
        int &indexObj;
        int &partIndexObj;
        bool onMaster;
    };

    virtual bool saveOasisObject( KPOasisSaveContext &sc ) const;

    //return true if we have a animation into object
    bool saveOasisObjectStyleShowAnimation( KoXmlWriter &animation, int objectId );
    bool saveOasisObjectStyleHideAnimation( KoXmlWriter &animation, int objectId );

    virtual void flip(bool horizontal );

    virtual ObjType getType() const
        { return OT_UNDEFINED; }
    virtual QString getTypeString() const
        { return QString(); }

    virtual bool isSelected() const
        { return selected; }
    virtual float getAngle() const
        { return angle; }
    virtual int getShadowDistance() const
        { return shadowDistance; }
    virtual ShadowDirection getShadowDirection() const
        { return shadowDirection; }
    virtual QColor getShadowColor() const
        { return shadowColor; }
    virtual KoSize getSize() const
        { return ext; }
    virtual KoPoint getOrig() const
        { return orig; }
    KoRect getRect() const
        { return KoRect( getOrig(), getSize() ); }

    virtual KoSize getRealSize() const;
    virtual KoPoint getRealOrig() const;
    KoRect getRealRect() const;
    /**
     * @brief Get the rect which has to be repainted
     *
     * This also contains the shadow.
     */
    KoRect getRepaintRect() const;

    virtual Effect getEffect() const
        { return effect; }
    virtual Effect2 getEffect2() const
        { return effect2; }
    virtual Effect3 getEffect3() const
        { return effect3; }
    virtual EffectSpeed getAppearSpeed() const
        { return m_appearSpeed; }
    virtual EffectSpeed getDisappearSpeed() const
        { return m_disappearSpeed; }
    virtual int getAppearTimer() const
        { return appearTimer; }
    virtual int getDisappearTimer() const
        { return disappearTimer; }
    virtual bool getAppearSoundEffect() const
        { return appearSoundEffect; }
    virtual bool getDisappearSoundEffect() const
        { return disappearSoundEffect; }
    virtual QString getAppearSoundEffectFileName() const
        { return a_fileName; }
    virtual QString getDisappearSoundEffectFileName() const
        { return d_fileName; }
    /**
     * get the step when the object appears on the site
     */
    virtual int getAppearStep() const
        { return appearStep; }
    virtual int getSubPresSteps() const
        { return 0; }
    virtual bool getDisappear() const
        { return disappear; }
    /**
     * get the step when the object disappears from the site
     */
    virtual int getDisappearStep() const
        { return disappearStep; }

    virtual void setOwnClipping( bool _ownClipping )
        { ownClipping = _ownClipping; }
    virtual void setSubPresStep( int _subPresStep )
        { subPresStep = _subPresStep; }
    virtual void doSpecificEffects( bool _specEffects, bool _onlyCurrStep = true )
        { specEffects = _specEffects; onlyCurrStep = _onlyCurrStep; }

    // the main drawing method.
    virtual void draw( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                       int /*pageNum*/, SelectionMode selectionMode, bool drawContour = false );

    /**
     * @brief Check if point lies in rect of object
     *
     * @param point to check
     *
     * @return true if point lies in rect of object
     * @return false otherwise
     */
    virtual bool contains( const KoPoint &point ) const;

    /**
     * @brief Check if rect intersects with rect of object
     *
     * @param rect to check
     *
     * @return true if rect interesct with rect of object
     * @return false otherwise
     */
    virtual bool intersects( const KoRect & rect ) const;

    virtual QCursor getCursor( const KoPoint &_point, ModifyType &_modType, KPrDocument *doc ) const;

    KoRect rotateRectObject() const;
    void rotateObject(QPainter *paint,KoTextZoomHandler *_zoomHandler);

    virtual void removeFromObjList()
        { inObjList = false; }
    virtual void addToObjList()
        { inObjList = true; }
    virtual void incCmdRef()
        { cmds++; }
    virtual void decCmdRef()
        { cmds--; doDelete(); }

    virtual void setProtect( bool b ) { protect = b; }
    bool isProtect() const { return protect; }

    virtual void setKeepRatio( bool b ) { keepRatio = b; }
    bool isKeepRatio() const { return keepRatio; }

    static void setupClipRegion( QPainter *painter, const QRegion &clipRegion );

    void paintSelection( QPainter *_painter,KoTextZoomHandler *_zoomHandler,
                         SelectionMode selectionMode );

    /**
     * Collect all textobjects.
     * (KPrTextObject returns the object it contains,
     * a KPrGroupObject returns all the text objects it contains)
     */
    virtual void addTextObjects( Q3PtrList<KoTextObject> & ) const {}

    virtual KPrTextObject *nextTextObject() { return 0L;} // deprecated
    virtual void getAllObjectSelectedList(Q3PtrList<KPrObject> &lst, bool force = false )
        { if (selected || force ) lst.append( this );}
    virtual KoPen getPen() const;

    bool hasAnimation() const;

    /**
     * Adds its edges to the KoGuides objectCollect all textobjects.
     */
    void addSelfToGuides( Q3ValueList<double> &horizontalPos, Q3ValueList<double> &verticalPos );

protected:
    /**
     * Helper function to caluclate the size and the orig of a point object
     * that might be also rotated.
     * The size and orig will be changed to the real size and orig in the
     * method.
     */
    static void getRealSizeAndOrigFromPoints( KoPointArray &points, float angle,
                                              KoSize &size, KoPoint &orig );

    /**
     * Modifies x and y to add the shadow offsets
     */
    void getShadowCoords( double& _x, double& _y ) const;

    virtual void doDelete();

    QDomElement createValueElement(const QString &tag, int value, QDomDocument &doc);
    QDomElement createGradientElement(const QString &tag, const QColor &c1, const QColor &c2,
                                      int type, bool unbalanced, int xfactor, int yfactor, QDomDocument &doc);
    QDomElement createPenElement(const QString &tag, const KoPen &pen, QDomDocument &doc);
    KoPen toPen(const QDomElement &element) const;

    QDomElement createBrushElement(const QString &tag, const QBrush &brush, QDomDocument &doc);
    QBrush toBrush(const QDomElement &element) const;

    QColor retrieveColor(const QDomElement &element, const QString &cattr="color",
                         const QString &rattr="red", const QString &gattr="green",
                         const QString &battr="blue") const;

    void saveOasisObjectProtectStyle( KoGenStyle &styleobjectauto ) const;
    void saveOasisShadowElement( KoGenStyle &styleobjectauto ) const;

    QString getStyle( KPOasisSaveContext &sc ) const;
    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;
    /**
     * Get the element name for saving the object
     */
    virtual const char * getOasisElementName() const = 0;
    //virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const = 0;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;
    virtual void saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj ) const;

    float angle;
    KoPoint orig;
    KoSize ext;
    int shadowDistance;
    ShadowDirection shadowDirection;
    QColor shadowColor;
    Effect effect;
    Effect2 effect2;
    Effect3 effect3;
    EffectSpeed m_appearSpeed;
    EffectSpeed m_disappearSpeed;
    int appearTimer, disappearTimer;
    QString a_fileName, d_fileName;
    QString m_objectName;
    // step when objects appears/disappears
    int appearStep, disappearStep;

    bool disappear:1;
    bool appearSoundEffect:1;
    bool disappearSoundEffect:1;
    bool selected:1;
    bool specEffects:1;
    bool onlyCurrStep:1;
    bool ownClipping:1;
    bool inObjList:1;
    bool resize:1;
    bool protect:1;
    bool keepRatio:1;

    int subPresStep;
    int cmds;

    KPrObjectAdaptor *dbus;

    static const QString &tagORIG, &attrX, &attrY,
        &tagSIZE, &attrWidth, &attrHeight, &tagSHADOW,
        &attrDistance, &attrDirection, &attrColor, &attrC1,
        &attrC2, &attrType, &attrUnbalanced, &attrXFactor,
        &attrYFactor, &attrStyle, &tagEFFECTS, &attrEffect,
        &attrEffect2, &tagPRESNUM, &tagANGLE,
        &tagDISAPPEAR, &attrDoit, &attrNum, &tagFILLTYPE,
        &tagGRADIENT, &tagPEN, &tagBRUSH, &attrValue;

private:
    // Don't copy or assign it
    KPrObject(const KPrObject &rhs);
    KPrObject &operator=(const KPrObject &rhs);
};

/**
 * Base class for objects with a pen and a brush,
 * and which takes care of painting the shadow in draw()
 * (by calling paint() twice)
 */
class KPrShadowObject : public KPrObject
{
public:
    KPrShadowObject();
    KPrShadowObject( const KoPen &_pen );

    KPrShadowObject &operator=( const KPrShadowObject & );

    virtual void setPen( const KoPen &_pen )
        { pen = _pen; }

    virtual KoPen getPen() const
        { return pen; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info);
    virtual void draw( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                       int pageNum, SelectionMode selectionMode, bool drawContour = false );
    virtual void saveOasisStrokeElement( KoGenStyles& mainStyles, KoGenStyle &styleobjectauto ) const;
    QString saveOasisStrokeStyle( KoGenStyles& mainStyles ) const;

protected:
    /**
     * Helper method for saving draw:points. The svg:viewBox is also saved.
     */
    static bool saveOasisDrawPoints( const KoPointArray &points, KPOasisSaveContext &sc );

    /**
     * Helper method for loading draw:points. The svg:viewBox is taken into account.
     */
    bool loadOasisDrawPoints( KoPointArray &points, const QDomElement &element,
                              KoOasisContext & context, KPrLoadingInfo *info );


    /**
     * @brief Apply the svg:viewBox attribute to the points.
     *
     * This calculates the points of the object. It uses the size of the object and the
     * viewBox attribute to calculate the object points.
     * When no svg:viewBox is  specified it is calculated form the supplied
     * points.
     *
     * @param element the xml element from which the viewBox attribute should be read.
     * @param points which should be updated.
     */
    bool loadOasisApplyViewBox( const QDomElement &element, KoPointArray &points );

    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;

    /**
     * save() only saves if the pen is different from the default pen.
     * The default pen can vary depending on the subclass of KPrShadowObject
     * (e.g. it's a black solidline for lines and rects, but it's NoPen
     * for text objects
     */
    virtual KoPen defaultPen() const;

    /**
     * This method is to be implemented by all KPShadowObjects, to draw themselves.
     * draw() took care of the shadow and of preparing @p painter for rotation.
     * paint() must take care of the gradient itself!
     *
     * @param drawingShadow true if called to draw the shadow of the object. Usually
     * objects will want to draw a simpler version of themselves in that case.
     *
     * This method isn't pure virtual because some objects implement draw() directly.
     */
    virtual void paint( QPainter* /* painter */, KoTextZoomHandler* /* zoomHandler */,
                        int /* pageNum */, bool drawingShadow, bool /* drawContour */ =  false );
    KoPen pen;
};

class KPr2DObject : public KPrShadowObject
{
public:
    KPr2DObject();
    KPr2DObject( const KoPen &_pen, const QBrush &_brush, FillType _fillType,
                const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPr2DObject() { delete gradient; }

    virtual void setFillType( FillType fillType );
    virtual void setBrush( const QBrush &brush )
        { m_brush.setBrush( brush ); }
    virtual void setGColor1( const QColor &gColor1 )
        { if ( gradient ) gradient->setColor1( gColor1 ); m_brush.setGColor1( gColor1 ); m_redrawGradientPix = true; }
    virtual void setGColor2( const QColor &gColor2 )
        { if ( gradient ) gradient->setColor2( gColor2 ); m_brush.setGColor2( gColor2 ); m_redrawGradientPix = true; }
    virtual void setGType( BCType gType )
        { if ( gradient ) gradient->setBackColorType( gType ); m_brush.setGType( gType ); m_redrawGradientPix = true; }
    virtual void setGUnbalanced( bool b )
        { if ( gradient ) gradient->setUnbalanced( b ); m_brush.setGUnbalanced( b ); m_redrawGradientPix = true; }
    virtual void setGXFactor( int xfactor )
        { if ( gradient ) gradient->setXFactor( xfactor ); m_brush.setGXFactor( xfactor ); m_redrawGradientPix = true; }
    virtual void setGYFactor( int yfactor )
        { if ( gradient ) gradient->setYFactor( yfactor ); m_brush.setGYFactor( yfactor ); m_redrawGradientPix = true; }

    virtual FillType getFillType() const
        { return m_brush.getFillType(); }
    virtual QBrush getBrush() const
        { return m_brush.getBrush(); }
    virtual QColor getGColor1() const
        { return m_brush.getGColor1(); }
    virtual QColor getGColor2() const
        { return m_brush.getGColor2(); }
    virtual BCType getGType() const
        { return m_brush.getGType(); }
    virtual bool getGUnbalanced() const
        { return m_brush.getGUnbalanced(); }
    virtual int getGXFactor() const
        { return m_brush.getGXFactor(); }
    virtual int getGYFactor() const
        { return m_brush.getGYFactor(); }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info);
    virtual void draw( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                       int pageNum, SelectionMode selectionMode, bool drawContour = false );

    virtual void flip(bool horizontal );

protected:
    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;

    KPrBrush m_brush;
    KPrGradient *gradient;
    /// holds the painted gradient
    QPixmap m_gradientPix;
    bool m_redrawGradientPix;
};

class KPrStartEndLine
{
public:
    KPrStartEndLine( LineEnd _start, LineEnd _end );
    void save( QDomDocumentFragment &fragment, QDomDocument& doc );
    void load( const QDomElement &element );

    void saveOasisMarkerElement( KoGenStyles& mainStyles,  KoGenStyle &styleobjectauto ) const;
    QString saveOasisMarkerStyle( KoGenStyles &mainStyles, const LineEnd &_element ) const;
    void loadOasisMarkerElement( KoOasisContext & context, const QString & attr, LineEnd &_element );

protected:
    //duplicate from kpobject
    QDomElement createValueElement(const QString &tag, int value, QDomDocument &doc);

    LineEnd lineBegin, lineEnd;
};

#endif
