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
/* Module: Part Object (header)                                   */
/******************************************************************/

#ifndef kppartobject_h
#define kppartobject_h

#include <koMainWindow.h>
#include <koView.h>
#include <openparts.h>

#include "kpobject.h"

#include <qpixmap.h>

class KPresenterChild;
class KPresenterFrame;
class KPGradient;

/******************************************************************/
/* Class: KPPartObject                                            */
/******************************************************************/

class KPPartObject : public KPObject
{
public:
    KPPartObject( KPresenterChild *_child );
    virtual ~KPPartObject();

    KPPartObject &operator=( const KPPartObject & );

    virtual void save( ostream& out );
    virtual void load( KOMLParser& parser, vector<KOMLAttrib>& lst );

    virtual void setPen( QPen _pen )
    { pen = _pen; }
    virtual void setBrush( QBrush _brush )
    { brush = _brush; }
    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( QColor _gColor1 )
    { if ( gradient ) gradient->setColor1( _gColor1 ); gColor1 = _gColor1; }
    virtual void setGColor2( QColor _gColor2 )
    { if ( gradient ) gradient->setColor2( _gColor2 ); gColor2 = _gColor2; }
    virtual void setGType( BCType _gType )
    { if ( gradient ) gradient->setBackColorType( _gType ); gType = _gType; }
    virtual void setGUnbalanced( bool b )
    { if ( gradient ) gradient->setUnbalanced( b ); unbalanced = b; }
    virtual void setGXFactor( int f )
    { if ( gradient ) gradient->setXFactor( f ); xfactor = f; }
    virtual void setGYFactor( int f )
    { if ( gradient ) gradient->setYFactor( f ); yfactor = f; }

    virtual ObjType getType()
    { return OT_PART; }
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
    virtual bool getGUnbalanced()
    { return unbalanced; }
    virtual int getGXFactor( )
    { return xfactor; }
    virtual int getGYFactor()
    { return yfactor; }

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    virtual void activate( QWidget *_widget, int diffx, int diffy );
    virtual void deactivate();

    virtual void setSize( int _width, int _height );
    virtual void resizeBy( int _dx, int _dy );
    virtual void setOrig( int _x, int _y );
    virtual void moveBy( int _dx, int _dy );

    KPresenterChild *getChild() { return child; }

    void enableDrawing( bool f ) { _enableDrawing = f; }

    void setGetNewPic( bool _new )
    { getNewPic = _new; }

    void setView( KOffice::View_var kv )
    { view = KOffice::View::_narrow( kv ); }

protected:
    void paint( QPainter *_painter );

    QPen pen;
    QBrush brush;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    bool unbalanced;
    int xfactor, yfactor;

    KPGradient *gradient;
    bool _enableDrawing;

    KPresenterFrame *frame;
    KPresenterChild *child;
    OpenParts::Id parentID;
    KOffice::View_var view;

    bool getNewPic;

};

#endif
