/* This file is part of the KDE project
   Copyright (C) 2000 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <ktextedit.h>

#include <qpixmap.h>
#include <qevent.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qlistbox.h>
#include <qvbox.h>
#include <qclipboard.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qdragobject.h>
#include <kglobalsettings.h>
#include <kcharsets.h>
#include <kdebug.h>
#include <klocale.h>

#include <qregexp.h>
#include <qapplication.h>
#include <qclipboard.h>
#ifdef __DECCXX
#include <alloca.h>
#endif

#include <stdlib.h>
#include <kpresenter_doc.h>
#include <commandhistory.h>
#include <textcmd.h>

QPixmap *KTextEdit::bufferPixmap( const QSize &s )
{
    if ( !buf_pixmap ) {
	buf_pixmap = new QPixmap( s );
    } else {
	if ( buf_pixmap->width() < s.width() ||
	     buf_pixmap->height() < s.height() ) {
	    buf_pixmap->resize( QMAX( s.width(), buf_pixmap->width() ),
				QMAX( s.height(), buf_pixmap->height() ) );
	}
    }
    return buf_pixmap;
}

KTextEdit::KTextEdit( KPresenterDoc *d, KPTextObject *txtobj, QWidget *parent, const char *name )
    : QWidget( parent, name, WNorthWestGravity | WRepaintNoErase ),
      doc( new KTextEditDocument( d, txtobj ) ), undoRedoInfo( doc )
{
    init();
}

KTextEdit::~KTextEdit()
{
    if ( painter.isActive() )
	painter.end();
    delete buf_pixmap;
    buf_pixmap=0L;
    delete formatTimer;
    formatTimer=0L;
    delete changeIntervalTimer;
    changeIntervalTimer=0L;
    delete blinkTimer;
    blinkTimer=0L;
    delete dragStartTimer;
    dragStartTimer=0L;
    delete cursor;
    cursor=0L;
    delete doc->formatter();
    doc->setFormatter(0L);
    delete doc;
    doc=0L;
    undoRedoInfo.doc=0L;
}

void KTextEdit::init()
{
    buf_pixmap = 0;
    doubleBuffer = 0;
    drawAll = TRUE;
    mousePressed = FALSE;
    inDoubleClick = FALSE;
    readOnly = FALSE;
    modified = FALSE;

    doc->setFormatter( new KTextEditFormatterBreakWords( doc ) );
    currentFormat = doc->formatCollection()->defaultFormat();
    currentAlignment = Qt::AlignLeft;
    currentParagType = Normal;

    setBackgroundMode( PaletteBase );
    setAcceptDrops( TRUE );
    resize( 200, 300 );

    setKeyCompression( TRUE );
    setMouseTracking( TRUE );
    setCursor( ibeamCursor );
    setFocusPolicy( WheelFocus );

    cursor = new KTextEditCursor( doc );

    formatTimer = new QTimer( this );
    connect( formatTimer, SIGNAL( timeout() ),
	     this, SLOT( formatMore() ) );
    lastFormatted = doc->firstParag();

    interval = 0;
    changeIntervalTimer = new QTimer( this );
    connect( changeIntervalTimer, SIGNAL( timeout() ),
	     this, SLOT( doChangeInterval() ) );

    cursorVisible = TRUE;
    blinkTimer = new QTimer( this );
    connect( blinkTimer, SIGNAL( timeout() ),
	     this, SLOT( blinkCursor() ) );

    dragStartTimer = new QTimer( this );
    connect( dragStartTimer, SIGNAL( timeout() ),
	     this, SLOT( startDrag() ) );

    formatMore();

    blinkCursorVisible = FALSE;

    mLines = -1;

    connect( this, SIGNAL( textChanged() ),
	     this, SLOT( setModified() ) );

    installEventFilter( this );
}

void KTextEdit::paintEvent( QPaintEvent *e )
{
    QPainter p( this );
    QRect r( rect() );
    if ( p.hasClipping() )
	r = QRect( e->rect() );
    drawContents( &p, r.x(), r.y(), r.width(), r.height() );
}

void KTextEdit::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    bool drawCur = hasFocus() || hasFocus();
    if ( !doc->firstParag() )
	return;

    KTextEditParag *parag = doc->firstParag();
    QSize s( doc->firstParag()->rect().size() );

    p->fillRect( 0, 0, width(), doc->y(),
		 colorGroup().brush( QColorGroup::Base ) );

    if ( !doubleBuffer ) {
	doubleBuffer = bufferPixmap( s );
	if ( painter.isActive() )
	    painter.end();
	painter.begin( doubleBuffer );
    }

    if ( !painter.isActive() )
	painter.begin( doubleBuffer );

    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	if ( !parag->rect().intersects( QRect( cx, cy, cw, ch ) ) ) {
	    if ( parag->rect().y() > cy + ch ) {
		cursorVisible = TRUE;
		return;
	    }
	    parag = parag->next();
	    continue;
	}

	if ( !parag->hasChanged() && !drawAll ) {
	    parag = parag->next();
	    continue;
	}

	parag->setChanged( FALSE );
	QSize s( parag->rect().size() );
	if ( s.width() > doubleBuffer->width() ||
	     s.height() > doubleBuffer->height() ) {
	    if ( painter.isActive() )
		painter.end();
	    doubleBuffer = bufferPixmap( s );
	    painter.begin( doubleBuffer );
	}
	painter.fillRect( QRect( 0, 0, s.width(), s.height() ),
			  colorGroup().brush( QColorGroup::Base ) );

	parag->paint( painter, colorGroup(), drawCur ? cursor : 0, TRUE );

	p->drawPixmap( parag->rect().topLeft(), *doubleBuffer, QRect( QPoint( 0, 0 ), s ) );
	if ( parag->rect().x() + parag->rect().width() < 0 + width() )
	    p->fillRect( parag->rect().x() + parag->rect().width(), parag->rect().y(),
			 ( 0 + width() ) - ( parag->rect().x() + parag->rect().width() ),
			 parag->rect().height(), colorGroup().brush( QColorGroup::Base ) );
	parag = parag->next();
    }

    parag = doc->lastParag();
    if ( parag->rect().y() + parag->rect().height() - 0 < height() )
	p->fillRect( 0, parag->rect().y() + parag->rect().height(), width(),
		     height() - ( parag->rect().y() + parag->rect().height() ),
		     colorGroup().brush( QColorGroup::Base ) );

    cursorVisible = TRUE;
}

void KTextEdit::keyPressEvent( QKeyEvent *e )
{
    changeIntervalTimer->stop();
    interval = 10;

    bool selChanged = FALSE;
    for ( int i = 1; i < doc->numSelections; ++i ) // start with 1 as we don't want to remove the Standard-Selection
	selChanged = doc->removeSelection( i ) || selChanged;

    if ( selChanged ) {
	repaintChanged();
    }

    bool clearUndoRedoInfo = TRUE;

    switch ( e->key() ) {
    case Key_Escape:
	emit exitEditMode();
	return;
    case Key_Left:
	moveCursor( MoveLeft, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Right:
	moveCursor( MoveRight, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Up:
	moveCursor( MoveUp, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Down:
	moveCursor( MoveDown, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Home:
	moveCursor( MoveHome, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_End:
	moveCursor( MoveEnd, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Prior:
	moveCursor( MovePgUp, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Next:
	moveCursor( MovePgDown, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Return: case Key_Enter:
	if ( mLines == 1 )
	    break;
	doc->removeSelection( KTextEditDocument::Standard );
	clearUndoRedoInfo = FALSE;
	doKeyboardAction( ActionReturn );
	break;
    case Key_Delete:
	if ( doc->hasSelection( KTextEditDocument::Standard ) ) {
	    removeSelectedText();
	    break;
	}

	doKeyboardAction( ActionDelete );
	clearUndoRedoInfo = FALSE;

	break;
    case Key_Backspace:
	if ( doc->hasSelection( KTextEditDocument::Standard ) ) {
	    removeSelectedText();
	    break;
	}

	if ( !cursor->parag()->prev() &&
	     cursor->atParagStart() &&
	     ( cursor->parag()->type() == KTextEditParag::Normal ||
	       cursor->parag()->listDepth() < 0 ) )
	    break;

	doKeyboardAction( ActionBackspace );
	clearUndoRedoInfo = FALSE;

	break;
    default: {
	    if ( e->text().length() && !( e->state() & AltButton ) &&
		 ( !e->ascii() || e->ascii() >= 32 ) ||
		 ( e->text() == "\t" && !( e->state() & ControlButton ) ) ) {
		clearUndoRedoInfo = FALSE;
		if ( e->key() == Key_Tab ) {
		    if ( cursor->index() == 0 && cursor->parag()->type() != KTextEditParag::Normal ) {
			cursor->parag()->setListDepth( cursor->parag()->listDepth() + 1 );
			drawCursor( FALSE );
			repaintChanged();
			drawCursor( TRUE );
			break;
		    }
		}

		if ( cursor->parag()->type() != KTextEditParag::BulletList &&
		     cursor->index() == 0 && ( e->text() == "-" || e->text() == "*" ) ) {
		    setParagType( BulletList );
		} else {
		    insert( e->text() );
		}
		break;
	    }
	    if ( e->state() & ControlButton ) {
		switch ( e->key() ) {
		case Key_C:
		    copy();
		    break;
		case Key_V:
		    paste();
		    break;
		case Key_X: {
		    cut();
		} break;
		case Key_I: case Key_T: case Key_Tab:
		    indent();
		    break;
		case Key_A:
		    moveCursor( MoveHome, e->state() & ShiftButton, FALSE );
		    break;
		case Key_E:
		    moveCursor( MoveEnd, e->state() & ShiftButton, FALSE );
		    break;
		case Key_Z:
		    undo();
		    break;
		case Key_Y:
		    redo();
		    break;
		}
		break;
	    }
	}
    }

    if ( clearUndoRedoInfo )
	undoRedoInfo.clear();

    changeIntervalTimer->start( 100, TRUE );
}

void KTextEdit::doKeyboardAction( int action )
{
    if ( readOnly )
	return;

    lastFormatted = cursor->parag();
    drawCursor( FALSE );

    switch ( action ) {
    case ActionDelete:
	checkUndoRedoInfo( UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.text = QString::null;
	}
	undoRedoInfo.text += cursor->parag()->at( cursor->index() )->c;
	if ( cursor->remove() )
	    undoRedoInfo.text += "\n";
	break;
    case ActionBackspace:
	if ( cursor->parag()->type() != KTextEditParag::Normal && cursor->index() == 0 ) {
	    if ( cursor->parag()->listDepth() > 0 ) {
		cursor->parag()->setListDepth( cursor->parag()->listDepth() - 1 );
	    } else {
		cursor->parag()->setType( KTextEditParag::Normal );
		currentParagType = Normal;
		emit currentParagTypeChanged( currentParagType );
	    }
	    lastFormatted = cursor->parag();
	    repaintChanged();
	    drawCursor( TRUE );
	    return;
	}
	checkUndoRedoInfo( UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.text = QString::null;
	}
	cursor->gotoLeft();
	undoRedoInfo.text.prepend( QString( cursor->parag()->at( cursor->index() )->c ) );
	undoRedoInfo.index = cursor->index();
	if ( cursor->remove() ) {
	    undoRedoInfo.text.remove( 0, 1 );
	    undoRedoInfo.text.prepend( "\n" );
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.id = cursor->parag()->paragId();
	}
	lastFormatted = cursor->parag();
	break;
    case ActionReturn:
	checkUndoRedoInfo( UndoRedoInfo::Return );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.text = QString::null;
	}
	undoRedoInfo.text += "\n";
	cursor->splitAndInsertEmtyParag();
	if ( cursor->parag()->prev() )
	    lastFormatted = cursor->parag()->prev();
	break;
    }

    formatMore();
    repaintChanged();
    drawCursor( TRUE );

    updateCurrentFormat();
    emit textChanged();
}

void KTextEdit::removeSelectedText()
{
    if ( readOnly )
	return;

    drawCursor( FALSE );
    checkUndoRedoInfo( UndoRedoInfo::RemoveSelected );
    if ( !undoRedoInfo.valid() ) {
	doc->selectionStart( KTextEditDocument::Standard, undoRedoInfo.id, undoRedoInfo.index );
	undoRedoInfo.text = QString::null;
    }
    undoRedoInfo.text = doc->selectedText( KTextEditDocument::Standard );
    doc->removeSelectedText( KTextEditDocument::Standard, cursor );
    lastFormatted = cursor->parag();
    formatMore();
    repaintChanged();
    drawCursor( TRUE );
    undoRedoInfo.clear();
    emit textChanged();
}

void KTextEdit::moveCursor( int direction, bool shift, bool control )
{
    drawCursor( FALSE );
    if ( shift ) {
	if ( !doc->hasSelection( KTextEditDocument::Standard ) )
	    doc->setSelectionStart( KTextEditDocument::Standard, cursor );
	moveCursor( direction, control );
	if ( doc->setSelectionEnd( KTextEditDocument::Standard, cursor ) ) {
	    repaintChanged();
	} else {
	    drawCursor( TRUE );
	}
    } else {
	bool redraw = doc->removeSelection( KTextEditDocument::Standard );
	moveCursor( direction, control );
	if ( !redraw ) {
	    drawCursor( TRUE );
	} else {
	    repaintChanged();
	    drawCursor( TRUE );
	}
    }

    updateCurrentFormat();
}

void KTextEdit::moveCursor( int direction, bool control )
{
    switch ( direction ) {
    case MoveLeft: {
	if ( !control )
	    cursor->gotoLeft();
	else
	    cursor->gotoWordLeft();
    } break;
    case MoveRight: {
	if ( !control )
	    cursor->gotoRight();
	else
	    cursor->gotoWordRight();
    } break;
    case MoveUp: {
	if ( !control )
	    cursor->gotoUp();
	else
	    cursor->gotoPageUp( this );
    } break;
    case MoveDown: {
	if ( !control )
	    cursor->gotoDown();
	else
	    cursor->gotoPageDown( this );
    } break;
    case MoveHome: {
	if ( !control )
	    cursor->gotoLineStart();
	else
	    cursor->gotoHome();
    } break;
    case MoveEnd: {
	if ( !control )
	    cursor->gotoLineEnd();
	else
	    cursor->gotoEnd();
    } break;
    case MovePgUp:
	cursor->gotoPageUp( this );
	break;
    case MovePgDown:
	cursor->gotoPageDown( this );
	break;
    }

    updateCurrentFormat();
}

void KTextEdit::resizeEvent( QResizeEvent *e )
{
    doResize();
    QWidget::resizeEvent( e );
}

void KTextEdit::drawCursor( bool visible )
{
    if ( !cursor->parag()->isValid() ||
	 ( !hasFocus() && !hasFocus() ) )
	return;

    cursorVisible = visible;

    QPainter p;
    p.begin( this );
    p.translate( -0, -0 );

    cursor->parag()->format();
    QSize s( cursor->parag()->rect().size() );

    if ( !doubleBuffer ) {
	doubleBuffer = bufferPixmap( s );
	if ( painter.isActive() )
	    painter.end();
	painter.begin( doubleBuffer );
    }

    if ( !painter.isActive() )
	painter.begin( doubleBuffer );

    if ( !painter.isActive() ) {
	qDebug( "something went wrong!" );
	return;
    }

    KTextEditString::Char *chr = cursor->parag()->at( cursor->index() );

    painter.setPen( QPen( chr->format->color() ) );
    painter.setFont( chr->format->font() );
    int h = 0; int y = 0;
    int bl = 0;
    int cw = chr->format->width( chr->c );
    h = cursor->parag()->lineHeightOfChar( cursor->index(), &bl, &y );

    bool fill = TRUE;
    for ( int i = 0; i < doc->numSelections; ++i ) {
	if ( cursor->parag()->hasSelection( i ) ) {
	    int start = cursor->parag()->selectionStart( i );
	    int end = cursor->parag()->selectionEnd( i );
	    if ( end == cursor->parag()->length() - 1 && cursor->parag()->next() &&
		 cursor->parag()->next()->hasSelection( i ) )
		end++;
	    if ( start <= cursor->index() && end > cursor->index() ) {
		if ( doc->invertSelectionText( i ) )
		    painter.setPen( QPen( colorGroup().color( QColorGroup::HighlightedText ) ) );
		painter.fillRect( chr->x, y, cw, h, doc->selectionColor( i ) );
		fill = FALSE;
	    }
	}
    }

    if ( fill )
	painter.fillRect( chr->x, y, cw, h,
			  colorGroup().brush( QColorGroup::Base ) );

    if ( chr->c != '\t' )
	painter.drawText( chr->x, y + bl, chr->c );

    if ( visible ) {
	int x = chr->x;
	int w = 1;
	painter.fillRect( QRect( x, y, w, h ), red );
    }

    p.drawPixmap( cursor->parag()->rect().topLeft() + QPoint( chr->x, y ), *doubleBuffer,
		  QRect( chr->x, y, cw, h ) );
    p.end();
}

void KTextEdit::mousePressEvent( QMouseEvent *e )
{
    undoRedoInfo.clear();
    KTextEditCursor c = *cursor;
    mousePos = e->pos();
    mightStartDrag = FALSE;

    if ( e->button() == LeftButton ) {
	mousePressed = TRUE;
	drawCursor( FALSE );
	placeCursor( e->pos() );

#if 0 // #### implement proper drag stuff
	if ( doc->hasSelection( KTextEditDocument::Standard ) &&
	     doc->inSelection( KTextEditDocument::Standard, e->pos() ) ) {
	    mightStartDrag = TRUE;
	    drawCursor( TRUE );
	    dragStartTimer->start( QApplication::startDragTime(), TRUE );
	    dragStartPos = e->pos();
	    return;
	}
#endif
	bool redraw = FALSE;
	if ( doc->hasSelection( KTextEditDocument::Standard ) ) {
	    if ( !( e->state() & ShiftButton ) ) {
		redraw = doc->removeSelection( KTextEditDocument::Standard );
		doc->setSelectionStart( KTextEditDocument::Standard, cursor );
	    } else {
		redraw = doc->setSelectionEnd( KTextEditDocument::Standard, cursor ) || redraw;
	    }
	} else {
	    if ( !( e->state() & ShiftButton ) ) {
		doc->setSelectionStart( KTextEditDocument::Standard, cursor );
	    } else {
		doc->setSelectionStart( KTextEditDocument::Standard, &c );
		redraw = doc->setSelectionEnd( KTextEditDocument::Standard, cursor ) || redraw;
	    }
	}

	for ( int i = 1; i < doc->numSelections; ++i ) // start with 1 as we don't want to remove the Standard-Selection
	    redraw = doc->removeSelection( i ) || redraw;

	if ( !redraw ) {
	    drawCursor( TRUE );
	} else {
	    repaintChanged();
	}
    } else if ( e->button() == MidButton ) {
	paste();
    }
    updateCurrentFormat();
}

void KTextEdit::mouseMoveEvent( QMouseEvent *e )
{
    if ( mousePressed ) {
	if ( mightStartDrag ) {
	    dragStartTimer->stop();
	    if ( ( e->pos() - dragStartPos ).manhattanLength() > QApplication::startDragDistance() )
		startDrag();
	    return;
	}
	if ( !mousePressed )
	    return;

	QPoint pos( mapFromGlobal( QCursor::pos() ) );
	drawCursor( FALSE );
	KTextEditCursor oldCursor = *cursor;
	placeCursor( pos );
	if ( inDoubleClick ) {
	    KTextEditCursor cl = *cursor;
	    cl.gotoWordLeft();
	    KTextEditCursor cr = *cursor;
	    cr.gotoWordRight();

	    int diff = QABS( oldCursor.parag()->at( oldCursor.index() )->x - mousePos.x() );
	    int ldiff = QABS( cl.parag()->at( cl.index() )->x - mousePos.x() );
	    int rdiff = QABS( cr.parag()->at( cr.index() )->x - mousePos.x() );


	    if ( cursor->parag()->lineStartOfChar( cursor->index() ) !=
		 oldCursor.parag()->lineStartOfChar( oldCursor.index() ) )
		diff = 0xFFFFFF;

	    if ( rdiff < diff && rdiff < ldiff )
		*cursor = cr;
	    else if ( ldiff < diff && ldiff < rdiff )
		*cursor = cl;
	    else
		*cursor = oldCursor;

	}

	bool redraw = FALSE;
	if ( doc->hasSelection( KTextEditDocument::Standard ) ) {
	    redraw = doc->setSelectionEnd( KTextEditDocument::Standard, cursor ) || redraw;
	}

	if ( !redraw ) {
	    drawCursor( TRUE );
	} else {
	    repaintChanged();
	    drawCursor( TRUE );
	}

	mousePos = e->pos();
	oldMousePos = mousePos;
    }
}

void KTextEdit::mouseReleaseEvent( QMouseEvent * )
{
    if ( dragStartTimer->isActive() )
	dragStartTimer->stop();
    if ( mightStartDrag ) {
	selectAll( FALSE );
	mousePressed = FALSE;
    }
    if ( mousePressed ) {
	if ( !doc->selectedText( KTextEditDocument::Standard ).isEmpty() )
	    doc->copySelectedText( KTextEditDocument::Standard );
	mousePressed = FALSE;
    }
    updateCurrentFormat();
    inDoubleClick = FALSE;
}

void KTextEdit::mouseDoubleClickEvent( QMouseEvent * )
{
    KTextEditCursor c1 = *cursor;
    KTextEditCursor c2 = *cursor;
    c1.gotoWordLeft();
    c2.gotoWordRight();

    doc->setSelectionStart( KTextEditDocument::Standard, &c1 );
    doc->setSelectionEnd( KTextEditDocument::Standard, &c2 );

    *cursor = c2;

    repaintChanged();

    inDoubleClick = TRUE;
    mousePressed = TRUE;
}

void KTextEdit::dragEnterEvent( QDragEnterEvent *e )
{
    e->acceptAction();
}

void KTextEdit::dragMoveEvent( QDragMoveEvent *e )
{
    drawCursor( FALSE );
    placeCursor( e->pos(),  cursor );
    drawCursor( TRUE );
    e->acceptAction();
}

void KTextEdit::dragLeaveEvent( QDragLeaveEvent * )
{
}

void KTextEdit::dropEvent( QDropEvent *e )
{
    e->acceptAction();
    QString text;
    int i = -1;
    while ( ( i = text.find( '\r' ) ) != -1 )
	text.replace( i, 1, "" );
    if ( QTextDrag::decode( e, text ) ) {
	if ( ( e->source() == this ||
	       e->source() == this ) &&
	     e->action() == QDropEvent::Move ) {
	    removeSelectedText();
	}
	insert( text, TRUE );
    }
}

void KTextEdit::placeCursor( const QPoint &pos, KTextEditCursor *c )
{
    if ( !c )
	c = cursor;

    KTextEditParag *s = doc->firstParag();
    QRect r;
    while ( s ) {
	r = s->rect();
	r.setWidth( width() );
	if ( r.contains( pos ) )
	    break;
	s = s->next();
    }

    if ( !s )
	return;

    c->setParag( s );
    int y = s->rect().y();
    int lines = s->lines();
    KTextEditString::Char *chr = 0, *c2;
    int index;
    int i = 0;
    int cy;
    int ch;
    for ( ; i < lines; ++i ) {
	chr = s->lineStartOfLine( i, &index );
	cy = s->lineY( i );
	ch = s->lineHeight( i );
	if ( !chr )
	    return;
	if ( pos.y() >= y + cy && pos.y() <= y + cy + ch )
	    break;
    }

    c2 = chr;
    i = index;
    int x = s->rect().x(), last = index;
    int lastw = 0;
    int h = ch;
    int bl;
    int cw;
    while ( TRUE ) {
	if ( c2->lineStart )
	    h = s->lineHeightOfChar( i, &bl, &cy );
	last = i;
	cw = c2->format->width( c2->c );
	if ( pos.x() >= x + c2->x - lastw && pos.x() <= x + c2->x + cw / 2 &&
	     pos.y() >= y + cy && pos.y() <= y + cy + h )
	    break;
	lastw = cw / 2;
	i++;
	if ( i < s->length() )
	    c2 = s->at( i );
	else
	    break;
    }

    cursor->setIndex( last );
}

void KTextEdit::formatMore()
{
    if ( !lastFormatted ) {
	return;
    }

    int bottom = doc->height();
    int lastBottom = -1;
    int to = !sender() ? 2 : 20;
    bool firstVisible = FALSE;
    QRect cr( 0, 0, width(), height() );
    for ( int i = 0; ( i < to || firstVisible ) && lastFormatted; ++i ) {
	lastFormatted->format();
	if ( i == 0 )
	    firstVisible = lastFormatted->rect().intersects( cr );
	else if ( firstVisible )
	    firstVisible = lastFormatted->rect().intersects( cr );
	bottom = QMAX( bottom, lastFormatted->rect().top() +
		       lastFormatted->rect().height() );
	lastBottom = lastFormatted->rect().top() + lastFormatted->rect().height();
	lastFormatted = lastFormatted->next();
	if ( lastFormatted )
	    lastBottom = -1;
    }

    if ( bottom > height() )
	resize( width(), bottom );

    if ( lastFormatted ) {
	formatTimer->start( interval, TRUE );
    } else {
	interval = QMAX( 0, interval );
    }
}

void KTextEdit::doResize()
{
    doc->setWidth( width() - 1 );
    doc->invalidate();
    repaint( FALSE );
    lastFormatted = doc->firstParag();
    interval = 0;
}

void KTextEdit::doChangeInterval()
{
    interval = 0;
}

bool KTextEdit::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return TRUE;

    if ( ( o == this ) ) {
	if ( e->type() == QEvent::FocusIn ) {
	    blinkTimer->start( QApplication::cursorFlashTime() / 2 );
	    return TRUE;
	} else if ( e->type() == QEvent::FocusOut ) {
	    blinkTimer->stop();
	    drawCursor( FALSE );
	    return TRUE;
	}
    }


    return QWidget::eventFilter( o, e );
}

void KTextEdit::insert( const QString &text, bool checkNewLine )
{
    if ( readOnly )
	return;

    QString txt( text );
    if ( mLines == 1 )
	txt = txt.replace( QRegExp( "\n" ), " " );

    drawCursor( FALSE );
    if ( doc->hasSelection( KTextEditDocument::Standard ) ) {
	checkUndoRedoInfo( UndoRedoInfo::RemoveSelected );
	if ( !undoRedoInfo.valid() ) {
	    doc->selectionStart( KTextEditDocument::Standard, undoRedoInfo.id, undoRedoInfo.index );
	    undoRedoInfo.text = QString::null;
	}
	undoRedoInfo.text = doc->selectedText( KTextEditDocument::Standard );
	doc->removeSelectedText( KTextEditDocument::Standard, cursor );
    }
    checkUndoRedoInfo( UndoRedoInfo::Insert );
    if ( !undoRedoInfo.valid() ) {
	undoRedoInfo.id = cursor->parag()->paragId();
	undoRedoInfo.index = cursor->index();
	undoRedoInfo.text = QString::null;
    }
    lastFormatted = checkNewLine && cursor->parag()->prev() ?
		    cursor->parag()->prev() : cursor->parag();
    int idx = cursor->index();
    cursor->insert( txt, checkNewLine );
    cursor->parag()->setFormat( idx, txt.length(), currentFormat, TRUE );

    formatMore();
    repaintChanged();
    drawCursor( TRUE );
    undoRedoInfo.text += txt;

    emit textChanged();
}

void KTextEdit::undo()
{
    if ( readOnly )
	return;

    undoRedoInfo.clear();
    drawCursor( FALSE );
    KTextEditCursor *c = doc->undo( cursor );
    if ( !c ) {
	drawCursor( TRUE );
	return;
    }
    repaintChanged();
    drawCursor( TRUE );
    emit textChanged();
}

void KTextEdit::redo()
{
    if ( readOnly )
	return;

    undoRedoInfo.clear();
    drawCursor( FALSE );
    KTextEditCursor *c = doc->redo( cursor );
    if ( !c ) {
	drawCursor( TRUE );
	return;
    }
    repaintChanged();
    drawCursor( TRUE );
    emit textChanged();
}

void KTextEdit::paste()
{
    if ( readOnly )
	return;

    QString s = QApplication::clipboard()->text();
    if ( !s.isEmpty() )
	insert( s, TRUE );
}

void KTextEdit::checkUndoRedoInfo( UndoRedoInfo::Type t )
{
    if ( undoRedoInfo.valid() && t != undoRedoInfo.type )
	undoRedoInfo.clear();
    undoRedoInfo.type = t;
}

void KTextEdit::repaintChanged()
{
    drawAll = FALSE;
    repaint( FALSE );
    drawAll = TRUE;
}

void KTextEdit::cut()
{
    if ( readOnly )
	return;

    if ( doc->hasSelection( KTextEditDocument::Standard ) ) {
	doc->copySelectedText( KTextEditDocument::Standard );
	removeSelectedText();
    }
}

void KTextEdit::copy()
{
    if ( !doc->selectedText( KTextEditDocument::Standard ).isEmpty() )
	doc->copySelectedText( KTextEditDocument::Standard );
}

void KTextEdit::indent()
{
    if ( readOnly )
	return;

    drawCursor( FALSE );
    if ( !doc->hasSelection( KTextEditDocument::Standard ) )
	cursor->indent();
    else
	doc->indentSelection( KTextEditDocument::Standard );
    repaintChanged();
    drawCursor( TRUE );
    emit textChanged();
}

bool KTextEdit::focusNextPrevChild( bool )
{
    return FALSE;
}

void KTextEdit::zoom( float f )
{
    doc->zoom( f );
}

void KTextEdit::unzoom()
{
    doc->unzoom();
}

void KTextEdit::setFormat( KTextEditFormat *f, int flags )
{
    if ( readOnly )
	return;

    if ( doc->hasSelection( KTextEditDocument::Standard ) ) {
	drawCursor( FALSE );
	doc->setFormat( KTextEditDocument::Standard, f, flags );
	repaintChanged();
	formatMore();
	drawCursor( TRUE );
	emit textChanged();
    }
    if ( currentFormat && currentFormat->key() != f->key() ) {
	currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( f );
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
	if ( cursor->index() == cursor->parag()->length() < 1 ) {
	    currentFormat->addRef();
	    cursor->parag()->string()->setFormat( cursor->index(), currentFormat, TRUE );
	}
    }
}

void KTextEdit::setParagType( ParagType t )
{
    if ( readOnly )
	return;

    KTextEditParag::Type type = (KTextEditParag::Type)t;
    drawCursor( FALSE );
    if ( !doc->hasSelection( KTextEditDocument::Standard ) ) {
	cursor->parag()->setType( type );
	cursor->parag()->setListDepth( cursor->parag()->listDepth() );
	repaintChanged();
    } else {
	KTextEditParag *start = doc->selectionStart( KTextEditDocument::Standard );
	KTextEditParag *end = doc->selectionEnd( KTextEditDocument::Standard );
	lastFormatted = start;
	while ( start ) {
	    start->setType( type );
	    start->setListDepth( cursor->parag()->listDepth() );
	    if ( start == end )
		break;
	    start = start->next();
	}
	repaintChanged();
	formatMore();
    }
    drawCursor( TRUE );
    if ( currentParagType != t ) {
	currentParagType = t;
	emit currentParagTypeChanged( currentParagType );
    }
    emit textChanged();
}

void KTextEdit::setListDepth( int d )
{
    if ( readOnly )
	return;

    drawCursor( FALSE );
    if ( !doc->hasSelection( KTextEditDocument::Standard ) ) {
	cursor->parag()->setListDepth( QMAX( -1, cursor->parag()->listDepth() + d ) );
	repaintChanged();
    } else {
	KTextEditParag *start = doc->selectionStart( KTextEditDocument::Standard );
	KTextEditParag *end = doc->selectionEnd( KTextEditDocument::Standard );
	lastFormatted = start;
	while ( start ) {
	    start->setListDepth( QMAX( cursor->parag()->listDepth() + d, -1 ) );
	    if ( start == end )
		break;
	    start = start->next();
	}
	repaintChanged();
	formatMore();
    }
    drawCursor( TRUE );
    emit textChanged();
}

void KTextEdit::setAlignment( int a )
{
    if ( readOnly )
	return;

    drawCursor( FALSE );
    if ( !doc->hasSelection( KTextEditDocument::Standard ) ) {
	cursor->parag()->setAlignment( a );
	repaintChanged();
    } else {
	KTextEditParag *start = doc->selectionStart( KTextEditDocument::Standard );
	KTextEditParag *end = doc->selectionEnd( KTextEditDocument::Standard );
	lastFormatted = start;
	while ( start ) {
	    start->setAlignment( a );
	    if ( start == end )
		break;
	    start = start->next();
	}
	repaintChanged();
	formatMore();
    }
    drawCursor( TRUE );
    if ( currentAlignment != a ) {
	currentAlignment = a;
	emit currentAlignmentChanged( currentAlignment );
    }
    emit textChanged();
}

void KTextEdit::updateCurrentFormat()
{
    int i = cursor->index();
    if ( i > 0 )
	--i;
    if ( currentFormat->key() != cursor->parag()->at( i )->format->key() ) {
	if ( currentFormat )
	    currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( cursor->parag()->at( i )->format );
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
    }

    if ( currentAlignment != cursor->parag()->alignment() ) {
	currentAlignment = cursor->parag()->alignment();
	emit currentAlignmentChanged( currentAlignment );
    }

    if ( currentParagType != (ParagType)cursor->parag()->type() ) {
	currentParagType = (ParagType)cursor->parag()->type();
	emit currentParagTypeChanged( currentParagType );
    }
}

void KTextEdit::setItalic( bool b )
{
    KTextEditFormat f( *currentFormat );
    f.setItalic( b );
    setFormat( &f, KTextEditFormat::Italic );
}

void KTextEdit::setBold( bool b )
{
    KTextEditFormat f( *currentFormat );
    f.setBold( b );
    setFormat( &f, KTextEditFormat::Bold );
}

void KTextEdit::setUnderline( bool b )
{
    KTextEditFormat f( *currentFormat );
    f.setUnderline( b );
    setFormat( &f, KTextEditFormat::Underline );
}

void KTextEdit::setFamily( const QString &f_ )
{
    KTextEditFormat f( *currentFormat );
    f.setFamily( f_ );
    setFormat( &f, KTextEditFormat::Family );
}

void KTextEdit::setPointSize( int s )
{
    KTextEditFormat f( *currentFormat );
    f.setPointSize( s );
    setFormat( &f, KTextEditFormat::Size );
}

void KTextEdit::setColor( const QColor &c )
{
    KTextEditFormat f( *currentFormat );
    f.setColor( c );
    setFormat( &f, KTextEditFormat::Color );
}

void KTextEdit::setFont( const QFont &f_ )
{
    if ( !isVisible() )
        return;

    KTextEditFormat f( *currentFormat );
    f.setFont( f_ );
    setFormat( &f, KTextEditFormat::Font );
}

QString KTextEdit::text() const
{
    return doc->text();
}

void KTextEdit::setText( const QString &txt )
{
    doc->setText( txt );
    cursor->setParag( doc->firstParag() );
    cursor->setIndex( 0 );
    repaint( FALSE );
    emit textChanged();
}

bool KTextEdit::find( const QString &expr, bool cs, bool wo, bool forward,
		      int *parag, int *index )
{
    drawCursor( FALSE );
    doc->removeSelection( KTextEditDocument::Search );
    bool found = doc->find( expr, cs, wo, forward, parag, index, cursor );
    drawCursor( TRUE );
    repaintChanged();
    return found;
}

void KTextEdit::blinkCursor()
{
    if ( !cursorVisible )
	return;
    bool cv = cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor( blinkCursorVisible );
    cursorVisible = cv;
}

void KTextEdit::setCursorPosition( int parag, int index )
{
    KTextEditParag *p = doc->paragAt( parag );
    if ( !p )
	return;

    if ( index > p->length() - 1 )
	index = p->length() - 1;

    drawCursor( FALSE );
    cursor->setParag( p );
    cursor->setIndex( index );
    drawCursor( TRUE );
}

void KTextEdit::cursorPosition( int &parag, int &index )
{
    parag = cursor->parag()->paragId();
    index = cursor->index();
}

void KTextEdit::setSelection( int parag_from, int index_from,
			      int parag_to, int index_to )
{
    KTextEditParag *p1 = doc->paragAt( parag_from );
    if ( !p1 )
	return;
    KTextEditParag *p2 = doc->paragAt( parag_to );
    if ( !p2 )
	return;

    if ( index_from > p1->length() - 1 )
	index_from = p1->length() - 1;
    if ( index_to > p2->length() - 1 )
	index_to = p2->length() - 1;

    drawCursor( FALSE );
    KTextEditCursor c = *cursor;
    c.setParag( p1 );
    c.setIndex( index_from );
    cursor->setParag( p2 );
    cursor->setIndex( index_to );
    doc->setSelectionStart( KTextEditDocument::Standard, &c );
    doc->setSelectionEnd( KTextEditDocument::Standard, cursor );
    repaintChanged();
    drawCursor( TRUE );
}

void KTextEdit::selection( int &parag_from, int &index_from,
			   int &parag_to, int &index_to )
{
    if ( !doc->hasSelection( KTextEditDocument::Standard ) ) {
	parag_from = -1;
	index_from = -1;
	parag_to = -1;
	index_to = -1;
	return;
    }

    doc->selectionStart( KTextEditDocument::Standard, parag_from, index_from );
    doc->selectionEnd( KTextEditDocument::Standard, parag_from, index_from );
}

int KTextEdit::paragraphs() const
{
    return doc->lastParag()->paragId() + 1;
}

int KTextEdit::linesOfParagraph( int parag ) const
{
    KTextEditParag *p = doc->paragAt( parag );
    if ( !p )
	return -1;
    return p->lines();
}

int KTextEdit::lines() const
{
    KTextEditParag *p = doc->firstParag();
    int l = 0;
    while ( p ) {
	l += p->lines();
	p = p->next();
    }

    return l;
}

int KTextEdit::lineOfChar( int parag, int chr )
{
    KTextEditParag *p = doc->paragAt( parag );
    if ( !p )
	return -1;

    int idx, line;
    KTextEditString::Char *c = p->lineStartOfChar( chr, &idx, &line );
    if ( !c )
	return -1;

    return line;
}

void KTextEdit::setReadOnly( bool ro )
{
    if ( ro == readOnly )
	return;
    readOnly = ro;
}

void KTextEdit::setModified( bool m )
{
    modified = m;
    if ( modified ) {
	disconnect( this, SIGNAL( textChanged() ),
		    this, SLOT( setModified() ) );
    } else {
	connect( this, SIGNAL( textChanged() ),
		 this, SLOT( setModified() ) );
    }
}

bool KTextEdit::isModified() const
{
    return modified;
}

void KTextEdit::setModified()
{
    setModified( TRUE );
}

bool KTextEdit::italic() const
{
    return currentFormat->font().italic();
}

bool KTextEdit::bold() const
{
    return currentFormat->font().bold();
}

bool KTextEdit::underline() const
{
    return currentFormat->font().underline();
}

QString KTextEdit::family() const
{
    return currentFormat->font().family();
}

int KTextEdit::pointSize() const
{
    return currentFormat->font().pointSize();
}

QColor KTextEdit::color() const
{
    return currentFormat->color();
}

QFont KTextEdit::font() const
{
    return currentFormat->font();
}

KTextEdit::ParagType KTextEdit::paragType() const
{
    return currentParagType;
}

int KTextEdit::alignment() const
{
    return currentAlignment;
}

void KTextEdit::startDrag()
{
    mousePressed = FALSE;
    inDoubleClick = FALSE;
    QDragObject *drag = new QTextDrag( doc->selectedText( KTextEditDocument::Standard ), this );
    if ( readOnly ) {
	drag->dragCopy();
    } else {
	if ( drag->drag() && QDragObject::target() != this ) {
	    doc->removeSelectedText( KTextEditDocument::Standard, cursor );
	    repaintChanged();
	}
    }
}

void KTextEdit::selectAll( bool select )
{
    // ############## Implement that!!!
    if ( !select ) {
	doc->removeSelection( KTextEditDocument::Standard );
	repaintChanged();
    }
}

void KTextEdit::clear()
{
    doc->clear();
}

void KTextEdit::UndoRedoInfo::clear()
{
    if ( valid() ) {
	if ( type == Insert || type == Return )
	    doc->addCommand( new KTextEditInsertCommand( doc, id, index, text ) );
	else if ( type != Invalid )
	    doc->addCommand( new KTextEditDeleteCommand( doc, id, index, text ) );
    }
    text = QString::null;
    id = -1;
    index = -1;
}

void KTextEdit::setMaxLines( int l )
{
    mLines = l;
}

int KTextEdit::maxLines() const
{
    return mLines;
}

void KTextEdit::resetFormat()
{
    setAlignment( Qt::AlignLeft );
    setParagType( KTextEdit::Normal );
    setFormat( doc->formatCollection()->defaultFormat(), KTextEditFormat::Format );
}

bool KTextEdit::event( QEvent * e )
{
    if ( e->type() == QEvent::AccelOverride ) {
        QKeyEvent* ke = (QKeyEvent*) e;
        switch ( ke->key() ) {
        case Key_Home:
        case Key_End:
        case Key_Prior:
        case Key_Next:
          ke->accept();
        default:
          break;
        }
    }
    return QWidget::event( e );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void KTextEditCommandHistory::addCommand( KTextEditCommand *cmd )
{
    if ( current < (int)history.count() - 1 ) {
	QList<KTextEditCommand> commands;
	commands.setAutoDelete( FALSE );

	for( int i = 0; i <= current; ++i ) {
	    commands.insert( i, history.at( 0 ) );
	    history.take( 0 );
	}

	commands.append( cmd );
	history.clear();
	history = commands;
	history.setAutoDelete( TRUE );
    } else {
	history.append( cmd );
    }

    if ( (int)history.count() > steps )
	history.removeFirst();
    else
	++current;

    // add a command to the global cmdhist.
    QString cmdAction;
    if(cmd->type()==KTextEditCommand::Invalid) {
        cmdAction="error, not supposed to happen";
    }
    else if(cmd->type()==KTextEditCommand::Insert) {
        cmdAction=i18n("Inserting Text");
    }
    else if(cmd->type()==KTextEditCommand::Delete) {
        kdDebug() << "KTextEditCommandHistory::addCommand -- delete text" << endl;
        cmdAction=i18n("Deleting Text");
    }
    else {
        kdDebug() << "KTextEditCommandHistory::addCommand -- formatting" << endl;
        cmdAction=i18n("Formatting Text");
    }
    document->commands()->addCommand(new TextCmd(cmdAction, document, kptextobject));
    // mark as modified
    document->setModified(true);
}

KTextEditCursor *KTextEditCommandHistory::undo( KTextEditCursor *c )
{
    if ( current > -1 ) {
	KTextEditCursor *c2 = history.at( current )->unexecute( c );
	--current;
	return c2;
    }
    return 0;
}

KTextEditCursor *KTextEditCommandHistory::redo( KTextEditCursor *c )
{
    if ( current > -1 ) {
	if ( current < (int)history.count() - 1 ) {
	    ++current;
	    return history.at( current )->execute( c );
	}
    } else {
	if ( history.count() > 0 ) {
	    ++current;
	    return history.at( current )->execute( c );
	}
    }
    return 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KTextEditCursor *KTextEditDeleteCommand::execute( KTextEditCursor *c )
{
    KTextEditParag *s = doc->paragAt( id );
    if ( !s ) {
	qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
	return 0;
    }

    cursor.setParag( s );
    cursor.setIndex( index );
    int len = text.length();
    doc->setSelectionStart( KTextEditDocument::Temp, &cursor );
    for ( int i = 0; i < len; ++i )
	cursor.gotoRight();
    doc->setSelectionEnd( KTextEditDocument::Temp, &cursor );
    doc->removeSelectedText( KTextEditDocument::Temp, &cursor );

    if ( c ) {
	c->setParag( s );
	c->setIndex( index );
    }

    return c;
}

KTextEditCursor *KTextEditDeleteCommand::unexecute( KTextEditCursor *c )
{
    KTextEditParag *s = doc->paragAt( id );
    if ( !s ) {
	qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
	return 0;
    }

    cursor.setParag( s );
    cursor.setIndex( index );
    cursor.insert( text, TRUE );
    cursor.setParag( s );
    cursor.setIndex( index );
    if ( c ) {
	c->setParag( s );
	c->setIndex( index );
	for ( int i = 0; i < (int)text.length(); ++i )
	    c->gotoRight();
    }

    s = cursor.parag();
    while ( s ) {
	s->format();
	s->setChanged( TRUE );
	if ( s == c->parag() )
	    break;
	s = s->next();
    }

    return &cursor;
}

KTextEditFormatCommand::KTextEditFormatCommand( KTextEditDocument *d, int selId, KTextEditFormat *f, int flgs )
    : KTextEditCommand( d ), selection( selId ),  flags( flgs )
{
    format = d->formatCollection()->format( f );
}

KTextEditFormatCommand::~KTextEditFormatCommand()
{
    format->removeRef();
}

KTextEditCursor *KTextEditFormatCommand::execute( KTextEditCursor *c )
{
    doc->setFormat( selection, format, flags );
    return c;
}

KTextEditCursor *KTextEditFormatCommand::unexecute( KTextEditCursor *c )
{
    return c;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KTextEditCursor::KTextEditCursor( KTextEditDocument *d )
    : doc( d )
{
    idx = 0;
    string = doc->firstParag();
    tmpIndex = -1;
}

void KTextEditCursor::insert( const QString &s, bool checkNewLine )
{
    tmpIndex = -1;
    bool justInsert = TRUE;
    if ( checkNewLine )
	justInsert = s.find( '\n' ) == -1;
    if ( justInsert ) {
	string->insert( idx, s );
	idx += s.length();
    } else {
	QStringList lst = QStringList::split( '\n', s, TRUE );
	QStringList::Iterator it = lst.begin();
	int y = string->rect().y() + string->rect().height();
	for ( ; it != lst.end(); ++it ) {
	    if ( it != lst.begin() ) {
		splitAndInsertEmtyParag( FALSE, FALSE );
		string->prev()->format( -1, FALSE );
	    }
	    QString s = *it;
	    if ( s.isEmpty() )
		continue;
	    string->insert( idx, s );
	    idx += s.length();
	}
	string->format( -1, FALSE );
	int dy = string->rect().y() + string->rect().height() - y;
	KTextEditParag *p = string->next();
	while ( p ) {
	    p->setParagId( p->prev()->paragId() + 1 );
	    p->move( dy );
	    p->invalidate( 0 );
	    p = p->next();
	}
    }
}

void KTextEditCursor::gotoLeft()
{
    tmpIndex = -1;
    if ( idx > 0 ) {
	idx--;
    } else if ( string->prev() ) {
	string = string->prev();
	idx = string->length() - 1;
    }
}

void KTextEditCursor::gotoRight()
{
    tmpIndex = -1;
    if ( idx < string->length() - 1 ) {
	idx++;
    } else if ( string->next() ) {
	string = string->next();
	idx = 0;
    }
}

void KTextEditCursor::gotoUp()
{
    int indexOfLineStart;
    int line;
    KTextEditString::Char *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( indexOfLineStart == 0 ) {
	if ( !string->prev() )
	    return;
	string = string->prev();
	int lastLine = string->lines() - 1;
	if ( !string->lineStartOfLine( lastLine, &indexOfLineStart ) )
	    return;
	if ( indexOfLineStart + tmpIndex < string->length() )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = string->length() - 1;
    } else {
	--line;
	int oldIndexOfLineStart = indexOfLineStart;
	if ( !string->lineStartOfLine( line, &indexOfLineStart ) )
	    return;
	if ( indexOfLineStart + tmpIndex < oldIndexOfLineStart )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = oldIndexOfLineStart - 1;
    }
}

void KTextEditCursor::gotoDown()
{
    int indexOfLineStart;
    int line;
    KTextEditString::Char *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( line == string->lines() - 1 ) {
	if ( !string->next() )
	    return;
	string = string->next();
	if ( !string->lineStartOfLine( 0, &indexOfLineStart ) )
	    return;
	int end;
	if ( string->lines() == 1 )
	    end = string->length();
	else
	    string->lineStartOfLine( 1, &end );
	if ( indexOfLineStart + tmpIndex < end )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = end - 1;
    } else {
	++line;
	int end;
	if ( line == string->lines() - 1 )
	    end = string->length();
	else
	    string->lineStartOfLine( line + 1, &end );
	if ( !string->lineStartOfLine( line, &indexOfLineStart ) )
	    return;
	if ( indexOfLineStart + tmpIndex < end )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = end - 1;
    }
}

void KTextEditCursor::gotoLineEnd()
{
    int indexOfLineStart;
    int line;
    KTextEditString::Char *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    if ( line == string->lines() - 1 ) {
	idx = string->length() - 1;
    } else {
	c = string->lineStartOfLine( ++line, &indexOfLineStart );
	indexOfLineStart--;
	idx = indexOfLineStart;
    }
}

void KTextEditCursor::gotoLineStart()
{
    int indexOfLineStart;
    int line;
    KTextEditString::Char *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    idx = indexOfLineStart;
}

void KTextEditCursor::gotoHome()
{
    tmpIndex = -1;
    string = doc->firstParag();
    idx = 0;
}

void KTextEditCursor::gotoEnd()
{
    if ( !doc->lastParag()->isValid() )
	return;

    tmpIndex = -1;
    string = doc->lastParag();
    idx = string->length() - 1;
}

void KTextEditCursor::gotoPageUp( KTextEdit *view )
{
    tmpIndex = -1;
    KTextEditParag *s = string;
    int h = view->height();
    int y = s->rect().y();
    while ( s ) {
	if ( y - s->rect().y() >= h )
	    break;
	s = s->prev();
    }

    if ( !s )
	s = doc->firstParag();

    string = s;
    idx = 0;
}

void KTextEditCursor::gotoPageDown( KTextEdit *view )
{
    tmpIndex = -1;
    KTextEditParag *s = string;
    int h = view->height();
    int y = s->rect().y();
    while ( s ) {
	if ( s->rect().y() - y >= h )
	    break;
	s = s->next();
    }

    if ( !s )
	s = doc->lastParag();

    if ( !s->isValid() )
	return;

    string = s;
    idx = 0;
}

void KTextEditCursor::gotoWordLeft()
{
    gotoLeft();
    tmpIndex = -1;
    KTextEditString *s = string->string();
    bool allowSame = FALSE;
    for ( int i = idx - 1; i >= 0; --i ) {
	if ( s->at( i ).c.isSpace() || s->at( i ).c == '\t' ) {
	    if ( !allowSame && s->at( i ).c == s->at( idx ).c )
		continue;
	    idx = i + 1;
	    return;
	}
	if ( !allowSame && s->at( i ).c != s->at( idx ).c )
	    allowSame = TRUE;
    }

    if ( string->prev() ) {
	string = string->prev();
	idx = string->length() - 1;
    } else {
	gotoLineStart();
    }
}

void KTextEditCursor::gotoWordRight()
{
    tmpIndex = -1;
    KTextEditString *s = string->string();
    bool allowSame = FALSE;
    for ( int i = idx + 1; i < (int)s->length(); ++i ) {
	if ( s->at( i ).c.isSpace() || s->at( i ).c == '\t' ) {
	    if ( !allowSame &&	s->at( i ).c == s->at( idx ).c )
		continue;
	    idx = i;
	    return;
	}
	if ( !allowSame && s->at( i ).c != s->at( idx ).c )
	    allowSame = TRUE;
    }

    if ( string->next() ) {
	string = string->next();
	idx = 0;
    } else {
	gotoLineEnd();
    }
}

bool KTextEditCursor::atParagStart()
{
    return idx == 0;
}

bool KTextEditCursor::atParagEnd()
{
    return idx == string->length() - 1;
}

void KTextEditCursor::splitAndInsertEmtyParag( bool ind, bool updateIds )
{
    tmpIndex = -1;
    KTextEditFormat *f = 0;
    f = string->at( idx )->format;
    if ( idx == string->length() - 1 && idx > 0 )
	f = string->at( idx - 1 )->format;


    if ( atParagStart() ) {
	KTextEditParag *p = string->prev();
	KTextEditParag *s = new KTextEditParag( doc, p, string, updateIds );
	s->append( " " );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->setType( string->type() );
	s->setListDepth( string->listDepth() );
	s->setAlignment( string->alignment() );
	if ( ind ) {
	    s->indent();
	    s->format();
	    indent();
	    string->format();
	}
    } else if ( atParagEnd() ) {
	KTextEditParag *n = string->next();
	KTextEditParag *s = new KTextEditParag( doc, string, n, updateIds );
	s->append( " " );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->setType( string->type() );
	s->setListDepth( string->listDepth() );
	s->setAlignment( string->alignment() );
	if ( ind ) {
	    int oi, ni;
	    s->indent( &oi, &ni );
	    string = s;
	    idx = ni;
	} else {
	    string = s;
	    idx = 0;
	}
    } else {
	QString str = string->string()->toString().mid( idx, 0xFFFFFF );
	KTextEditParag *n = string->next();
	KTextEditParag *s = new KTextEditParag( doc, string, n, updateIds );
	s->setType( string->type() );
	s->setListDepth( string->listDepth() );
	s->setAlignment( string->alignment() );
	s->append( str );
	for ( uint i = 0; i < str.length(); ++i )
	    s->setFormat( i, 1, string->at( idx + i )->format, TRUE );
	string->truncate( idx );
	if ( ind ) {
	    int oi, ni;
	    s->indent( &oi, &ni );
	    string = s;
	    idx = ni;
	} else {
	    string = s;
	    idx = 0;
	}
    }
}

bool KTextEditCursor::remove()
{
    tmpIndex = -1;
    if ( !atParagEnd() ) {
	string->remove( idx, 1 );
	return FALSE;
    } else if ( string->next() ) {
	string->join( string->next() );
	return TRUE;
    }
    return FALSE;
}

void KTextEditCursor::indent()
{
    int oi = 0, ni = 0;
    string->indent( &oi, &ni );
    if ( oi == ni )
	return;

    if ( idx >= oi )
	idx += ni - oi;
    else
	idx = ni;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const int KTextEditDocument::numSelections = 2; // Don't count the Temp one!

KTextEditDocument::KTextEditDocument( KPresenterDoc *doc, KPTextObject *txtobj )
{
    kpr_doc = doc;
    listMult = 15;
    allInOne = FALSE;
    marg = 0;
    pFormatter = 0;
    fParag = 0;
    ls = ps = 0;

    lParag = fParag = new KTextEditParag( this, 0, 0 );
    lParag->append( " " );

    cx = 0;
    cy = 0;
    cw = 600;

    selectionColors[ Standard ] = QApplication::palette().color( QPalette::Normal, QColorGroup::Highlight );
    selectionColors[ Search ] = Qt::yellow;
    selectionText[ Standard ] = TRUE;
    selectionText[ Search ] = FALSE;
    commandHistory = new KTextEditCommandHistory( 100, doc, txtobj ); // ### max undo/redo steps should be configurable
}

void KTextEditDocument::clear()
{
    if ( fParag ) {
	KTextEditParag *p = 0;
	while ( fParag ) {
	    p = fParag->next();
	    delete fParag;
	    fParag = p;
	}
	fParag = 0;
    }

    ls = ps = 0;
    lParag = fParag = new KTextEditParag( this, 0, 0 );
    lParag->append( " " );
}

void KTextEditDocument::setTextSettings( TextSettings s )
{
    txtSettings = s;
    setLineSpacing( s.lineSpacing );
    setParagSpacing( s.paragSpacing );
    setMargin( s.margin );
    invalidate();
}

void KTextEditDocument::setText( const QString &text )
{
    if ( fParag ) {
	KTextEditParag *p = 0;
	while ( fParag ) {
	    p = fParag->next();
	    delete fParag;
	    fParag = p;
	}
	fParag = 0;
    }

    ls = ps = 0;

    QString s;
    lParag = 0;
    QStringList lst = QStringList::split( '\n', text, TRUE );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	lParag = new KTextEditParag( this, lParag, 0 );
	if ( !fParag )
	    fParag = lParag;
	s = *it;
	if ( !s.isEmpty() ) {
	    if ( s.right( 1 ) != " " )
		s += " ";
	    lParag->append( s );
	} else {
	    lParag->append( " " );
	}
    }

    if ( !lParag ) {
	lParag = fParag = new KTextEditParag( this, 0, 0 );
	lParag->append( " " );
    }
}

QString KTextEditDocument::text( KTextEditParag *p ) const
{
    if ( !p ) {
	QString buffer;
	QString s;
	KTextEditParag *p = fParag;
	while ( p ) {
	    s = p->string()->toString();
	    s += "\n";
	    buffer += s;
	    p = p->next();
	}
	return buffer;
    } else {
	return p->string()->toString();
    }
}

void KTextEditDocument::invalidate()
{
    KTextEditParag *s = fParag;
    while ( s ) {
	s->invalidate( 0 );
	s = s->next();
    }
}

void KTextEditDocument::selectionStart( int id, int &paragId, int &index )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;
    Selection &sel = *it;
    paragId = QMIN( sel.startParag->paragId(), sel.endParag->paragId() );
    index = sel.startIndex;
}

void KTextEditDocument::selectionEnd( int id, int &paragId, int &index )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;
    Selection &sel = *it;
    paragId = QMAX( sel.startParag->paragId(), sel.endParag->paragId() );
    if ( paragId == sel.startParag->paragId() )
	index = sel.startParag->selectionEnd( id );
    else
	index = sel.endParag->selectionEnd( id );
}

KTextEditParag *KTextEditDocument::selectionStart( int id )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return 0;
    Selection &sel = *it;
    if ( sel.startParag->paragId() <  sel.endParag->paragId() )
	return sel.startParag;
    return sel.endParag;
}

KTextEditParag *KTextEditDocument::selectionEnd( int id )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return 0;
    Selection &sel = *it;
    if ( sel.startParag->paragId() >  sel.endParag->paragId() )
	return sel.startParag;
    return sel.endParag;
}

bool KTextEditDocument::setSelectionEnd( int id, KTextEditCursor *cursor )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return FALSE;

    Selection &sel = *it;
    KTextEditParag *oldEndParag = sel.endParag;
    KTextEditParag *oldStartParag = sel.startParag;
    if ( sel.endParag->paragId() < sel.startParag->paragId() ) {
	oldStartParag = sel.endParag;
	oldEndParag = sel.startParag;
    }
    sel.endParag = cursor->parag();
    int start = sel.startIndex;
    int end = cursor->index();
    bool swapped = FALSE;
    if ( sel.endParag->paragId() < sel.startParag->paragId() ) {
	sel.endParag = sel.startParag;
	sel.startParag = cursor->parag();
	end = sel.startIndex;
	start = cursor->index();
	swapped = TRUE;
    }

    if ( sel.startParag == sel.endParag ) {
	if ( end < start) {
	    end = sel.startIndex;
	    start = cursor->index();
	}
	sel.endParag->setSelection( id, start, end );

	KTextEditParag *p = 0;
	if ( sel.endParag->paragId() < oldEndParag->paragId() ) {
	    p = sel.endParag;
	    p = p->next();
	    while ( p ) {
		p->removeSelection( id );
		if ( p == oldEndParag )
		    break;
		p = p->next();
	    }
	}

	if ( sel.startParag->paragId() > oldStartParag->paragId() ) {
	    p = sel.startParag;
	    p = p->prev();
	    while ( p ) {
		p->removeSelection( id );
		if ( p == oldStartParag )
		    break;
		p = p->prev();
	    }
	}
    } else {
	KTextEditParag *p = sel.startParag;
	p->setSelection( id, start, p->length() - 1 );
	p->setChanged( TRUE );
	p = p->next();
	if ( p )
	    p->setChanged( TRUE );
	while ( p && p != sel.endParag ) {
	    p->setSelection( id, 0, p->length() - 1 );
	    p->setChanged( TRUE );
	    p = p->next();
	}
	sel.endParag->setSelection( id, 0, end );
	sel.endParag->setChanged( TRUE );

	if ( sel.endParag->paragId() < oldEndParag->paragId() ) {
	    p = sel.endParag;
	    p = p->next();
	    while ( p ) {
		p->removeSelection( id );
		if ( p == oldEndParag )
		    break;
		p = p->next();
	    }
	}

	if ( sel.startParag->paragId() > oldStartParag->paragId() ) {
	    p = sel.startParag;
	    p = p->prev();
	    while ( p ) {
		p->removeSelection( id );
		if ( p == oldStartParag )
		    break;
		p = p->prev();
	    }
	}

	if ( swapped ) {
	    p = sel.startParag;
	    sel.startParag = sel.endParag;
	    sel.endParag = p;
	}
    }

    return TRUE;
}

bool KTextEditDocument::removeSelection( int id )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return FALSE;

    KTextEditParag *start = ( *it ).startParag;
    KTextEditParag *end = ( *it ).endParag;
    if ( end->paragId() < start->paragId() ) {
	end = ( *it ).startParag;
	start = ( *it ).endParag;
    }

    KTextEditParag *p = start;
    while ( p ) {
	p->removeSelection( id );
	if ( p == end )
	    break;
	p = p->next();
    }

    selections.remove( id );
    return TRUE;
}

QString KTextEditDocument::selectedText( int id ) const
{
    // ######## TODO: look at textFormat() and return rich text or plain text (like the text() method!)
    QMap<int, Selection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
	return QString::null;

    Selection sel = *it;

    KTextEditParag *endParag = sel.endParag;
    KTextEditParag *startParag = sel.startParag;
    if ( sel.endParag->paragId() < sel.startParag->paragId() ) {
	startParag = sel.endParag;
	endParag = sel.startParag;
    }

    QString buffer;
    QString s;
    KTextEditParag *p = startParag;
    while ( p ) {
	s = p->string()->toString().mid( p->selectionStart( id ),
					 p->selectionEnd( id ) - p->selectionStart( id ) );
	if ( p->selectionEnd( id ) == p->length() - 1 && p != endParag )
	    s += "\n";
	buffer += s;
	if ( p == endParag )
	    break;
	p = p->next();
    }

    return buffer;
}

void KTextEditDocument::setFormat( int id, KTextEditFormat *f, int flags )
{
    QMap<int, Selection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    Selection sel = *it;

    KTextEditParag *endParag = sel.endParag;
    KTextEditParag *startParag = sel.startParag;
    if ( sel.endParag->paragId() < sel.startParag->paragId() ) {
	startParag = sel.endParag;
	endParag = sel.startParag;
    }

    KTextEditParag *p = startParag;
    while ( p ) {
	int end = p->selectionEnd( id );
	if ( end == p->length() - 1 )
	    end++;
	p->setFormat( p->selectionStart( id ), end - p->selectionStart( id ),
		      f, TRUE, flags );
	if ( p == endParag )
	    break;
	p = p->next();
    }
}

void KTextEditDocument::copySelectedText( int id )
{
    if ( !hasSelection( id ) )
	return;

    QApplication::clipboard()->setText( selectedText( id ) );
}

void KTextEditDocument::removeSelectedText( int id, KTextEditCursor *cursor )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    Selection sel = *it;
    KTextEditParag *startParag = sel.startParag;
    KTextEditParag *endParag = sel.endParag;
    if ( sel.endParag->paragId() < sel.startParag->paragId() ) {
	endParag = sel.startParag;
	startParag = sel.endParag;
    }

    if ( startParag == endParag ) {
	int idx = -1;
	if ( cursor->parag() == startParag &&
	     cursor->index() > startParag->selectionStart( id ) )
	    idx = startParag->selectionStart( id );
	startParag->remove( startParag->selectionStart( id ),
			    startParag->selectionEnd( id ) - startParag->selectionStart( id ) );
	if ( idx != -1 )
	    cursor->setIndex( idx );
    } else {
	int idx = -1;
	KTextEditParag *cp = 0;

	if ( cursor->parag() == startParag &&
	     cursor->index() > startParag->selectionStart( id ) )
	    idx = startParag->selectionStart( id );
	else if ( cursor->parag()->paragId() > startParag->paragId() &&
		  cursor->parag()->paragId() <= endParag->paragId() ) {
	    cp = startParag;
	    idx = startParag->selectionStart( id );
	}

	startParag->remove( startParag->selectionStart( id ),
			    startParag->selectionEnd( id ) - startParag->selectionStart( id ) );
	endParag->remove( 0, endParag->selectionEnd( id ) );
	KTextEditParag *p = startParag, *tmp;
	p = p->next();
	int dy = 0;
	while ( p ) {
	    if ( p == endParag )
		break;
	    tmp = p->next();
	    dy += p->rect().height();
	    delete p;
	    p = tmp;
	}

	while ( p ) {
	    p->move( -dy );
	    p->invalidate( 0 );
	    p = p->next();
	}

	startParag->join( endParag );

	if ( cp )
	    cursor->setParag( cp );
	if ( idx != -1 )
	    cursor->setIndex( idx );
    }

    removeSelection( id );
}

void KTextEditDocument::indentSelection( int id )
{
    QMap<int, Selection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    Selection sel = *it;
    KTextEditParag *startParag = sel.startParag;
    KTextEditParag *endParag = sel.endParag;
    if ( sel.endParag->paragId() < sel.startParag->paragId() ) {
	endParag = sel.startParag;
	startParag = sel.endParag;
    }

    KTextEditParag *p = startParag;
    while ( p && p != endParag ) {
	p->indent();
	p = p->next();
    }
}

void KTextEditDocument::addCommand( KTextEditCommand *cmd )
{
    commandHistory->addCommand( cmd );
}

KTextEditCursor *KTextEditDocument::undo( KTextEditCursor *c )
{
    return commandHistory->undo( c );
}

KTextEditCursor *KTextEditDocument::redo( KTextEditCursor *c )
{
    return commandHistory->redo( c );
}

bool KTextEditDocument::find( const QString &expr, bool cs, bool /*wo*/, bool /*forward*/,
			      int *parag, int *index, KTextEditCursor *cursor )
{
    // #### wo and forward is ignored at the moment
    KTextEditParag *p = fParag;
    if ( parag )
	p = paragAt( *parag );
    else if ( cursor )
	p = cursor->parag();
    bool first = TRUE;

    while ( p ) {
	QString s = p->string()->toString();
	int start = 0;
	if ( first && index )
	    start = *index;
	else if ( first )
	    start = cursor->index();
	first = FALSE;
	int res = s.find( expr, start, cs );
	if ( res != -1 ) {
	    cursor->setParag( p );
	    cursor->setIndex( res );
	    setSelectionStart( Search, cursor );
	    cursor->setIndex( res + expr.length() );
	    setSelectionEnd( Search, cursor );
	    if ( parag )
		*parag = p->paragId();
	    if ( index )
		*index = res;
	    return TRUE;
	}
	p = p->next();
    }

    return FALSE;
}

void KTextEditDocument::setParagSpacing( int s )
{
    ps = s;
    txtSettings.paragSpacing = s;
}

void KTextEditDocument::setLineSpacing( int s )
{
    ls = s;
    txtSettings.lineSpacing = s;
}

void KTextEditDocument::setMargin( int m )
{
    marg = m;
    txtSettings.margin = m;
}

bool KTextEditDocument::inSelection( int selId, const QPoint &pos ) const
{
    QMap<int, Selection>::ConstIterator it = selections.find( selId );
    if ( it == selections.end() )
	return FALSE;

    Selection sel = *it;
    KTextEditParag *startParag = sel.startParag;
    KTextEditParag *endParag = sel.endParag;
    if ( sel.endParag->paragId() < sel.startParag->paragId() ) {
	endParag = sel.startParag;
	startParag = sel.endParag;
    }

    // ######### Real implementation needed!!!!!!!

    QRect r = startParag->rect();
    r = r.unite( endParag->rect() );

    return r.contains( pos );
}

void KTextEditDocument::draw( QPainter *p, const QColorGroup &cg )
{
    if ( !firstParag() )
	return;

    KTextEditParag *parag = firstParag();

    while ( parag ) {
	if ( !parag->isValid() )
	    parag->format();

	p->translate( 0, parag->rect().y() );
	parag->paint( *p, cg, 0, FALSE );
	p->translate( 0, -parag->rect().y() );
	parag = parag->next();
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KTextEditString::KTextEditString()
{
}

void KTextEditString::insert( int index, const QString &s, KTextEditFormat *f )
{
    int os = data.size();
    data.resize( data.size() + s.length() );
    if ( index < os ) {
	memmove( data.data() + index + s.length(), data.data() + index,
		 sizeof( Char ) * ( os - index ) );
    }
    for ( int i = 0; i < (int)s.length(); ++i ) {
	data[ (int)index + i ].x = 0;
	data[ (int)index + i ].lineStart = 0;
#if defined(_WS_X11_)
	//### workaround for broken courier fonts on X11
	if ( s[ i ] == QChar( 0x00a0U ) )
	    data[ (int)index + i ].c = ' ';
	else
	    data[ (int)index + i ].c = s[ i ];
#else
	data[ (int)index + i ].c = s[ i ];
#endif
	data[ (int)index + i ].format = f;
    }
    cache.insert( index, s );
}

void KTextEditString::truncate( int index )
{
    data.truncate( index );
    cache.truncate( index );
}

void KTextEditString::remove( int index, int len )
{
    memmove( data.data() + index, data.data() + index + len,
	     sizeof( Char ) * ( data.size() - index - len ) );
    data.resize( data.size() - len );
    cache.remove( index, len );
}

void KTextEditString::setFormat( int index, KTextEditFormat *f, bool useCollection )
{
    if ( useCollection && data[ index ].format )
	data[ index ].format->removeRef();
    data[ index ].format = f;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KTextEditParag::KTextEditParag( KTextEditDocument *d, KTextEditParag *pr, KTextEditParag *nx, bool updateIds )
    : invalid( -1 ), p( pr ), n( nx ), doc( d ), typ( Normal ), align( Qt::AlignLeft )
{
    if ( p )
	p->n = this;
    if ( n )
	n->p = this;
    if ( !p )
	doc->setFirstParag( this );
    if ( !n )
	doc->setLastParag( this );

    changed = FALSE;
    firstFormat = TRUE;

    if ( p )
	id = p->id + 1;
    else
	id = 0;
    if ( n && updateIds ) {
	KTextEditParag *s = n;
	while ( s ) {
	    s->id = s->p->id + 1;
	    s = s->n;
	}
    }

    str = new KTextEditString();

    left = depth = 0;
}

void KTextEditParag::setNext( KTextEditParag *s )
{
    n = s;
    if ( !n )
	doc->setLastParag( this );
}

void KTextEditParag::setPrev( KTextEditParag *s )
{
    p = s;
    if ( !p )
	doc->setFirstParag( this );
}

void KTextEditParag::invalidate( int chr )
{
    if ( invalid < 0 )
	invalid = chr;
    else
	invalid = QMIN( invalid, chr );
}

void KTextEditParag::insert( int index, const QString &s )
{
    str->insert( index, s, doc->formatCollection()->defaultFormat() );
    invalidate( index );
}

void KTextEditParag::truncate( int index )
{
    str->truncate( index );
    append( " " );
}

void KTextEditParag::remove( int index, int len )
{
    str->remove( index, len );
    invalidate( 0 );
}

void KTextEditParag::join( KTextEditParag *s )
{
    int oh = r.height() + s->r.height();
    n = s->n;
    if ( n )
	n->p = this;
    else
	doc->setLastParag( this );

    if ( str->at( str->length() -1 ).c == ' ' ) // #### check this
	str->truncate( str->length() - 1 );
    int start = str->length();
    append( s->str->toString() );
    for ( int i = 0; i < s->length(); ++i ) {
	s->str->at( i ).format->addRef();
	str->setFormat( i + start, s->str->at( i ).format, TRUE );
    }
    delete s;
    invalidate( 0 );
    r.setHeight( oh );
    format();
    if ( n ) {
	KTextEditParag *s = n;
	while ( s ) {
	    s->id = s->p->id + 1;
	    s->changed = TRUE;
	    s = s->n;
	}
    }
}

void KTextEditParag::move( int dy )
{
    if ( dy == 0 )
	return;
    changed = TRUE;
    r.moveBy( 0, dy );
}

void KTextEditParag::format( int start, bool doMove )
{
    if ( str->length() == 0 || !doc->formatter() )
	return;

    if ( invalid == -1 )
	return;

    r.moveTopLeft( QPoint( doc->x(), p ? p->r.y() + p->r.height() : doc->y() ) );
    r.setWidth( doc->width() );
    QMap<int, LineStart*>::Iterator it = lineStarts.begin();
    for ( ; it != lineStarts.end(); ++it )
	delete *it;
    lineStarts.clear();
    int y = doc->formatter()->format( this, start );

    KTextEditString::Char *c = 0;
    if ( lineStarts.count() == 1 ) {
	c = &str->at( str->length() - 1 );
	r.setWidth( c->x + c->format->width( c->c ) );
    }

    if ( y != r.height() )
	r.setHeight( y );

    if ( n && doMove && n->invalid == -1 && r.y() + r.height() != n->r.y() ) {
	int dy = ( r.y() + r.height() ) - n->r.y();
	KTextEditParag *s = n;
	while ( s ) {
	    s->move( dy );
	    s = s->n;
	}
    }

    firstFormat = FALSE;
    changed = TRUE;
    invalid = -1;
}

int KTextEditParag::lineHeightOfChar( int i, int *bl, int *y ) const
{
    if ( !isValid() )
	( (KTextEditParag*)this )->format();

    QMap<int, LineStart*>::ConstIterator it = lineStarts.end();
    --it;
    for ( ;; ) {
	if ( i >= it.key() ) {
	    if ( bl )
		*bl = ( *it )->baseLine;
	    if ( y )
		*y = ( *it )->y;
	    return ( *it )->h;
	}
	if ( it == lineStarts.begin() )
	    break;
	--it;
    }

    qWarning( "KTextEditParag::lineHeightOfChar: couldn't find lh for %d", i );
    return 15;
}

KTextEditString::Char *KTextEditParag::lineStartOfChar( int i, int *index, int *line ) const
{
    if ( !isValid() )
	( (KTextEditParag*)this )->format();

    int l = lineStarts.count() - 1;
    QMap<int, LineStart*>::ConstIterator it = lineStarts.end();
    --it;
    for ( ;; ) {
	if ( i >= it.key() ) {
	    if ( index )
		*index = it.key();
	    if ( line )
		*line = l;
	    return &str->at( it.key() );
	}
	if ( it == lineStarts.begin() )
	    break;
	--it;
	--l;
    }

    qWarning( "KTextEditParag::lineStartOfChar: couldn't find %d", i );
    return 0;
}

int KTextEditParag::lines() const
{
    if ( !isValid() )
	( (KTextEditParag*)this )->format();

    return lineStarts.count();
}

KTextEditString::Char *KTextEditParag::lineStartOfLine( int line, int *index ) const
{
    if ( !isValid() )
	( (KTextEditParag*)this )->format();

    if ( line >= 0 && line < (int)lineStarts.count() ) {
	QMap<int, LineStart*>::ConstIterator it = lineStarts.begin();
	while ( line-- > 0 )
	    ++it;
	int i = it.key();
	if ( index )
	    *index = i;
	return &str->at( i );
    }

    qWarning( "KTextEditParag::lineStartOfLine: couldn't find %d", line );
    return 0;
}

void KTextEditParag::setFormat( int index, int len, KTextEditFormat *f, bool useCollection, int flags )
{
    if ( index < 0 )
	index = 0;
    if ( index > str->length() - 1 )
	index = str->length() - 1;
    if ( index + len > str->length() )
	len = str->length() - 1 - index;

    KTextEditFormatCollection *fc = 0;
    if ( useCollection )
	fc = doc->formatCollection();
    KTextEditFormat *of;
    for ( int i = 0; i < len; ++i ) {
	of = str->at( i + index ).format;
	if ( !changed && f->key() != of->key() )
	    changed = TRUE;
	if ( invalid == -1 &&
	     ( f->font().family() != of->font().family() ||
	       f->font().pointSize() != of->font().pointSize() ||
	       f->font().weight() != of->font().weight() ||
	       f->font().italic() != of->font().italic() ) ) {
	    invalidate( 0 );
	}
	if ( flags == -1 || flags == KTextEditFormat::Format || !fc ) {
	    if ( fc )
		f = fc->format( f );
	    str->setFormat( i + index, f, useCollection );
	} else {
	    KTextEditFormat *fm = fc->format( of, f, flags );
	    str->setFormat( i + index, fm, useCollection );
	}
    }
}

void KTextEditParag::indent( int *oldIndent, int *newIndent )
{
    if ( oldIndent )
	*oldIndent = 0;
    if ( newIndent )
	*newIndent = 0;
    if ( oldIndent && newIndent )
	*newIndent = *oldIndent;
}

void KTextEditParag::setListDepth( int d )
{
    if ( typ == Normal ) {
	depth = d;
	left = 0;
	return;
    }
    left = doc->listIndent( d );
    depth = d;
    invalidate( 0 );
}

void KTextEditParag::paint( QPainter &painter, const QColorGroup &cg, KTextEditCursor *cursor, bool drawSelections )
{
    KTextEditString::Char *chr = at( 0 );
    int i = 0;
    int h = 0;
    int baseLine = 0, lastBaseLine = 0;
    KTextEditFormat *lastFormat = 0;
    int lastY = -1;
    QString buffer;
    int startX = 0;
    int bw = 0;
    int cy = 0;
    int curx = -1, cury, curh;

    // #### draw other selections too here!!!!!!!
#ifdef __DECCXX
    int *selectionStarts = (int*) alloca(doc->numSelections * sizeof(int));
    int *selectionEnds   = (int*) alloca(doc->numSelections * sizeof(int));
#else
    int selectionStarts[ doc->numSelections ];
    int selectionEnds[ doc->numSelections ];
#endif
    if ( drawSelections ) {
	bool hasASelection = FALSE;
	for ( i = 0; i < doc->numSelections; ++i ) {
	    if ( !hasSelection( i ) ) {
		selectionStarts[ i ] = -1;
		selectionEnds[ i ] = -1;
	    } else {
		hasASelection = TRUE;
		selectionStarts[ i ] = selectionStart( i );
		int end = selectionEnd( i );
		if ( end == length() - 1 && n && n->hasSelection( i ) )
		    end++;
		selectionEnds[ i ] = end;
	    }
	}
	if ( !hasASelection )
	    drawSelections = FALSE;
    }

    int line = -1;
    int cw;
    for ( i = 0; i < length(); i++ ) {
	chr = at( i );
	cw = chr->format->width( chr->c );

	// init a new line
	if ( chr->lineStart ) {
	    ++line;
	    lineInfo( line, cy, h, baseLine );
	    if ( lastBaseLine == 0 )
		lastBaseLine = baseLine;
	}

	// draw bullet list items
	if ( line == 0 && type() == KTextEditParag::BulletList && ( length() > 1 || drawSelections ) ) {
	    int ext = QMIN( doc->listIndent( 0 ), h );
	    ext -= 10;

	    // ######## use pixmaps for drawing that stuff - this way it's very slow when having long lists!
	    switch ( doc->bullet( listDepth() ) ) {
	    case KTextEditDocument::FilledCircle: {
		painter.setPen( Qt::NoPen );
		painter.setBrush( doc->bulletColor( listDepth() ) );
		painter.drawEllipse( leftIndent() - ext - 4, cy + ( h - ext ) / 2, ext, ext );
	    } break;
	    case KTextEditDocument::FilledSquare: {
		painter.fillRect( leftIndent() - ext - 4, cy + ( h - ext ) / 2, ext, ext,
				  doc->bulletColor( listDepth() ) );
	    } break;
	    case KTextEditDocument::OutlinedCircle: {
		painter.setPen( QPen( doc->bulletColor( listDepth() ) ) );
		painter.setBrush( Qt::NoBrush );
		painter.drawEllipse( leftIndent() - ext - 4, cy + ( h - ext ) / 2, ext, ext );
	    } break;
	    case KTextEditDocument::OutlinedSquare: {
		painter.setPen( QPen( doc->bulletColor( listDepth() ) ) );
		painter.setBrush( Qt::NoBrush );
		painter.drawRect( leftIndent() - ext - 4, cy + ( h - ext ) / 2, ext, ext );
	    } break;
	    }
	}

	// check for cursor mark
	if ( cursor && this == cursor->parag() && i == cursor->index() ) {
	    curx = chr->x;
	    curh = h;
	    cury = cy;
	}

	// first time - start again...
	if ( !lastFormat || lastY == -1 ) {
	    lastFormat = chr->format;
	    lastY = cy;
	    startX = chr->x;
	    buffer += chr->c;
	    bw = cw;
	    continue;
	}

	// check if selection state changed
	bool selectionChange = FALSE;
	if ( drawSelections ) {
	    for ( int j = 0; j < doc->numSelections; ++j ) {
		selectionChange = selectionStarts[ j ] == i || selectionEnds[ j ] == i;
		if ( selectionChange )
		    break;
	    }
	}

	// if something (format, etc.) changed, draw what we have so far
	if ( lastY != cy || chr->format != lastFormat || buffer == "\t" || chr->c == '\t' || selectionChange ) {
	    drawParagBuffer( painter, buffer, startX, lastY, lastBaseLine, bw, h, drawSelections,
			     lastFormat, i, selectionStarts, selectionEnds, cg );
	    buffer = chr->c;
	    lastFormat = chr->format;
	    lastY = cy;
	    startX = chr->x;
	    bw = cw;
	} else {
	    buffer += chr->c;
	    bw += cw;
	}
	lastBaseLine = baseLine;
    }

    // if we are through thg parag, but still have some stuff left to draw, draw it now
    if ( !buffer.isEmpty() ) {
	bool selectionChange = FALSE;
	if ( drawSelections ) {
	    for ( int j = 0; j < doc->numSelections; ++j ) {
		selectionChange = selectionStarts[ j ] == i || selectionEnds[ j ] == i;
		if ( selectionChange )
		    break;
	    }
	}
	drawParagBuffer( painter, buffer, startX, lastY, lastBaseLine, bw, h, drawSelections,
			 lastFormat, i, selectionStarts, selectionEnds, cg );
    }

    // if we should draw a cursor, draw it now
    if ( curx != -1 && cursor )
	painter.fillRect( QRect( curx, cury, 1, curh ), Qt::red );
}

void KTextEditParag::drawParagBuffer( QPainter &painter, const QString &buffer, int startX,
				      int lastY, int baseLine, int bw, int h, bool drawSelections,
				      KTextEditFormat *lastFormat, int i, int *selectionStarts,
				      int *selectionEnds, const QColorGroup &cg )
{
    if ( !doc->drawAllInOneColor() )
	painter.setPen( QPen( lastFormat->color() ) );
    else
	painter.setPen( QPen( doc->allInOneColor() ) );
    painter.setFont( lastFormat->font() );
    if ( drawSelections ) {
	for ( int j = 0; j < doc->numSelections; ++j ) {
	    if ( i > selectionStarts[ j ] && i <= selectionEnds[ j ] ) {
		if ( doc->invertSelectionText( j ) )
		    painter.setPen( QPen( cg.color( QColorGroup::HighlightedText ) ) );
		painter.fillRect( startX, lastY, bw, h, doc->selectionColor( j ) );
	    }
	}
    }
    if ( buffer != "\t" )
	painter.drawText( startX, lastY + baseLine, buffer );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


KTextEditFormatter::KTextEditFormatter( KTextEditDocument *d )
    : doc( d )
{
}

KTextEditFormatterBreakWords::KTextEditFormatterBreakWords( KTextEditDocument *d )
    : KTextEditFormatter( d )
{
}

int KTextEditFormatterBreakWords::format( KTextEditParag *parag, int start )
{
    KTextEditString::Char *c = 0;
    int left = parag->leftIndent();
    int x = left;
    int w = doc->width() - x;
    int y = 0;
    int h = 0;
    int len = parag->length();

    // #########################################
    // Should be optimized so that we start formatting
    // really at start (this means the last line begin before start)
    // and not always at the beginnin of the parag!
    start = 0;
    if ( start == 0 ) {
	c = &parag->string()->at( 0 );
    }
    // #########################################

    int i = start;
    KTextEditParag::LineStart *lineStart = new KTextEditParag::LineStart( 0, 0, 0 );
    parag->lineStartList().insert( 0, lineStart );
    int lastSpace = -1;
    int tmpBaseLine = 0, tmph = 0;

    for ( ; i < len; ++i ) {
	c = &parag->string()->at( i );
	if ( i > 0 && x > left ) {
	    c->lineStart = 0;
	} else {
	    c->lineStart = 1;
	}
	int ww = 0;
	if ( c->c.unicode() >= 32 || c->c == '\t' ) {
	    ww = c->format->width( c->c );
	} else {
	    ww = c->format->width( ' ' );
	}

	if ( x + ww > left + w ) {
	    if ( lastSpace == -1 ) {
		if ( lineStart ) {
		    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
		    h = QMAX( h, tmph );
		    lineStart->h = h;
		}
		x = left;
		y += h + doc->lineSpacing();
		tmph = c->format->height();
		h = 0;
		lineStart = new KTextEditParag::LineStart( y, 0, 0 );
		parag->lineStartList().insert( i, lineStart );
		lineStart->baseLine = c->format->ascent();
		lineStart->h = c->format->height();
		c->lineStart = 1;
		tmpBaseLine = lineStart->baseLine;
		lastSpace = -1;
	    } else {
		i = lastSpace;
		x = left;
		y += h + doc->lineSpacing();
		tmph = c->format->height();
		h = tmph;
		lineStart = new KTextEditParag::LineStart( y, 0, 0 );
		parag->lineStartList().insert( i + 1, lineStart );
		lineStart->baseLine = c->format->ascent();
		lineStart->h = c->format->height();
		c->lineStart = 1;
		tmpBaseLine = lineStart->baseLine;
		lastSpace = -1;
		continue;
	    }
	} else if ( lineStart && c->c == ' ' ) {
	    if ( len < 2 || i < len - 1 ) {
		tmpBaseLine = QMAX( tmpBaseLine, c->format->ascent() );
		tmph = QMAX( tmph, c->format->height() );
	    }
	    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
	    h = QMAX( h, tmph );
	    lineStart->h = h;
	    lastSpace = i;
	} else {
	    tmpBaseLine = QMAX( tmpBaseLine, c->format->ascent() );
	    tmph = QMAX( tmph, c->format->height() );
	}

	c->x = x;
	x += ww;
    }

    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
    h = QMAX( h, tmph );
    lineStart->h = h;

    // ############## unefficient!!!!!!!!!!!!!!!!!!!!!! - rewrite that!!!!
    if ( parag->alignment() & Qt::AlignHCenter || parag->alignment() & Qt::AlignRight ) {
	int last = 0;
	QMap<int, KTextEditParag::LineStart*>::Iterator it = parag->lineStartList().begin();
	while ( TRUE ) {
	    it++;
	    int i = 0;
	    if ( it == parag->lineStartList().end() )
		i = parag->length() - 1;
	    else
		i = it.key() - 1;
	    c = &parag->string()->at( i );
	    int lw = c->x + c->format->width( c->c );
	    int diff = w - lw;
	    if ( parag->alignment() & Qt::AlignHCenter )
		diff /= 2;
	    for ( int j = last; j <= i; ++j )
		parag->string()->at( j ).x += diff;
	    last = i + 1;
	    if ( it == parag->lineStartList().end() )
		break;
	}
    }


    y += h + doc->paragSpacing( parag );
    return y;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KTextEditFormatCollection::KTextEditFormatCollection()
{
    zoomFakt = 1;
    //lukas: there is a reason why not to use hardcoded
    //fonts! not everyone has utopia...
    QFont font = KGlobalSettings::generalFont();
    font.setPointSize(20);
    KGlobal::charsets()->setQFont(font, KGlobal::locale()->charset());
    defFormat = new KTextEditFormat( font, Qt::black );
    lastFormat = cres = 0;
    cflags = -1;
    cachedFormat = 0;
}

KTextEditFormatCollection::~KTextEditFormatCollection()
{
}

KTextEditFormat *KTextEditFormatCollection::format( KTextEditFormat *f )
{
    if ( f->parent() == this ) {
#ifdef DEBUG_COLLECTION
	qDebug( "need '%s', best case!", f->key().latin1() );
#endif
	lastFormat = f;
	lastFormat->addRef();
	return lastFormat;
    }

    if ( f == lastFormat || ( lastFormat && f->key() == lastFormat->key() ) ) {
#ifdef DEBUG_COLLECTION
	qDebug( "need '%s', good case!", f->key().latin1() );
#endif
	lastFormat->addRef();
	return lastFormat;
    }

    KTextEditFormat *fm = cKey.find( f->key() );
    if ( fm ) {
#ifdef DEBUG_COLLECTION
	qDebug( "need '%s', normal case!", f->key().latin1() );
#endif
	lastFormat = fm;
	lastFormat->addRef();
	return lastFormat;
    }

#ifdef DEBUG_COLLECTION
    qDebug( "need '%s', worst case!", f->key().latin1() );
#endif
    lastFormat = new KTextEditFormat( *f );
    lastFormat->collection = this;
    cKey.insert( lastFormat->key(), lastFormat );
    return lastFormat;
}

KTextEditFormat *KTextEditFormatCollection::format( KTextEditFormat *of, KTextEditFormat *nf, int flags )
{
    if ( cres && kof == of->key() && knf == nf->key() && cflags == flags ) {
#ifdef DEBUG_COLLECTION
	qDebug( "mix of '%s' and '%s, best case!", of->key().latin1(), nf->key().latin1() );
#endif
	cres->addRef();
	return cres;
    }

    cres = new KTextEditFormat( *of );
    kof = of->key();
    knf = nf->key();
    cflags = flags;
    if ( flags & KTextEditFormat::Bold )
	cres->fn.setBold( nf->fn.bold() );
    if ( flags & KTextEditFormat::Italic )
	cres->fn.setItalic( nf->fn.italic() );
    if ( flags & KTextEditFormat::Underline )
	cres->fn.setUnderline( nf->fn.underline() );
    if ( flags & KTextEditFormat::Family )
	cres->fn.setFamily( nf->fn.family() );
    if ( flags & KTextEditFormat::Size )
	cres->fn.setPointSize( nf->fn.pointSize() );
    if ( flags & KTextEditFormat::Color )
	cres->col = nf->col;
    cres->update();

    KTextEditFormat *fm = cKey.find( cres->key() );
    if ( !fm ) {
#ifdef DEBUG_COLLECTION
	qDebug( "mix of '%s' and '%s, worst case!", of->key().latin1(), nf->key().latin1() );
#endif
	cres->collection = this;
	cKey.insert( cres->key(), cres );
    } else {
#ifdef DEBUG_COLLECTION
	qDebug( "mix of '%s' and '%s, good case!", of->key().latin1(), nf->key().latin1() );
#endif
	delete cres;
	cres = fm;
	cres->addRef();
    }

    return cres;
}

KTextEditFormat *KTextEditFormatCollection::format( const QFont &f, const QColor &c )
{
    if ( cachedFormat && cfont == f && ccol == c ) {
#ifdef DEBUG_COLLECTION
	qDebug( "format of font and col '%s' - best case", cachedFormat->key().latin1() );
#endif
	cachedFormat->addRef();
	return cachedFormat;
    }

    QString key = KTextEditFormat::getKey( f, c );
    cachedFormat = cKey.find( key );
    cfont = f;
    ccol = c;

    if ( cachedFormat ) {
#ifdef DEBUG_COLLECTION
	qDebug( "format of font and col '%s' - good case", cachedFormat->key().latin1() );
#endif
	cachedFormat->addRef();
	return cachedFormat;
    }

    cachedFormat = new KTextEditFormat( f, c );
    cachedFormat->collection = this;
    cKey.insert( cachedFormat->key(), cachedFormat );
#ifdef DEBUG_COLLECTION
    qDebug( "format of font and col '%s' - worst case", cachedFormat->key().latin1() );
#endif
    return cachedFormat;
}

void KTextEditFormatCollection::remove( KTextEditFormat *f )
{
    if ( lastFormat == f )
	lastFormat = 0;
    if ( cres == f )
	cres = 0;
    if ( cachedFormat == f )
	cachedFormat = 0;
    cKey.remove( f->key() );
}

void KTextEditFormatCollection::debug()
{
#ifdef DEBUG_COLLECTION
    qDebug( "------------ KTextEditFormatCollection: debug --------------- BEGIN" );
    QDictIterator<KTextEditFormat> it( cKey );
    for ( ; it.current(); ++it ) {
	qDebug( "format '%s' (%p): refcount: %d", it.current()->key().latin1(),
		it.current(), it.current()->ref );
    }
    qDebug( "------------ KTextEditFormatCollection: debug --------------- END" );
#endif
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void KTextEditFormat::setBold( bool b )
{
    if ( b == fn.bold() )
	return;
    fn.setBold( b );
    update();
}

void KTextEditFormat::setItalic( bool b )
{
    if ( b == fn.italic() )
	return;
    fn.setItalic( b );
    update();
}

void KTextEditFormat::setUnderline( bool b )
{
    if ( b == fn.underline() )
	return;
    fn.setUnderline( b );
    update();
}

void KTextEditFormat::setFamily( const QString &f )
{
    if ( f == fn.family() )
	return;
    fn.setFamily( f );
    update();
}

void KTextEditFormat::setPointSize( int s )
{
    if ( s == fn.pointSize() )
	return;
    fn.setPointSize( s );
    update();
}

void KTextEditFormat::setFont( const QFont &f )
{
    if ( f == fn )
	return;
    fn = f;
    update();
}

void KTextEditFormat::setColor( const QColor &c )
{
    if ( c == col )
	return;
    col = c;
    update();
}

void KTextEditFormatCollection::zoom( float f )
{
    if ( zoomFakt == f )
	return;
    zoomFakt = f;
    QDictIterator<KTextEditFormat> it( cKey );
    KTextEditFormat *format;
    orig.clear();
    while ( ( format = it.current() ) ) {
	orig.insert( (void*)format, new int( format->font().pointSize() ) );
	++it;
    }
    it.toFirst();
    while ( ( format = it.current() ) ) {
	++it;
	format->fn.setPointSize( (int)( (float)( format->fn.pointSize() ) * f ) );
	format->update();
    }
}

void KTextEditFormatCollection::unzoom()
{
    zoomFakt = 1;
    QDictIterator<KTextEditFormat> it( cKey );
    KTextEditFormat *fm;
    while ( ( fm = it.current() ) ) {
	if ( !orig.find( (void*)fm ) )
	    ;//qDebug( "*baaaaaaaaaaa: %s", it.currentKey().latin1() );
	else {
	    fm->fn.setPointSize( *orig.find( (void*)fm ) );
	    fm->update();
	}
	++it;
    }
    orig.clear();
}

void KTextEditDocument::zoom( float f )
{
    oldListMult = listMult;
    listMult = (int)( (float)listMult * ( f - f / 5.0 ) );
    formatCollection()->zoom( f );
    oldLineSpacing = lineSpacing();
    oldParagSpacing = paragSpacing();
    invalidate();
}

void KTextEditDocument::unzoom()
{
    listMult = oldListMult;
    formatCollection()->unzoom();
    setLineSpacing( oldLineSpacing );
    setParagSpacing( oldParagSpacing );
    invalidate();
}

void KTextEditDocument::setAlignmentToAll( int a )
{
    KTextEditParag *p = fParag;
    while ( p ) {
	p->setAlignment( a );
	p = p->next();
    }
}

void KTextEditDocument::setFontToAll( const QFont &fn )
{
    KTextEditParag *p = fParag;
    KTextEditFormat *f = new KTextEditFormat( fn, QColor() );
    while ( p ) {
	p->setFormat( 0, p->length(), f, TRUE, KTextEditFormat::Font );
	p = p->next();
    }
    delete f;
}

void KTextEditDocument::setColorToAll( const QColor &c )
{
    KTextEditParag *p = fParag;
    KTextEditFormat *f = new KTextEditFormat( QFont(), c );
    while ( p ) {
	p->setFormat( 0, p->length(), f, TRUE, KTextEditFormat::Color );
	p = p->next();
    }
    delete f;
}

void KTextEditDocument::setBoldToAll( bool b )
{
    KTextEditParag *p = fParag;
    QFont fn;
    fn.setBold( b );
    KTextEditFormat *f = new KTextEditFormat( fn, QColor() );
    while ( p ) {
	p->setFormat( 0, p->length(), f, TRUE, KTextEditFormat::Bold );
	p = p->next();
    }
    delete f;
}

void KTextEditDocument::setItalicToAll( bool b )
{
    KTextEditParag *p = fParag;
    QFont fn;
    fn.setItalic( b );
    KTextEditFormat *f = new KTextEditFormat( fn, QColor() );
    while ( p ) {
	p->setFormat( 0, p->length(), f, TRUE, KTextEditFormat::Italic );
	p = p->next();
    }
    delete f;
}

void KTextEditDocument::setUnderlineToAll( bool b )
{
    KTextEditParag *p = fParag;
    QFont fn;
    fn.setUnderline( b );
    KTextEditFormat *f = new KTextEditFormat( fn, QColor() );
    while ( p ) {
	p->setFormat( 0, p->length(), f, TRUE, KTextEditFormat::Underline );
	p = p->next();
    }
    delete f;
}

void KTextEditDocument::setPointSizeToAll( int s )
{
    KTextEditParag *p = fParag;
    QFont fn;
    fn.setPointSize( s );
    KTextEditFormat *f = new KTextEditFormat( fn, QColor() );
    while ( p ) {
	p->setFormat( 0, p->length(), f, TRUE, KTextEditFormat::Size );
	p = p->next();
    }
    delete f;
}

void KTextEditDocument::setFamilyToAll( const QString &fam )
{
    KTextEditParag *p = fParag;
    QFont fn;
    fn.setFamily( fam );
    KTextEditFormat *f = new KTextEditFormat( fn, QColor() );
    while ( p ) {
	p->setFormat( 0, p->length(), f, TRUE, KTextEditFormat::Family );
	p = p->next();
    }
    delete f;
}

void KTextEdit::extendContents2Height()
{
    int h = height() - ( doc->lastParag()->rect().bottom() + 1 );
    int sp = QMAX( 0, h / ( doc->lastParag()->paragId() + 1 ) );
    doc->setParagSpacing( sp );
    doc->invalidate();
}

KTextEditFormatCollection *KTextEditDocument::formatCollection() const
{
    return kpr_doc->formatCollection();
}

#include <ktextedit.moc>
