/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: rect object (header)                                   */
/******************************************************************/

#ifndef kprectobject_h
#define kprectobject_h

#include "kpobject.h"
#include "kpgradient.h"

class KPGradient;

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

class KPRectObject : public KPObject
{
public:
    KPRectObject();
    KPRectObject( QPen _pen, QBrush _brush, FillType _fillType,
                  QColor _gColor1, QColor _gColor2, BCType _gType, int _xRnd, int _yRnd );
    virtual ~KPRectObject()
    { if ( gradient ) delete gradient; }

    KPRectObject &operator=( const KPRectObject & );
    
    virtual void setSize( int _width, int _height );
    virtual void resizeBy( int _dx, int _dy );

    virtual void setPen( QPen _pen )
    { pen = _pen; }
    virtual void setBrush( QBrush _brush )
    { brush = _brush; }
    virtual void setRnds( int _xRnd, int _yRnd )
    { xRnd = _xRnd; yRnd = _yRnd; }
    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( QColor _gColor1 )
    { if ( gradient ) gradient->setColor1( _gColor1 ); gColor1 = _gColor1; }
    virtual void setGColor2( QColor _gColor2 )
    { if ( gradient ) gradient->setColor2( _gColor2 ); gColor2 = _gColor2; }
    virtual void setGType( BCType _gType )
    { if ( gradient ) gradient->setBackColorType( _gType ); gType = _gType; }

    virtual ObjType getType()
    { return OT_RECT; }
    virtual QPen getPen()
    { return pen; }
    virtual QBrush getBrush()
    { return brush; }
    virtual void getRnds( int &_xRnd, int &_yRnd )
    { _xRnd = xRnd; _yRnd = yRnd; }
    virtual FillType getFillType()
    { return fillType; }
    virtual QColor getGColor1()
    { return gColor1; }
    virtual QColor getGColor2()
    { return gColor2; }
    virtual BCType getGType()
    { return gType; }

    virtual void save( ostream& out );
    virtual void load( KOMLParser& parser, vector<KOMLAttrib>& lst );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

protected:
    void paint( QPainter *_painter );

    QPen pen;
    QBrush brush;
    int xRnd, yRnd;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;

    KPGradient *gradient;
    bool drawShadow;

};

#endif
