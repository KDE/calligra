/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998		  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: pie/arc/chord object (header)			  */
/******************************************************************/

#ifndef kppieobject_h
#define kppieobject_h

#include "kpobject.h"

class KPGradient;

/******************************************************************/
/* Class: KPPieObject						  */
/******************************************************************/

class KPPieObject : public KPObject
{
public:
    KPPieObject();
    KPPieObject( QPen _pen, QBrush _brush, FillType _fillType,
		 QColor _gColor1, QColor _gColor2, BCType _gType, PieType _pieType,
		 int _p_angle, int _p_len, LineEnd _lineBegin, LineEnd _lineEnd,
		 bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPPieObject()
    { if ( gradient ) delete gradient; }

    KPPieObject &operator=( const KPPieObject & );

    virtual void setSize( int _width, int _height );
    virtual void resizeBy( int _dx, int _dy );

    virtual void setPen( QPen _pen )
    { pen = _pen; }
    virtual void setBrush( QBrush _brush )
    { brush = _brush; }
    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( QColor _gColor1 )
    { if ( gradient ) gradient->setColor1( _gColor1 ); gColor1 = _gColor1; redrawPix = true; }
    virtual void setGColor2( QColor _gColor2 )
    { if ( gradient ) gradient->setColor2( _gColor2 ); gColor2 = _gColor2; redrawPix = true; }
    virtual void setGType( BCType _gType )
    { if ( gradient ) gradient->setBackColorType( _gType ); gType = _gType; redrawPix = true; }
    virtual void setPieType( PieType _pieType )
    { pieType = _pieType; }
    virtual void setPieAngle( int _p_angle )
    { p_angle = _p_angle; }
    virtual void setPieLength( int _p_len )
    { p_len = _p_len; }
    virtual void setLineBegin( LineEnd _lineBegin )
    { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd )
    { lineEnd = _lineEnd; }
    virtual void setGUnbalanced( bool b )
    { if ( gradient ) gradient->setUnbalanced( b ); unbalanced = b; }
    virtual void setGXFactor( int f )
    { if ( gradient ) gradient->setXFactor( f ); xfactor = f; }
    virtual void setGYFactor( int f )
    { if ( gradient ) gradient->setYFactor( f ); yfactor = f; }

    virtual ObjType getType()
    { return OT_PIE; }
    virtual QPen getPen()
    { return pen; }
    virtual QBrush getBrush()
    { return brush; }
    virtual FillType getFillType()
    { return fillType; }
    virtual QColor getGColor1()
    { return gColor1; }
    virtual QColor getGColor2()
    { return gColor2; }
    virtual BCType getGType()
    { return gType; }
    virtual PieType getPieType()
    { return pieType; }
    virtual int getPieAngle()
    { return p_angle; }
    virtual int getPieLength()
    { return p_len; }
    virtual LineEnd getLineBegin()
    { return lineBegin; }
    virtual LineEnd getLineEnd()
    { return lineEnd; }
    virtual bool getGUnbalanced()
    { return unbalanced; }
    virtual int getGXFactor( )
    { return xfactor; }
    virtual int getGYFactor()
    { return yfactor; }

    virtual void save( ostream& out );
    virtual void load( KOMLParser& parser, vector<KOMLAttrib>& lst );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

protected:
    void paint( QPainter *_painter );

    QPen pen;
    QBrush brush;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    PieType pieType;
    int p_angle, p_len;
    LineEnd lineBegin, lineEnd;
    bool unbalanced;
    int xfactor, yfactor;

    KPGradient *gradient;
    bool drawShadow;
    QPixmap pix;
    bool redrawPix;

};

#endif
