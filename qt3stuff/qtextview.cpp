/****************************************************************************
** $Id$
**
** Implementation of the QTextView class
**
** Created : 990101
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qtextview.h"
#include "qrichtext_p.h"
#include "qpainter.h"
#include "qpen.h"
#include "qbrush.h"
#include "qpixmap.h"
#include "qfont.h"
#include "qcolor.h"
#include "qsize.h"
#include "qevent.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qlistbox.h"
#include "qvbox.h"
#include "qapplication.h"
#include "qclipboard.h"
#include "qcolordialog.h"
#include "qfontdialog.h"
#include "qstylesheet.h"
#include "qdragobject.h"
#include "qurl.h"
#include "qcursor.h"
#include "qregexp.h"
#include "qpopupmenu.h"

struct QUndoRedoInfoPrivate
{
    QTextString text;
};

class QTextViewPrivate
{
public:
    int id[ 7 ];
};

static bool block_set_alignment = FALSE;

/*!
  \class QTextView qtextview.h
  \brief The QTextView class provides a sophisticated, single-page rich text viewer.
  \ingroup basic
  \ingroup helpsystem

  Unlike QSimpleRichText, which merely draws small pieces of rich
  text, a QTextView is a real widget with scroll bars, when necessary,
  to show large text documents.

  The rendering style and available tags are defined by a
  styleSheet(). Currently, a small XML/CSS1 subset including embedded
  images and tables is supported. See QStyleSheet for
  details. Possible images within the text document are resolved by
  using a QMimeSourceFactory.  See setMimeSourceFactory() for details.

  Using QTextView is quite similar to QLabel. It's mainly a call to
  setText() to set the contents. Setting the background color is
  slightly different from other widgets, because a text view is a
  scrollable widget that naturally provides a scrolling
  background. You can specify the paper background with
  setPaper(). QTextView supports both plain color and complex pixmap
  backgrounds.

  The API also offers functions to set different word-wrapping modes,
  using setWordWrap(), setWrapPolicy() and setWrapColumnOrWidth().

  To get information about the length of the text, use paragraphs(),
  lines() and length().

  It is also possible to get the currently selected text by
  selectedText(). hasSelection() tells you, if there is currently a
  selection at all. Using getSelection() it is possible to get the
  exact position of the selection.

  To find an expression in the text and select this one, use
  find(). Other functions like documentTitle(), context() and
  fileName() give you more information about the displayed document.

  Note that we do not intend to add a full-featured web browser widget
  to Qt (because that would easily double Qt's size and only few
  applications would benefit from it). In particular, the rich text
  support in Qt is supposed to provide a fast, portable and efficient
  way to add reasonable online help facilities to applications. We
  will, however, extend it to some degree in future versions of Qt.

  For more, including hypertext capabilities, see QTextBrowser.

  The richtext engine that is used by QTextView also provides the
  possibility to edit the document using QTextEdit.
*/

/*!  \fn void QTextView::copyAvailable (bool yes)

  This signal is emitted when the availability of cut/copy changes.
  If \a yes is TRUE, then cut() and copy() will work until
  copyAvailable( FALSE ) is next emitted.
*/


/*!  \fn void QTextView::textChanged()

  This signal is always emitted when the contents of the view changed.
 */

/*!  \fn void QTextView::selectionChanged()

  This signal is always emitted when the selection is changed.
*/

/*!  \fn QTextDocument *QTextView::document() const

  This function returns the QTextDocument which is used by the
  view. QTextDocument is a class, which is not in the public API. If
  you want to do more specialized stuff, you might use that
  anyway. But be aware that the API of QTextDocument might change in a
  incompatible manner in the future.
*/

/*!  \fn void QTextView::setDocument( QTextDocument *doc )

  This function sets the QTextDocument which should be used by this
  view. This can be used if you e.g. want to display one document in
  multiple views. Just create a QTextDocument and set it to the views
  which should display it. But you need then connect to textChanged()
  and selectionChanged() of all the views and update the others
  (preferable a bit delayed for efficiece reasons).

  Note that QTextDocument is a class, which is not in the public
  API. If you want to do more specialized stuff (like described), you
  might use that anyway. But be aware that the API of QTextDocument
  might change in a incompatible manner in the future.
*/

/*!  Constructs an empty QTextView with the standard \a parent and \a
  name optional arguments.
*/

QTextView::QTextView( QWidget *parent, const char *name )
    : QScrollView( parent, name, WNorthWestGravity | WRepaintNoErase | WResizeNoErase ),
      doc( new QTextDocument( 0 ) ), undoRedoInfo( doc )
{
    init();
}

/*!  Constructs a QTextView displaying the contents \a text with
  context \a context, with the standard \a parent and \a name optional
  arguments.
*/

QTextView::QTextView( const QString& text, const QString& context,
		      QWidget *parent, const char *name)
    : QScrollView( parent, name, WNorthWestGravity | WRepaintNoErase | WResizeNoErase ),
      doc( new QTextDocument( 0 ) ), undoRedoInfo( doc )
{
    init();
    setText( text, context );
}

/*! \reimp */

QTextView::~QTextView()
{
    delete undoRedoInfo.d;
    undoRedoInfo.d = 0;
    delete cursor;
    delete doc;
    delete d;
}

void QTextView::init()
{
    d = new QTextViewPrivate;
    connect( doc, SIGNAL( minimumWidthChanged( int ) ),
	     this, SLOT( setRealWidth( int ) ) );

    mousePressed = FALSE;
    inDoubleClick = FALSE;
    modified = FALSE;
    onLink = QString::null;
    overWrite = FALSE;
    wrapMode = WidgetWidth;
    wrapWidth = -1;
    wPolicy = AtWhiteSpace;
    setMode = Auto;
    inDnD = FALSE;

    doc->setFormatter( new QTextFormatterBreakWords );
    currentFormat = doc->formatCollection()->defaultFormat();
    currentAlignment = Qt3::AlignAuto;

    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setAcceptDrops( TRUE );
    resizeContents( 0, doc->lastParag() ?
		    ( doc->lastParag()->paragId() + 1 ) * doc->formatCollection()->defaultFormat()->height() : 0 );

    setKeyCompression( TRUE );
    viewport()->setMouseTracking( TRUE );
#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
    cursor = new QTextCursor( doc );

    formatTimer = new QTimer( this );
    connect( formatTimer, SIGNAL( timeout() ),
	     this, SLOT( formatMore() ) );
    lastFormatted = doc->firstParag();

    scrollTimer = new QTimer( this );
    connect( scrollTimer, SIGNAL( timeout() ),
	     this, SLOT( doAutoScroll() ) );

    interval = 0;
    changeIntervalTimer = new QTimer( this );
    connect( changeIntervalTimer, SIGNAL( timeout() ),
	     this, SLOT( doChangeInterval() ) );

    cursorVisible = TRUE;
    blinkTimer = new QTimer( this );
    connect( blinkTimer, SIGNAL( timeout() ),
	     this, SLOT( blinkCursor() ) );

#ifndef QT_NO_DRAGANDDROP
    dragStartTimer = new QTimer( this );
    connect( dragStartTimer, SIGNAL( timeout() ),
	     this, SLOT( startDrag() ) );
#endif

    resizeTimer = new QTimer( this );
    connect( resizeTimer, SIGNAL( timeout() ),
	     this, SLOT( doResize() ) );

    formatMore();

    blinkCursorVisible = FALSE;

    connect( this, SIGNAL( textChanged() ),
	     this, SLOT( setModified() ) );
    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    viewport()->installEventFilter( this );
    installEventFilter( this );

#if 0 // ### background paper test code
    QBrush *b = new QBrush( red, QPixmap( "/home/reggie/kde2/share/wallpapers/All-Good-People-1.jpg" ) );
    doc->setPaper( b );
    QPalette pal( palette() );
    pal.setBrush( QColorGroup::Base, *b );
    setPalette( pal );
#endif
}

void QTextView::paintDocument( bool drawAll, QPainter *p, int cx, int cy, int cw, int ch )
{
    bool drawCur = hasFocus() || viewport()->hasFocus();
    if ( isReadOnly() || !cursorVisible )
	drawCur = FALSE;
    QColorGroup g = colorGroup();
    if ( doc->paper() )
	g.setBrush( QColorGroup::Base, *doc->paper() );

    if ( contentsY() == 0 ) {
	p->fillRect( contentsX(), contentsY(), visibleWidth(), doc->y(),
		     g.brush( QColorGroup::Base ) );
    }

    p->setBrushOrigin( -contentsX(), -contentsY() );

    lastFormatted = doc->draw( p, cx, cy, cw, ch, g, !drawAll, drawCur, cursor );

    if ( lastFormatted == doc->lastParag() )
	resizeContents( contentsWidth(), doc->height() );

    if ( contentsHeight() < visibleHeight() && ( !doc->lastParag() || doc->lastParag()->isValid() ) && drawAll )
	p->fillRect( 0, contentsHeight(), visibleWidth(),
		     visibleHeight() - contentsHeight(), g.brush( QColorGroup::Base ) );
}

/*! \reimp */

void QTextView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    paintDocument( TRUE, p, cx, cy, cw, ch );
}

/*! \reimp */

bool QTextView::event( QEvent *e )
{
    if ( e->type() == QEvent::AccelOverride && !isReadOnly() ) {
	QKeyEvent* ke = (QKeyEvent*) e;
	if ( ke->state() & ControlButton ) {
	    switch ( ke->key() ) {
	    case Key_A:
	    case Key_E:
#if defined (Q_WS_WIN)
	    case Key_Insert:
#endif
	    case Key_X:
	    case Key_V:
	    case Key_C:
	    case Key_Left:
	    case Key_Right:
	    case Key_Up:
	    case Key_Down:
	    case Key_Home:
	    case Key_End:
		ke->accept();
	    default:
		break;
	    }
	} else {
	    switch ( ke->key() ) {
	    case Key_Delete:
	    case Key_Home:
	    case Key_End:
	    case Key_Backspace:
		ke->accept();
	    default:
		break;
	    }
	}
    }
    return QWidget::event( e );
}

/*! Provides scrolling and paging.
 */

void QTextView::keyPressEvent( QKeyEvent *e )
{
    changeIntervalTimer->stop();
    interval = 10;

    if ( isReadOnly() ) {
	handleReadOnlyKeyEvent( e );
	changeIntervalTimer->start( 100, TRUE );
	return;
    }


    bool selChanged = FALSE;
    for ( int i = 1; i < QTextDocument::Temp; ++i ) // start with 1 as we don't want to remove the Standard-Selection
 	selChanged = doc->removeSelection( i ) || selChanged;

    if ( selChanged ) {
	cursor->parag()->document()->nextDoubleBuffered = TRUE;
	repaintChanged();
    }

    bool clearUndoRedoInfo = TRUE;

    switch ( e->key() ) {
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
	doc->removeSelection( QTextDocument::Standard );
#ifndef QT_NO_CURSOR
	viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	clearUndoRedoInfo = FALSE;
	doKeyboardAction( ActionReturn );
	emitReturnPressed();
	break;
    case Key_Delete:
	if ( doc->hasSelection( QTextDocument::Standard ) ) {
	    removeSelectedText();
	    break;
	}

	doKeyboardAction( ActionDelete );
	clearUndoRedoInfo = FALSE;

	break;
    case Key_Backspace:
	if ( doc->hasSelection( QTextDocument::Standard ) ) {
	    removeSelectedText();
	    break;
	}

	if ( !cursor->parag()->prev() &&
	     cursor->atParagStart() )
	    break;

	doKeyboardAction( ActionBackspace );
	clearUndoRedoInfo = FALSE;

	break;
    case Key_F16: // Copy key on Sun keyboards
	copy();
	break;
    case Key_F18:  // Paste key on Sun keyboards
	paste();
	break;
    case Key_F20:  // Cut key on Sun keyboards
	cut();
	break;
    default: {
	    if ( e->text().length() &&
//		 !( e->state() & AltButton ) && !( e->state() & MetaButton ) &&
		 ( !e->ascii() || e->ascii() >= 32 ) ||
		 ( e->text() == "\t" && !( e->state() & ControlButton ) ) ) {
		clearUndoRedoInfo = FALSE;
		if ( e->key() == Key_Tab ) {
		    if ( cursor->index() == 0 && cursor->parag()->style() &&
			 cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayListItem ) {
			cursor->parag()->incDepth();
			drawCursor( FALSE );
			repaintChanged();
			drawCursor( TRUE );
			break;
		    }
		}
		if ( cursor->parag()->style() &&
		     cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayBlock &&
		     cursor->index() == 0 && ( e->text() == "-" || e->text() == "*" ) ) {
		    setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
		} else {
		    insert( e->text(), TRUE, FALSE );
		}
		break;
	    }
	    if ( e->state() & ControlButton ) {
		switch ( e->key() ) {
		case Key_C: case Key_F16: // Copy key on Sun keyboards
		    copy();
		    break;
		case Key_V:
		    paste();
		    break;
		case Key_X:
		    cut();
		    break;
		case Key_I: case Key_T: case Key_Tab:
		    indent();
		    break;
		case Key_A:
#if defined(_WS_X11_)
		    moveCursor( MoveHome, e->state() & ShiftButton, FALSE );
#else
		    selectAll( TRUE );
#endif
		    break;
		case Key_B:
		    moveCursor( MoveLeft, e->state() & ShiftButton, FALSE );
		    break;
		case Key_F:
		    moveCursor( MoveRight, e->state() & ShiftButton, FALSE );
		    break;
		case Key_D:
		    if ( doc->hasSelection( QTextDocument::Standard ) ) {
			removeSelectedText();
			break;
		    }
		    doKeyboardAction( ActionDelete );
		    clearUndoRedoInfo = FALSE;
		    break;
		case Key_H:
		    if ( doc->hasSelection( QTextDocument::Standard ) ) {
			removeSelectedText();
			break;
		    }
		    if ( !cursor->parag()->prev() &&
			 cursor->atParagStart() )
			break;

		    doKeyboardAction( ActionBackspace );
		    clearUndoRedoInfo = FALSE;
		    break;
		case Key_E:
		    moveCursor( MoveEnd, e->state() & ShiftButton, FALSE );
		    break;
		case Key_N:
		    moveCursor( MoveDown, e->state() & ShiftButton, FALSE );
		    break;
		case Key_P:
		    moveCursor( MoveUp, e->state() & ShiftButton, FALSE );
		    break;
		case Key_Z:
		    undo();
		    break;
		case Key_Y:
		    redo();
		    break;
		case Key_K:
		    doKeyboardAction( ActionKill );
		    break;
		case Key_Insert:
#if defined(Q_WS_WIN)
		    copy();
#endif
		    break;
		}
		break;
	    }
	}
    }

    emitCursorPositionChanged( cursor );
    if ( clearUndoRedoInfo )
	clearUndoRedo();
    changeIntervalTimer->start( 100, TRUE );
}

void QTextView::doKeyboardAction( KeyboardActionPrivate action )
{
    if ( isReadOnly() )
	return;

    if ( cursor->nestedDepth() != 0 ) // #### for 3.0, disable editing of tables as this is not advanced enough
	return;

    lastFormatted = cursor->parag();
    drawCursor( FALSE );

    switch ( action ) {
    case ActionDelete:
	checkUndoRedoInfo( UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.d->text = QString::null;
	}
	undoRedoInfo.d->text += cursor->parag()->at( cursor->index() )->c;
 	if ( cursor->parag()->at( cursor->index() )->format() ) {
 	    cursor->parag()->at( cursor->index() )->format()->addRef();
 	    undoRedoInfo.d->text.at( undoRedoInfo.d->text.length() - 1 ).setFormat( cursor->parag()->at( cursor->index() )->format() );
 	}
	if ( cursor->remove() )
	    undoRedoInfo.d->text += "\n";
	break;
    case ActionBackspace:
	if ( cursor->parag()->style() && cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayListItem &&
	     cursor->index() == 0 ) {
	    cursor->parag()->decDepth();
	    lastFormatted = cursor->parag();
	    repaintChanged();
	    drawCursor( TRUE );
	    return;
	}
	checkUndoRedoInfo( UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.d->text = QString::null;
	}
	cursor->gotoLeft();
	undoRedoInfo.d->text.prepend( QString( cursor->parag()->at( cursor->index() )->c ) );
 	if ( cursor->parag()->at( cursor->index() )->format() ) {
 	    cursor->parag()->at( cursor->index() )->format()->addRef();
 	    undoRedoInfo.d->text.at( 0 ).setFormat( cursor->parag()->at( cursor->index() )->format() );
 	}
	undoRedoInfo.index = cursor->index();
	if ( cursor->remove() ) {
	    undoRedoInfo.d->text.remove( 0, 1 );
	    undoRedoInfo.d->text.prepend( "\n" );
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
	    undoRedoInfo.d->text = QString::null;
	}
	undoRedoInfo.d->text += "\n";
	cursor->splitAndInsertEmptyParag();
	if ( cursor->parag()->prev() )
	    lastFormatted = cursor->parag()->prev();
	break;
    case ActionKill:
	checkUndoRedoInfo( UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.d->text = QString::null;
	}
	if ( cursor->atParagEnd() ) {
	    undoRedoInfo.d->text += cursor->parag()->at( cursor->index() )->c;
	    if ( cursor->parag()->at( cursor->index() )->format() ) {
		cursor->parag()->at( cursor->index() )->format()->addRef();
		undoRedoInfo.d->text.at( undoRedoInfo.d->text.length() - 1 ).setFormat( cursor->parag()->at( cursor->index() )->format() );
	    }
	    if ( cursor->remove() )
		undoRedoInfo.d->text += "\n";
	} else {
	    int oldLen = undoRedoInfo.d->text.length();
	    undoRedoInfo.d->text += cursor->parag()->string()->toString().mid( cursor->index() );
	    for ( int i = cursor->index(); i < cursor->parag()->length(); ++i ) {
		if ( cursor->parag()->at( i )->format() ) {
		    cursor->parag()->at( i )->format()->addRef();
		    undoRedoInfo.d->text.at( oldLen + i - cursor->index() ).setFormat( cursor->parag()->at( i )->format() );
		}
	    }
	    undoRedoInfo.d->text.remove( undoRedoInfo.d->text.length() - 1, 1 );
	    cursor->killLine();
	}
	break;
    }

    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );

    updateCurrentFormat();
    emit textChanged();
}

void QTextView::readFormats( QTextCursor &c1, QTextCursor &c2, int oldLen, QTextString &text, bool fillStyles )
{
    c2.restoreState();
    c1.restoreState();
    if ( c1.parag() == c2.parag() ) {
	for ( int i = c1.index(); i < c2.index(); ++i ) {
	    if ( c1.parag()->at( i )->format() ) {
		c1.parag()->at( i )->format()->addRef();
		text.at( oldLen + i - c1.index() ).setFormat( c1.parag()->at( i )->format() );
	    }
	}
	if ( fillStyles ) {
	    undoRedoInfo.oldAligns[ 0 ] = c1.parag()->alignment();
	    undoRedoInfo.oldStyles << c1.parag()->styleSheetItems();
	    undoRedoInfo.oldListStyles << c1.parag()->listStyle();
	}
    } else {
	int lastIndex = oldLen;
	int i;
	for ( i = c1.index(); i < c1.parag()->length(); ++i ) {
	    if ( c1.parag()->at( i )->format() ) {
		c1.parag()->at( i )->format()->addRef();
		text.at( lastIndex ).setFormat( c1.parag()->at( i )->format() );
		lastIndex++;
	    }
	}
	lastIndex++;
	QTextParag *p = c1.parag()->next();
	while ( p && p != c2.parag() ) {
	    for ( int i = 0; i < p->length(); ++i ) {
		if ( p->at( i )->format() ) {
		    p->at( i )->format()->addRef();
		    text.at( i + lastIndex ).setFormat( p->at( i )->format() );
		}
	    }
	    lastIndex += p->length() + 1;
	    p = p->next();
	}
	for ( i = 0; i < c2.index(); ++i ) {
	    if ( c2.parag()->at( i )->format() ) {
		c2.parag()->at( i )->format()->addRef();
		text.at( i + lastIndex ).setFormat( c2.parag()->at( i )->format() );
	    }
	}
	if ( fillStyles ) {
	    QTextParag *p = c1.parag();
	    i = 0;
	    while ( p ) {
		if ( i < (int)undoRedoInfo.oldAligns.size() )
		    undoRedoInfo.oldAligns[ i ] = p->alignment();
		undoRedoInfo.oldStyles << p->styleSheetItems();
		undoRedoInfo.oldListStyles << p->listStyle();
		if ( p == c2.parag() )
		    break;
		p = p->next();
		++i;
	    }
	}
    }
}

void QTextView::removeSelectedText()
{
    if ( isReadOnly() )
	return;

    for ( int i = 1; i < (int)QTextDocument::Temp; ++i )
	doc->removeSelection( i );

    drawCursor( FALSE );
    checkUndoRedoInfo( UndoRedoInfo::RemoveSelected );
    if ( !undoRedoInfo.valid() ) {
	doc->selectionStart( QTextDocument::Standard, undoRedoInfo.id, undoRedoInfo.index );
	undoRedoInfo.d->text = QString::null;
    }
    int oldLen = undoRedoInfo.d->text.length();
    undoRedoInfo.d->text = doc->selectedText( QTextDocument::Standard );
    QTextCursor c1 = doc->selectionStartCursor( QTextDocument::Standard );
    QTextCursor c2 = doc->selectionEndCursor( QTextDocument::Standard );
    undoRedoInfo.oldAligns.resize( undoRedoInfo.oldAligns.size() + QMAX( 0, c2.parag()->paragId() - c1.parag()->paragId() + 1 ) );
    readFormats( c1, c2, oldLen, undoRedoInfo.d->text, TRUE );
    doc->removeSelectedText( QTextDocument::Standard, cursor );
    ensureCursorVisible();
    lastFormatted = cursor->parag();
    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );
    clearUndoRedo();
    emit textChanged();
#if defined(Q_WS_WIN)
    // there seems to be a problem with repainting or erasing the area
    // of the scrollview which is not the contents on windows
    if ( contentsHeight() < visibleHeight() )
	viewport()->repaint( 0, contentsHeight(), visibleWidth(), visibleHeight() - contentsHeight(), TRUE );
#endif
#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
}

void QTextView::moveCursor( MoveDirectionPrivate direction, bool shift, bool control )
{
    drawCursor( FALSE );
    if ( shift ) {
	if ( !doc->hasSelection( QTextDocument::Standard ) )
	    doc->setSelectionStart( QTextDocument::Standard, cursor );
	moveCursor( direction, control );
	if ( doc->setSelectionEnd( QTextDocument::Standard, cursor ) ) {
	    cursor->parag()->document()->nextDoubleBuffered = TRUE;
	    repaintChanged();
	} else {
	    drawCursor( TRUE );
	}
	ensureCursorVisible();
	emit selectionChanged();
	emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
    } else {
	bool redraw = doc->removeSelection( QTextDocument::Standard );
	moveCursor( direction, control );
	if ( !redraw ) {
	    ensureCursorVisible();
	    drawCursor( TRUE );
	} else {
	    cursor->parag()->document()->nextDoubleBuffered = TRUE;
	    repaintChanged();
	    ensureCursorVisible();
	    drawCursor( TRUE );
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
	if ( redraw ) {
	    emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
	    emit selectionChanged();
	}
    }

    drawCursor( TRUE );
    updateCurrentFormat();
}

void QTextView::moveCursor( MoveDirectionPrivate direction, bool control )
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
	    cursor->gotoPageUp( visibleHeight() );
    } break;
    case MoveDown: {
	if ( !control )
	    cursor->gotoDown();
	else
	    cursor->gotoPageDown( visibleHeight() );
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
	cursor->gotoPageUp( visibleHeight() );
	break;
    case MovePgDown:
	cursor->gotoPageDown( visibleHeight() );
	break;
    }

    updateCurrentFormat();
}

/*! \reimp */

void QTextView::resizeEvent( QResizeEvent *e )
{
    QScrollView::resizeEvent( e );
#if defined(_WS_X11_)
    if ( e->oldSize().width() != e->size().width() )
#endif
	doResize();
}

void QTextView::ensureCursorVisible()
{
    lastFormatted = cursor->parag();
    formatMore();
    QTextStringChar *chr = cursor->parag()->at( cursor->index() );
    int h = cursor->parag()->lineHeightOfChar( cursor->index() );
    int x = cursor->parag()->rect().x() + chr->x + cursor->offsetX();
    int y = 0; int dummy;
    cursor->parag()->lineHeightOfChar( cursor->index(), &dummy, &y );
    y += cursor->parag()->rect().y() + cursor->offsetY();
    int w = 1;
    ensureVisible( x, y + h / 2, w, h / 2 + 2 );
}

void QTextView::drawCursor( bool visible )
{
    if ( !cursor->parag() ||
	 !cursor->parag()->isValid() ||
	 ( !hasFocus() && !viewport()->hasFocus() && !inDnD ) ||
	 isReadOnly() )
	return;

    QPainter p( viewport() );
    QRect r( cursor->topParag()->rect() );
    cursor->parag()->setChanged( TRUE );
    p.translate( -contentsX() + cursor->totalOffsetX(), -contentsY() + cursor->totalOffsetY() );
    QPixmap *pix = 0;
    QColorGroup cg( colorGroup() );
    if ( cursor->parag()->background() )
	cg.setBrush( QColorGroup::Base, *cursor->parag()->background() );
    else if ( doc->paper() )
	cg.setBrush( QColorGroup::Base, *doc->paper() );
    p.setBrushOrigin( -contentsX(), -contentsY() );
    cursor->parag()->document()->nextDoubleBuffered = TRUE;
    doc->drawParag( &p, cursor->parag(), r.x() - cursor->totalOffsetX(),
		    r.y() - cursor->totalOffsetY(), r.width(), r.height(),
		    pix, cg, visible, cursor );
    cursorVisible = visible;
}

enum {
    IdUndo = 0,
    IdRedo = 1,
    IdCut = 2,
    IdCopy = 3,
    IdPaste = 4,
    IdClear = 5,
    IdSelectAll = 6
};

/*! \reimp */

void QTextView::contentsWheelEvent( QWheelEvent *e )
{
    if ( isReadOnly() ) {
	if ( e->state() & ControlButton ) {
	    if ( e->delta() > 0 )
		zoomOut();
	    else if ( e->delta() < 0 )
		zoomIn();
	    return;
	}
    }
    QScrollView::contentsWheelEvent( e );
}

/*! \reimp */

void QTextView::contentsMousePressEvent( QMouseEvent *e )
{
    clearUndoRedo();
    QTextCursor oldCursor = *cursor;
    QTextCursor c = *cursor;
    mousePos = e->pos();
    mightStartDrag = FALSE;
    pressedLink = QString::null;

    if ( !isReadOnly() && e->button() == RightButton ) {
	QPopupMenu *popup = createPopupMenu();
	int r = popup->exec( e->globalPos() );
	delete popup;

	if ( r == d->id[ IdClear ] )
	    clear();
	else if ( r == d->id[ IdSelectAll ] )
	    selectAll();
 	else if ( r == d->id[ IdUndo ] )
 	    undo();
 	else if ( r == d->id[ IdRedo ] )
 	    redo();
#ifndef QT_NO_CLIPBOARD
	else if ( r == d->id[ IdCut ] )
	    cut();
	else if ( r == d->id[ IdCopy ] )
	    copy();
	else if ( r == d->id[ IdPaste ] )
	    paste();
#endif
	return;
    }

    if ( e->button() == LeftButton ) {
	mousePressed = TRUE;
	drawCursor( FALSE );
	placeCursor( e->pos() );
	ensureCursorVisible();

	if ( isReadOnly() && linksEnabled() ) {
	    QTextCursor c = *cursor;
	    placeCursor( e->pos(), &c );
#ifndef QT_NO_NETWORKPROTOCOL
	    if ( c.parag() && c.parag()->at( c.index() ) &&
		 c.parag()->at( c.index() )->format()->isAnchor() ) {
#endif
		pressedLink = c.parag()->at( c.index() )->format()->anchorHref();
	    }
	}

#ifndef QT_NO_DRAGANDDROP
	if ( doc->inSelection( QTextDocument::Standard, e->pos() ) ) {
	    mightStartDrag = TRUE;
	    drawCursor( TRUE );
	    dragStartTimer->start( QApplication::startDragTime(), TRUE );
	    dragStartPos = e->pos();
	    return;
	}
#endif

	bool redraw = FALSE;
	if ( doc->hasSelection( QTextDocument::Standard ) ) {
	    emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
	    emit selectionChanged();
	    if ( !( e->state() & ShiftButton ) ) {
		redraw = doc->removeSelection( QTextDocument::Standard );
		doc->setSelectionStart( QTextDocument::Standard, cursor );
	    } else {
		redraw = doc->setSelectionEnd( QTextDocument::Standard, cursor ) || redraw;
	    }
	} else {
	    if ( !( e->state() & ShiftButton ) ) {
		doc->setSelectionStart( QTextDocument::Standard, cursor );
	    } else {
		doc->setSelectionStart( QTextDocument::Standard, &c );
		redraw = doc->setSelectionEnd( QTextDocument::Standard, cursor ) || redraw;
	    }
	}

	for ( int i = 1; i < QTextDocument::Temp; ++i ) // start with 1 as we don't want to remove the Standard-Selection
	    redraw = doc->removeSelection( i ) || redraw;

	if ( !redraw ) {
	    drawCursor( TRUE );
	} else {
	    repaintChanged();
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
    } else if ( e->button() == MidButton ) {
#if 0 // QT2HACK
	if (QApplication::clipboard()->supportsSelection()) {
	    // only do middle-click pasting on systems that have selections (ie. X11)
	    QApplication::clipboard()->setSelectionMode(TRUE);
#endif
	    paste();
#if 0 // QT2HACK
	    QApplication::clipboard()->setSelectionMode(FALSE);
	}
#endif
    }

    if ( *cursor != oldCursor )
	updateCurrentFormat();
}

/*! \reimp */

void QTextView::contentsMouseMoveEvent( QMouseEvent *e )
{
    if ( mousePressed ) {
#ifndef QT_NO_DRAGANDDROP
	if ( mightStartDrag ) {
	    dragStartTimer->stop();
	    if ( ( e->pos() - dragStartPos ).manhattanLength() > QApplication::startDragDistance() )
		startDrag();
	    if ( !isReadOnly() )
		viewport()->setCursor( ibeamCursor );
	    return;
	}
#endif
	mousePos = e->pos();
	doAutoScroll();
	oldMousePos = mousePos;
    }

    if ( !isReadOnly() && !mousePressed ) {
	if ( doc->hasSelection( QTextDocument::Standard ) && doc->inSelection( QTextDocument::Standard, e->pos() ) )
	    viewport()->setCursor( arrowCursor );
	else
	    viewport()->setCursor( ibeamCursor );
    }

    if ( isReadOnly() && linksEnabled() ) {
	QTextCursor c = *cursor;
	placeCursor( e->pos(), &c );
#ifndef QT_NO_NETWORKPROTOCOL
	if ( c.parag() && c.parag()->at( c.index() ) &&
	     c.parag()->at( c.index() )->format()->isAnchor() ) {
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( pointingHandCursor );
#endif
	    onLink = c.parag()->at( c.index() )->format()->anchorHref();
	    QUrl u( doc->context(), onLink, TRUE );
	    emitHighlighted( u.toString( FALSE, FALSE ) );
	} else {
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	    onLink = QString::null;
	    emitHighlighted( QString::null );
	}
#endif
    }
}

/*! \reimp */

void QTextView::contentsMouseReleaseEvent( QMouseEvent * )
{
    QTextCursor oldCursor = *cursor;
    if ( scrollTimer->isActive() )
	scrollTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    if ( dragStartTimer->isActive() )
	dragStartTimer->stop();
    if ( mightStartDrag ) {
	selectAll( FALSE );
	mousePressed = FALSE;
    }
#endif
    if ( mousePressed ) {
	mousePressed = FALSE;
#if 0 // QT2HACK
	if (QApplication::clipboard()->supportsSelection())
#endif
        {
#if 0 // QT2HACK
	    QApplication::clipboard()->setSelectionMode(TRUE);
#endif
	    // only do middle-click selection on systems that support it (ie. X11)
	    if ( !doc->selectedText( QTextDocument::Standard ).isEmpty() )
		doc->copySelectedText( QTextDocument::Standard );
#if 0 // QT2HACK
	    QApplication::clipboard()->setSelectionMode(FALSE);
#endif

	    emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
	    emit selectionChanged();
	}
    }
    emitCursorPositionChanged( cursor );
    if ( oldCursor != *cursor )
	updateCurrentFormat();
    inDoubleClick = FALSE;

#ifndef QT_NO_NETWORKPROTOCOL
    if ( !onLink.isEmpty() && onLink == pressedLink && linksEnabled() ) {
	QUrl u( doc->context(), onLink, TRUE );
	emitLinkClicked( u.toString( FALSE, FALSE ) );
    }
#endif
    drawCursor( TRUE );
}

/*! \reimp */

void QTextView::contentsMouseDoubleClickEvent( QMouseEvent * )
{
    QTextCursor c1 = *cursor;
    QTextCursor c2 = *cursor;
    c1.gotoWordLeft();
    c2.gotoWordRight();

    doc->setSelectionStart( QTextDocument::Standard, &c1 );
    doc->setSelectionEnd( QTextDocument::Standard, &c2 );

    *cursor = c2;

    repaintChanged();

    inDoubleClick = TRUE;
    mousePressed = TRUE;
}

#ifndef QT_NO_DRAGANDDROP

/*! \reimp */

void QTextView::contentsDragEnterEvent( QDragEnterEvent *e )
{
    e->acceptAction();
    inDnD = TRUE;
}

/*! \reimp */

void QTextView::contentsDragMoveEvent( QDragMoveEvent *e )
{
    drawCursor( FALSE );
    placeCursor( e->pos(),  cursor );
    drawCursor( TRUE );
    e->acceptAction();
}

/*! \reimp */

void QTextView::contentsDragLeaveEvent( QDragLeaveEvent * )
{
    inDnD = FALSE;
}

/*! \reimp */

void QTextView::contentsDropEvent( QDropEvent *e )
{
    if ( isReadOnly() )
	return;
    inDnD = FALSE;
    e->acceptAction();
    QString text;
    int i = -1;
    while ( ( i = text.find( '\r' ) ) != -1 )
	text.replace( i, 1, "" );
    if ( QTextDrag::decode( e, text ) ) {
	if ( ( e->source() == this ||
	       e->source() == viewport() ) &&
	     e->action() == QDropEvent::Move ) {
	    removeSelectedText();
	} else {
	    doc->removeSelection( QTextDocument::Standard );
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
	drawCursor( FALSE );
	placeCursor( e->pos(),  cursor );
	drawCursor( TRUE );
	insert( text, FALSE, TRUE, FALSE );
    }
}

#endif

void QTextView::doAutoScroll()
{
    if ( !mousePressed )
	return;

    QPoint pos( mapFromGlobal( QCursor::pos() ) );
    drawCursor( FALSE );
    QTextCursor oldCursor = *cursor;
    placeCursor( viewportToContents( pos ) );
    if ( inDoubleClick ) {
	QTextCursor cl = *cursor;
	cl.gotoWordLeft();
	QTextCursor cr = *cursor;
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
    ensureCursorVisible();

    bool redraw = FALSE;
    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	redraw = doc->setSelectionEnd( QTextDocument::Standard, cursor ) || redraw;
    }

    if ( !redraw ) {
	drawCursor( TRUE );
    } else {
	repaintChanged();
	drawCursor( TRUE );
    }

    if ( !scrollTimer->isActive() && pos.y() < 0 || pos.y() > height() )
	scrollTimer->start( 100, FALSE );
    else if ( scrollTimer->isActive() && pos.y() >= 0 && pos.y() <= height() )
	scrollTimer->stop();
}

void QTextView::placeCursor( const QPoint &pos, QTextCursor *c )
{
    if ( !c )
	c = cursor;

    c->restoreState();
    QTextParag *s = doc->firstParag();
    c->place( pos,  s );
}

void QTextView::formatMore()
{
    if ( !lastFormatted )
	return;

    int bottom = contentsHeight();
    int lastBottom = -1;
    int to = !sender() ? 2 : 20;
    bool firstVisible = FALSE;
    QRect cr( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
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

    if ( bottom > contentsHeight() && !cursor->document()->parent() )
	resizeContents( contentsWidth(), QMAX( doc->height(), bottom ) );
    else if ( lastBottom != -1 && lastBottom < contentsHeight() && !cursor->document()->parent() )
	resizeContents( contentsWidth(), QMAX( doc->height(), lastBottom ) );

    if ( lastFormatted )
	formatTimer->start( interval, TRUE );
    else
	interval = QMAX( 0, interval );
}

void QTextView::doResize()
{
    if ( wrapMode != WidgetWidth )
	return;
    doc->setMinimumWidth( -1, 0 );
    resizeContents( width() - verticalScrollBar()->width(), contentsHeight() );
    QScrollView::setHScrollBarMode( AlwaysOff );
    doc->setWidth( visibleWidth() );
    wrapWidth = visibleWidth();
    doc->invalidate();
    viewport()->repaint( FALSE );
    lastFormatted = doc->firstParag();
    interval = 0;
    formatMore();
}

/* \internal */

void QTextView::doChangeInterval()
{
    interval = 0;
}

/*! \reimp */

bool QTextView::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return TRUE;

    if ( o == this || o == viewport() ) {
	if ( e->type() == QEvent::FocusIn ) {
	    blinkTimer->start( QApplication::cursorFlashTime() / 2 );
	    return TRUE;
	} else if ( e->type() == QEvent::FocusOut ) {
	    blinkTimer->stop();
	    drawCursor( FALSE );
	    return TRUE;
	}
    }

    return QScrollView::eventFilter( o, e );
}

void QTextView::insert( const QString &text, bool indent, bool checkNewLine, bool removeSelected )
{
    if ( cursor->nestedDepth() != 0 ) // #### for 3.0, disable editing of tables as this is not advanced enough
	return;
    QTextCursor c2 = *cursor;
    QString txt( text );
    drawCursor( FALSE );
    if ( !isReadOnly() && doc->hasSelection( QTextDocument::Standard ) && removeSelected ) {
	checkUndoRedoInfo( UndoRedoInfo::RemoveSelected );
	if ( !undoRedoInfo.valid() ) {
	    doc->selectionStart( QTextDocument::Standard, undoRedoInfo.id, undoRedoInfo.index );
	    undoRedoInfo.d->text = QString::null;
	}
	undoRedoInfo.d->text = doc->selectedText( QTextDocument::Standard );
	doc->removeSelectedText( QTextDocument::Standard, cursor );
    }
    checkUndoRedoInfo( UndoRedoInfo::Insert );
    if ( !undoRedoInfo.valid() ) {
	undoRedoInfo.id = cursor->parag()->paragId();
	undoRedoInfo.index = cursor->index();
	undoRedoInfo.d->text = QString::null;
    }
    int oldLen = undoRedoInfo.d->text.length();
    lastFormatted = checkNewLine && cursor->parag()->prev() ?
		    cursor->parag()->prev() : cursor->parag();
    int idx = cursor->index();
    QTextCursor oldCursor = *cursor;
    cursor->insert( txt, checkNewLine );
    if ( doc->useFormatCollection() )
	cursor->parag()->setFormat( idx, txt.length(), currentFormat, TRUE );

    if ( indent && ( txt == "{" || txt == "}" ) )
	cursor->indent();
    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );
    undoRedoInfo.d->text += txt;

    if ( !doc->preProcessor() ) {
	for ( int i = 0; i < (int)txt.length(); ++i ) {
	    if ( txt[ i ] == '\n' )
		continue;
	    if ( c2.parag()->at( c2.index() )->format() ) {
		c2.parag()->at( c2.index() )->format()->addRef();
		undoRedoInfo.d->text.setFormat( oldLen + i, c2.parag()->at( c2.index() )->format(), TRUE );
	    }
	    c2.gotoRight();
	}
    }

    emit textChanged();
    if ( !removeSelected ) {
	doc->setSelectionStart( QTextDocument::Standard, &oldCursor );
	doc->setSelectionEnd( QTextDocument::Standard, cursor );
	repaintChanged();
    }
}

void QTextView::undo()
{
    if ( isReadOnly() )
	return;

    for ( int i = 0; i < (int)QTextDocument::Temp; ++i )
	doc->removeSelection( i );

#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif

    clearUndoRedo();
    drawCursor( FALSE );
    QTextCursor *c = doc->undo( cursor );
    if ( !c ) {
	drawCursor( TRUE );
	return;
    }
    lastFormatted = 0;
    ensureCursorVisible();
    repaintChanged();
    drawCursor( TRUE );
    emit textChanged();
}

void QTextView::redo()
{
    if ( isReadOnly() )
	return;

    for ( int i = 0; i < (int)QTextDocument::Temp; ++i )
	doc->removeSelection( i );

#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif

    clearUndoRedo();
    drawCursor( FALSE );
    QTextCursor *c = doc->redo( cursor );
    if ( !c ) {
	drawCursor( TRUE );
	return;
    }
    lastFormatted = 0;
    ensureCursorVisible();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );
    emit textChanged();
}

void QTextView::paste()
{
#ifndef QT_NO_CLIPBOARD
    if ( isReadOnly() )
	return;
    pasteSubType( "plain" );
#endif
}

void QTextView::checkUndoRedoInfo( UndoRedoInfo::Type t )
{
    if ( undoRedoInfo.valid() && t != undoRedoInfo.type ) {
	clearUndoRedo();
    }
    undoRedoInfo.type = t;
}

/*! Repaints the changed paragraphs. This is needed for many
  operations, but you normally never need to call that yourself.
*/

void QTextView::repaintChanged()
{
    QPainter p( viewport() );
    p.translate( -contentsX(), -contentsY() );
    paintDocument( FALSE, &p, contentsX(), contentsY(), visibleWidth(), visibleHeight() );
}

void QTextView::cut()
{
    if ( isReadOnly() )
	return;

    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	doc->copySelectedText( QTextDocument::Standard );
	removeSelectedText();
    }
}

/*! Copies the selected text (if there is any) to the clipboard.
 */

void QTextView::copy()
{
    if ( !doc->selectedText( QTextDocument::Standard ).isEmpty() )
	doc->copySelectedText( QTextDocument::Standard );
}

void QTextView::indent()
{
    if ( isReadOnly() )
	return;

    drawCursor( FALSE );
    if ( !doc->hasSelection( QTextDocument::Standard ) )
	cursor->indent();
    else
	doc->indentSelection( QTextDocument::Standard );
    repaintChanged();
    drawCursor( TRUE );
    emit textChanged();
}

/*! Reimplemented to allow tabbing through links
 */

bool QTextView::focusNextPrevChild( bool n )
{
    if ( !isReadOnly() || !linksEnabled() )
	return FALSE;
    bool b = doc->focusNextPrevChild( n );
    if ( b ) {
	repaintChanged();
	makeParagVisible( doc->focusIndicator.parag );
    }
    return b;
}

void QTextView::setFormat( QTextFormat *f, int flags )
{
    if ( isReadOnly() )
	return;

    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	drawCursor( FALSE );
	QString str = doc->selectedText( QTextDocument::Standard );
	QTextCursor c1 = doc->selectionStartCursor( QTextDocument::Standard );
	QTextCursor c2 = doc->selectionEndCursor( QTextDocument::Standard );
	clearUndoRedo();
	undoRedoInfo.type = UndoRedoInfo::Format;
	undoRedoInfo.id = c1.parag()->paragId();
	undoRedoInfo.index = c1.index();
	undoRedoInfo.eid = c2.parag()->paragId();
	undoRedoInfo.eindex = c2.index();
	undoRedoInfo.d->text = str;
	readFormats( c1, c2, 0, undoRedoInfo.d->text );
	undoRedoInfo.format = f;
	undoRedoInfo.flags = flags;
	clearUndoRedo();
	doc->setFormat( QTextDocument::Standard, f, flags );
	repaintChanged();
	formatMore();
	drawCursor( TRUE );
	emit textChanged();
    }
    if ( currentFormat && currentFormat->key() != f->key() ) {
	currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( f );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(), currentFormat->color() );
	}
	emitCurrentFontChanged( currentFormat->font() );
	emitCurrentColorChanged( currentFormat->color() );
	if ( cursor->index() == cursor->parag()->length() - 1 ) {
	    currentFormat->addRef();
	    cursor->parag()->string()->setFormat( cursor->index(), currentFormat, TRUE );
	}
    }
}

/* \reimp */

void QTextView::setPalette( const QPalette &p )
{
    QScrollView::setPalette( p );
    if ( textFormat() == PlainText ) {
	QTextFormat *f = doc->formatCollection()->defaultFormat();
	f->setColor( colorGroup().text() );
	viewport()->repaint( FALSE );
    }
}

void QTextView::setParagType( QStyleSheetItem::DisplayMode dm, QStyleSheetItem::ListStyle listStyle )
{
    if ( isReadOnly() )
	return;

    drawCursor( FALSE );
    if ( !doc->hasSelection( QTextDocument::Standard ) ) {
	clearUndoRedo();
	undoRedoInfo.type = UndoRedoInfo::ParagType;
	QValueList< QVector<QStyleSheetItem> > oldStyles;
	undoRedoInfo.oldStyles.clear();
	undoRedoInfo.oldStyles << cursor->parag()->styleSheetItems();
	undoRedoInfo.oldListStyles.clear();
	undoRedoInfo.oldListStyles << cursor->parag()->listStyle();
	undoRedoInfo.list = dm == QStyleSheetItem::DisplayListItem;
	undoRedoInfo.listStyle = listStyle;
	undoRedoInfo.id = cursor->parag()->paragId();
	undoRedoInfo.eid = cursor->parag()->paragId();
	undoRedoInfo.d->text = " ";
	undoRedoInfo.index = 1;
	clearUndoRedo();
	cursor->parag()->setList( dm == QStyleSheetItem::DisplayListItem, listStyle );
	repaintChanged();
    } else {
	QTextParag *start = doc->selectionStart( QTextDocument::Standard );
	QTextParag *end = doc->selectionEnd( QTextDocument::Standard );
	lastFormatted = start;
	clearUndoRedo();
	undoRedoInfo.type = UndoRedoInfo::ParagType;
	undoRedoInfo.id = start->paragId();
	undoRedoInfo.eid = end->paragId();
	undoRedoInfo.list = dm == QStyleSheetItem::DisplayListItem;
	undoRedoInfo.listStyle = listStyle;
	undoRedoInfo.oldStyles.clear();
	undoRedoInfo.oldListStyles.clear();
	while ( start ) {
	    undoRedoInfo.oldStyles << start->styleSheetItems();
	    undoRedoInfo.oldListStyles << start->listStyle();
	    start->setList( dm == QStyleSheetItem::DisplayListItem, listStyle );
	    if ( start == end )
		break;
	    start = start->next();
	}
	undoRedoInfo.d->text = " ";
	undoRedoInfo.index = 1;
	clearUndoRedo();
	repaintChanged();
	formatMore();
    }
    drawCursor( TRUE );
    emit textChanged();
}

void QTextView::setAlignment( int a )
{
    if ( isReadOnly() || block_set_alignment )
	return;

    drawCursor( FALSE );
    if ( !doc->hasSelection( QTextDocument::Standard ) ) {
	if ( cursor->parag()->alignment() != a ) {
	    clearUndoRedo();
	    undoRedoInfo.type = UndoRedoInfo::Alignment;
	    QArray<int> oa( 1 );
	    oa[ 0 ] = cursor->parag()->alignment();
	    undoRedoInfo.oldAligns = oa;
	    undoRedoInfo.newAlign = a;
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.eid = cursor->parag()->paragId();
	    undoRedoInfo.d->text = " ";
	    undoRedoInfo.index = 1;
	    clearUndoRedo();
	    cursor->parag()->setAlignment( a );
	    repaintChanged();
	}
    } else {
	QTextParag *start = doc->selectionStart( QTextDocument::Standard );
	QTextParag *end = doc->selectionEnd( QTextDocument::Standard );
	lastFormatted = start;
	int len = end->paragId() - start->paragId() + 1;
	clearUndoRedo();
	undoRedoInfo.type = UndoRedoInfo::Alignment;
	undoRedoInfo.id = start->paragId();
	undoRedoInfo.eid = end->paragId();
	QArray<int> oa( QMAX( 0, len ) );
	int i = 0;
	while ( start ) {
	    if ( i < (int)oa.size() )
		oa[ i ] = start->alignment();
	    start->setAlignment( a );
	    if ( start == end )
		break;
	    start = start->next();
	    ++i;
	}
	undoRedoInfo.oldAligns = oa;
	undoRedoInfo.newAlign = a;
	undoRedoInfo.d->text = " ";
	undoRedoInfo.index = 1;
	clearUndoRedo();
	repaintChanged();
	formatMore();
    }
    drawCursor( TRUE );
    if ( currentAlignment != a ) {
	currentAlignment = a;
	emitCurrentAlignmentChanged( currentAlignment );
    }
    emit textChanged();
}

void QTextView::updateCurrentFormat()
{
    int i = cursor->index();
    if ( i > 0 )
	--i;
    if ( currentFormat->key() != cursor->parag()->at( i )->format()->key() && doc->useFormatCollection() ) {
	if ( currentFormat )
	    currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( cursor->parag()->at( i )->format() );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(), currentFormat->color() );
	}
	emitCurrentFontChanged( currentFormat->font() );
	emitCurrentColorChanged( currentFormat->color() );
    }

    if ( currentAlignment != cursor->parag()->alignment() ) {
	currentAlignment = cursor->parag()->alignment();
	block_set_alignment = TRUE;
	emitCurrentAlignmentChanged( currentAlignment );
	block_set_alignment = FALSE;
    }
}

void QTextView::setItalic( bool b )
{
    QTextFormat f( *currentFormat );
    f.setItalic( b );
    setFormat( &f, QTextFormat::Italic );
}

void QTextView::setBold( bool b )
{
    QTextFormat f( *currentFormat );
    f.setBold( b );
    setFormat( &f, QTextFormat::Bold );
}

void QTextView::setUnderline( bool b )
{
    QTextFormat f( *currentFormat );
    f.setUnderline( b );
    setFormat( &f, QTextFormat::Underline );
}

void QTextView::setFamily( const QString &f_ )
{
    QTextFormat f( *currentFormat );
    f.setFamily( f_ );
    setFormat( &f, QTextFormat::Family );
}

void QTextView::setPointSize( int s )
{
    QTextFormat f( *currentFormat );
    f.setPointSize( s );
    setFormat( &f, QTextFormat::Size );
}

void QTextView::setColor( const QColor &c )
{
    QTextFormat f( *currentFormat );
    f.setColor( c );
    setFormat( &f, QTextFormat::Color );
}

void QTextView::setFontInternal( const QFont &f_ )
{
    QTextFormat f( *currentFormat );
    f.setFont( f_ );
    setFormat( &f, QTextFormat::Font );
}

/*! Returns the contents of the view.

  If the view is readonly (i.e. it is a QTextView or QTextBrowser),
  exactly the same contents as you did set is returned. If it is
  editable (i.e. it is a QTextEdit), the current contens is set, and
  depending on textFormat() the text will contain HTML formatting tags
  or not.
 */

QString QTextView::text() const
{
    if ( isReadOnly() )
	return doc->originalText();
    return doc->text();
}

/*! Returns the text of the paragraph \a parag. Depending on
  textFormat(), the returned string contains formatting HTML tags or
  not.
*/

QString QTextView::text( int parag ) const
{
    return doc->text( parag );
}

/*!  Changes the contents of the view to the string \a text and the
  context to \a context.

  \a text may be interpreted either as plain text or as rich text,
  depending on the textFormat(). The default setting is \c AutoText,
  i.e. the text view autodetects the format from \a text.

  The optional \a context is used to resolve references within the
  text document, for example image sources. It is passed directly to
  the mimeSourceFactory() when quering data.

  \sa text(), setTextFormat()
*/

void QTextView::setText( const QString &text, const QString &context )
{
    emitUndoAvailable( FALSE );
    emitRedoAvailable( FALSE );
    undoRedoInfo.clear();
    doc->commands()->clear();

    lastFormatted = 0;
    cursor->restoreState();
    doc->setText( text, context );
    doc->setMinimumWidth( -1, 0 );
    resizeContents( 0, 0 );
    cursor->setDocument( doc );
    cursor->setParag( doc->firstParag() );
    cursor->setIndex( 0 );

    if ( qApp->font().pointSize() != QScrollView::font().pointSize() )
	setFont( QScrollView::font() );

    viewport()->repaint( FALSE );
    emit textChanged();
    formatMore();

}

/*! If you used load() to load and set the contents, this function
  returnes the filename you specified there. Else an empty string is
  returned.
*/

QString QTextView::fileName() const
{
    return doc->fileName();
}

/*! Loads the file \a fn and displayes its contents. The contents is
  interprated depending to textFormat()
*/

void QTextView::load( const QString &fn )
{
    resizeContents( 0, 0 );
    doc->load( fn );
    cursor->setParag( doc->firstParag() );
    cursor->setIndex( 0 );
    viewport()->repaint( FALSE );
    emit textChanged();
    doResize();
}

void QTextView::save( const QString &fn )
{
    doc->save( fn );
}

/*!  Finds the next occurance of \a expr after \a parag and \a
  index. If they are 0, the first occurance of \a expr os searched.
  \a parag and index are set to the position where \a expr has been
  found, if they are not 0. If \a expr couldn't be found, FALSE is
  returned, else TRUE is returned and the found part in the text is
  highlighted. \a cs specifies if the search should be case sensitive,
  \a wo specifies of only whole words are searched, and \a forward
  secifies the search direction.
*/

bool QTextView::find( const QString &expr, bool cs, bool wo, bool forward,
		      int *parag, int *index )
{
    drawCursor( FALSE );
    doc->removeSelection( QTextDocument::Standard );
#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
    bool found = doc->find( expr, cs, wo, forward, parag, index, cursor );
    ensureCursorVisible();
    drawCursor( TRUE );
    repaintChanged();
    return found;
}

void QTextView::blinkCursor()
{
    if ( !cursorVisible )
	return;
    bool cv = cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor( blinkCursorVisible );
    cursorVisible = cv;
}

void QTextView::setCursorPosition( int parag, int index )
{
    QTextParag *p = doc->paragAt( parag );
    if ( !p )
	return;

    if ( index > p->length() - 1 )
	index = p->length() - 1;

    drawCursor( FALSE );
    cursor->setParag( p );
    cursor->setIndex( index );
    ensureCursorVisible();
    drawCursor( TRUE );
}

void QTextView::getCursorPosition( int &parag, int &index ) const
{
    parag = cursor->parag()->paragId();
    index = cursor->index();
}

void QTextView::setSelection( int parag_from, int index_from,
			      int parag_to, int index_to )
{
    QTextParag *p1 = doc->paragAt( parag_from );
    if ( !p1 )
	return;
    QTextParag *p2 = doc->paragAt( parag_to );
    if ( !p2 )
	return;

    if ( index_from > p1->length() - 1 )
	index_from = p1->length() - 1;
    if ( index_to > p2->length() - 1 )
	index_to = p2->length() - 1;

    drawCursor( FALSE );
    QTextCursor c = *cursor;
    c.setParag( p1 );
    c.setIndex( index_from );
    cursor->setParag( p2 );
    cursor->setIndex( index_to );
    doc->setSelectionStart( QTextDocument::Standard, &c );
    doc->setSelectionEnd( QTextDocument::Standard, cursor );
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );
}

/*!  Sets the specified parameters to the currently selection
  (selection start and end). If there is currently now selection, all
  parameters are set to -1
*/

void QTextView::getSelection( int &parag_from, int &index_from,
			      int &parag_to, int &index_to ) const
{
    if ( !doc->hasSelection( QTextDocument::Standard ) ) {
	parag_from = -1;
	index_from = -1;
	parag_to = -1;
	index_to = -1;
	return;
    }

    doc->selectionStart( QTextDocument::Standard, parag_from, index_from );
    doc->selectionEnd( QTextDocument::Standard, parag_from, index_from );
}

/*!  Sets the text format to \a format. Possible choices are

  <ul>

  <li> \c PlainText - all characters are displayed verbatim, including
  all blanks and linebreaks.

  <li> \c RichText - rich text rendering. The available styles are
  defined in the default stylesheet QStyleSheet::defaultSheet().

  <li> \c AutoText - this is also the default. The view autodetects
  which rendering style suits best, \c PlainText or \c
  RichText. Technically, this is done by using the
  QStyleSheet::mightBeRichText() heuristic.

  </ul>
*/

void QTextView::setTextFormat( TextFormat format )
{
    doc->setTextFormat( format );
}

/*!  Returns the current text format.

  \sa setTextFormat()
 */

Qt::TextFormat QTextView::textFormat() const
{
    return doc->textFormat();
}

/*! Returns the number of paragraphs of the contents
 */

int QTextView::paragraphs() const
{
    return doc->lastParag()->paragId() + 1;
}

/*! Returns the number of lines of the paragraph \a parag.
 */

int QTextView::linesOfParagraph( int parag ) const
{
    QTextParag *p = doc->paragAt( parag );
    if ( !p )
	return -1;
    return p->lines();
}

/*! Returns the number of lines in the view.

  WARNING: This function is slow. As lines change all the time during
  word wrapping, this function has to iterate over all paragraphs and
  ask for the number of lines of that.
 */

int QTextView::lines() const
{
    QTextParag *p = doc->firstParag();
    int l = 0;
    while ( p ) {
	l += p->lines();
	p = p->next();
    }

    return l;
}

/*! returns in which line of the paragraph \a parag the index \a chr
  is
*/

int QTextView::lineOfChar( int parag, int chr )
{
    QTextParag *p = doc->paragAt( parag );
    if ( !p )
	return -1;

    int idx, line;
    QTextStringChar *c = p->lineStartOfChar( chr, &idx, &line );
    if ( !c )
	return -1;

    return line;
}

void QTextView::setModified( bool m )
{
    if ( modified != m )
	emitModificationChanged( m );
    modified = m;
    if ( modified ) {
	disconnect( this, SIGNAL( textChanged() ),
		    this, SLOT( setModified() ) );
    } else {
	connect( this, SIGNAL( textChanged() ),
		 this, SLOT( setModified() ) );
    }
}

bool QTextView::isModified() const
{
    return modified;
}

void QTextView::setModified()
{
    setModified( TRUE );
}

bool QTextView::italic() const
{
    return currentFormat->font().italic();
}

bool QTextView::bold() const
{
    return currentFormat->font().bold();
}

bool QTextView::underline() const
{
    return currentFormat->font().underline();
}

QString QTextView::family() const
{
    return currentFormat->font().family();
}

int QTextView::pointSize() const
{
    return currentFormat->font().pointSize();
}

QColor QTextView::color() const
{
    return currentFormat->color();
}

QFont QTextView::font() const
{
    return currentFormat->font();
}

int QTextView::alignment() const
{
    return currentAlignment;
}

void QTextView::startDrag()
{
#ifndef QT_NO_DRAGANDDROP
    mousePressed = FALSE;
    inDoubleClick = FALSE;
    QDragObject *drag = new QTextDrag( doc->selectedText( QTextDocument::Standard ), viewport() );
    if ( isReadOnly() ) {
	drag->dragCopy();
    } else {
	if ( drag->drag() && QDragObject::target() != this && QDragObject::target() != viewport() ) {
	    doc->removeSelectedText( QTextDocument::Standard, cursor );
	    repaintChanged();
	}
    }
#endif
}

/*! Selectes the whole text, if \a select is TRUE, else clears the
  selection
*/

void QTextView::selectAll( bool select )
{
    if ( !select )
	doc->removeSelection( QTextDocument::Standard );
    else
	doc->selectAll( QTextDocument::Standard );
    repaintChanged();
    emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
    emit selectionChanged();
#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
}

void QTextView::UndoRedoInfo::clear()
{
    if ( valid() ) {
	if ( type == Insert || type == Return )
	    doc->addCommand( new QTextInsertCommand( doc, id, index, d->text.rawData(), oldStyles, oldListStyles, oldAligns ) );
	else if ( type == Format )
	    doc->addCommand( new QTextFormatCommand( doc, id, index, eid, eindex, d->text.rawData(), format, flags ) );
	else if ( type == Alignment )
	    doc->addCommand( new QTextAlignmentCommand( doc, id, eid, newAlign, oldAligns ) );
	else if ( type == ParagType )
	    doc->addCommand( new QTextParagTypeCommand( doc, id, eid, list, listStyle, oldStyles, oldListStyles ) );
	else if ( type != Invalid )
	    doc->addCommand( new QTextDeleteCommand( doc, id, index, d->text.rawData(), oldStyles, oldListStyles, oldAligns ) );
    }
    d->text = QString::null;
    id = -1;
    index = -1;
    oldStyles.clear();
    oldListStyles.clear();
    oldAligns.resize( 0 );
}

/*! Deletes the character on the right side of the text cursor. If a
  text has been marked by the user (e.g. by clicking and dragging) the
  cursor is put at the beginning of the marked text and the marked
  text is removed.
*/

void QTextView::del()
{
    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	removeSelectedText();
	return;
    }

    doKeyboardAction( ActionDelete );
}


QTextView::UndoRedoInfo::UndoRedoInfo( QTextDocument *dc )
    : type( Invalid ), doc( dc )
{
    d = new QUndoRedoInfoPrivate;
    d->text = QString::null;
    id = -1;
    index = -1;
}

QTextView::UndoRedoInfo::~UndoRedoInfo()
{
    delete d;
}

bool QTextView::UndoRedoInfo::valid() const
{
    return d->text.length() > 0  && id >= 0 && index >= 0;
}

void QTextView::resetFormat()
{
    setAlignment( Qt3::AlignAuto );
    setParagType( QStyleSheetItem::DisplayBlock, QStyleSheetItem::ListDisc );
    setFormat( doc->formatCollection()->defaultFormat(), QTextFormat::Format );
}

/*! Returns the QStyleSheet which is currently used in this view.
 */

QStyleSheet* QTextView::styleSheet() const
{
    return doc->styleSheet();
}

/*! Sets the styleheet which should be used by this view.
 */

void QTextView::setStyleSheet( QStyleSheet* styleSheet )
{
    doc->setStyleSheet( styleSheet );
}

/*! Sets the paper which should be used for drawing the background.
 */

void QTextView::setPaper( const QBrush& pap )
{
    doc->setPaper( new QBrush( pap ) );
    viewport()->setBackgroundColor( pap.color() );
    viewport()->update();
}

/*! Returns the brush which is used for the background, or an empty
  one of there is none
*/

QBrush QTextView::paper() const
{
    if ( doc->paper() )
	return *doc->paper();
    return QBrush();
}

/*! Specifies the color which should be used for displaying
  links. This defaults to blue
*/

void QTextView::setLinkColor( const QColor &c )
{
    doc->setLinkColor( c );
}

/*! Returnes the color used for links.
 */

QColor QTextView::linkColor() const
{
    return doc->linkColor();
}

/*! Specifies whether links should be displayed underlined or
  not. This defaults to TRUE.
 */

void QTextView::setLinkUnderline( bool b )
{
    doc->setUnderlineLinks( b );
}

/*! Returns whether links are displayed underlined or not
 */

bool QTextView::linkUnderline() const
{
    return doc->underlineLinks();
}

/*! Sets the mimesource factory which should be used by this view.
 */

void QTextView::setMimeSourceFactory( QMimeSourceFactory* factory )
{
    doc->setMimeSourceFactory( factory );
}

/*! Returns the QMimeSourceFactory which is currently used by this
  view.
*/

QMimeSourceFactory* QTextView::mimeSourceFactory() const
{
    return doc->mimeSourceFactory();
}

/*! Returns how many pixles height are needed for this document of it
  would be \a w pixels wide.
*/

int QTextView::heightForWidth( int w ) const
{
    int oldw = doc->width();
    doc->doLayout( 0, w );
    int h = doc->height();
    doc->setWidth( oldw );
    doc->invalidate();
    ( (QTextView*)this )->formatMore();
    return h;
}

/*! Appends \a text at the end of the view.
 */

void QTextView::append( const QString &text )
{
    doc->removeSelection( QTextDocument::Standard );
    TextFormat f = doc->textFormat();
    if ( f == AutoText ) {
	if ( QStyleSheet::mightBeRichText( text ) )
	    f = RichText;
	else
	    f = PlainText;
    }
    if ( f == PlainText ) {
	QTextCursor oldc( *cursor );
	cursor->gotoEnd();
	insert( text, FALSE, TRUE );
	*cursor = oldc;
    } else if ( f == RichText ) {
	doc->setRichTextInternal( text );
	repaintChanged();
    }
}

/*! Returns whether there is some text selected.
 */

bool QTextView::hasSelectedText() const
{
    return doc->hasSelection( QTextDocument::Standard );
}

/*! Returns the selected text, if there is one, else an empty
 string. Up to now only plain text is supported here. In the future
 some kind of HTML subset might be returned here depending on
 textFormat().
 */

QString QTextView::selectedText() const
{
    return doc->selectedText( QTextDocument::Standard );
}

void QTextView::handleReadOnlyKeyEvent( QKeyEvent *e )
{
    switch( e->key() ) {
    case Key_Down:
	setContentsPos( contentsX(), contentsY() + 10 );
	break;
    case Key_Up:
	setContentsPos( contentsX(), contentsY() - 10 );
	break;
    case Key_Left:
	setContentsPos( contentsX() - 10, contentsY() );
	break;
    case Key_Right:
	setContentsPos( contentsX() + 10, contentsY() );
	break;
    case Key_PageUp:
	setContentsPos( contentsX(), contentsY() - visibleHeight() );
	break;
    case Key_PageDown:
	setContentsPos( contentsX(), contentsY() + visibleHeight() );
	break;
    case Key_Home:
	setContentsPos( contentsX(), 0 );
	break;
    case Key_End:
	setContentsPos( contentsX(), contentsHeight() - visibleHeight() );
	break;
#ifndef QT_NO_NETWORKPROTOCOL
    case Key_Return:
    case Key_Enter:
    case Key_Space: {
	if ( !doc->focusIndicator.href.isEmpty() ) {
	    QUrl u( doc->context(), doc->focusIndicator.href, TRUE );
	    emitLinkClicked( u.toString( FALSE, FALSE ) );
	}
    } break;
#endif
    default:
	break;
    }
}

/*!  Returns the context of the view.

  \sa text(), setText()
*/

QString QTextView::context() const
{
    return doc->context();
}

/*!  Returns the document title parsed from the content.
*/

QString QTextView::documentTitle() const
{
    return doc->attributes()[ "title" ];
}

void QTextView::makeParagVisible( QTextParag *p )
{
    setContentsPos( contentsX(), QMIN( p->rect().y(), contentsHeight() - visibleHeight() ) );
}

/*! Scrolls the view to make the anchor \a name visible, if it can be
  found in the document.
*/

void QTextView::scrollToAnchor( const QString& name )
{
    if ( name.isEmpty() )
	return;

    QTextParag *p = doc->firstParag();

    if ( !doc->lastParag()->isValid() ) {
	while ( p ) {
	    if ( !p->isValid() )
		p->format();
	    p = p->next();
	}
	resizeContents( contentsWidth(), doc->height() );
    }

    p = doc->firstParag();
    while ( p ) {
	for ( int i = 0; i < p->length(); ++i ) {
	    if ( p->at( i )->format()->isAnchor() &&
		 p->at( i )->format()->anchorName() == name ) {
		makeParagVisible( p );
		return;
	    }
	}
	p = p->next();
    }
}

/*! If there is an anchor at the position \a pos (in contents
  coordinates), this one is returned, else an empty string
*/

QString QTextView::anchorAt( const QPoint& pos )
{
    QTextCursor c( doc );
    placeCursor( pos, &c );
    return c.parag()->at( c.index() )->format()->anchorHref();
}

void QTextView::setRealWidth( int w )
{
    w = QMAX( w, visibleWidth() - verticalScrollBar()->width() );
    resizeContents( w, contentsHeight() );
    QScrollView::setHScrollBarMode( setMode );
}

/*! If you changed something in the styleSheet(), use this function to
  tell the document to update all formats, etc.
*/

void QTextView::updateStyles()
{
    doc->updateStyles();
}

void QTextView::setDocument( QTextDocument *dc )
{
    if ( dc == doc )
	return;
    doc = dc;
    cursor->setDocument( doc );
    clearUndoRedo();
    lastFormatted = 0;
}

#ifndef QT_NO_CLIPBOARD

/*!
  Copies text in MIME subtype \a subtype from the clipboard onto the current
  cursor position.
  Any marked text is first deleted.
*/
void QTextView::pasteSubType( const QCString& subtype )
{
    QCString st = subtype;
    QString t = QApplication::clipboard()->text(st);
    if ( !t.isEmpty() ) {
#if defined(Q_OS_WIN32)
	// Need to convert CRLF to NL
	QRegExp crlf( QString::fromLatin1("\r\n") );
	t.replace( crlf, QChar('\n') );
#endif
	for ( int i=0; (uint) i<t.length(); i++ ) {
	    if ( t[ i ] < ' ' && t[ i ] != '\n' && t[ i ] != '\t' )
		t[ i ] = ' ';
	}
	if ( !t.isEmpty() )
	    insert( t, FALSE, TRUE );
    }
}

#ifndef QT_NO_MIMECLIPBOARD
/*!
  Prompts the user for a type from a list of text types available,
  Then copies text from the clipboard onto the current cursor position.
  Any marked text is first deleted.
*/
void QTextView::pasteSpecial( const QPoint& pt )
{
    QCString st = pickSpecial( QApplication::clipboard()->data(), TRUE, pt );
    if ( !st.isEmpty() )
	pasteSubType( st );
}
#endif
#ifndef QT_NO_MIME
QCString QTextView::pickSpecial( QMimeSource* ms, bool always_ask, const QPoint& pt )
{
    if ( ms )  {
	QPopupMenu popup( this );
	QString fmt;
	int n = 0;
	QDict<void> done;
	for (int i = 0; !( fmt = ms->format( i ) ).isNull(); i++) {
	    int semi = fmt.find( ";" );
	    if ( semi >= 0 )
		fmt = fmt.left( semi );
	    if ( fmt.left( 5 ) == "text/" ) {
		fmt = fmt.mid( 5 );
		if ( !done.find( fmt ) ) {
		    done.insert( fmt,(void*)1 );
		    popup.insertItem( fmt, i );
		    n++;
		}
	    }
	}
	if ( n ) {
	    int i = n ==1 && !always_ask ? popup.idAt( 0 ) : popup.exec( pt );
	    if ( i >= 0 )
		return popup.text(i).latin1();
	}
    }
    return QCString();
}
#endif // QT_NO_MIME
#endif // QT_NO_CLIPBOARD

/*! \enum QTextView::WordWrap

  This enum describes the QTextView's word wrap mode.  The following
  values are valid:

  \value NoWrap  no word wrap at all.

  \value WidgetWidth word wrap depending on the current width of the
  widget

  \value FixedPixelWidth wrap according to a fixed amount of pixels
  (see wrapColumnOrWidth() )

  \value FixedColumnWidth wrap according to a fixed character
  column. This is useful whenever you need formatted text that can
  also be displayed gracefully on devices with monospaced fonts, for
  example a standard VT100 terminal. In that case wrapColumnOrWidth()
  should typically be set to 80.

 \sa setWordWrap()
*/

/*!  Sets the word wrap mode.

  Per default, wrapping keeps words intact. To allow breaking within
  words, set the wrap policy to \c Anywhere (see setWrapPolicy() ).

  The default wrap mode is \c WidgetWidth.

  \sa wordWrap(), setWrapColumnOrWidth(), setWrapPolicy()
*/

void QTextView::setWordWrap( WordWrap mode )
{
    wrapMode = mode;
    switch ( mode ) {
    case NoWrap:
	document()->formatter()->setWrapEnabled( FALSE );
	document()->formatter()->setWrapAtColumn( -1 );
	break;
    case WidgetWidth:
	document()->formatter()->setWrapEnabled( TRUE );
	document()->formatter()->setWrapAtColumn( -1 );
	doResize();
	break;
    case FixedPixelWidth:
	document()->formatter()->setWrapEnabled( TRUE );
	document()->formatter()->setWrapAtColumn( -1 );
	setWrapColumnOrWidth( wrapWidth );
	break;
    case FixedColumnWidth:
	document()->formatter()->setWrapEnabled( TRUE );
	document()->formatter()->setWrapAtColumn( wrapWidth );
	setWrapColumnOrWidth( wrapWidth );
	break;
    }
}

/*!  Returns the current word wrap mode.

  \sa setWordWrap()
 */
QTextView::WordWrap QTextView::wordWrap() const
{
    return wrapMode;
}

/*!  Sets the wrap column or wrap width, depending on the word wrap
  mode.

  \sa setWordWrap()
 */

void QTextView::setWrapColumnOrWidth( int value )
{
    wrapWidth = value;
    if ( wrapMode == FixedColumnWidth ) {
	document()->formatter()->setWrapAtColumn( wrapWidth );
    } else {
	document()->formatter()->setWrapAtColumn( -1 );
	resizeContents( wrapWidth, contentsHeight() );
	doc->setWidth( wrapWidth );
	doc->invalidate();
	viewport()->repaint( FALSE );
	lastFormatted = doc->firstParag();
	interval = 0;
	formatMore();
    }
}

/*!  Returns the wrap column or wrap width, depending on the word wrap
  mode.

  \sa setWordWrap(), setWrapColumnOrWidth()
 */
int QTextView::wrapColumnOrWidth() const
{
    return wrapWidth;
}


/*! \enum QTextView::WrapPolicy

  Defines where text can be wrapped in word wrap mode.

  The following values are valid:
  \value AtWhiteSpace  break only after whitespace
  \value Anywhere  break anywhere

   \sa setWrapPolicy()
*/

/*!  Defines where text can be wrapped in word wrap mode.

   The default is \c AtWhiteSpace.

  \sa setWordWrap(), wrapPolicy()
 */

void QTextView::setWrapPolicy( WrapPolicy policy )
{
    if ( wPolicy == policy )
	return;
    QTextFormatter *formatter;
    if ( policy == AtWhiteSpace )
	formatter = new QTextFormatterBreakWords;
    else
	formatter = new QTextFormatterBreakInWords;
    formatter->setWrapAtColumn( document()->formatter()->wrapAtColumn() );
    formatter->setWrapEnabled( document()->formatter()->isWrapEnabled() );
    document()->setFormatter( formatter );
    doc->invalidate();
    viewport()->repaint( FALSE );
    lastFormatted = doc->firstParag();
    interval = 0;
    formatMore();
}

/*!

  Returns the current word wrap policy.

  \sa setWrapPolicy()
 */
QTextView::WrapPolicy QTextView::wrapPolicy() const
{
    return wPolicy;
}

/*! Clears the view.
 */

void QTextView::clear()
{
    cursor->restoreState();
    doc->clear( TRUE );
    cursor->setDocument( doc );
    cursor->setParag( doc->firstParag() );
    cursor->setIndex( 0 );
    viewport()->repaint( FALSE );
}

int QTextView::undoDepth() const
{
    return document()->undoDepth();
}

/*! Returns the number of characters of the text.
 */

int QTextView::length() const
{
    return document()->length();
}

/*! Returns the width of a tab as used be the view.
 */

int QTextView::tabStopWidth() const
{
    return document()->tabStopWidth();
}

void QTextView::setUndoDepth( int d )
{
    document()->setUndoDepth( d );
}

/*! Sets the tabstop width to \a ts.
 */

void QTextView::setTabStops( int ts )
{
    document()->setTabStops( ts );
}

/*! \reimp */

void  QTextView::setHScrollBarMode( ScrollBarMode sm )
{
    setMode = sm;
}

/*! \reimp */

QSize QTextView::sizeHint() const
{
    // ### calculate a reasonable one
    return QSize( 100, 100 );
}

void QTextView::clearUndoRedo()
{
    undoRedoInfo.clear();
    emitUndoAvailable( doc->commands()->isUndoAvailable() );
    emitRedoAvailable( doc->commands()->isRedoAvailable() );
}

bool QTextView::getFormat( int parag, int index, QFont &font, QColor &color )
{
    QTextParag *p = doc->paragAt( parag );
    if ( !p )
	return FALSE;
    if ( index < 0 || index >= p->length() )
	return FALSE;
    font = p->at( index )->format()->font();
    color = p->at( index )->format()->color();
    return TRUE;
}

/*! This function is called to create the popup menu which is shown
  when the user clicks on the textview with the right mouse button. If
  you want to create a custom popup menu, reimplement this function
  and return the created popup menu. The ownership is transferred to
  the caller.
*/

QPopupMenu *QTextView::createPopupMenu()
{
    QPopupMenu *popup = new QPopupMenu( this );
    d->id[ IdUndo ] = popup->insertItem( tr( "Undo" ) );
    d->id[ IdRedo ] = popup->insertItem( tr( "Redo" ) );
    popup->insertSeparator();
#ifndef QT_NO_CLIPBOARD
    d->id[ IdCut ] = popup->insertItem( tr( "Cut" ) );
    d->id[ IdCopy ] = popup->insertItem( tr( "Copy" ) );
    d->id[ IdPaste ] = popup->insertItem( tr( "Paste" ) );
#endif
    d->id[ IdClear ] = popup->insertItem( tr( "Clear" ) );
    popup->insertSeparator();
    d->id[ IdSelectAll ] = popup->insertItem( tr( "Select All" ) );
    popup->setItemEnabled( d->id[ IdUndo ], !isReadOnly() && doc->commands()->isUndoAvailable() );
    popup->setItemEnabled( d->id[ IdRedo ], !isReadOnly() && doc->commands()->isRedoAvailable() );
#ifndef QT_NO_CLIPBOARD
    popup->setItemEnabled( d->id[ IdCut ], !isReadOnly() && doc->hasSelection( QTextDocument::Standard ) );
    popup->setItemEnabled( d->id[ IdCopy ], doc->hasSelection( QTextDocument::Standard ) );
    popup->setItemEnabled( d->id[ IdPaste ], !isReadOnly() && !QApplication::clipboard()->text().isEmpty() );
#endif
    popup->setItemEnabled( d->id[ IdClear ], !isReadOnly() && !text().isEmpty() );
    popup->setItemEnabled( d->id[ IdSelectAll ], (bool)text().length() );
    return popup;
}

/*! \reimp */

void QTextView::setFont( const QFont &f )
{
    QScrollView::setFont( f );
    doc->setMinimumWidth( -1, 0 );

    // ### that is a bit hacky
    static short diff = 1;
    diff *= -1;
    doc->setWidth( visibleWidth() + diff );

    doc->updateFontSizes( f.pointSize() );
    lastFormatted = doc->firstParag();
    formatMore();
    repaintChanged();
}

/*! Zooms in the text by making the standard font size one point larger
  and recalculating all fontsizes. This does not change the size of
  images.
*/

void QTextView::zoomIn()
{
    QFont f( QScrollView::font() );
    f.setPointSize( f.pointSize() + 1 );
    setFont( f );
}

/*! Zooms out the text by making the standard font size one point
  smaller and recalculating all fontsizes. This does not change the
  size of images.
*/

void QTextView::zoomOut()
{
    QFont f( QScrollView::font() );
    f.setPointSize( QMAX( 1, f.pointSize() - 1 ) );
    setFont( f );
}

/* As the engine of QTextView is optimized for large amounts text, it
   is not sure that after e.g. calling setText() the whole document is
   formatted, as only the visible part is formatted immediately, and
   the rest delayed or on demand if needed.

   If you need some information (like contentsHeight() to get the
   height of the document) to be correct after e.g. calling setText(),
   call this function to ensure that the whole document has been
   formatted properly.
*/

void QTextView::sync()
{
    if ( !lastFormatted )
	return;
    QTextParag *p = lastFormatted;
    while ( p ) {
	p->format();
	p = p->next();
    }
    resizeContents( contentsWidth(), doc->height() );
}

/*! \reimp */

void QTextView::setEnabled( bool b )
{
    QScrollView::setEnabled( b );
    if ( textFormat() == PlainText ) {
	QTextFormat *f = doc->formatCollection()->defaultFormat();
	f->setColor( colorGroup().text() );
	viewport()->repaint( FALSE );
    }
}
