// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kpobject_h
#define kpobject_h

#include <qpen.h>
#include <qbrush.h>
#include <qcursor.h>

#include "global.h"

#include "kpgradient.h"
#include "koPointArray.h"
#include <klocale.h>
#include <koPoint.h>
#include <koSize.h>
#include <koRect.h>
#include "kpobject.h"
#include <koStyleStack.h>
#include <koOasisStyles.h>
#include <koxmlwriter.h>
#include <koGenStyles.h>
#include "kprloadinginfo.h"

class KoSavingContext;
class QPainter;
class DCOPObject;
class QDomDocumentFragment;
class QDomDocument;
class QDomElement;
class KoZoomHandler;
class KPresenterDoc;
class KPTextObject;
class KoTextObject;
class KoOasisContext;

#define RAD_FACTOR 180.0 / M_PI

class KPObject
{
public:
    KPObject();
    virtual ~KPObject();


    virtual DCOPObject* dcopObject();

    virtual void setSelected( bool _selected )
        { selected = _selected; }
    virtual void rotate( float _angle )
        { angle = _angle; }
    virtual void setSize( const KoSize & _size )
        { setSize( _size.width(), _size.height() ); }
    virtual void setSize(double _width, double _height )
        { ext = KoSize( QMAX( 10, _width ), QMAX( 10, _height ) ); } // no object is smaller than 10x10
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
        { objectName = _objectName; }
    virtual QString getObjectName() const
        { return objectName; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context,  KPRLoadingInfo *info);

    virtual bool saveOasis( KoXmlWriter &xmlWriter, KoSavingContext &context, int indexObj ) =0;

    void saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj );
    //return true if we have a animation into object
    bool saveOasisObjectStyleShowAnimation( KoXmlWriter &animation, int objectId );
    bool saveOasisObjectStyleHideAnimation( KoXmlWriter &animation, int objectId );

    virtual void flip(bool horizontal );

    virtual ObjType getType() const
        { return OT_UNDEFINED; }
    virtual QString getTypeString() const
        { return QString(); }

    KoRect getBoundingRect() const;
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
    virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
                       SelectionMode selectionMode, bool drawContour = FALSE );

    virtual bool contains( const KoPoint &_point ) const;
    virtual bool intersects( const KoRect & _rect ) const;
    virtual QCursor getCursor( const KoPoint &_point, ModifyType &_modType, KPresenterDoc *doc ) const;

    KoRect rotateRectObject() const;
    void rotateObject(QPainter *paint,KoZoomHandler *_zoomHandler);
    void rotateObjectWithShadow(QPainter *paint,KoZoomHandler *_zoomHandler);

    virtual void removeFromObjList()
        { inObjList = false; doDelete(); }
    virtual void addToObjList()
        { inObjList = true; }
    virtual void incCmdRef()
        { cmds++; }
    virtual void decCmdRef()
        { cmds--; doDelete(); }

    virtual void setSticky( bool b ) { sticky = b; }
    bool isSticky() const { return sticky; }

    virtual void setProtect( bool b ) { protect = b; }
    bool isProtect() const { return protect; }

    virtual void setKeepRatio( bool b ) { keepRatio = b; }
    bool isKeepRatio() const { return keepRatio; }

    static void setupClipRegion( QPainter *painter, const QRegion &clipRegion );

    void paintSelection( QPainter *_painter,KoZoomHandler *_zoomHandler,
                         SelectionMode selectionMode );

    /**
     * Collect all textobjects.
     * (KPTextObject returns the object it contains,
     * a KPGroupObject returns all the text objects it contains)
     */
    virtual void addTextObjects( QPtrList<KoTextObject> & ) const {}

    virtual KPTextObject *nextTextObject() { return 0L;} // deprecated
    virtual void getAllObjectSelectedList(QPtrList<KPObject> &lst, bool force = false )
        { if (selected || force ) lst.append( this );}
    virtual QPen getPen() const;

    bool haveAnimation() const;

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
    void toGradient(const QDomElement &element, QColor &c1, QColor &c2, BCType &type,
                    bool &unbalanced, int &xfactor, int &yfactor) const;

    QDomElement createPenElement(const QString &tag, const QPen &pen, QDomDocument &doc);
    QPen toPen(const QDomElement &element) const;

    QDomElement createBrushElement(const QString &tag, const QBrush &brush, QDomDocument &doc);
    QBrush toBrush(const QDomElement &element) const;

    QColor retrieveColor(const QDomElement &element, const QString &cattr="color",
                         const QString &rattr="red", const QString &gattr="green",
                         const QString &battr="blue") const;

    void saveOasisObjectProtectStyle( KoGenStyle &styleobjectauto );
    void saveOasisShadowElement( KoGenStyle &styleobjectauto );
    virtual void saveOasisPictureElement( KoGenStyle &styleobjectauto ) {};

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
    QString objectName;
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
    bool sticky:1;
    bool protect:1;
    bool keepRatio:1;

    int subPresStep;
    int cmds;

    DCOPObject *dcop;

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
    KPObject(const KPObject &rhs);
    KPObject &operator=(const KPObject &rhs);
};

/**
 * Base class for objects with a pen and a brush,
 * and which takes care of painting the shadow in @ref draw
 * (by calling @ref paint twice)
 */
class KPShadowObject : public KPObject
{
public:
    KPShadowObject();
    KPShadowObject( const QPen &_pen );
    KPShadowObject( const QPen &_pen, const QBrush &_brush );

    KPShadowObject &operator=( const KPShadowObject & );

    virtual void setPen( const QPen &_pen )
        { pen = _pen; }
    virtual void setBrush( const QBrush &_brush )
        { brush = _brush; }

    virtual QPen getPen() const
        { return pen; }
    virtual QBrush getBrush() const
        { return brush; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info);
    virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
                       SelectionMode selectionMode, bool drawContour = FALSE );
    virtual void saveOasisStrokeElement( KoGenStyles& mainStyles, KoGenStyle &styleobjectauto );
    QString saveOasisStrokeStyle( KoGenStyles& mainStyles );

protected:
    /**
     * @ref save() only saves if the pen is different from the default pen.
     * The default pen can vary depending on the subclass of KPShadowObject
     * (e.g. it's a black solidline for lines and rects, but it's NoPen
     * for text objects
     */
    virtual QPen defaultPen() const;

    /**
     * This method is to be implemented by all KPShadowObjects, to draw themselves.
     * @ref draw took care of the shadow and of preparing @p painter for rotation.
     * @ref paint must take care of the gradient itself!
     *
     * @param drawingShadow true if called to draw the shadow of the object. Usually
     * objects will want to draw a simpler version of themselves in that case.
     *
     * This method isn't pure virtual because some objects implement draw() directly.
     */
    virtual void paint( QPainter* /*painter*/, KoZoomHandler* /*zoomHandler*/,
                        bool /*drawingShadow*/, bool /*drawContour*/ =  FALSE ) {}
    QPen pen;
    QBrush brush;
};

class KP2DObject : public KPShadowObject
{
public:
    KP2DObject();
    KP2DObject( const QPen &_pen, const QBrush &_brush, FillType _fillType,
                const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KP2DObject() { delete gradient; }

    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( const QColor &_gColor1 )
        { if ( gradient ) gradient->setColor1( _gColor1 ); gColor1 = _gColor1; }
    virtual void setGColor2( const QColor &_gColor2 )
        { if ( gradient ) gradient->setColor2( _gColor2 ); gColor2 = _gColor2; }
    virtual void setGType( BCType _gType )
        { if ( gradient ) gradient->setBackColorType( _gType ); gType = _gType; }
    virtual void setGUnbalanced( bool b )
        { if ( gradient ) gradient->setUnbalanced( b ); unbalanced = b; }
    virtual void setGXFactor( int f )
        { if ( gradient ) gradient->setXFactor( f ); xfactor = f; }
    virtual void setGYFactor( int f )
        { if ( gradient ) gradient->setYFactor( f ); yfactor = f; }

    virtual FillType getFillType() const
        { return fillType; }
    virtual QColor getGColor1() const
        { return gColor1; }
    virtual QColor getGColor2() const
        { return gColor2; }
    virtual BCType getGType() const
        { return gType; }
    virtual bool getGUnbalanced() const
        { return unbalanced; }
    virtual int getGXFactor() const
        { return xfactor; }
    virtual int getGYFactor() const
        { return yfactor; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info);

    virtual void flip(bool horizontal );

protected:
    QString saveOasisHatchStyle( KoGenStyles& mainStyles );
    QString saveOasisGradientStyle( KoGenStyles& mainStyles );
    QString saveOasisBackgroundStyle( KoXmlWriter &xmlWriter, KoGenStyles& mainStyles, int indexObj );

    virtual void saveOasisMarginElement( KoGenStyle &styleobjectauto ) { /* nothing just used into kptextobject*/};

    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    bool unbalanced;
    int xfactor, yfactor;

    KPGradient *gradient;
};

class KPStartEndLine
{
public:
    KPStartEndLine( LineEnd _start, LineEnd _end );
    void save( QDomDocumentFragment &fragment, QDomDocument& doc );
    void load( const QDomElement &element );

    void saveOasisMarkerElement( KoGenStyles& mainStyles,  KoGenStyle &styleobjectauto );
    QString saveOasisMarkerStyle( KoGenStyles &mainStyles, LineEnd &_element );
    void loadOasisMarkerElement( KoOasisContext & context, const QString & attr, LineEnd &_element );

protected:
    //duplicate from kpobject
    QDomElement createValueElement(const QString &tag, int value, QDomDocument &doc);

    LineEnd lineBegin, lineEnd;
};

#endif
