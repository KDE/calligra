/* -*- Mode: C++ -*-
   $Id$
*/

/****************************************************************************
 ** Copyright (C)  2002-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDGantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDGantt licenses may use this file in
 ** accordance with the KDGantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
 **   information about KDGantt Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 ** As a special exception, permission is given to link this program
 ** with any edition of Qt, and distribute the resulting executable,
 ** without including the source code for Qt in the source distribution.
 **
 **********************************************************************/

#include "KDGanttMinimizeSplitter.h"
#ifndef QT_NO_SPLITTER

#include "qpainter.h"
#include "qdrawutil.h"
#include "qbitmap.h"
#if QT_VERSION >= 300
#include "qptrlist.h"
#include "qmemarray.h"
#else
#include <qlist.h>
#include <qarray.h>
#define QPtrList QList
#define QMemArray QArray
#endif
#include "qlayoutengine_p.h"
#include "qobjectlist.h"
#include "qstyle.h"
#include "qapplication.h" //sendPostedEvents
#include <qvaluelist.h>
#include <qcursor.h>
#ifndef KDGANTT_MASTER_CVS
#include "KDGanttMinimizeSplitter.moc"
#endif


#ifndef DOXYGEN_SKIP_INTERNAL

#if QT_VERSION >= 300
static int mouseOffset;
static int opaqueOldPos = -1; //### there's only one mouse, but this is a bit risky


KDGanttSplitterHandle::KDGanttSplitterHandle( Qt::Orientation o,
				  KDGanttMinimizeSplitter *parent, const char * name )
    : QWidget( parent, name ), _activeButton( 0 ), _collapsed( false )
{
    s = parent;
    setOrientation(o);
    setMouseTracking( true );
}

QSize KDGanttSplitterHandle::sizeHint() const
{
    return QSize(8,8);
}

void KDGanttSplitterHandle::setOrientation( Qt::Orientation o )
{
    orient = o;
#ifndef QT_NO_CURSOR
    if ( o == KDGanttMinimizeSplitter::Horizontal )
	setCursor( splitHCursor );
    else
	setCursor( splitVCursor );
#endif
}


void KDGanttSplitterHandle::mouseMoveEvent( QMouseEvent *e )
{
    updateCursor( e->pos() );
    if ( !(e->state()&LeftButton) )
	return;

    if ( _activeButton != 0)
        return;

    QCOORD pos = s->pick(parentWidget()->mapFromGlobal(e->globalPos()))
		 - mouseOffset;
    if ( opaque() ) {
	s->moveSplitter( pos, id() );
    } else {
	int min = pos; int max = pos;
	s->getRange( id(), &min, &max );
	s->setRubberband( QMAX( min, QMIN(max, pos )));
    }
    _collapsed = false;
}

void KDGanttSplitterHandle::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() == LeftButton ) {
        _activeButton = onButton( e->pos() );
        mouseOffset = s->pick(e->pos());
        if ( _activeButton != 0)
            repaint();
        updateCursor( e->pos() );
    }
}

void KDGanttSplitterHandle::updateCursor( const QPoint& p)
{
    if ( onButton( p ) != 0 ) {
        setCursor( arrowCursor );
    }
    else {
        if ( orient == KDGanttMinimizeSplitter::Horizontal )
            setCursor( splitHCursor );
        else
            setCursor( splitVCursor );
    }
}


void KDGanttSplitterHandle::mouseReleaseEvent( QMouseEvent *e )
{
    if ( _activeButton != 0 ) {
        if ( onButton( e->pos() ) == _activeButton )
        {
            int pos;
            int min, max;
            if ( !_collapsed ) {
                s->expandPos( id(), &min, &max );
                if ( s->minimizeDirection() == KDGanttMinimizeSplitter::Left
                     || s->minimizeDirection() == KDGanttMinimizeSplitter::Up ) {
                    pos = min;
                }
                else {
                    pos = max;
                }

                _origPos = s->pick(mapToParent( QPoint( 0,0 ) ));
                s->moveSplitter( pos, id() );
                _collapsed = true;
            }
            else {
                s->moveSplitter( _origPos, id() );
                _collapsed = false;
            }

        }
        _activeButton = 0;
        updateCursor( e->pos() );
    }
    else {
        if ( !opaque() && e->button() == LeftButton ) {
            QCOORD pos = s->pick(parentWidget()->mapFromGlobal(e->globalPos()))
                - mouseOffset;
            s->setRubberband( -1 );
            s->moveSplitter( pos, id() );
        }
    }
    repaint();
}

int KDGanttSplitterHandle::onButton( const QPoint& p )
{
    QValueList<QPointArray> list = buttonRegions();
    int index = 1;
    for( QValueList<QPointArray>::Iterator it = list.begin(); it != list.end(); ++it ) {
        QRect rect = (*it).boundingRect();
        rect.setLeft( rect.left()- 4 );
        rect.setRight( rect.right() + 4);
        rect.setTop( rect.top()- 4 );
        rect.setBottom( rect.bottom() + 4);
        if ( rect.contains( p ) ) {
            return index;
        }
        index++;
    }
    return 0;
}


QValueList<QPointArray> KDGanttSplitterHandle::buttonRegions()
{
    QValueList<QPointArray> list;

    int sw = 8;
    int voffset[] = { (int) -sw*3, (int) sw*3 };
    for ( int i = 0; i < 2; i++ ) {
        QPointArray arr;
        if ( !_collapsed && s->minimizeDirection() == KDGanttMinimizeSplitter::Right ||
             _collapsed  && s->minimizeDirection() == KDGanttMinimizeSplitter::Left) {
            int mid = height()/2 + voffset[i];
            arr.setPoints( 3,
                           1, mid - sw + 4,
                           sw-3, mid,
                           1, mid + sw -4);
        }
        else if ( !_collapsed &&  s->minimizeDirection() == KDGanttMinimizeSplitter::Left ||
                  _collapsed  && s->minimizeDirection() == KDGanttMinimizeSplitter::Right ) {
            int mid = height()/2 + voffset[i];
            arr.setPoints( 3,
                           sw-4, mid - sw + 4,
                           0, mid,
                           sw-4, mid + sw - 4);
        }
        else if ( !_collapsed &&  s->minimizeDirection() == KDGanttMinimizeSplitter::Up ||
                  _collapsed  && s->minimizeDirection() == KDGanttMinimizeSplitter::Down) {
            int mid = width()/2 + voffset[i];
            arr.setPoints( 3,
                           mid - sw + 4, sw-4,
                           mid, 0,
                           mid + sw - 4, sw-4 );
        }
        else if ( !_collapsed && s->minimizeDirection() == KDGanttMinimizeSplitter::Down ||
                  _collapsed  && s->minimizeDirection() == KDGanttMinimizeSplitter::Up ) {
            int mid = width()/2 + voffset[i];
            arr.setPoints( 3,
                           mid - sw + 4, 1,
                           mid, sw-3,
                           mid + sw -4, 1);
        }
        list.append( arr );
    }
    return list;
}

void KDGanttSplitterHandle::paintEvent( QPaintEvent * )
{
    QPixmap buffer( size() );
    QPainter p( &buffer );

    // Draw the splitter rectangle
    p.setBrush( colorGroup().background() );
    p.setPen( colorGroup().foreground() );
    p.drawRect( rect() );
    parentWidget()->style().drawPrimitive( QStyle::PE_Panel, &p, rect(),
                                           parentWidget()->colorGroup());

    int sw = 8; // Hardcoded, given I didn't use styles anymore, I didn't like to use their size

    // arrow color
    QColor col = colorGroup().background().dark( 200 );
    p.setBrush( col );
    p.setPen( col  );

    QValueList<QPointArray> list = buttonRegions();
    int index = 1;
    for ( QValueList<QPointArray>::Iterator it = list.begin(); it != list.end(); ++it ) {
        if ( index == _activeButton ) {
            p.save();
            p.translate( parentWidget()->style().pixelMetric( QStyle::PM_ButtonShiftHorizontal ),
                         parentWidget()->style().pixelMetric( QStyle::PM_ButtonShiftVertical ) );
            p.drawPolygon( *it, true );
            p.restore();
        }
        else {
            p.drawPolygon( *it, true );
        }
        index++;
    }

    // Draw the lines between the arrows
    if ( s->minimizeDirection() == KDGanttMinimizeSplitter::Left ||
         s->minimizeDirection() == KDGanttMinimizeSplitter::Right ) {
        int mid = height()/2;
        p.drawLine ( 2, mid - sw, 2, mid + sw );
        p.drawLine ( 4, mid - sw, 4, mid + sw );
    }
    else if ( s->minimizeDirection() == KDGanttMinimizeSplitter::Up ||
              s->minimizeDirection() == KDGanttMinimizeSplitter::Down ) {
        int mid = width()/2;
        p.drawLine( mid -sw, 2, mid +sw, 2 );
        p.drawLine( mid -sw, 4, mid +sw, 4 );
    }
    bitBlt( this, 0, 0, &buffer );
}
#endif

class QSplitterLayoutStruct
{
public:
    KDGanttMinimizeSplitter::ResizeMode mode;
    QCOORD sizer;
    bool isSplitter;
    QWidget *wid;
};

class QSplitterData
{
public:
    QSplitterData() : opaque( FALSE ), firstShow( TRUE ) {}

    QPtrList<QSplitterLayoutStruct> list;
    bool opaque;
    bool firstShow;
};

void kdganttGeomCalc( QMemArray<QLayoutStruct> &chain, int start, int count, int pos,
                 int space, int spacer );
#endif // DOXYGEN_SKIP_INTERNAL


/*!
  \class KDGanttMinimizeSplitter KDGanttMinimizeSplitter.h
  \brief The KDGanttMinimizeSplitter class implements a splitter
  widget with minimize buttons.

  This class (and its documentation) is largely a copy of Qt's
  QSplitter; the copying was necessary because QSplitter is not
  extensible at all. QSplitter and its documentation are licensed
  according to the GPL and the Qt Professional License (if you hold
  such a license) and are (C) Trolltech AS.

  A splitter lets the user control the size of child widgets by
  dragging the boundary between the children. Any number of widgets
  may be controlled.

  To show a QListBox, a QListView and a QTextEdit side by side:

  \code
    KDGanttMinimizeSplitter *split = new KDGanttMinimizeSplitter( parent );
    QListBox *lb = new QListBox( split );
    QListView *lv = new QListView( split );
    QTextEdit *ed = new QTextEdit( split );
  \endcode

  In KDGanttMinimizeSplitter, the boundary can be either horizontal or
  vertical.  The default is horizontal (the children are side by side)
  but you can use setOrientation( QSplitter::Vertical ) to set it to
  vertical.

  Use setResizeMode() to specify
  that a widget should keep its size when the splitter is resized.

  Although KDGanttMinimizeSplitter normally resizes the children only
  at the end of a resize operation, if you call setOpaqueResize( TRUE
  ) the widgets are resized as often as possible.

  The initial distribution of size between the widgets is determined
  by the initial size of each widget. You can also use setSizes() to
  set the sizes of all the widgets. The function sizes() returns the
  sizes set by the user.

  If you hide() a child, its space will be distributed among the other
  children. It will be reinstated when you show() it again. It is also
  possible to reorder the widgets within the splitter using
  moveToFirst() and moveToLast().
*/



static QSize minSize( const QWidget* /*w*/ )
{
    return QSize(0,0);
}

// This is the original version of minSize
static QSize minSizeHint( const QWidget* w )
{
    QSize min = w->minimumSize();
    QSize s;
    if ( min.height() <= 0 || min.width() <= 0 )
	s = w->minimumSizeHint();
    if ( min.height() > 0 )
	s.setHeight( min.height() );
    if ( min.width() > 0 )
	s.setWidth( min.width() );
    return s.expandedTo(QSize(0,0));
}



/*!
  Constructs a horizontal splitter with the \a parent and \a
  name arguments being passed on to the QFrame constructor.
*/
KDGanttMinimizeSplitter::KDGanttMinimizeSplitter( QWidget *parent, const char *name )
    :QFrame(parent,name,WPaintUnclipped)
{
#if QT_VERSION >= 300
     orient = Horizontal;
     init();
#endif
}

/*!
  Constructs a splitter with orientation \a o with the \a parent
  and \a name arguments being passed on to the QFrame constructor.
*/
KDGanttMinimizeSplitter::KDGanttMinimizeSplitter( Orientation o, QWidget *parent, const char *name )
    :QFrame(parent,name,WPaintUnclipped)
{
#if QT_VERSION >= 300
     orient = o;
     init();
#endif
}

/*!
  Destroys the splitter and any children.
*/
KDGanttMinimizeSplitter::~KDGanttMinimizeSplitter()
{
#if QT_VERSION >= 300
    data->list.setAutoDelete( TRUE );
    delete data;
#endif
}


#if QT_VERSION >= 300
void KDGanttMinimizeSplitter::init()
{
    data = new QSplitterData;
    if ( orient == Horizontal )
	setSizePolicy( QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum) );
    else
	setSizePolicy( QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding) );
}
#endif



/*!
  \brief the orientation of the splitter

  By default the orientation is horizontal (the widgets are side by side).
  The possible orientations are Qt:Vertical and Qt::Horizontal (the default).
*/
void KDGanttMinimizeSplitter::setOrientation( Orientation o )
{
#if QT_VERSION >= 300
    if ( orient == o )
	return;
    orient = o;

    if ( orient == Horizontal )
	setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    else
	setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QSplitterLayoutStruct *s = data->list.first();
    while ( s ) {
	if ( s->isSplitter )
	    ((KDGanttSplitterHandle*)s->wid)->setOrientation( o );
	s = data->list.next();  // ### next at end of loop, no iterator
    }
    recalc( isVisible() );
#endif
}


#if QT_VERSION >= 300
/*!
  Reimplemented from superclass.
*/
void KDGanttMinimizeSplitter::resizeEvent( QResizeEvent * )
{
    doResize();
}


/*
  Inserts the widget \a w at the end (or at the beginning if \a first
  is TRUE) of the splitter's list of widgets.

  It is the responsibility of the caller of this function to make sure
  that \a w is not already in the splitter and to call recalcId if
  needed.  (If \a first is TRUE, then recalcId is very probably
  needed.)
*/
QSplitterLayoutStruct *KDGanttMinimizeSplitter::addWidget( QWidget *w, bool first )
{
    QSplitterLayoutStruct *s;
    KDGanttSplitterHandle *newHandle = 0;
    if ( data->list.count() > 0 ) {
	s = new QSplitterLayoutStruct;
	s->mode = KeepSize;
	QString tmp = "qt_splithandle_";
	tmp += w->name();
	newHandle = new KDGanttSplitterHandle( orientation(), this, tmp.latin1() );
	s->wid = newHandle;
	newHandle->setId(data->list.count());
	s->isSplitter = TRUE;
	s->sizer = pick( newHandle->sizeHint() );
	if ( first )
	    data->list.insert( 0, s );
	else
	    data->list.append( s );
    }
    s = new QSplitterLayoutStruct;
    s->mode = Stretch;
    s->wid = w;
    if ( !testWState( WState_Resized ) && w->sizeHint().isValid() )
	s->sizer = pick( w->sizeHint() );
    else
	s->sizer = pick( w->size() );
    s->isSplitter = FALSE;
    if ( first )
	data->list.insert( 0, s );
    else
	data->list.append( s );
    if ( newHandle && isVisible() )
	newHandle->show(); //will trigger sending of post events
    return s;
}


/*!
  Tells the splitter that a child widget has been inserted or removed.
  The event is passed in \a c.
*/
void KDGanttMinimizeSplitter::childEvent( QChildEvent *c )
{
    if ( c->type() == QEvent::ChildInserted ) {
	if ( !c->child()->isWidgetType() )
	    return;

	if ( ((QWidget*)c->child())->testWFlags( WType_TopLevel ) )
	    return;

	QSplitterLayoutStruct *s = data->list.first();
	while ( s ) {
	    if ( s->wid == c->child() )
		return;
	    s = data->list.next();
	}
	addWidget( (QWidget*)c->child() );
	recalc( isVisible() );

    } else if ( c->type() == QEvent::ChildRemoved ) {
	QSplitterLayoutStruct *p = 0;
	if ( data->list.count() > 1 )
	    p = data->list.at(1); //remove handle _after_ first widget.
	QSplitterLayoutStruct *s = data->list.first();
	while ( s ) {
	    if ( s->wid == c->child() ) {
		data->list.removeRef( s );
		delete s;
		if ( p && p->isSplitter ) {
		    data->list.removeRef( p );
		    delete p->wid; //will call childEvent
		    delete p;
		}
		recalcId();
		doResize();
		return;
	    }
	    p = s;
	    s = data->list.next();
	}
    }
}


/*!
  Shows a rubber band at position \a p. If \a p is negative, the
  rubber band is removed.
*/
void KDGanttMinimizeSplitter::setRubberband( int p )
{
    QPainter paint( this );
    paint.setPen( gray );
    paint.setBrush( gray );
    paint.setRasterOp( XorROP );
    QRect r = contentsRect();
    const int rBord = 3; //Themable????
    int sw = style().pixelMetric(QStyle::PM_SplitterWidth, this);
    if ( orient == Horizontal ) {
	if ( opaqueOldPos >= 0 )
	    paint.drawRect( opaqueOldPos + sw/2 - rBord , r.y(),
			    2*rBord, r.height() );
	if ( p >= 0 )
	    paint.drawRect( p  + sw/2 - rBord, r.y(), 2*rBord, r.height() );
    } else {
	if ( opaqueOldPos >= 0 )
	    paint.drawRect( r.x(), opaqueOldPos + sw/2 - rBord,
			    r.width(), 2*rBord );
	if ( p >= 0 )
	    paint.drawRect( r.x(), p + sw/2 - rBord, r.width(), 2*rBord );
    }
    opaqueOldPos = p;
}


/*! Reimplemented from superclass. */
bool KDGanttMinimizeSplitter::event( QEvent *e )
{
    if ( e->type() == QEvent::LayoutHint || ( e->type() == QEvent::Show && data->firstShow ) ) {
	recalc( isVisible() );
	if ( e->type() == QEvent::Show )
	    data->firstShow = FALSE;
    }
    return QWidget::event( e );
}


/*!
  \obsolete

  Draws the splitter handle in the rectangle described by \a x, \a y,
  \a w, \a h using painter \a p.
  \sa QStyle::drawPrimitive()
*/
void KDGanttMinimizeSplitter::drawSplitter( QPainter *p,
			      QCOORD x, QCOORD y, QCOORD w, QCOORD h )
{
    style().drawPrimitive(QStyle::PE_Splitter, p, QRect(x, y, w, h), colorGroup(),
			  (orientation() == Qt::Horizontal ?
			   QStyle::Style_Horizontal : 0));
}


/*!
  Returns the id of the splitter to the right of or below the widget \a w,
  or 0 if there is no such splitter
  (i.e. it is either not in this KDGanttMinimizeSplitter or it is at the end).
*/
int KDGanttMinimizeSplitter::idAfter( QWidget* w ) const
{
    QSplitterLayoutStruct *s = data->list.first();
    bool seen_w = FALSE;
    while ( s ) {
	if ( s->isSplitter && seen_w )
	    return data->list.at();
	if ( !s->isSplitter && s->wid == w )
	    seen_w = TRUE;
	s = data->list.next();
    }
    return 0;
}


/*!
  Moves the left/top edge of the splitter handle with id \a id as
  close as possible to position \a p, which is the distance from the
  left (or top) edge of the widget.

  For Arabic and Hebrew the layout is reversed, and using this
  function to set the position of the splitter might lead to
  unexpected results, since in Arabic and Hebrew the position of
  splitter one is to the left of the position of splitter zero.

  \sa idAfter()
*/
void KDGanttMinimizeSplitter::moveSplitter( QCOORD p, int id )
{
    p = adjustPos( p, id );

    QSplitterLayoutStruct *s = data->list.at(id);
    int oldP = orient == Horizontal ? s->wid->x() : s->wid->y();
    bool upLeft;
    if ( QApplication::reverseLayout() && orient == Horizontal ) {
	p += s->wid->width();
	upLeft = p > oldP;
    } else
	upLeft = p < oldP;

    moveAfter( p, id, upLeft );
    moveBefore( p-1, id-1, upLeft );

    storeSizes();
}


void KDGanttMinimizeSplitter::setG( QWidget *w, int p, int s, bool isSplitter )
{
    if ( orient == Horizontal ) {
	if ( QApplication::reverseLayout() && orient == Horizontal && !isSplitter )
	    p = contentsRect().width() - p - s;
	w->setGeometry( p, contentsRect().y(), s, contentsRect().height() );
    } else
	w->setGeometry( contentsRect().x(), p, contentsRect().width(), s );
}


/*
  Places the right/bottom edge of the widget at \a id at position \a pos.

  \sa idAfter()
*/
void KDGanttMinimizeSplitter::moveBefore( int pos, int id, bool upLeft )
{
    if( id < 0 )
	return;
    QSplitterLayoutStruct *s = data->list.at(id);
    if ( !s )
	return;
    QWidget *w = s->wid;
    if ( w->isHidden() ) {
	moveBefore( pos, id-1, upLeft );
    } else if ( s->isSplitter ) {
	int pos1, pos2;
	int dd = s->sizer;
	if( QApplication::reverseLayout() && orient == Horizontal ) {
	    pos1 = pos;
	    pos2 = pos + dd;
	} else {
	    pos2 = pos - dd;
	    pos1 = pos2 + 1;
	}
	if ( upLeft ) {
	    setG( w, pos1, dd, TRUE );
	    moveBefore( pos2, id-1, upLeft );
	} else {
	    moveBefore( pos2, id-1, upLeft );
	    setG( w, pos1, dd, TRUE );
	}
    } else {
	int dd, newLeft, nextPos;
	if( QApplication::reverseLayout() && orient == Horizontal ) {
	    dd = w->geometry().right() - pos;
	    dd = QMAX( pick(minSize(w)), QMIN(dd, pick(w->maximumSize())));
	    newLeft = pos+1;
	    nextPos = newLeft + dd;
	} else {
	    dd = pos - pick( w->pos() ) + 1;
	    dd = QMAX( pick(minSize(w)), QMIN(dd, pick(w->maximumSize())));
	    newLeft = pos-dd+1;
	    nextPos = newLeft - 1;
	}
	setG( w, newLeft, dd, TRUE );
	moveBefore( nextPos, id-1, upLeft );
    }
}


/*
  Places the left/top edge of the widget at \a id at position \a pos.

  \sa idAfter()
*/
void KDGanttMinimizeSplitter::moveAfter( int pos, int id, bool upLeft )
{
    QSplitterLayoutStruct *s = id < int(data->list.count()) ?
			       data->list.at(id) : 0;
    if ( !s )
	return;
    QWidget *w = s->wid;
    if ( w->isHidden() ) {
	moveAfter( pos, id+1, upLeft );
    } else if ( pick( w->pos() ) == pos ) {
	//No need to do anything if it's already there.
	return;
    } else if ( s->isSplitter ) {
	int dd = s->sizer;
	int pos1, pos2;
	if( QApplication::reverseLayout() && orient == Horizontal ) {
	    pos2 = pos - dd;
	    pos1 = pos2 + 1;
	} else {
	    pos1 = pos;
	    pos2 = pos + dd;
	}
	if ( upLeft ) {
	    setG( w, pos1, dd, TRUE );
	    moveAfter( pos2, id+1, upLeft );
	} else {
	    moveAfter( pos2, id+1, upLeft );
	    setG( w, pos1, dd, TRUE );
	}
    } else {
	int left = pick( w->pos() );
	int right, dd,/* newRight,*/ newLeft, nextPos;
	if ( QApplication::reverseLayout() && orient == Horizontal ) {
	    dd = pos - left + 1;
	    dd = QMAX( pick(minSize(w)), QMIN(dd, pick(w->maximumSize())));
	    newLeft = pos-dd+1;
	    nextPos = newLeft - 1;
	} else {
	    right = pick( w->geometry().bottomRight() );
	    dd = right - pos + 1;
	    dd = QMAX( pick(minSize(w)), QMIN(dd, pick(w->maximumSize())));
	    /*newRight = pos+dd-1;*/
	    newLeft = pos;
	    nextPos = newLeft + dd;
	}
	setG( w, newLeft, dd, TRUE );
	/*if( right != newRight )*/
	moveAfter( nextPos, id+1, upLeft );
    }
}


void KDGanttMinimizeSplitter::expandPos( int id, int*  min, int* max )
{
    QSplitterLayoutStruct *s = data->list.at(id-1);
    QWidget* w = s->wid;
    *min = pick( w->mapToParent( QPoint(0,0) ) );

    if ( (uint) id == data->list.count() ) {
        pick( size() );
    }
    else {
        QSplitterLayoutStruct *s = data->list.at(id+1);
        QWidget* w = s->wid;
        *max = pick( w->mapToParent( QPoint( w->width(), w->height() ) ) ) -8;
    }
}


/*!
  Returns the valid range of the splitter with id \a id in \a *min and \a *max.

  \sa idAfter()
*/

void KDGanttMinimizeSplitter::getRange( int id, int *min, int *max )
{
    int minB = 0;	//before
    int maxB = 0;
    int minA = 0;
    int maxA = 0;	//after
    int n = data->list.count();
    if ( id < 0 || id >= n )
	return;
    int i;
    for ( i = 0; i < id; i++ ) {
	QSplitterLayoutStruct *s = data->list.at(i);
	if ( s->wid->isHidden() ) {
	    //ignore
	} else if ( s->isSplitter ) {
	    minB += s->sizer;
	    maxB += s->sizer;
	} else {
	    minB += pick( minSize(s->wid) );
	    maxB += pick( s->wid->maximumSize() );
	}
    }
    for ( i = id; i < n; i++ ) {
	QSplitterLayoutStruct *s = data->list.at(i);
	if ( s->wid->isHidden() ) {
	    //ignore
	} else if ( s->isSplitter ) {
	    minA += s->sizer;
	    maxA += s->sizer;
	} else {
	    minA += pick( minSize(s->wid) );
	    maxA += pick( s->wid->maximumSize() );
	}
    }
    QRect r = contentsRect();
    if ( orient == Horizontal && QApplication::reverseLayout() ) {
	int splitterWidth = style().pixelMetric(QStyle::PM_SplitterWidth, this);
	if ( min )
	    *min = pick(r.topRight()) - QMIN( maxB, pick(r.size())-minA ) - splitterWidth;
	if ( max )
	    *max = pick(r.topRight()) - QMAX( minB, pick(r.size())-maxA ) - splitterWidth;
    } else {
	if ( min )
	    *min = pick(r.topLeft()) + QMAX( minB, pick(r.size())-maxA );
	if ( max )
	    *max = pick(r.topLeft()) + QMIN( maxB, pick(r.size())-minA );
    }
}


/*!
  Returns the closest legal position to \a p of the splitter with id \a id.

  \sa idAfter()
*/

int KDGanttMinimizeSplitter::adjustPos( int p, int id )
{
    int min = 0;
    int max = 0;
    getRange( id, &min, &max );
    p = QMAX( min, QMIN( p, max ) );

    return p;
}


void KDGanttMinimizeSplitter::doResize()
{
    QRect r = contentsRect();
    int i;
    int n = data->list.count();
    QMemArray<QLayoutStruct> a( n );
    for ( i = 0; i< n; i++ ) {
	a[i].init();
	QSplitterLayoutStruct *s = data->list.at(i);
	if ( s->wid->isHidden() ) {
	    a[i].stretch = 0;
	    a[i].sizeHint = a[i].minimumSize = 0;
	    a[i].maximumSize = 0;
	} else if ( s->isSplitter ) {
	    a[i].stretch = 0;
	    a[i].sizeHint = a[i].minimumSize = a[i].maximumSize = s->sizer;
	    a[i].empty = FALSE;
	} else if ( s->mode == KeepSize ) {
	    a[i].stretch = 0;
	    a[i].minimumSize = pick( minSize(s->wid) );
	    a[i].sizeHint = s->sizer;
	    a[i].maximumSize = pick( s->wid->maximumSize() );
	    a[i].empty = FALSE;
	} else if ( s->mode == FollowSizeHint ) {
	    a[i].stretch = 0;
	    a[i].minimumSize = a[i].sizeHint = pick( s->wid->sizeHint() );
	    a[i].maximumSize = pick( s->wid->maximumSize() );
	    a[i].empty = FALSE;
	} else { //proportional
	    a[i].stretch = s->sizer;
	    a[i].maximumSize = pick( s->wid->maximumSize() );
	    a[i].sizeHint = a[i].minimumSize = pick( minSize(s->wid) );
	    a[i].empty = FALSE;
	}
    }

    kdganttGeomCalc( a, 0, n, pick( r.topLeft() ), pick( r.size() ), 0 );

    for ( i = 0; i< n; i++ ) {
	QSplitterLayoutStruct *s = data->list.at(i);
	setG( s->wid, a[i].pos, a[i].size );
    }

}


void KDGanttMinimizeSplitter::recalc( bool update )
{
    int fi = 2*frameWidth();
    int maxl = fi;
    int minl = fi;
    int maxt = QWIDGETSIZE_MAX;
    int mint = fi;
    int n = data->list.count();
    bool first = TRUE;
    /*
      The splitter before a hidden widget is always hidden.
      The splitter before the first visible widget is hidden.
      The splitter before any other visible widget is visible.
    */
    for ( int i = 0; i< n; i++ ) {
	QSplitterLayoutStruct *s = data->list.at(i);
	if ( !s->isSplitter ) {
	    QSplitterLayoutStruct *p = (i > 0) ? data->list.at( i-1 ) : 0;
	    if ( p && p->isSplitter )
		if ( first || s->wid->isHidden() )
		    p->wid->hide(); //may trigger new recalc
		else
		    p->wid->show(); //may trigger new recalc
	    if ( !s->wid->isHidden() )
		first = FALSE;
	}
    }

    bool empty=TRUE;
    for ( int j = 0; j< n; j++ ) {
	QSplitterLayoutStruct *s = data->list.at(j);
	if ( !s->wid->isHidden() ) {
	    empty = FALSE;
	    if ( s->isSplitter ) {
		minl += s->sizer;
		maxl += s->sizer;
	    } else {
		QSize minS = minSize(s->wid);
		minl += pick( minS );
		maxl += pick( s->wid->maximumSize() );
		mint = QMAX( mint, trans( minS ));
		int tm = trans( s->wid->maximumSize() );
		if ( tm > 0 )
		    maxt = QMIN( maxt, tm );
	    }
	}
    }
    if ( empty ) {
        if ( parentWidget() != 0 && parentWidget()->inherits("KDGanttMinimizeSplitter") ) {
            // nested splitters; be nice
            maxl = maxt = 0;
        } else {
            // KDGanttMinimizeSplitter with no children yet
            maxl = QWIDGETSIZE_MAX;
        }
    } else {
        maxl = QMIN( maxl, QWIDGETSIZE_MAX );
    }
    if ( maxt < mint )
	maxt = mint;

    if ( orient == Horizontal ) {
	setMaximumSize( maxl, maxt );
	setMinimumSize( minl, mint );
    } else {
	setMaximumSize( maxt, maxl );
	setMinimumSize( mint, minl );
    }
    if ( update )
	doResize();
}

/*!
  Sets resize mode of \a w to \a mode.

  \sa ResizeMode
*/

void KDGanttMinimizeSplitter::setResizeMode( QWidget *w, ResizeMode mode )
{
    processChildEvents();
    QSplitterLayoutStruct *s = data->list.first();
    while ( s ) {
	if ( s->wid == w  ) {
	    s->mode = mode;
	    return;
	}
	s = data->list.next();
    }
    s = addWidget( w, TRUE );
    s->mode = mode;
}


/*!
  Returns TRUE if opaque resize is on; otherwise returns FALSE.

  \sa setOpaqueResize()
*/

bool KDGanttMinimizeSplitter::opaqueResize() const
{
    return data->opaque;
}


/*!
    If \a on is TRUE then opaque resizing is turned on; otherwise
    opaque resizing is turned off.
  Opaque resizing is initially turned off.

  \sa opaqueResize()
*/

void KDGanttMinimizeSplitter::setOpaqueResize( bool on )
{
    data->opaque = on;
}


/*!
  Moves widget \a w to the leftmost/top position.
*/

void KDGanttMinimizeSplitter::moveToFirst( QWidget *w )
{
    processChildEvents();
    bool found = FALSE;
    QSplitterLayoutStruct *s = data->list.first();
    while ( s ) {
	if ( s->wid == w  ) {
	    found = TRUE;
	    QSplitterLayoutStruct *p = data->list.prev();
	    if ( p ) { // not already at first place
		data->list.take(); //take p
		data->list.take(); // take s
		data->list.insert( 0, p );
		data->list.insert( 0, s );
	    }
	    break;
	}
	s = data->list.next();
    }
     if ( !found )
	addWidget( w, TRUE );
     recalcId();
}


/*!
  Moves widget \a w to the rightmost/bottom position.
*/

void KDGanttMinimizeSplitter::moveToLast( QWidget *w )
{
    processChildEvents();
    bool found = FALSE;
    QSplitterLayoutStruct *s = data->list.first();
    while ( s ) {
	if ( s->wid == w  ) {
	    found = TRUE;
	    data->list.take(); // take s
	    QSplitterLayoutStruct *p = data->list.current();
	    if ( p ) { // the splitter handle after s
		data->list.take(); //take p
		data->list.append( p );
	    }
	    data->list.append( s );
	    break;
	}
	s = data->list.next();
    }
     if ( !found )
	addWidget( w);
     recalcId();
}


void KDGanttMinimizeSplitter::recalcId()
{
    int n = data->list.count();
    for ( int i = 0; i < n; i++ ) {
	QSplitterLayoutStruct *s = data->list.at(i);
	if ( s->isSplitter )
	    ((KDGanttSplitterHandle*)s->wid)->setId(i);
    }
}


/*! Reimplemented from superclass.
*/
QSize KDGanttMinimizeSplitter::sizeHint() const
{
    constPolish();
    int l = 0;
    int t = 0;
    if ( children() ) {
	const QObjectList * c = children();
	QObjectListIt it( *c );
	QObject * o;

	while( (o=it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() &&
		 !((QWidget*)o)->isHidden() ) {
		QSize s = ((QWidget*)o)->sizeHint();
		if ( s.isValid() ) {
		    l += pick( s );
		    t = QMAX( t, trans( s ) );
		}
	    }
	}
    }
    return orientation() == Horizontal ? QSize( l, t ) : QSize( t, l );
}


/*!
\reimp
*/

QSize KDGanttMinimizeSplitter::minimumSizeHint() const
{
    constPolish();
    int l = 0;
    int t = 0;
    if ( children() ) {
	const QObjectList * c = children();
	QObjectListIt it( *c );
	QObject * o;

	while( (o=it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() &&
		 !((QWidget*)o)->isHidden() ) {
		QSize s = minSizeHint((QWidget*)o);
		if ( s.isValid() ) {
		    l += pick( s );
		    t = QMAX( t, trans( s ) );
		}
	    }
	}
    }
    return orientation() == Horizontal ? QSize( l, t ) : QSize( t, l );
}


/*
  Calculates stretch parameters from current sizes
*/

void KDGanttMinimizeSplitter::storeSizes()
{
    QSplitterLayoutStruct *s = data->list.first();
    while ( s ) {
	if ( !s->isSplitter )
	    s->sizer = pick( s->wid->size() );
	s = data->list.next();
    }
}


#if 0 // ### remove this code ASAP

/*!
  Hides \a w if \a hide is TRUE and updates the splitter.

  \warning Due to a limitation in the current implementation,
  calling QWidget::hide() will not work.
*/

void KDGanttMinimizeSplitter::setHidden( QWidget *w, bool hide )
{
    if ( w == w1 ) {
	w1show = !hide;
    } else if ( w == w2 ) {
	w2show = !hide;
    } else {
#ifdef QT_CHECK_RANGE
	qWarning( "KDGanttMinimizeSplitter::setHidden(), unknown widget" );
#endif
	return;
    }
    if ( hide )
	w->hide();
    else
	w->show();
    recalc( TRUE );
}


/*!
  Returns the hidden status of \a w
*/

bool KDGanttMinimizeSplitter::isHidden( QWidget *w ) const
{
    if ( w == w1 )
	return !w1show;
     else if ( w == w2 )
	return !w2show;
#ifdef QT_CHECK_RANGE
    else
	qWarning( "KDGanttMinimizeSplitter::isHidden(), unknown widget" );
#endif
    return FALSE;
}
#endif


/*!
  Returns a list of the size parameters of all the widgets in this
  splitter.

  Giving the values to another splitter's setSizes() function will
  produce a splitter with the same layout as this one.

  Note that if you want to iterate over the list, you should
  iterate over a copy, e.g.
    \code
    QValueList<int> list = mySplitter.sizes();
    QValueList<int>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  \sa setSizes()
*/

QValueList<int> KDGanttMinimizeSplitter::sizes() const
{
    if ( !testWState(WState_Polished) ) {
	QWidget* that = (QWidget*) this;
	that->polish();
    }
    QValueList<int> list;
    QSplitterLayoutStruct *s = data->list.first();
    while ( s ) {
	if ( !s->isSplitter )
	    list.append( s->sizer );
	s = data->list.next();
    }
    return list;
}



/*!
  Sets the size parameters to the values given in \a list.
  If the splitter is horizontal, the values set the sizes from
  left to right. If it is vertical, the sizes are applied from
  top to bottom.
  Extra values in \a list are ignored.

  If \a list contains too few values, the result is undefined
  but the program will still be well-behaved.

  \sa sizes()
*/

void KDGanttMinimizeSplitter::setSizes( QValueList<int> list )
{
    processChildEvents();
    QValueList<int>::Iterator it = list.begin();
    QSplitterLayoutStruct *s = data->list.first();
    while ( s && it != list.end() ) {
	if ( !s->isSplitter ) {
	    s->sizer = *it;
	    ++it;
	}
	s = data->list.next();
    }
    doResize();
}


/*!
  Gets all posted child events, ensuring that the internal state of
  the splitter is consistent.
*/

void KDGanttMinimizeSplitter::processChildEvents()
{
    QApplication::sendPostedEvents( this, QEvent::ChildInserted );
}


/*!
  Reimplemented from superclass.
*/

void KDGanttMinimizeSplitter::styleChange( QStyle& old )
{
    int sw = style().pixelMetric(QStyle::PM_SplitterWidth, this);
    QSplitterLayoutStruct *s = data->list.first();
    while ( s ) {
	if ( s->isSplitter )
	    s->sizer = sw;
	s = data->list.next();
    }
    doResize();
    QFrame::styleChange( old );
}

#endif

/*!
  Specifies the direction of the minimize buttons.
  If the orientation of the splitter is horizontal then with
  KDGanttMinimizeSplitter::Left or KDGanttMinimizeSplitter::Right should be used,
  otherwise either KDGanttMinimizeSplitter::Up or KDGanttMinimizeSplitter::Down
  should be used.
*/
void KDGanttMinimizeSplitter::setMinimizeDirection( Direction direction )
{
    _direction = direction;
}

/*!
  Returns the direction of the minimize buttons.
*/
KDGanttMinimizeSplitter::Direction KDGanttMinimizeSplitter::minimizeDirection() const
{
    return _direction;
}

/*
  This is a copy of qGeomCalc() in qlayoutengine.cpp which
  unfortunately isn't exported.
*/
static inline int toFixed( int i ) { return i * 256; }
static inline int fRound( int i ) {
    return ( i % 256 < 128 ) ? i / 256 : 1 + i / 256;
}
void kdganttGeomCalc( QMemArray<QLayoutStruct> &chain, int start, int count, int pos,
		int space, int spacer )
{
    typedef int fixed;
    int cHint = 0;
    int cMin = 0;
    int cMax = 0;
    int sumStretch = 0;
    int spacerCount = 0;

    bool wannaGrow = FALSE; // anyone who really wants to grow?
    //    bool canShrink = FALSE; // anyone who could be persuaded to shrink?

    int i;
    for ( i = start; i < start + count; i++ ) {
	chain[i].done = FALSE;
	cHint += chain[i].sizeHint;
	cMin += chain[i].minimumSize;
	cMax += chain[i].maximumSize;
	sumStretch += chain[i].stretch;
	if ( !chain[i].empty )
	    spacerCount++;
	wannaGrow = wannaGrow || chain[i].expansive;
    }

    int extraspace = 0;
    if ( spacerCount )
	spacerCount--; // only spacers between things
    if ( space < cMin + spacerCount * spacer ) {
	//	qDebug("not enough space");
	for ( i = start; i < start+count; i++ ) {
	    chain[i].size = chain[i].minimumSize;
	    chain[i].done = TRUE;
	}
    } else if ( space < cHint + spacerCount*spacer ) {
	// Less space than sizeHint, but more than minimum.
	// Currently take space equally from each, like in Qt 2.x.
	// Commented-out lines will give more space to stretchier items.
	int n = count;
	int space_left = space - spacerCount*spacer;
	int overdraft = cHint - space_left;
	//first give to the fixed ones:
	for ( i = start; i < start+count; i++ ) {
	    if ( !chain[i].done && chain[i].minimumSize >= chain[i].sizeHint) {
		chain[i].size = chain[i].sizeHint;
		chain[i].done = TRUE;
		space_left -= chain[i].sizeHint;
		// sumStretch -= chain[i].stretch;
		n--;
	    }
	}
	bool finished = n == 0;
	while ( !finished ) {
	    finished = TRUE;
	    fixed fp_over = toFixed( overdraft );
	    fixed fp_w = 0;

	    for ( i = start; i < start+count; i++ ) {
		if ( chain[i].done )
		    continue;
		// if ( sumStretch <= 0 )
		fp_w += fp_over / n;
		// else
		//    fp_w += (fp_over * chain[i].stretch) / sumStretch;
		int w = fRound( fp_w );
		chain[i].size = chain[i].sizeHint - w;
		fp_w -= toFixed( w ); //give the difference to the next
		if ( chain[i].size < chain[i].minimumSize ) {
		    chain[i].done = TRUE;
		    chain[i].size = chain[i].minimumSize;
		    finished = FALSE;
		    overdraft -= chain[i].sizeHint - chain[i].minimumSize;
		    // sumStretch -= chain[i].stretch;
		    n--;
		    break;
		}
	    }
	}
    } else { //extra space
	int n = count;
	int space_left = space - spacerCount*spacer;
	// first give to the fixed ones, and handle non-expansiveness
	for ( i = start; i < start + count; i++ ) {
	    if ( !chain[i].done && (chain[i].maximumSize <= chain[i].sizeHint
				    || wannaGrow && !chain[i].expansive) ) {
		chain[i].size = chain[i].sizeHint;
		chain[i].done = TRUE;
		space_left -= chain[i].sizeHint;
		sumStretch -= chain[i].stretch;
		n--;
	    }
	}
	extraspace = space_left;
	/*
	  Do a trial distribution and calculate how much it is off.
	  If there are more deficit pixels than surplus pixels, give
	  the minimum size items what they need, and repeat.
	  Otherwise give to the maximum size items, and repeat.

	  I have a wonderful mathematical proof for the correctness
	  of this principle, but unfortunately this comment is too
	  small to contain it.
	*/
	int surplus, deficit;
	do {
	    surplus = deficit = 0;
	    fixed fp_space = toFixed( space_left );
	    fixed fp_w = 0;
	    for ( i = start; i < start+count; i++ ) {
		if ( chain[i].done )
		    continue;
		extraspace = 0;
		if ( sumStretch <= 0 )
		    fp_w += fp_space / n;
		else
		    fp_w += (fp_space * chain[i].stretch) / sumStretch;
		int w = fRound( fp_w );
		chain[i].size = w;
		fp_w -= toFixed( w ); // give the difference to the next
		if ( w < chain[i].sizeHint ) {
		    deficit +=  chain[i].sizeHint - w;
		} else if ( w > chain[i].maximumSize ) {
		    surplus += w - chain[i].maximumSize;
		}
	    }
	    if ( deficit > 0 && surplus <= deficit ) {
		// give to the ones that have too little
		for ( i = start; i < start+count; i++ ) {
		    if ( !chain[i].done &&
			 chain[i].size < chain[i].sizeHint ) {
			chain[i].size = chain[i].sizeHint;
			chain[i].done = TRUE;
			space_left -= chain[i].sizeHint;
			sumStretch -= chain[i].stretch;
			n--;
		    }
		}
	    }
	    if ( surplus > 0 && surplus >= deficit ) {
		// take from the ones that have too much
		for ( i = start; i < start+count; i++ ) {
		    if ( !chain[i].done &&
			 chain[i].size > chain[i].maximumSize ) {
			chain[i].size = chain[i].maximumSize;
			chain[i].done = TRUE;
			space_left -= chain[i].maximumSize;
			sumStretch -= chain[i].stretch;
			n--;
		    }
		}
	    }
	} while ( n > 0 && surplus != deficit );
	if ( n == 0 )
	    extraspace = space_left;
    }

    // as a last resort, we distribute the unwanted space equally
    // among the spacers (counting the start and end of the chain).

    //### should do a sub-pixel allocation of extra space
    int extra = extraspace / ( spacerCount + 2 );
    int p = pos + extra;
    for ( i = start; i < start+count; i++ ) {
	chain[i].pos = p;
	p = p + chain[i].size;
	if ( !chain[i].empty )
	    p += spacer+extra;
    }
}

#endif

/*!
  \enum KDGanttMinimizeSplitter::Direction

  The values of this enumeration describe into which direction the
  splitter will collapse its child widgets. By extension, it also
  specifies the orientation of the splitter; collapsing to the left or
  to the right results in a horizontal splitter, collapsing to the top
  or bottom in a vertical splitter.
*/

/*!
  \fn Orientation KDGanttMinimizeSplitter::orientation() const

  Returns the orientation of the splitter.
*/

/*! \enum KDGanttMinimizeSplitter::ResizeMode

  This enum type describes how KDGanttMinimizeSplitter will resize each of its child widgets.  The currently defined values are:

  Stretch: the widget will be resized when the splitter
  itself is resized.

  KeepSize: KDGanttMinimizeSplitter will try to keep this widget's size
  unchanged.

  FollowSizeHint: KDGanttMinimizeSplitter will resize the widget when the
  widget's size hint changes.
*/

