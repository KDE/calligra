/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Frame						  */
/******************************************************************/

#include "kword_doc.h"
#include "kword_view.h"
#include "frame.h"
#include "image.h"
#include "parag.h"
#include "defs.h"
#include "kword_utils.h"

#include <koView.h>
#include <koPageLayoutDia.h>
#include <koApplication.h>

#include <limits.h>

#include <qpicture.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qfile.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

/******************************************************************/
/* Class: KWFrame						  */
/******************************************************************/

/*================================================================*/
KWFrame::KWFrame()
    : QRect(), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom()
{
    runAround = RA_NO;
    intersections.setAutoDelete( true );
    selected = false;
    runAroundGap = 1;
    mostRight = false;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame( const QPoint &topleft, const QPoint &bottomright )
    : QRect( topleft, bottomright ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom()
{
    runAround = RA_NO;
    intersections.setAutoDelete( true );
    selected = false;
    mostRight = false;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame( const QPoint &topleft, const QSize &size )
    : QRect( topleft, size ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom()
{
    runAround = RA_NO;
    intersections.setAutoDelete( true );
    selected = false;
    mostRight = false;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame( int left, int top, int width, int height )
    : QRect( left, top, width, height ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom()
{
    runAround = RA_NO;
    intersections.setAutoDelete( true );
    selected = false;
    mostRight = false;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame( int left, int top, int width, int height, RunAround _ra, KWUnit _gap )
    : QRect( left, top, width, height ), runAroundGap( _gap ), intersections(), bleft(), bright(), btop(), bbottom()
{
    runAround = _ra;
    intersections.setAutoDelete( true );
    selected = false;
    mostRight = false;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame( const QRect &_rect )
    : QRect( _rect ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom()
{
    runAround = RA_NO;
    intersections.setAutoDelete( true );
    selected = false;
    mostRight = false;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;
}

/*================================================================*/
void KWFrame::addIntersect( QRect &_r )
{
    intersections.append( new QRect( _r.x(), _r.y(), _r.width(), _r.height() ) );
}

/*================================================================*/
int KWFrame::getLeftIndent( int _y, int _h )
{
    if ( runAround == RA_NO || intersections.isEmpty() ) return 0;

    int _left = 0;
    QRect rect;

    for ( unsigned int i = 0; i < intersections.count(); i++ )
    {
	rect = *intersections.at( i );

	if ( rect.intersects( QRect( left(), _y, width(), _h ) ) )
	{
	    if ( rect.left() == left() )
		_left = max(_left,static_cast<int>(rect.width() + runAroundGap.pt()));
	}
    }

    return _left;
}

/*================================================================*/
int KWFrame::getRightIndent( int _y, int _h )
{
    if ( runAround == RA_NO || intersections.isEmpty() ) return 0;

    int _right = 0;
    QRect rect;

    for ( unsigned int i = 0; i < intersections.count(); i++ )
    {
	rect = *intersections.at( i );

	if ( rect.intersects( QRect( left(), _y, width(), _h ) ) )
	{
	    if ( rect.right() == right() )
		_right = max(_right,static_cast<int>(rect.width() + runAroundGap.pt()));
	}
    }

    return _right;
}

/*================================================================*/
unsigned int KWFrame::getNextFreeYPos( unsigned int _y, unsigned int _h )
{
    QRect rect;
    unsigned int __y = _y;

    for ( unsigned int i = 0; i < intersections.count(); i++ )
    {
	rect = *intersections.at( i );

	if ( rect.intersects( QRect( 0, _y, INT_MAX, _h ) ) )
	    __y = __y == _y ? rect.bottom() : min(static_cast<int>(__y),rect.bottom());
    }

    return __y;
}

/*================================================================*/
QRegion KWFrame::getEmptyRegion()
{
    QRegion region( x(), y(), width(), height() );
    QRect rect;

    for ( unsigned int i = 0; i < intersections.count(); i++ )
    {
	rect = *intersections.at( i );
	region = region.subtract( QRect( rect.x() - 1, rect.y() - 1, rect.width() + 2, rect.height() + 2 ) );
    }

    return QRegion( region );
}

/*================================================================*/
QCursor KWFrame::getMouseCursor( int mx, int my, bool table )
{
    if ( !table )
    {
	if ( mx >= x() && my >= y() && mx <= x() + 6 && my <= y() + 6 )
	    return Qt::sizeFDiagCursor;
	if ( mx >= x() && my >= y() + height() / 2 - 3 && mx <= x() + 6 && my <= y() + height() / 2 + 3 )
	    return Qt::sizeHorCursor;
	if ( mx >= x() && my >= y() + height() - 6 && mx <= x() + 6 && my <= y() + height() )
	    return Qt::sizeBDiagCursor;
	if ( mx >= x() + width() / 2 - 3 && my >= y() && mx <= x() + width() / 2 + 3 && my <= y() + 6 )
	    return Qt::sizeVerCursor;
	if ( mx >= x() + width() / 2 - 3 && my >= y() + height() - 6 && mx <= x() + width() / 2 + 3 && my <= y() + height() )
	    return Qt::sizeVerCursor;
	if ( mx >= x() + width() - 6 && my >= y() && mx <= x() + width() && my <= y() + 6 )
	    return Qt::sizeBDiagCursor;
	if ( mx >= x() + width() - 6 && my >= y() + height() / 2 - 3 && mx <= x() + width() && my <= y() + height() / 2 + 3 )
	    return Qt::sizeHorCursor;
	if ( mx >= x() + width() - 6 && my >= y() + height() - 6 && mx <= x() + width() && my <= y() + height() )
	    return Qt::sizeFDiagCursor;

	if ( selected )
	    return Qt::sizeAllCursor;
    }
    else
    {
	if ( mx >= x() + width() - 6 && my >= y() && mx <= x() + width() && my <= y() + height() )
	    return Qt::sizeHorCursor;
	if ( mx >= x() && my >= y() + height() - 6 && mx <= x() + width() && my <= y() + height() )
	    return Qt::sizeVerCursor;
	return Qt::sizeAllCursor;
    }

    return Qt::arrowCursor;
}

/*================================================================*/
void  KWFrame::saveLeftBrd2( QDomElement &e )
{
    e.setAttribute( "lWidth", (int)brd_left.ptWidth );
    e.setAttribute( "lColor", colorToName( brd_left.color ) );
    e.setAttribute( "lStyle", (int)brd_left.style );
}

/*================================================================*/
void KWFrame::saveRightBrd2( QDomElement &e )
{
    e.setAttribute( "rWidth", (int)brd_right.ptWidth );
    e.setAttribute( "rColor", colorToName( brd_right.color ) );
    e.setAttribute( "rStyle", (int)brd_right.style );
}

/*================================================================*/
void KWFrame::saveTopBrd2( QDomElement &e )
{
    e.setAttribute( "tWidth", (int)brd_top.ptWidth );
    e.setAttribute( "tColor", colorToName( brd_top.color ) );
    e.setAttribute( "tStyle", (int)brd_top.style );
}

/*================================================================*/
void KWFrame::saveBottomBrd2( QDomElement &e )
{
    e.setAttribute( "bWidth", (int)brd_bottom.ptWidth );
    e.setAttribute( "bColor", colorToName( brd_bottom.color ) );
    e.setAttribute( "bStyle", (int)brd_bottom.style );
}

/******************************************************************/
/* Class: KWFrameSet						  */
/******************************************************************/

/*================================================================*/
KWFrameSet::KWFrameSet( KWordDocument *_doc )
    : frames(), removeableHeader( false ), visible( true )
{
    doc = _doc;
    frames.setAutoDelete( true );
    frameInfo = FI_BODY;
    current = 0;
    grpMgr = 0L;
}

/*================================================================*/
void KWFrameSet::addFrame( KWFrame _frame )
{
    KWFrame *frm = new KWFrame( _frame.x(), _frame.y(), _frame.width(), _frame.height(), _frame.getRunAround(),
				_frame.getRunAroundGap() );
    frm->setLeftBorder( _frame.getLeftBorder2() );
    frm->setRightBorder( _frame.getRightBorder2() );
    frm->setTopBorder( _frame.getTopBorder2() );
    frm->setBottomBorder( _frame.getBottomBorder2() );
    frm->setBLeft( _frame.getBLeft() );
    frm->setBRight( _frame.getBRight() );
    frm->setBTop( _frame.getBTop() );
    frm->setBBottom( _frame.getBBottom() );
    frm->setBackgroundColor( QBrush( _frame.getBackgroundColor() ) );

    frames.append( frm );
    if ( frames.count() == 1 ) init();
    update();
}

/*================================================================*/
void KWFrameSet::addFrame( KWFrame *_frame )
{
    if ( frames.findRef( _frame ) != -1 ) return;

    frames.append( _frame );
    if ( frames.count() == 1 ) init();
    update();
}

/*================================================================*/
void KWFrameSet::delFrame( unsigned int _num )
{
    KWFrame *frm = frames.at( _num ), *f;
    bool del = true;
    unsigned int i = 0;
    for ( f = frames.first(); f != 0; f = frames.next(), i++ )
    {
	if ( f == frm && i != _num )
	{
	    del = false;
	    break;
	}
    }

    if ( !del )
	frames.take( _num );
    else
	frames.remove( _num );

    update();
}

/*================================================================*/
void KWFrameSet::delFrame( KWFrame *frm, bool remove )
{
    int _num = frames.findRef( frm );
    if ( _num == -1 )
	return;

    KWFrame *f;

    bool del = true;
    int i = 0;
    for ( f = frames.first(); f != 0; f = frames.next(), i++ )
    {
	if ( f == frm && i != _num )
	{
	    del = false;
	    break;
	}
    }

    if ( !del || !remove )
	frames.take( _num );
    else
	frames.remove( _num );

    update();
}

/*================================================================*/
int KWFrameSet::getFrame( int _x, int _y )
{
    for ( unsigned int i = 0; i < getNumFrames(); i++ )
    {
	if ( getFrame( i )->contains( QPoint( _x, _y ) ) ) return i;
    }
    return -1;
}

/*================================================================*/
KWFrame *KWFrameSet::getFrame( unsigned int _num )
{
    return frames.at( _num );
}

/*================================================================*/
bool KWFrameSet::contains( unsigned int mx, unsigned int my )
{
    for ( unsigned int i = 0; i < frames.count(); i++ )
    {
	if ( frames.at( i )->contains( QPoint( mx, my ) ) ) return true;
    }

    return false;
}

/*================================================================*/
int KWFrameSet::selectFrame( unsigned int mx, unsigned int my, bool simulate )
{
    for ( unsigned int i = 0; i < frames.count(); i++ )
    {
	if ( frames.at( i )->contains( QPoint( mx, my ) ) )
	{
	    int r = 1;
	    if ( frames.at( i )->isSelected() )
		r = 2;
	    if ( !simulate )
		frames.at( i )->setSelected( true );
	    return r;
	}
    }
    return 0;
}

/*================================================================*/
void KWFrameSet::deSelectFrame( unsigned int mx, unsigned int my )
{
    for ( unsigned int i = 0; i < frames.count(); i++ )
    {
	if ( frames.at( i )->contains( QPoint( mx, my ) ) )
	    frames.at( i )->setSelected( false );
    }
}

/*================================================================*/
QCursor KWFrameSet::getMouseCursor( unsigned int mx, unsigned int my )
{
    int frame = getFrame( mx, my );

    if ( frame == -1 )
	return Qt::arrowCursor;

    if ( !getFrame( frame )->isSelected() && !grpMgr ) return Qt::arrowCursor;

    return getFrame( frame )->getMouseCursor( mx, my, grpMgr ? true : false );
}

/*================================================================*/
QDomElement KWFrameSet::save( QDomDocument &doc )
{
    QDomElement frameset = doc.createElement( "FRAMESET" );

    KWFrame *frame;

    for ( unsigned int i = 0; i < frames.count(); i++ ) {
	frame = getFrame( i );
	
	QDomElement frm = doc.createElement( "FRAME" );
	frm.setAttribute( "left", (int)frame->left() );
	frm.setAttribute( "right", (int)frame->right() );
	frm.setAttribute( "top", (int)frame->top() );
	frm.setAttribute( "bottom", (int)frame->bottom() );
	frm.setAttribute( "run-around", (int)frame->getRunAround() );
	frm.setAttribute( "run-around-gap-pt", (int)frame->getRunAroundGap().pt() );
	frm.setAttribute( "run-around-gap-mm", (int)frame->getRunAroundGap().mm() );
	frm.setAttribute( "run-around-gap-inch", (int)frame->getRunAroundGap().inch() );
	frame->saveLeftBrd2( frm );
	frame->saveRightBrd2( frm );
	frame->saveTopBrd2( frm );
	frame->saveBottomBrd2( frm );
	frm.setAttribute( "background-color", colorToName( frame->getBackgroundColor().color() ) );
	frm.setAttribute( "border-left-pt", (int)frame->getBLeft().pt() );
	frm.setAttribute( "border-right-pt", (int)frame->getBRight().pt() );
	frm.setAttribute( "border-top-pt", (int)frame->getBTop().pt() );
	frm.setAttribute( "border-bottom-pt", (int)frame->getBBottom().pt() );
	frm.setAttribute( "border-left-inch", (int)frame->getBLeft().inch() );
	frm.setAttribute( "border-right-inch", (int)frame->getBRight().inch() );
	frm.setAttribute( "border-top-inch", (int)frame->getBTop().inch() );
	frm.setAttribute( "border-bottom-inch", (int)frame->getBBottom().inch() );
	frm.setAttribute( "border-left-mm", (int)frame->getBLeft().mm() );
	frm.setAttribute( "border-right-mm", (int)frame->getBRight().mm() );
	frm.setAttribute( "border-top-mm", (int)frame->getBTop().mm() );
	frm.setAttribute( "border-bottom-mm", (int)frame->getBBottom().mm() );
	frameset.appendChild( frm );
    }

    return frameset;
}

/*================================================================*/
bool KWFrameSet::load( QDomElement& element )
{
    init();

    KWParag *last = 0L;

    QDomElement f = element.firstChild().toElement();
    for( ; !f.isNull(); f = f.nextSibling().toElement() ) {
	 KWFrame rect;
	 KWParagLayout::Border l, r, t, b;
	 float lmm = 0, linch = 0, rmm = 0, rinch = 0, tmm = 0, tinch = 0, bmm = 0, binch = 0, ramm = 0, rainch = -1;
	 unsigned int lpt = 0, rpt = 0, tpt = 0, bpt = 0, rapt = 0;
	 
	 l.color = Qt::white;
	 l.style = KWParagLayout::SOLID;
	 l.ptWidth = 1;
	 r.color = Qt::white;
	 r.style = KWParagLayout::SOLID;
	 r.ptWidth = 1;
	 t.color = Qt::white;
	 t.style = KWParagLayout::SOLID;
	 t.ptWidth = 1;
	 b.color = Qt::white;
	 b.style = KWParagLayout::SOLID;
	 b.ptWidth = 1;
	 QColor c( Qt::white );

	 if ( f.hasAttribute( "left" ) )
	     rect.setLeft( f.attribute( "left" ).toInt() );
	 if ( f.hasAttribute( "right" ) )
	     rect.setRight( f.attribute( "right" ).toInt() );
	 if ( f.hasAttribute( "top" ) )
	     rect.setTop( f.attribute( "top" ).toInt() );
	 if ( f.hasAttribute( "bottom" ) )
	     rect.setBottom( f.attribute( "bottom" ).toInt() );

	 if ( f.hasAttribute( "run-around" ) )
	     rect.setRunAround( (RunAround)f.attribute( "run-around" ).toInt() );
	 if ( f.hasAttribute( "run-around-gap-mm" ) )
	     ramm = f.attribute( "run-around-gap-mm" ).toInt();
	 if ( f.hasAttribute( "run-around-gap-pt" ) )
	     rapt = f.attribute( "run-around-gap-pt" ).toInt();
	 if ( f.hasAttribute( "run-around-gap-inch" ) )
	     rainch = f.attribute( "run-around-gap-inch" ).toInt();

	 if ( f.hasAttribute( "lWidth" ) )
	     l.ptWidth = f.attribute( "lWidth" ).toInt();
	 if ( f.hasAttribute( "lColor" ) )
	     l.color = QColor( f.attribute( "lColor" ) );
	 if ( f.hasAttribute( "lStyle" ) )
	     l.style = (KWParagLayout::BorderStyle) f.attribute( "lStyle" ).toInt();

	 if ( f.hasAttribute( "rWidth" ) )
	     r.ptWidth = f.attribute( "rWidth" ).toInt();
	 if ( f.hasAttribute( "rColor" ) )
	     r.color = QColor( f.attribute( "rColor" ) );
	 if ( f.hasAttribute( "rStyle" ) )
	     r.style = (KWParagLayout::BorderStyle) f.attribute( "rStyle" ).toInt();
	    
	 if ( f.hasAttribute( "bWidth" ) )
	     b.ptWidth = f.attribute( "bWidth" ).toInt();
	 if ( f.hasAttribute( "bColor" ) )
	     b.color = QColor( f.attribute( "bColor" ) );
	 if ( f.hasAttribute( "bStyle" ) )
	     b.style = (KWParagLayout::BorderStyle) f.attribute( "bStyle" ).toInt();

	 if ( f.hasAttribute( "tWidth" ) )
	     t.ptWidth = f.attribute( "tWidth" ).toInt();
	 if ( f.hasAttribute( "tColor" ) )
	     t.color = QColor( f.attribute( "tColor" ) );
	 if ( f.hasAttribute( "tStyle" ) )
	     t.style = (KWParagLayout::BorderStyle) f.attribute( "tStyle" ).toInt();
	    
	 if ( f.hasAttribute( "background-color" ) )	    
	     c = f.attribute( "background-color" );

	 if ( f.hasAttribute( "border-left-mm" ) )
	     lmm = f.attribute( "border-left-mm" ).toInt();
	 if ( f.hasAttribute( "border-left-pt" ) )
	     lpt = f.attribute( "border-left-pt" ).toInt();
	 if ( f.hasAttribute( "border-left-inch" ) )
	     linch = f.attribute( "border-left-inch" ).toInt();

	 if ( f.hasAttribute( "border-right-mm" ) )
	     rmm = f.attribute( "border-right-mm" ).toInt();
	 if ( f.hasAttribute( "border-right-pt" ) )
	     rpt = f.attribute( "border-right-pt" ).toInt();
	 if ( f.hasAttribute( "border-right-inch" ) )
	     rinch = f.attribute( "border-right-inch" ).toInt();

	 if ( f.hasAttribute( "border-top-mm" ) )
	     tmm = f.attribute( "border-top-mm" ).toInt();
	 if ( f.hasAttribute( "border-top-pt" ) )
	     tpt = f.attribute( "border-top-pt" ).toInt();
	 if ( f.hasAttribute( "border-top-inch" ) )
	     tinch = f.attribute( "border-top-inch" ).toInt();

	 if ( f.hasAttribute( "border-bottom-mm" ) )
	     bmm = f.attribute( "border-bottom-mm" ).toInt();
	 if ( f.hasAttribute( "border-bottom-pt" ) )
	     bpt = f.attribute( "border-bottom-pt" ).toInt();
	 if ( f.hasAttribute( "border-bottom-inch" ) )
	     binch = f.attribute( "border-bottom-inch" ).toInt();
    }
    KWFrame *_frame = new KWFrame( rect.x(), rect.y(), rect.width(), rect.height(), rect.getRunAround(),
				    rainch == -1 ? rect.getRunAroundGap() : KWUnit( rapt, ramm, rainch ) );
    _frame->setLeftBorder( l );
    _frame->setRightBorder( r );
    _frame->setTopBorder( t );
    _frame->setBottomBorder( b );
    _frame->setBackgroundColor( QBrush( c ) );
    _frame->setBLeft( KWUnit( lpt, lmm, linch ) );
    _frame->setBRight( KWUnit( rpt, rmm, rinch ) );
    _frame->setBTop( KWUnit( tpt, tmm, tinch ) );
    _frame->setBBottom( KWUnit( bpt, bmm, binch ) );
    frames.append( _frame );
     
    updateCounters();
     
    return true;
}

/*================================================================*/
int KWFrameSet::getNext( QRect _rect )
{
    for ( unsigned int i = 0; i < frames.count(); i++ )
    {
	if ( frames.at( i )->intersects( _rect ) )
	    return i;
    }

    return -1;
}

/*================================================================*/
bool KWFrameSet::hasSelectedFrame()
{
    for ( unsigned int i = 0; i < frames.count(); i++ )
    {
	if ( frames.at( i )->isSelected() )
	    return true;
    }

    return false;
}

/******************************************************************/
/* Class: KWTextFrameSet					  */
/******************************************************************/

/*================================================================*/
void KWTextFrameSet::init()
{
    parags = 0L;

    autoCreateNewFrame = true;

    parags = new KWParag( this, doc, 0L, 0L, doc->getDefaultParagLayout() );
    KWFormat *format = new KWFormat( doc );
    format->setDefaults( doc );

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::assign( KWTextFrameSet *fs )
{
    if ( parags ) delete parags;

    //parags = fs->getFirstParag();

    parags = new KWParag( *fs->getFirstParag() );
    parags->setFrameSet( this );
    parags->setPrev( 0L );
    parags->setDocument( doc );

    KWParag *p1 = fs->getFirstParag()->getNext(), *p2 = parags, *tmp;
    while ( p1 )
    {
	tmp = p2;
	p2 = new KWParag( *p1 );
	//*p2 = *p1;
	tmp->setNext( p2 );
	p2->setPrev( tmp );
	p2->setFrameSet( this );
	p2->setDocument( doc );
	tmp = p2;

	p1 = p1->getNext();
    }

    p2->setNext( 0L );

//   QPainter p;
//   QPicture pic;
//   p.begin( &pic );

//   KWFormatContext fc( doc, doc->getFrameSetNum( this ) + 1 );
//   fc.init( parags, p, true, true );

//   bool bend = false;
//   while ( !bend )
//     bend = !fc.makeNextLineLayout( p );

//   p.end();

    getFrame( 0 )->setBackgroundColor( fs->getFrame( 0 )->getBackgroundColor() );
    getFrame( 0 )->setLeftBorder( fs->getFrame( 0 )->getLeftBorder2() );
    getFrame( 0 )->setRightBorder( fs->getFrame( 0 )->getRightBorder2() );
    getFrame( 0 )->setTopBorder( fs->getFrame( 0 )->getTopBorder2() );
    getFrame( 0 )->setBottomBorder( fs->getFrame( 0 )->getBottomBorder2() );
    getFrame( 0 )->setBLeft( fs->getFrame( 0 )->getBLeft() );
    getFrame( 0 )->setBRight( fs->getFrame( 0 )->getBRight() );
    getFrame( 0 )->setBTop( fs->getFrame( 0 )->getBTop() );
    getFrame( 0 )->setBBottom( fs->getFrame( 0 )->getBBottom() );
}

/*================================================================*/
KWTextFrameSet::~KWTextFrameSet()
{
    KWParag *p = getLastParag();

    while ( p != parags )
    {
	p = p->getPrev();
	delete p->getNext();
	p->setNext( 0L );
    }

    delete parags;
    parags = 0L;
}

/*================================================================*/
void KWTextFrameSet::update()
{
    typedef QList<KWFrame> FrameList;
    QList<FrameList> frameList;
    frameList.setAutoDelete( true );

    QRect pageRect;
    for ( unsigned int i = 0; i < static_cast<unsigned int>( doc->getPages() + 1 ); i++ )
    {
	pageRect = QRect( 0, i * doc->getPTPaperHeight(), doc->getPTPaperWidth(), doc->getPTPaperHeight() );

	FrameList *l = new FrameList();
	l->setAutoDelete( false );
	for ( unsigned int j = 0; j < frames.count(); j++ )
	{
	    if ( frames.at( j )->intersects( pageRect ) )
	    {
		frames.at( j )->setPageNum( i );
		l->append( frames.at( j ) );
	    }
	}

	if ( !l->isEmpty() )
	{
	    FrameList *ll = new FrameList();
	    ll->setAutoDelete( false );
	    ll->append( l->first() );
	    unsigned int k = 0, m = 0;
	    for ( k = 1; k < l->count(); k++ )
	    {
		bool inserted = false;
		for ( m = 0; m < ll->count(); m++ )
		{
		    if ( l->at( k )->y() < ll->at( m )->y() )
		    {
			inserted = true;
			ll->insert( m, l->at( k ) );
			break;
		    }
		}
		if ( !inserted ) ll->append( l->at( k ) );
	    }
	    FrameList *l2 = new FrameList();
	    l2->setAutoDelete( false );
	    l2->append( ll->first() );
	    for ( k = 1; k < ll->count(); k++ )
	    {
		bool inserted = false;
		for ( m = 0; m < l2->count(); m++ )
		{
		    if ( ll->at( k )->x() < l2->at( m )->x() )
		    {
			inserted = true;
			l2->insert( m, ll->at( k ) );
			break;
		    }
		}
		if ( !inserted ) l2->append( ll->at( k ) );
	    }

	    delete ll;
	    delete l;
	    l = l2;
	}

	frameList.append( l );
    }

    frames.setAutoDelete( false );
    frames.clear();

    int rm = 0;
    for ( unsigned int n = 0; n < frameList.count(); n++ )
    {
	for ( unsigned int o = 0; o < frameList.at( n )->count(); o++ )
	{
	    frames.append( frameList.at( n )->at( o ) );
	    frames.at( frames.count() - 1 )->setMostRight( false );
	    if ( frames.count() > 1 )
	    {
		if ( frames.at( frames.count() - 2 )->right() > frames.at( frames.count() - 1 )->right() )
		{
		    frames.at( frames.count() - 2 )->setMostRight( true );
		    rm++;
		}
	    }
	}
    }

    frames.setAutoDelete( true );
}

/*================================================================*/
void KWTextFrameSet::setFirstParag( KWParag *_parag )
{
    parags = _parag;
}

/*================================================================*/
KWParag* KWTextFrameSet::getFirstParag()
{
    return parags;
}

/*================================================================*/
bool KWTextFrameSet::isPTYInFrame( unsigned int _frame, unsigned int _ypos )
{
    KWFrame *frame = getFrame( _frame );
    return ( static_cast<int>( _ypos ) >= static_cast<int>( frame->top() + frame->getBTop().pt() ) &&
	     static_cast<int>( _ypos ) <= static_cast<int>( frame->bottom() - frame->getBBottom().pt() ) );
}

/*================================================================*/
void KWTextFrameSet::deleteParag( KWParag *_parag )
{
    if ( _parag->getInfo() == KWParag::PI_FOOTNOTE )
	return;

    KWParag *p, *p2;

    if ( !getFirstParag()->getPrev() && !getFirstParag()->getNext() ) return;

    if ( !_parag->getPrev() )
    {
	p = _parag->getNext();
	p->setPrev( 0L );
	setFirstParag( p );
	delete _parag;
    }
    else
    {
	p = _parag->getNext();
	p2 = _parag->getPrev();
	if ( p ) p->setPrev( p2 );
	p2->setNext( p );
	delete _parag;
    }

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::joinParag( KWParag *_parag1, KWParag *_parag2 )
{
    if ( !_parag1 || !_parag2 ) return;

    if ( _parag2->getNext() ) _parag2->getNext()->setPrev( _parag1 );
    _parag1->setNext( _parag2->getNext() );

    _parag1->appendText( _parag2->getText(), _parag2->getTextLen() );

    delete _parag2;

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::insertParag( KWParag *_parag, InsertPos _pos )
{
    KWParag *_new = 0L, *_prev = 0L, *_next = 0L;

    if ( _parag )
    {
	_prev = _parag->getPrev();
	_next = _parag->getNext();
    }

    switch ( _pos )
    {
    case I_AFTER:
    {
	_new = new KWParag( this, doc, _parag, _next, doc->findParagLayout( _parag->getParagLayout()->getFollowingParagLayout() ) );
	if ( _new->getParagLayout()->getName() == _parag->getParagLayout()->getName() )
	    _new->setParagLayout( _parag->getParagLayout() );
	if ( _next ) _next->setPrev( _new );
    } break;
    case I_BEFORE:
    {
	_new = new KWParag( this, doc, _prev, _parag, _parag->getParagLayout() );
	if ( _parag ) _parag->setPrev( _new );
	if ( !_prev ) setFirstParag( _new );
    } break;
    }

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::splitParag( KWParag *_parag, unsigned int _pos )
{
    KWParag *_new = 0L, *_next = 0L;

    if ( _parag ) _next = _parag->getNext();

    unsigned int len = _parag->getTextLen() - _pos;
    KWChar* _string = _parag->getKWString()->split( _pos );
    _new = new KWParag( this, doc, _parag, _next, _parag->getParagLayout() );
    if ( _next ) _next->setPrev( _new );

    _new->appendText( _string, len );

    updateCounters();
}

/*================================================================*/
QDomElement KWTextFrameSet::save( QDomDocument &doc )
{
    QDomElement frameset = KWFrameSet::save( doc );

    frameset.setAttribute( "frame-type", (int)getFrameType() );
    frameset.setAttribute( "auto-create-new-frame", autoCreateNewFrame );
    frameset.setAttribute( "frame-info", (int)frameInfo );

    if ( grpMgr ) {
	unsigned int _row = 0, _col = 0;
	grpMgr->getFrameSet( this, _row, _col );
	KWGroupManager::Cell *cell = grpMgr->getCell( _row, _col );
	frameset.setAttribute( "row", (int)_row );
	frameset.setAttribute( "col", (int)_col );
	frameset.setAttribute( "rows", (int)cell->rows );
	frameset.setAttribute( "cols", (int)cell->cols );
    }
    frameset.setAttribute( "removeable", (int)removeableHeader );
    frameset.setAttribute( "visible", (int)visible );
    frameset.setAttribute( "name", name );

    KWParag *parag = getFirstParag();
    while ( parag ) {
	QDomElement p = parag->save( doc );
	if ( p.isNull() )
	    return p;
	frameset.appendChild( p );
    }

    return frameset;
}

/*================================================================*/
bool KWTextFrameSet::load( QDomElement& element )
{
    
// #### todo
// void KWTextFrameSet::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
// {
//     init();

//     string tag;
//     string name;

//     KWParag *last = 0L;

//     while ( parser.open( 0L, tag ) )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );

// 	// paragraph
// 	if ( name == "PARAGRAPH" )
// 	{
// 	    KOMLParser::parseTag( tag.c_str(), name, lst );
// 	    vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	    for( ; it != lst.end(); it++ )
// 	    {
// 	    }

// 	    if ( !last )
// 	    {
// 		delete parags;
// 		parags = new KWParag( this, doc, 0L, 0L, doc->getDefaultParagLayout() );
// 		if ( doc->getNumFrameSets() == 0 )
// 		{
// 		    parags->insertText( 0, " " );
// 		    KWFormat *format = new KWFormat( doc );
// 		    format->setDefaults( doc );
// 		    parags->setFormat( 0, 1, *format );
// 		}
// 		parags->load( parser, lst );
// 		last = parags;
// 	    }
// 	    else
// 	    {
// 		last = new KWParag( this, doc, last, 0L, doc->getDefaultParagLayout() );
// 		last->load( parser, lst );
// 	    }
// 	}

// 	else if ( name == "FRAME" )
// 	{
// 	    KWFrame rect;
// 	    KWParagLayout::Border l, r, t, b;
// 	    float lmm = 0, linch = 0, rmm = 0, rinch = 0, tmm = 0, tinch = 0, bmm = 0, binch = 0, ramm = 0, rainch = -1;
// 	    unsigned int lpt = 0, rpt = 0, tpt = 0, bpt = 0, rapt = 0;

// 	    l.color = Qt::white;
// 	    l.style = KWParagLayout::SOLID;
// 	    l.ptWidth = 1;
// 	    r.color = Qt::white;
// 	    r.style = KWParagLayout::SOLID;
// 	    r.ptWidth = 1;
// 	    t.color = Qt::white;
// 	    t.style = KWParagLayout::SOLID;
// 	    t.ptWidth = 1;
// 	    b.color = Qt::white;
// 	    b.style = KWParagLayout::SOLID;
// 	    b.ptWidth = 1;
// 	    QColor c( Qt::white );

// 	    KOMLParser::parseTag( tag.c_str(), name, lst );
// 	    vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	    for( ; it != lst.end(); it++ )
// 	    {
// 		if ( ( *it ).m_strName == "left" )
// 		    rect.setLeft( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "top" )
// 		    rect.setTop( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "right" )
// 		    rect.setRight( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bottom" )
// 		    rect.setBottom( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "runaround" )
// 		    rect.setRunAround( static_cast<RunAround>( atoi( ( *it ).m_strValue.c_str() ) ) );
// 		else if ( ( *it ).m_strName == "runaroundGap" )
// 		    rect.setRunAroundGap( KWUnit( atof( ( *it ).m_strValue.c_str() ) ) );
// 		else if ( ( *it ).m_strName == "runaGapPT" )
// 		    rapt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "runaGapMM" )
// 		    ramm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "runaGapINCH" )
// 		    rainch = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "lWidth" )
// 		    l.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "rWidth" )
// 		    r.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "tWidth" )
// 		    t.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bWidth" )
// 		    b.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "lRed" )
// 		    l.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), l.color.green(), l.color.blue() );
// 		else if ( ( *it ).m_strName == "rRed" )
// 		    r.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), r.color.green(), r.color.blue() );
// 		else if ( ( *it ).m_strName == "tRed" )
// 		    t.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), t.color.green(), t.color.blue() );
// 		else if ( ( *it ).m_strName == "bRed" )
// 		    b.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), b.color.green(), b.color.blue() );
// 		else if ( ( *it ).m_strName == "lGreen" )
// 		    l.color.setRgb( l.color.red(), atoi( ( *it ).m_strValue.c_str() ), l.color.blue() );
// 		else if ( ( *it ).m_strName == "rGreen" )
// 		    r.color.setRgb( r.color.red(), atoi( ( *it ).m_strValue.c_str() ), r.color.blue() );
// 		else if ( ( *it ).m_strName == "tGreen" )
// 		    t.color.setRgb( t.color.red(), atoi( ( *it ).m_strValue.c_str() ), t.color.blue() );
// 		else if ( ( *it ).m_strName == "bGreen" )
// 		    b.color.setRgb( b.color.red(), atoi( ( *it ).m_strValue.c_str() ), b.color.blue() );
// 		else if ( ( *it ).m_strName == "lBlue" )
// 		    l.color.setRgb( l.color.red(), l.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "rBlue" )
// 		    r.color.setRgb( r.color.red(), r.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "tBlue" )
// 		    t.color.setRgb( t.color.red(), t.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bBlue" )
// 		    b.color.setRgb( b.color.red(), b.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "lStyle" )
// 		    l.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "rStyle" )
// 		    r.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "tStyle" )
// 		    t.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bStyle" )
// 		    b.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bkRed" )
// 		    c.setRgb( atoi( ( *it ).m_strValue.c_str() ), c.green(), c.blue() );
// 		else if ( ( *it ).m_strName == "bkGreen" )
// 		    c.setRgb( c.red(), atoi( ( *it ).m_strValue.c_str() ), c.blue() );
// 		else if ( ( *it ).m_strName == "bkBlue" )
// 		    c.setRgb( c.red(), c.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bleftpt" )
// 		    lpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "brightpt" )
// 		    rpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "btoppt" )
// 		    tpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bbottompt" )
// 		    bpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bleftmm" )
// 		    lmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "brightmm" )
// 		    rmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "btopmm" )
// 		    tmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bbottommm" )
// 		    bmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bleftinch" )
// 		    linch = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "brightinch" )
// 		    rinch = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "btopinch" )
// 		    tinch = atof( ( *it ).m_strValue.c_str() );
// 		else if ((*it).m_strName == "bbottominch")
// 		    binch = atof( ( *it ).m_strValue.c_str() );
// 	    }
// 	    KWFrame *_frame = new KWFrame( rect.x(), rect.y(), rect.width(), rect.height(), rect.getRunAround(),
// 					   rainch == -1 ? rect.getRunAroundGap() : KWUnit( rapt, ramm, rainch ) );
// 	    _frame->setLeftBorder( l );
// 	    _frame->setRightBorder( r );
// 	    _frame->setTopBorder( t );
// 	    _frame->setBottomBorder( b );
// 	    _frame->setBackgroundColor( QBrush( c ) );
// 	    _frame->setBLeft( KWUnit( lpt, lmm, linch ) );
// 	    _frame->setBRight( KWUnit( rpt, rmm, rinch ) );
// 	    _frame->setBTop( KWUnit( tpt, tmm, tinch ) );
// 	    _frame->setBBottom( KWUnit( bpt, bmm, binch ) );
// 	    frames.append( _frame );
// 	}

// 	else
// 	    cerr << "Unknown tag '" << tag << "' in FRAMESET" << endl;

// 	if ( !parser.close( tag ) )
// 	{
// 	    cerr << "ERR: Closing Child" << endl;
// 	    return;
// 	}
//     }

//     updateCounters();
// }

/*================================================================*/
void KWTextFrameSet::updateCounters()
{
    KWParag *p = getFirstParag();

    int counterData[ 16 ], listData[ 16 ];
    unsigned int i = 0;
    for ( i = 0; i < 16; i++ )
    {
	counterData[ i ] = -2;
	listData[ i ] = -2;
    }
    KWParagLayout::CounterType ct = KWParagLayout::CT_NONE;

    while ( p )
    {
	if ( p->getParagLayout()->getCounterType() != KWParagLayout::CT_NONE )
	{
	    if ( p->getParagLayout()->getNumberingType() == KWParagLayout::NT_CHAPTER )
	    {
		counterData[ p->getParagLayout()->getCounterDepth() ]++;
		for ( i = 0; i < 16; i++ )
		{
		    if ( counterData[ i ] < 0 )
		    {
			switch ( p->getParagLayout()->getCounterType() )
			{
			case KWParagLayout::CT_NUM: case KWParagLayout::CT_ROM_NUM_L: case KWParagLayout::CT_ROM_NUM_U:
			    counterData[ i ] = atoi( p->getParagLayout()->getStartCounter() );
			    break;
			case KWParagLayout::CT_ALPHAB_L:
			    counterData[ i ] = QChar( p->getParagLayout()->getStartCounter()[ 0 ] );
			    break;
			case KWParagLayout::CT_ALPHAB_U:
			    counterData[ i ] = QChar( p->getParagLayout()->getStartCounter()[ 0 ] );
			    break;
			default: break;
			}
		    }
		    p->getCounterData()[ i ] = counterData[ i ];
		}
		p->makeCounterText();
		for ( i = p->getParagLayout()->getCounterDepth() + 1; i < 16; i++ )
		    counterData[ i ] = -2;
		if ( listData[ 0 ] != -2 )
		{
		    for ( i = 0; i < 16; i++ )
			listData[ i ] = -2;
		}
	    }
	    else
	    {
		if ( ct != p->getParagLayout()->getCounterType() )
		{
		    for ( i = 0; i < 16; i++ )
			listData[ i ] = -2;
		}
		ct = p->getParagLayout()->getCounterType();
		if ( p->getParagLayout()->getCounterType() != KWParagLayout::CT_BULLET )
		    listData[ p->getParagLayout()->getCounterDepth() ]++;
		else if ( listData[ 0 ] != -2 )
		{
		    for ( i = 0; i < 16; i++ )
			listData[ i ] = -2;
		}
		for ( i = 0; i < 16; i++ )
		{
		    if ( listData[ i ] < 0 )
		    {
			switch ( p->getParagLayout()->getCounterType() )
			{
			case KWParagLayout::CT_NUM: case KWParagLayout::CT_ROM_NUM_L: case KWParagLayout::CT_ROM_NUM_U:
			    listData[ i ] = atoi( p->getParagLayout()->getStartCounter() );
			    break;
			case KWParagLayout::CT_ALPHAB_L:
			    listData[ i ] = QChar( p->getParagLayout()->getStartCounter()[ 0 ] );
			    break;
			case KWParagLayout::CT_ALPHAB_U:
			    listData[ i ] = QChar( p->getParagLayout()->getStartCounter()[ 0 ] );
			    break;
			default: break;
			}
		    }
		    p->getCounterData()[ i ] = listData[ i ];
		}
		p->makeCounterText();
		for ( i = p->getParagLayout()->getCounterDepth() + 1; i < 16; i++ )
		    listData[ i ] = -2;
	    }
	}
	else if ( listData[ 0 ] != -2 )
	{
	    for ( i = 0; i < 16; i++ )
		listData[ i ] = -2;
	}
	p = p->getNext();
    }
}

/*================================================================*/
void KWTextFrameSet::updateAllStyles()
{
    KWParag *p = getFirstParag();

    while ( p )
    {
	if ( doc->isStyleChanged( p->getParagLayout()->getName() ) )
	    p->applyStyle( p->getParagLayout()->getName() );
	p = p->getNext();
    }

    updateCounters();
}

/*================================================================*/
KWParag *KWTextFrameSet::getLastParag()
{
    KWParag *p = getFirstParag();
    KWParag *last = p;

    while ( p )
    {
	last = p;
	p = p->getNext();
    }

    return last;
}

/******************************************************************/
/* Class: KWPictureFrameSet					  */
/******************************************************************/

/*================================================================*/
void KWPictureFrameSet::setFileName( QString _filename )
{
    int dashdash = _filename.findRev( "--" );
    if ( dashdash != -1 )
	_filename == _filename.left( dashdash );

    if ( image )
    {
	image->decRef();
	image = 0L;
    }

    filename = _filename;

    bool del = false;
    KWImage *_image = doc->getImageCollection()->getImage( filename );
    if ( !_image )
    {
	del = true;
	_image = new KWImage( doc, filename );
    }

    QString key;
    image = doc->getImageCollection()->getImage( *_image, key );

    if ( del )
	delete _image;
}

/*================================================================*/
void KWPictureFrameSet::setFileName( QString _filename, QSize _imgSize )
{
    int dashdash = _filename.findRev( "--" );
    if ( dashdash != -1 )
	_filename == _filename.left( dashdash );

    if ( image )
    {
	image->decRef();
	image = 0L;
    }

    filename = _filename;

    bool del = false;
    KWImage *_image = doc->getImageCollection()->getImage( filename );
    if ( !_image )
    {
	del = true;
	_image = new KWImage( doc, filename );
    }

    QString key;
    image = doc->getImageCollection()->getImage( *_image, key, _imgSize );

    if ( del )
	delete _image;
}

/*================================================================*/
void KWPictureFrameSet::setSize( QSize _imgSize )
{
    if ( image && _imgSize == image->size() ) return;

    QString key;
    image = doc->getImageCollection()->getImage( *image, key, _imgSize );
    if ( !image )
	setFileName( filename, _imgSize );
}

/*================================================================*/
QDomElement KWPictureFrameSet::save( QDomDocument &doc )
{
    QDomElement frameset = KWFrameSet::save( doc );
    frameset.setAttribute( "frameType", (int)getFrameType() );
    frameset.setAttribute( "frameInfo", (int)frameInfo );

    QDomElement img = image->save( doc );
    if ( img.isNull() )
	return img;
    frameset.appendChild( img );

    return frameset;
}

/*================================================================*/
// #### todo
// void KWPictureFrameSet::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
// {
//     string tag;
//     string name;

//     while ( parser.open( 0L, tag ) )
//     {
// 	KOMLParser::parseTag( tag.c_str(), name, lst );

// 	if ( name == "IMAGE" )
// 	{
// 	    KOMLParser::parseTag( tag.c_str(), name, lst );
// 	    vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	    for( ; it != lst.end(); it++ )
// 	    {
// 	    }

// 	    KWImage *_image = new KWImage();
// 	    _image->load( parser, lst, doc );
// 	    //setFileName( _image->getFilename() );
// 	    doc->addImageRequest( _image->getFilename(), this );
// 	    delete _image;
// 	}

// 	else if ( name == "FRAME" )
// 	{
// 	    KWFrame rect;
// 	    KWParagLayout::Border l, r, t, b;
// 	    float lmm = 0, linch = 0, rmm = 0, rinch = 0, tmm = 0, tinch = 0, bmm = 0, binch = 0, ramm = 0, rainch = -1;
// 	    unsigned int lpt = 0, rpt = 0, tpt = 0, bpt = 0, rapt = 0;

// 	    l.color = Qt::white;
// 	    l.style = KWParagLayout::SOLID;
// 	    l.ptWidth = 1;
// 	    r.color = Qt::white;
// 	    r.style = KWParagLayout::SOLID;
// 	    r.ptWidth = 1;
// 	    t.color = Qt::white;
// 	    t.style = KWParagLayout::SOLID;
// 	    t.ptWidth = 1;
// 	    b.color = Qt::white;
// 	    b.style = KWParagLayout::SOLID;
// 	    b.ptWidth = 1;
// 	    QColor c( Qt::white );

// 	    KOMLParser::parseTag( tag.c_str(), name, lst );
// 	    vector<KOMLAttrib>::const_iterator it = lst.begin();
// 	    for( ; it != lst.end(); it++ )
// 	    {
// 		if ( ( *it ).m_strName == "left" )
// 		    rect.setLeft( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "top" )
// 		    rect.setTop( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "right" )
// 		    rect.setRight( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bottom" )
// 		    rect.setBottom( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "runaround" )
// 		    rect.setRunAround( static_cast<RunAround>( atoi( ( *it ).m_strValue.c_str() ) ) );
// 		else if ( ( *it ).m_strName == "runaroundGap" )
// 		    rect.setRunAroundGap( KWUnit( atof( ( *it ).m_strValue.c_str() ) ) );
// 		else if ( ( *it ).m_strName == "runaGapPT" )
// 		    rapt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "runaGapMM" )
// 		    ramm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "runaGapINCH" )
// 		    rainch = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "lWidth" )
// 		    l.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "rWidth" )
// 		    r.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "tWidth" )
// 		    t.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bWidth" )
// 		    b.ptWidth = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "lRed" )
// 		    l.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), l.color.green(), l.color.blue() );
// 		else if ( ( *it ).m_strName == "rRed" )
// 		    r.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), r.color.green(), r.color.blue() );
// 		else if ( ( *it ).m_strName == "tRed" )
// 		    t.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), t.color.green(), t.color.blue() );
// 		else if ( ( *it ).m_strName == "bRed" )
// 		    b.color.setRgb( atoi( ( *it ).m_strValue.c_str() ), b.color.green(), b.color.blue() );
// 		else if ( ( *it ).m_strName == "lGreen" )
// 		    l.color.setRgb( l.color.red(), atoi( ( *it ).m_strValue.c_str() ), l.color.blue() );
// 		else if ( ( *it ).m_strName == "rGreen" )
// 		    r.color.setRgb( r.color.red(), atoi( ( *it ).m_strValue.c_str() ), r.color.blue() );
// 		else if ( ( *it ).m_strName == "tGreen" )
// 		    t.color.setRgb( t.color.red(), atoi( ( *it ).m_strValue.c_str() ), t.color.blue() );
// 		else if ( ( *it ).m_strName == "bGreen" )
// 		    b.color.setRgb( b.color.red(), atoi( ( *it ).m_strValue.c_str() ), b.color.blue() );
// 		else if ( ( *it ).m_strName == "lBlue" )
// 		    l.color.setRgb( l.color.red(), l.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "rBlue" )
// 		    r.color.setRgb( r.color.red(), r.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "tBlue" )
// 		    t.color.setRgb( t.color.red(), t.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bBlue" )
// 		    b.color.setRgb( b.color.red(), b.color.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "lStyle" )
// 		    l.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "rStyle" )
// 		    r.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "tStyle" )
// 		    t.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bStyle" )
// 		    b.style = static_cast<KWParagLayout::BorderStyle>( atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bkRed" )
// 		    c.setRgb( atoi( ( *it ).m_strValue.c_str() ), c.green(), c.blue() );
// 		else if ( ( *it ).m_strName == "bkGreen" )
// 		    c.setRgb( c.red(), atoi( ( *it ).m_strValue.c_str() ), c.blue() );
// 		else if ( ( *it ).m_strName == "bkBlue" )
// 		    c.setRgb( c.red(), c.green(), atoi( ( *it ).m_strValue.c_str() ) );
// 		else if ( ( *it ).m_strName == "bleftpt" )
// 		    lpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "brightpt" )
// 		    rpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "btoppt" )
// 		    tpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bbottompt" )
// 		    bpt = atoi( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bleftmm" )
// 		    lmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "brightmm" )
// 		    rmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "btopmm" )
// 		    tmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bbottommm" )
// 		    bmm = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "bleftinch" )
// 		    linch = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "brightinch" )
// 		    rinch = atof( ( *it ).m_strValue.c_str() );
// 		else if ( ( *it ).m_strName == "btopinch" )
// 		    tinch = atof( ( *it ).m_strValue.c_str() );
// 		else if ((*it).m_strName == "bbottominch")
// 		    binch = atof( ( *it ).m_strValue.c_str() );
// 	    }
// 	    KWFrame *_frame = new KWFrame( rect.x(), rect.y(), rect.width(), rect.height(), rect.getRunAround(),
// 					   rainch == -1 ? rect.getRunAroundGap() : KWUnit( rapt, ramm, rainch ) );
// 	    _frame->setLeftBorder( l );
// 	    _frame->setRightBorder( r );
// 	    _frame->setTopBorder( t );
// 	    _frame->setBottomBorder( b );
// 	    _frame->setBackgroundColor( QBrush( c ) );
// 	    _frame->setBLeft( KWUnit( lpt, lmm, linch ) );
// 	    _frame->setBRight( KWUnit( rpt, rmm, rinch ) );
// 	    _frame->setBTop( KWUnit( tpt, tmm, tinch ) );
// 	    _frame->setBBottom( KWUnit( bpt, bmm, binch ) );
// 	    frames.append( _frame );
// 	}

// 	else
// 	    cerr << "Unknown tag '" << tag << "' in FRAMESET" << endl;

// 	if ( !parser.close( tag ) )
// 	{
// 	    cerr << "ERR: Closing Child" << endl;
// 	    return;
// 	}
//     }
// }

/******************************************************************/
/* Class: KWPartFrameSet					  */
/******************************************************************/

/*================================================================*/
KWPartFrameSet::KWPartFrameSet( KWordDocument *_doc, KWordChild *_child )
    : KWFrameSet( _doc )
{
    child = _child;
    _enableDrawing = true;
    frame = 0L;
}

/*================================================================*/
KWPartFrameSet::~KWPartFrameSet()
{
    if ( frame )
    {
	frame->detach();
	delete frame;
    }
}

/*================================================================*/
QPicture *KWPartFrameSet::getPicture()
{
    if ( !_enableDrawing ) return 0L;

    return child->draw( 1.0, false );
}

/*================================================================*/
void KWPartFrameSet::activate( QWidget *_widget, int diffx, int diffy, int diffxx )
{
    if ( !frame )
    {
	frame = new KWordFrame( dynamic_cast<KWordView*>( _widget ), child );
	frame->attachView( view );
	frame->show();
    }
    frame->setGeometry( frames.at( 0 )->x() - diffx + diffxx, frames.at( 0 )->y() - diffy + 20,
			frames.at( 0 )->width(), frames.at( 0 )->height() );
    frame->view()->mainWindow()->setActivePart( frame->view()->id() );

    parentID = dynamic_cast<KWordView*>( _widget )->getID();
}

/*================================================================*/
void KWPartFrameSet::deactivate()
{
    if ( frame )
    {
	frame->view()->mainWindow()->setActivePart( parentID );
	// HACK!
	frame->setGeometry( -10, -10, 1, 1 );
    }
}

/*================================================================*/
void KWPartFrameSet::update()
{
    child->setGeometry( QRect( frames.at( 0 )->x(), frames.at( 0 )->y(), frames.at( 0 )->width(), frames.at( 0 )->height() ) );
}

/******************************************************************/
/* Class: KWGroupManager					  */
/******************************************************************/

/*================================================================*/
void KWGroupManager::addFrameSet( KWFrameSet *fs, unsigned int row, unsigned int col )
{
    unsigned int sum = row * 10 + col;
    unsigned int i = 0;

    rows = max(row + 1,rows);
    cols = max(col + 1,cols);

    for ( i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->row * 10 + cells.at( i )->col > sum )
	    break;
    }

    Cell *cell = new Cell;
    cell->frameSet = fs;
    cell->row = row;
    cell->col = col;
    cell->rows = 1;
    cell->cols = 1;

    cells.insert( i, cell );
}

/*================================================================*/
KWFrameSet *KWGroupManager::getFrameSet( unsigned int row, unsigned int col )
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->row == row && cells.at( i )->col == col )
	    return cells.at( i )->frameSet;
    }

    return 0L;
}

/*================================================================*/
KWGroupManager::Cell *KWGroupManager::getCell( unsigned int row, unsigned int col )
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->row == row && cells.at( i )->col == col )
	    return cells.at( i );
    }

    return 0L;
}

/*================================================================*/
bool KWGroupManager::getFrameSet( KWFrameSet *fs, unsigned int &row, unsigned int &col )
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->frameSet == fs )
	{
	    row = cells.at( i )->row;
	    col = cells.at( i )->col;
	    return true;
	}
    }

    return false;
}

/*================================================================*/
bool KWGroupManager::isTableHeader( KWFrameSet *fs )
{
    for ( unsigned int i = 0; i < cols; i++ )
    {
	if ( cells.at( i )->frameSet == fs )
	    return true;
    }

    return false;
}

/*================================================================*/
void KWGroupManager::init( unsigned int x, unsigned int y, unsigned int width, unsigned int height )
{
    unsigned int wid = width / cols - 2;
    unsigned int hei = height / rows - 2;

    // some error checking to avoid infinite loops
    if ( static_cast<int>( hei ) < doc->getDefaultParagLayout()->getFormat()->getPTFontSize() + 10 )
	hei = doc->getDefaultParagLayout()->getFormat()->getPTFontSize() + 10;
    if ( wid < 60 ) wid = 60;

    unsigned int _wid, _hei;

    KWFrame *frame;

    for ( unsigned int i = 0; i < rows; i++ )
    {
	for ( unsigned int j = 0; j < cols; j++ )
	{
	    KWUnit u;
	    u.setMM( 1 );
	    frame = getFrameSet( i, j )->getFrame( 0 );
	    frame->setBLeft( u );
	    frame->setBRight( u );
	    frame->setBTop( u );
	    frame->setBBottom( u );
	    _wid = wid;
	    _wid += frame->getBLeft().pt() + frame->getBRight().pt();
	    _hei = hei;
	    _hei += frame->getBTop().pt() + frame->getBBottom().pt();
	    frame->setRect( x + j * _wid + 2 * j, y + i * _hei + 2 * i, _wid, _hei );
	}
    }

    for ( unsigned int k = 0; k < cells.count(); k++ )
	doc->addFrameSet( cells.at( k )->frameSet );
}

/*================================================================*/
void KWGroupManager::init()
{
    for ( unsigned int k = 0; k < cells.count(); k++ )
	doc->addFrameSet( cells.at( k )->frameSet );
}

/*================================================================*/
void KWGroupManager::recalcCols()
{
    for ( unsigned int i = 0; i < cols; i++ )
    {
	unsigned int j = 0;
	int wid = -1;
	int _wid = 100000;
	for ( j = 0; j < rows; j++ )
	{
	    if ( getFrameSet( j, i )->getFrame( 0 )->isSelected() )
		wid = getFrameSet( j, i )->getFrame( 0 )->width();
	    _wid = min(getFrameSet(j,i)->getFrame(0)->width(),_wid);
	}
	if ( wid != -1 )
	{
	    if ( getBoundingRect().x() + getBoundingRect().width() + ( wid - _wid ) >
		 static_cast<int>( doc->getPTPaperWidth() ) )
		wid = _wid;
	    for ( j = 0; j < rows; j++ )
	    {
		KWFrame *frame = getFrameSet( j, i )->getFrame( 0 );
		if ( wid < static_cast<int>( doc->getRastX() + frame->getBLeft().pt() + frame->getBRight().pt() ) )
		    frame->setWidth( doc->getRastX() + frame->getBLeft().pt() + frame->getBRight().pt() );
		else
		    frame->setWidth( wid );
	    }
	}
    }

    unsigned int x = getFrameSet( 0, 0 )->getFrame( 0 )->x();
    for ( unsigned int i = 0; i < cols; i++ )
    {
	unsigned int j = 0;
	for ( j = 0; j < rows; j++ )
	{
	    getFrameSet( j, i )->getFrame( 0 )->moveTopLeft( QPoint( x, getFrameSet( j, i )->getFrame( 0 )->y() ) );
	    getFrameSet( j, i )->update();
	}
	x = getFrameSet( 0, i )->getFrame( 0 )->right() + 3;
    }


    // Reggie: this has to be improved!
    QList<int> ws;
    ws.setAutoDelete( true );

    for ( unsigned int i = 0; i < cols; i++ )
	ws.append( new int( getFrameSet( 0, i )->getFrame( 0 )->width() ) );

    for ( unsigned int i = 0; i < rows; i++ )
    {
	for ( unsigned int j = 0; j < cols; j++ )
	{
	    Cell *cell = getCell( i, j );
	    if ( cell->cols != 1 )
	    {
		if ( cell->cols > 0 )
		{
		    int w = 0;
		    for ( unsigned int k = 0; k < cell->cols; k++ )
			w += *ws.at( k + cell->col );

		    cell->frameSet->getFrame( 0 )->setWidth( w + ( cell->cols - 2 ) * 2 + 2 );
		}
	    }
	}
    }
}

/*================================================================*/
void KWGroupManager::recalcRows()
{
    // remove atomatically added headers
    for ( unsigned int j = 0; j < rows; j++ )
    {
	if ( getFrameSet( j, 0 )->isRemoveableHeader() )
	{
	    deleteRow( j, false );
	    j--;
	}
    }
    hasTmpHeaders = false;

    for ( unsigned int j = 0; j < rows; j++ )
    {
	unsigned int i = 0;
	int hei = -1;
	int _hei = 100000;
	for ( i = 0; i < cols; i++ )
	{
	    if ( getFrameSet( j, i )->getFrame( 0 )->isSelected() )
		hei = getFrameSet( j, i )->getFrame( 0 )->height();
	    _hei = min(getFrameSet(j,i)->getFrame(0)->height(),_hei);
	}
	if ( hei != -1 )
	{
	    for ( i = 0; i < cols; i++ )
		getFrameSet( j, i )->getFrame( 0 )->setHeight( hei );
	}
    }

    unsigned int y = getFrameSet( 0, 0 )->getFrame( 0 )->y();
    for ( unsigned int j = 0; j < rows; j++ )
    {
	unsigned int i = 0;
	bool _addRow = false;

	if ( doc->getProcessingType() == KWordDocument::DTP )
	{
	    if ( j > 0 && y + getFrameSet( j, i )->getFrame( 0 )->height() >
		 ( getFrameSet( j - 1, i )->getPageOfFrame( 0 ) + 1 ) * doc->getPTPaperHeight() - doc->getPTBottomBorder() )
	    {
		y = ( getFrameSet( j - 1, i )->getPageOfFrame( 0 ) + 1 ) * doc->getPTPaperHeight() + doc->getPTTopBorder();
		_addRow = true;
	    }
	}
	else
	{
	    if ( j > 0 && static_cast<int>( y + getFrameSet( j, i )->getFrame( 0 )->height() ) >
		 static_cast<int>( ( doc->getFrameSet( 0 )->getFrame( getFrameSet( j - 1, i )->getPageOfFrame( 0 ) * doc->getColumns() )->bottom() ) ) )
	    {
		if ( doc->getPages() < getFrameSet( j - 1, i )->getPageOfFrame( 0 ) + 2 )
		    doc->appendPage( doc->getPages() - 1 );
		{
		    _addRow = true;
		    y = doc->getFrameSet( 0 )->getFrame( ( getFrameSet( j - 1, i )->getPageOfFrame( 0 ) + 1 ) * doc->getColumns() )->y();
		}
	    }
	}

	if ( _addRow && showHeaderOnAllPages )
	{
	    hasTmpHeaders = true;
	    insertRow( j, false, true );
	}

	for ( i = 0; i < cols; i++ )
	{
	    if ( _addRow )
	    {
		KWTextFrameSet *f1, *f2;
		f1 = dynamic_cast<KWTextFrameSet*>( getFrameSet( j, i ) );
		f2 = dynamic_cast<KWTextFrameSet*>( getFrameSet( 0, i ) );
		f1->assign( f2 );
		f1->getFrame( 0 )->setHeight( f2->getFrame( 0 )->height() );
	    }

	    getFrameSet( j, i )->getFrame( 0 )->moveTopLeft( QPoint( getFrameSet( j, i )->getFrame( 0 )->x(), y ) );
	    getFrameSet( j, i )->update();
	}

	y = getFrameSet( j, 0 )->getFrame( 0 )->bottom() + 3;
    }

    if ( getBoundingRect().y() + getBoundingRect().height() >
	 static_cast<int>( doc->getPTPaperHeight() * doc->getPages() ) )
	doc->appendPage( doc->getPages() - 1 );

  // Reggie: UHHHHHHHHHHHH: Ugly and slow but it helps for now
    Cell *c;
    for ( unsigned int f = 0; f < cells.count(); f++ )
    {
	c = cells.at( f );
	if ( c->frameSet->getNumFrames() > 1 )
	{
	    while ( true )
	    {
		if ( c->frameSet->getNumFrames() > 1 )
		    c->frameSet->delFrame( 1 );
		else
		    break;
	    }
	}
    }

    // Reggie: this has to be improved
    QList<int> hs;
    hs.setAutoDelete( true );

    for ( unsigned int i = 0; i < rows; i++ )
	hs.append( new int( getFrameSet( i, 0 )->getFrame( 0 )->height() ) );

    for ( unsigned int i = 0; i < cols; i++ )
    {
	for ( unsigned int j = 0; j < rows; j++ )
	{
	    Cell *cell = getCell( j, i );
	    if ( cell->rows != 1 )
	    {
		if ( cell->rows > 0 )
		{
		    int h = 0;
		    for ( unsigned int k = 0; k < cell->rows; k++ )
			h += *hs.at( k + cell->row );

		    debug( "expanded %d/%d", cell->row, cell->col );

		    cell->frameSet->getFrame( 0 )->setHeight( h + ( cell->rows - 2 ) * 2 + 2 );
		}
	    }
	}
    }
}

/*================================================================*/
QRect KWGroupManager::getBoundingRect()
{
    QRect r1, r2;
    KWFrame *first = getFrameSet( 0, 0 )->getFrame( 0 );
    KWFrame *last = getFrameSet( rows - 1, cols - 1 )->getFrame( 0 );

    r1 = QRect( first->x(), first->y(), first->width(), first->height() );
    r2 = QRect( last->x(), last->y(), last->width(), last->height() );

    r1 = r1.unite( r2 );
    return QRect( r1 );
}

/*================================================================*/
bool KWGroupManager::hasSelectedFrame()
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->frameSet->getFrame( 0 )->isSelected() )
	    return true;
    }

    return false;
}

/*================================================================*/
void KWGroupManager::moveBy( unsigned int dx, unsigned int dy )
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
	cells.at( i )->frameSet->getFrame( 0 )->moveBy( dx, dy );
}

/*================================================================*/
void KWGroupManager::drawAllRects( QPainter &p, int xOffset, int yOffset )
{
    KWFrame *frame = 0L;

    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	frame = cells.at( i )->frameSet->getFrame( 0 );
	p.drawRect( frame->x() - xOffset, frame->y() - yOffset, frame->width(), frame->height() );
    }
}


/*================================================================*/
void KWGroupManager::deselectAll()
{
    KWFrame *frame = 0L;

    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	frame = cells.at( i )->frameSet->getFrame( 0 );
	frame->setSelected( false );
    }
}

/*================================================================*/
void KWGroupManager::selectUntil( KWFrameSet *fs )
{
    unsigned int row = 0, col = 0;
    getFrameSet( fs, row, col );

    unsigned int srow = 0, scol = 0;
    if ( !isOneSelected( fs, srow, scol ) )
	srow = scol = 0;

    if ( srow > row )
    {
	srow = srow^row;
	row = srow^row;
	srow = srow^row;
    }

    if ( scol > col )
    {
	scol = scol^col;
	col = scol^col;
	scol = scol^col;
    }

    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->row <= row && cells.at( i )->col <= col &&
	     cells.at( i )->row >= srow && cells.at( i )->col >= scol )
	    cells.at( i )->frameSet->getFrame( 0 )->setSelected( true );
	else
	    cells.at( i )->frameSet->getFrame( 0 )->setSelected( false );
    }
}

/*================================================================*/
bool KWGroupManager::isOneSelected( KWFrameSet *fs, unsigned int &row, unsigned int &col )
{
    bool one = false;

    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->frameSet->getFrame( 0 )->isSelected() && fs != cells.at( i )->frameSet )
	{
	    if ( !one )
	    {
		row = cells.at( i )->row;
		col = cells.at( i )->col;
		one = true;
	    }
	}
    }

    return one;
}

/*================================================================*/
void KWGroupManager::insertRow( unsigned int _idx, bool _recalc, bool _removeable )
{
    unsigned int i = 0;
    unsigned int _rows = rows;

    QList<int> w;
    w.setAutoDelete( true );
    QRect r = getBoundingRect();

    for ( i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->row == 0 ) w.append( new int( cells.at( i )->frameSet->getFrame( 0 )->width() ) );
	if ( cells.at( i )->row >= _idx ) cells.at( i )->row++;
    }

    QList<KWTextFrameSet> nCells;
    nCells.setAutoDelete( false );

    int ww = 0;
    for ( i = 0; i < getCols(); i++ )
    {
	KWFrame *frame = new KWFrame( r.x() + ww, r.y(), *w.at( i ),
				      doc->getDefaultParagLayout()->getFormat()->getPTFontSize() + 10 );
	KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
	_frameSet->addFrame( frame );
	_frameSet->setAutoCreateNewFrame( false );
	_frameSet->setGroupManager( this );
	_frameSet->setIsRemoveableHeader( _removeable );
	addFrameSet( _frameSet, _idx, i );
	nCells.append( _frameSet );
	ww += *w.at( i ) + 2;
    }

    rows = ++_rows;

    for ( i = 0; i < nCells.count(); i++ )
    {
	KWUnit u;
	u.setMM( 1 );
	doc->addFrameSet( nCells.at( i ) );
	KWFrame *frame = nCells.at( i )->getFrame( 0 );
	frame->setBLeft( u );
	frame->setBRight( u );
	frame->setBTop( u );
	frame->setBBottom( u );
    }

    if ( _recalc )
	recalcRows();
}

/*================================================================*/
void KWGroupManager::insertCol( unsigned int _idx )
{
    unsigned int i = 0;
    unsigned int _cols = cols;

    QList<int> h;
    h.setAutoDelete( true );
    QRect r = getBoundingRect();

    for ( i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->col == 0 ) h.append( new int( cells.at( i )->frameSet->getFrame( 0 )->height() ) );
	if ( cells.at( i )->col >= _idx ) cells.at( i )->col++;
    }

    QList<KWTextFrameSet> nCells;
    nCells.setAutoDelete( false );

    int hh = 0;
    for ( i = 0; i < getRows(); i++ )
    {
	KWFrame *frame = new KWFrame( r.x(), r.y() + hh, 60, *h.at( i ) );
	KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
	_frameSet->addFrame( frame );
	_frameSet->setAutoCreateNewFrame( false );
	_frameSet->setGroupManager( this );
	addFrameSet( _frameSet, i, _idx );
	nCells.append( _frameSet );
	hh += *h.at( i ) + 2;
    }

    cols = ++_cols;

    for ( i = 0; i < nCells.count(); i++ )
    {
	KWUnit u;
	u.setMM( 1 );
	doc->addFrameSet( nCells.at( i ) );
	KWFrame *frame = nCells.at( i )->getFrame( 0 );
	frame->setBLeft( u );
	frame->setBRight( u );
	frame->setBTop( u );
	frame->setBBottom( u );
    }

    recalcCols();
}

/*================================================================*/
void KWGroupManager::deleteRow( unsigned int _idx, bool _recalc )
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->row == _idx )
	{
	    doc->delFrameSet( cells.at( i )->frameSet );
	    cells.at( i )->frameSet = 0L;
	    cells.remove( i );
	    i--;
	    continue;
	}
	if ( cells.at( i )->row > _idx ) cells.at( i )->row--;
    }

    rows--;

    if ( _recalc )
	recalcRows();
}

/*================================================================*/
void KWGroupManager::deleteCol( unsigned int _idx )
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
    {
	if ( cells.at( i )->col == _idx )
	{
	    doc->delFrameSet( cells.at( i )->frameSet );
	    cells.at( i )->frameSet = 0L;
	    cells.remove( i );
	    i--;
	    continue;
	}
	if ( cells.at( i )->col > _idx ) cells.at( i )->col--;
    }

    cols--;

    recalcCols();
}

/*================================================================*/
void KWGroupManager::updateTempHeaders()
{
    //if ( !hasTmpHeaders ) return;

    for ( unsigned int i = 1; i < rows; i++ )
    {
	for ( unsigned int j = 0; j < cols; j++ )
	{
	    KWFrameSet *fs = getFrameSet( i, j );
	    if ( fs->isRemoveableHeader() )
	    {
		dynamic_cast<KWTextFrameSet*>( fs )->assign( dynamic_cast<KWTextFrameSet*>( getFrameSet( 0, j ) ) );

		QPainter p;
		QPicture pic;
		p.begin( &pic );

		KWFormatContext fc( doc, doc->getFrameSetNum( fs ) + 1 );
		fc.init( dynamic_cast<KWTextFrameSet*>( fs )->getFirstParag(), true );

		bool bend = false;
		while ( !bend )
		    bend = !fc.makeNextLineLayout();

		p.end();
	    }
	}
    }
}

/*================================================================*/
void KWGroupManager::ungroup()
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
	cells.at( i )->frameSet->setGroupManager( 0L );

    cells.setAutoDelete( false );
    cells.clear();

    active = false;
}

/*================================================================*/
bool KWGroupManager::joinCells()
{
    enum Orientation {Vertical, Horizontal};

    unsigned int col, row;
    if ( !isOneSelected( 0L, row, col ) ) return false;

    bool enough = false;
    Orientation orient = Horizontal;

    if ( col > 0 && getFrameSet( row, col - 1 )->getFrame( 0 )->isSelected() )
    {
	enough = true;
	orient = Horizontal;
    }
    else if ( col < cols - 1 && getFrameSet( row, col + 1 )->getFrame( 0 )->isSelected() )
    {
	enough = true;
	orient = Horizontal;
    }
    else if ( row > 0 && getFrameSet( row - 1, col )->getFrame( 0 )->isSelected() )
    {
	enough = true;
	orient = Vertical;
    }
    else if ( row < rows - 1 && getFrameSet( row + 1, col )->getFrame( 0 )->isSelected() )
    {
	enough = true;
	orient = Vertical;
    }

    QList<Cell> _cells;
    _cells.setAutoDelete( false );

    if ( enough )
    {
	switch ( orient )
	{
	case Horizontal:
	{
	    Cell *cell = getCell( row, col );
	    int tmpCol = col;
	    while ( cell && cell->frameSet->getFrame( 0 )->isSelected() )
	    {
		if ( cell->rows > 1 || cell->cols > 1 )
		    return false;
		_cells.insert( 0, cell );
		if ( --tmpCol >= 0 )
		    cell = getCell( row, tmpCol );
		else
		    cell = 0L;
	    }
	    if ( col + 1 > cols - 1 ) break;

	    cell = getCell( row, col + 1 );
	    tmpCol = col + 1;
	    while ( cell && cell->frameSet->getFrame( 0 )->isSelected() )
	    {
		if ( cell->rows > 1 || cell->cols > 1 )
		    return false;
		_cells.append( cell );
		if ( ++tmpCol <= static_cast<int>( cols - 1 ) )
		    cell = getCell( row, tmpCol );
		else
		    cell = 0L;
	    }
	} break;
	case Vertical:
	{
	    Cell *cell = getCell( row, col );
	    int tmpRow = row;
	    while ( cell && cell->frameSet->getFrame( 0 )->isSelected() )
	    {
		if ( cell->rows > 1 || cell->cols > 1 )
		    return false;
		_cells.insert( 0, cell );
		if ( --tmpRow >= 0 )
		    cell = getCell( tmpRow, col );
		else
		    cell = 0L;
	    }
	    if ( row + 1 > rows - 1 ) break;

	    cell = getCell( row + 1, col );
	    tmpRow = row + 1;
	    while ( cell && cell->frameSet->getFrame( 0 )->isSelected() )
	    {
		if ( cell->rows > 1 || cell->cols > 1 )
		    return false;
		_cells.append( cell );
		if ( ++tmpRow <= static_cast<int>( rows - 1 ) )
		    cell = getCell( tmpRow, col );
		else
		    cell = 0L;
	    }
	} break;
	}

	// this just can't happen :- )
	if ( _cells.count() < 1 )
	    return false;

	switch ( orient )
	{
	case Horizontal:
	{
	    Cell *cell = _cells.first();
	    cell->cols = _cells.count();
	    for ( cell = _cells.next(); cell != 0; cell = _cells.next() )
	    {
		cell->cols = 0;
		cell->frameSet->setVisible( false );
	    }
	    recalcCols();
	} break;
	case Vertical:
	{
	    Cell *cell = _cells.first();
	    cell->rows = _cells.count();
	    for ( cell = _cells.next(); cell != 0; cell = _cells.next() )
	    {
		cell->frameSet->setVisible( false );
		cell->rows = 0;
	    }
	    recalcRows();
	} break;
	}

	return true;
    }

    return false;
}

/*================================================================*/
bool KWGroupManager::splitCell()
{
    unsigned int col, row;
    if ( !isOneSelected( 0L, row, col ) ) return false;

    Cell *cell = getCell( row, col );
    if ( cell->rows > 1 )
    {
	unsigned int rows = cell->rows;
	for ( unsigned int i = 0; i < rows; i++ )
	{
	    getCell( i + cell->row, col )->rows = 1;
	    getCell( i + cell->row, col )->frameSet->setVisible( true );
	}
	recalcRows();

	return true;
    }
    else if ( cell->cols > 1 )
    {
	unsigned int cols = cell->cols;
	for ( unsigned int i = 0; i < cols; i++ )
	{
	    getCell( row, i + cell->col )->cols = 1;
	    getCell( row, i + cell->col )->frameSet->setVisible( true );
	}
	recalcCols();

	return true;
    }

    return false;
}

/*================================================================*/
bool isAHeader( FrameInfo fi )
{
    return ( fi == FI_FIRST_HEADER || fi == FI_EVEN_HEADER || fi == FI_ODD_HEADER );
}

/*================================================================*/
bool isAFooter( FrameInfo fi )
{
    return ( fi == FI_FIRST_FOOTER || fi == FI_EVEN_FOOTER || fi == FI_ODD_FOOTER );
}

/*================================================================*/
bool isAWrongHeader( FrameInfo fi, KoHFType t )
{
    switch ( fi )
    {
    case FI_FIRST_HEADER:
    {
	if ( t == HF_FIRST_DIFF ) return false;
	return true;
    } break;
    case FI_EVEN_HEADER:
    {
	return false;
    } break;
    case FI_ODD_HEADER:
    {
	if ( t == HF_EO_DIFF ) return false;
	return true;
    } break;
    default: return false;
    }

    return false;
}

/*================================================================*/
bool isAWrongFooter( FrameInfo fi, KoHFType t )
{
    switch ( fi )
    {
    case FI_FIRST_FOOTER:
    {
	if ( t == HF_FIRST_DIFF ) return false;
	return true;
    } break;
    case FI_EVEN_FOOTER:
    {
	return false;
    } break;
    case FI_ODD_FOOTER:
    {
	if ( t == HF_EO_DIFF ) return false;
	return true;
    } break;
    default: return false;
    }

    return false;
}
