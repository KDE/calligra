/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kotextview.h"
#include "koparagcounter.h"
#include "kotextobject.h"
#include <klocale.h>
#include <kstdaccel.h>

KoTextView::KoTextView( KoTextObject *textobj )
{
    m_bReadWrite = true;
    m_textobj = textobj;

    connect( m_textobj, SIGNAL( hideCursor() ), this, SLOT( hideCursor() ) );
    connect( m_textobj, SIGNAL( showCursor() ), this, SLOT( showCursor() ) );
    connect( m_textobj, SIGNAL( setCursor( QTextCursor * ) ), this, SLOT( setCursor( QTextCursor * ) ) );
    connect( m_textobj, SIGNAL( updateUI(bool, bool) ), this, SLOT( updateUI(bool, bool) ) );
    connect( m_textobj, SIGNAL( showCurrentFormat() ), this, SLOT( showCurrentFormat() ) );
    connect( m_textobj, SIGNAL( ensureCursorVisible() ), this, SLOT( ensureCursorVisible() ) );

    m_cursor = new QTextCursor( m_textobj->textDocument() );

    m_cursorVisible = false;

    showCursor();
    blinkTimer = new QTimer( this );
    connect( blinkTimer, SIGNAL( timeout() ),
             this, SLOT( blinkCursor() ) );
    blinkTimer->start( QApplication::cursorFlashTime() / 2 );

    dragStartTimer = new QTimer( this );
    connect( dragStartTimer, SIGNAL( timeout() ),
             this, SLOT( startDrag() ) );

    m_textobj->formatMore();

    blinkCursorVisible = FALSE;
    inDoubleClick = FALSE;
    mightStartDrag = FALSE;

    m_currentFormat = 0;
    //updateUI( true, true );
}

KoTextView::~KoTextView()
{
    delete m_cursor;
}

void KoTextView::terminate()
{
    textObject()->clearUndoRedoInfo();
    if ( textDocument()->removeSelection( QTextDocument::Standard ) )
        textObject()->selectionChangedNotify();
    hideCursor();
}

void KoTextView::deleteWordForward()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
        textObject()->removeSelectedText( m_cursor );
        return;
    }
    textDocument()->setSelectionStart( QTextDocument::Standard, m_cursor );

    do {
        m_cursor->gotoRight();
    } while ( !m_cursor->atParagEnd()
              && !m_cursor->parag()->at( m_cursor->index() )->c.isSpace() );
    textDocument()->setSelectionEnd( QTextDocument::Standard, m_cursor );
    textObject()->removeSelectedText( m_cursor, QTextDocument::Standard, i18n("Remove word") );
}

void KoTextView::deleteWordBack()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
        textObject()->removeSelectedText( m_cursor );
        return;
    }
    textDocument()->setSelectionStart( QTextDocument::Standard, m_cursor );

    do {
        m_cursor->gotoLeft();
    } while ( !m_cursor->atParagStart()
              && !m_cursor->parag()->at( m_cursor->index()-1 )->c.isSpace() );
    textDocument()->setSelectionEnd( QTextDocument::Standard, m_cursor );
    textObject()->removeSelectedText( m_cursor, QTextDocument::Standard, i18n("Remove word") );
}

void KoTextView::handleKeyPressEvent( QKeyEvent * e )
{
    textObject()->typingStarted();

    /* bool selChanged = FALSE;
    for ( int i = 1; i < textDocument()->numSelections(); ++i )
        selChanged = textDocument()->removeSelection( i ) || selChanged;

    if ( selChanged ) {
        // m_cursor->parag()->document()->nextDoubleBuffered = TRUE; ######## we need that only if we have nested items/documents
        textFrameSet()->selectionChangedNotify();
    }*/

    bool clearUndoRedoInfo = TRUE;

    if ( KStdAccel::isEqual( e, KStdAccel::deleteWordBack() ) )
    {
        deleteWordBack();
        clearUndoRedoInfo = TRUE;
    } else if ( KStdAccel::isEqual( e, KStdAccel::deleteWordForward() ) )
    {
        deleteWordForward();
        clearUndoRedoInfo = TRUE;
    }
    else
    switch ( e->key() ) {
    case Key_Left:
        moveCursor( e->state() & ControlButton ? MoveWordBackward : MoveBackward, e->state() & ShiftButton );
        break;
    case Key_Right:
        moveCursor( e->state() & ControlButton ? MoveWordForward : MoveForward, e->state() & ShiftButton );
        break;
    case Key_Up:
        moveCursor( e->state() & ControlButton ? MoveParagUp : MoveUp, e->state() & ShiftButton );
        break;
    case Key_Down:
        moveCursor( e->state() & ControlButton ? MoveParagDown : MoveDown, e->state() & ShiftButton );
        break;
    case Key_Home:
        moveCursor( e->state() & ControlButton ? MoveHome : MoveLineStart, e->state() & ShiftButton );
        break;
    case Key_End:
        moveCursor( e->state() & ControlButton ? MoveEnd : MoveLineEnd, e->state() & ShiftButton );
        break;
    case Key_Prior:
        moveCursor( e->state() & ControlButton ? MovePgUp : MoveViewportUp, e->state() & ShiftButton );
        break;
    case Key_Next:
        moveCursor( e->state() & ControlButton ? MovePgDown : MoveViewportDown, e->state() & ShiftButton );
        break;
    case Key_Return: case Key_Enter:
        if ( textDocument()->hasSelection( QTextDocument::Standard ) )
            textObject()->removeSelectedText( m_cursor );
        clearUndoRedoInfo = FALSE;
        textObject()->doKeyboardAction( m_cursor, m_currentFormat, KoTextObject::ActionReturn );
        break;
    case Key_Delete:
        if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
            textObject()->removeSelectedText( m_cursor );
            break;
        }

        textObject()->doKeyboardAction( m_cursor, m_currentFormat, KoTextObject::ActionDelete );

        clearUndoRedoInfo = FALSE;
        break;
    case Key_Backspace:
        if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
            textObject()->removeSelectedText( m_cursor );
            break;
        }
        if ( !m_cursor->parag()->prev() &&
             m_cursor->atParagStart() )
        {
            KoTextParag * parag = static_cast<KoTextParag *>(m_cursor->parag());
            if ( parag->counter() && parag->counter()->style() != KoParagCounter::STYLE_NONE)
                textObject()->doKeyboardAction( m_cursor, m_currentFormat, KoTextObject::ActionBackspace );
            break;
        }
        textObject()->doKeyboardAction( m_cursor, m_currentFormat, KoTextObject::ActionBackspace );

        clearUndoRedoInfo = FALSE;
        break;
    case Key_F16: // Copy key on Sun keyboards
        emit copy();
        break;
    case Key_F18:  // Paste key on Sun keyboards
        emit paste();
        break;
    case Key_F20:  // Cut key on Sun keyboards
        emit cut();
        break;
    default: {
            //kdDebug() << "KoTextView::keyPressEvent ascii=" << e->ascii() << " text=" << e->text()[0].unicode() << endl;
            if ( e->text().length() &&
//               !( e->state() & AltButton ) &&
                 ( !e->ascii() || e->ascii() >= 32 ) ||
                 ( e->text() == "\t" && !( e->state() & ControlButton ) ) ) {
                clearUndoRedoInfo = FALSE;
                if ( e->key() == Key_Tab ) {
                    // We don't have support for nested counters at the moment.
                    /*if ( m_cursor->index() == 0 && m_cursor->parag()->style() &&
                         m_cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayListItem ) {
                        static_cast<KWTextParag * >(m_cursor->parag())->incDepth();
                        emit hideCursor();
                        emit repaintChanged();
                        emit showCursor();
                        break;
                    }*/
                }
                // Port to setCounter if we really want that - and make configurable
                /*if ( m_cursor->parag()->style() &&
                     m_cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayBlock &&
                     m_cursor->index() == 0 && ( e->text() == "-" || e->text() == "*" ) ) {
                    setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
                    break;
                }*/
                QString text = e->text();
                // Don't want a single undo/redo here.
                /*if(textObject()->hasSelection() )
                    frameSet()->kWordDocument()->addCommand(textObject()->replaceSelectionCommand(  m_cursor, text, QTextDocument::Standard , i18n("Insert Text")));
                else*/
                    textObject()->insert( m_cursor, m_currentFormat, text, false, true, i18n("Insert Text") );

#warning TODO autoformat
#if 0
                KWAutoFormat * autoFormat = textFrameSet()->kWordDocument()->getAutoFormat();
                if ( autoFormat )
                    autoFormat->doAutoFormat( m_cursor, static_cast<KWTextParag*>(m_cursor->parag()),
                                              m_cursor->index() - 1,
                                              text[ text.length() - 1 ] );
#endif
                break;
            }
            // We should use KAccel instead, to make this configurable !
            if ( e->state() & ControlButton ) {
                switch ( e->key() ) {
                case Key_F16: // Copy key on Sun keyboards
                    copy();
                    break;
                case Key_A:
                    moveCursor( MoveLineStart, e->state() & ShiftButton );
                    break;
                case Key_E:
                    moveCursor( MoveLineEnd, e->state() & ShiftButton );
                    break;
                case Key_K:
                    textObject()->doKeyboardAction( m_cursor, m_currentFormat, KoTextObject::ActionKill );
                    break;
                case Key_Insert:
                    copy();
                    break;
                }
                break;
            }
        }
    }

    if ( clearUndoRedoInfo ) {
        textObject()->clearUndoRedoInfo();
    }
    textObject()->typingDone();
}

void KoTextView::moveCursor( CursorAction action, bool select )
{
    hideCursor();
    if ( select ) {
        if ( !textDocument()->hasSelection( QTextDocument::Standard ) )
            textDocument()->setSelectionStart( QTextDocument::Standard, m_cursor );
        moveCursor( action );
        if ( textDocument()->setSelectionEnd( QTextDocument::Standard, m_cursor ) ) {
            //      m_cursor->parag()->document()->nextDoubleBuffered = TRUE; ##### we need that only if we have nested items/documents
            textObject()->selectionChangedNotify();
        } else {
            showCursor();
        }
    } else {
        bool redraw = textDocument()->removeSelection( QTextDocument::Standard );
        moveCursor( action );
        if ( redraw ) {
            //m_cursor->parag()->document()->nextDoubleBuffered = TRUE; // we need that only if we have nested items/documents
            textObject()->selectionChangedNotify();
        }
    }

    ensureCursorVisible();
    showCursor();
    updateUI( true );
}

void KoTextView::moveCursor( CursorAction action )
{
    switch ( action ) {
        case MoveBackward:
            m_cursor->gotoLeft();
            break;
        case MoveWordBackward:
            m_cursor->gotoWordLeft();
            break;
        case MoveForward:
            m_cursor->gotoRight();
            break;
        case MoveWordForward:
            m_cursor->gotoWordRight();
            break;
        case MoveUp:
            m_cursor->gotoUp();
            break;
        case MoveDown:
            m_cursor->gotoDown();
            break;
        case MoveViewportUp:
            pgUpKeyPressed();
            break;
        case MoveViewportDown:
            pgDownKeyPressed();
            break;
        case MovePgUp:
            ctrlPgUpKeyPressed();
            break;
        case MovePgDown:
            ctrlPgDownKeyPressed();
            break;
        case MoveLineStart:
            m_cursor->gotoLineStart();
            break;
        case MoveHome:
            m_cursor->gotoHome();
            break;
        case MoveLineEnd:
            m_cursor->gotoLineEnd();
            break;
        case MoveEnd:
            textObject()->ensureFormatted( textDocument()->lastParag() );
            m_cursor->gotoEnd();
            break;
        case MoveParagUp: {
            QTextParag * parag = m_cursor->parag()->prev();
            if ( parag )
            {
                m_cursor->setParag( parag );
                m_cursor->setIndex( 0 );
            }
        } break;
        case MoveParagDown: {
            QTextParag * parag = m_cursor->parag()->next();
            if ( parag )
            {
                m_cursor->setParag( parag );
                m_cursor->setIndex( 0 );
            }
        } break;
    }

    updateUI( true );
}

QTextCursor KoTextView::selectWordUnderCursor()
{
    QTextCursor c1 = *m_cursor;
    QTextCursor c2 = *m_cursor;
    if ( m_cursor->index() > 0 && !m_cursor->parag()->at( m_cursor->index()-1 )->c.isSpace() && !m_cursor->parag()->at( m_cursor->index()-1 )->isCustom())
        c1.gotoWordLeft();
    if ( !m_cursor->parag()->at( m_cursor->index() )->c.isSpace() && !m_cursor->atParagEnd() && !m_cursor->parag()->at( m_cursor->index() )->isCustom())
        c2.gotoWordRight();

    textDocument()->setSelectionStart( QTextDocument::Standard, &c1 );
    textDocument()->setSelectionEnd( QTextDocument::Standard, &c2 );
    return c2;
}

void KoTextView::handleMousePressEvent( QMouseEvent *e, const QPoint &iPoint )
{
    mightStartDrag = FALSE;
    hideCursor();

    QTextCursor oldCursor = *m_cursor;
    placeCursor( iPoint );
    ensureCursorVisible();

    if ( e->button() != LeftButton )
    {
        showCursor();
        return;
    }

    QTextDocument * textdoc = textDocument();
    if ( textdoc->inSelection( QTextDocument::Standard, iPoint ) ) {
        mightStartDrag = TRUE;
        m_textobj->emitShowCursor();
        dragStartTimer->start( QApplication::startDragTime(), TRUE );
        dragStartPos = e->pos();
        return;
    }

    bool redraw = FALSE;
    if ( textdoc->hasSelection( QTextDocument::Standard ) ) {
        if ( !( e->state() & ShiftButton ) ) {
            redraw = textdoc->removeSelection( QTextDocument::Standard );
            textdoc->setSelectionStart( QTextDocument::Standard, m_cursor );
        } else {
            redraw = textdoc->setSelectionEnd( QTextDocument::Standard, m_cursor ) || redraw;
        }
    } else {
        if ( !( e->state() & ShiftButton ) ) {
            textdoc->setSelectionStart( QTextDocument::Standard, m_cursor );
        } else {
            textdoc->setSelectionStart( QTextDocument::Standard, &oldCursor );
            redraw = textdoc->setSelectionEnd( QTextDocument::Standard, m_cursor ) || redraw;
        }
    }

    //for ( int i = 1; i < textdoc->numSelections(); ++i )
    //    redraw = textdoc->removeSelection( i ) || redraw;

    //kdDebug() << "KoTextView::mousePressEvent redraw=" << redraw << endl;
    if ( !redraw ) {
        showCursor();
    } else {
        textObject()->selectionChangedNotify();
    }
}

void KoTextView::handleMouseMoveEvent( QMouseEvent*, const QPoint& iPoint )
{
    hideCursor();
    QTextCursor oldCursor = *m_cursor;
    placeCursor( iPoint );

    // Double click + mouse still down + moving the mouse selects full words.
    if ( inDoubleClick ) {
        QTextCursor cl = *m_cursor;
        cl.gotoWordLeft();
        QTextCursor cr = *m_cursor;
        cr.gotoWordRight();

        int diff = QABS( oldCursor.parag()->at( oldCursor.index() )->x - iPoint.x() );
        int ldiff = QABS( cl.parag()->at( cl.index() )->x - iPoint.x() );
        int rdiff = QABS( cr.parag()->at( cr.index() )->x - iPoint.x() );

        if ( m_cursor->parag()->lineStartOfChar( m_cursor->index() ) !=
             oldCursor.parag()->lineStartOfChar( oldCursor.index() ) )
            diff = 0xFFFFFF;

        if ( rdiff < diff && rdiff < ldiff )
            *m_cursor = cr;
        else if ( ldiff < diff && ldiff < rdiff )
            *m_cursor = cl;
        else
            *m_cursor = oldCursor;

    }

    bool redraw = FALSE;
    if ( textDocument()->hasSelection( QTextDocument::Standard ) )
        redraw = textDocument()->setSelectionEnd( QTextDocument::Standard, m_cursor ) || redraw;
    else // it may be that the initial click was out of the frame
        textDocument()->setSelectionStart( QTextDocument::Standard, m_cursor );

    if ( redraw )
        textObject()->selectionChangedNotify( false );

    showCursor();
}

void KoTextView::handleMouseReleaseEvent()
{
    if ( dragStartTimer->isActive() )
        dragStartTimer->stop();
    if ( mightStartDrag ) {
        textObject()->selectAll( FALSE );
        mightStartDrag = false;
    }
    else
    {
        if ( textDocument()->selectionStartCursor( QTextDocument::Standard ) == textDocument()->selectionEndCursor( QTextDocument::Standard ) )
            textDocument()->removeSelection( QTextDocument::Standard );

        textObject()->selectionChangedNotify();
        // No auto-copy, will readd with Qt 3 using setSelectionMode(true/false)
        // But auto-copy in readonly mode, since there is no action available in that case.
        if ( !m_bReadWrite )
            emit copy();
    }

    inDoubleClick = FALSE;
    m_textobj->emitShowCursor();
}

void KoTextView::handleMouseDoubleClickEvent( QMouseEvent*, const QPoint& /* Currently unused */ )
{
    inDoubleClick = TRUE;
    *m_cursor = selectWordUnderCursor();
    textObject()->selectionChangedNotify();
    // No auto-copy, will readd with Qt 3 using setSelectionMode(true/false)
    // But auto-copy in readonly mode, since there is no action available in that case.
    if ( !m_bReadWrite )
        emit copy();
}

bool KoTextView::maybeStartDrag( QMouseEvent* e )
{
    if ( mightStartDrag ) {
        dragStartTimer->stop();
        if ( ( e->pos() - dragStartPos ).manhattanLength() > QApplication::startDragDistance() )
            startDrag();
        return true;
    }
    return false;
}

void KoTextView::placeCursor( const QPoint &pos )
{
    m_cursor->restoreState();
    QTextParag *s = textDocument()->firstParag();
    m_cursor->place( pos,  s );
    updateUI( true );
}

void KoTextView::blinkCursor()
{
    //kdDebug() << "KoTextView::blinkCursor m_cursorVisible=" << m_cursorVisible
    //          << " blinkCursorVisible=" << blinkCursorVisible << endl;
    if ( !m_cursorVisible )
        return;
    bool cv = m_cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor( blinkCursorVisible );
    m_cursorVisible = cv;
}

void KoTextView::drawCursor( bool visible )
{
    m_cursorVisible = visible;
    // The rest is up to the app ;)
}

void KoTextView::focusInEvent()
{
    blinkTimer->start( QApplication::cursorFlashTime() / 2 );
    showCursor();
}

void KoTextView::focusOutEvent()
{
    blinkTimer->stop();
    hideCursor();
}

void KoTextView::setDefaultFormat() {
    QTextFormatCollection * coll = textDocument()->formatCollection();
    KoTextFormat * format = static_cast<KoTextFormat *>(coll->defaultFormat());
    textObject()->setFormat( m_cursor, m_currentFormat, format, QTextFormat::Format );
}

void KoTextView::setBold( bool on ) {
    KoTextFormat format( *m_currentFormat );
    format.setBold( on );
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::Bold );
   //kdDebug(32003) << "KoTextView::setBold new m_currentFormat " << m_currentFormat << " " << m_currentFormat->key() << endl;
}

void KoTextView::setItalic( bool on ) {
    KoTextFormat format( *m_currentFormat );
    format.setItalic( on );
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::Italic );
}

void KoTextView::setUnderline( bool on ) {
    KoTextFormat format( *m_currentFormat );
    format.setUnderline( on );
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::Underline );
}

void KoTextView::setStrikeOut( bool on ) {
    KoTextFormat format( *m_currentFormat );
    format.setStrikeOut( on);
    textObject()->setFormat( m_cursor, m_currentFormat, &format, KoTextFormat::StrikeOut );
}

QColor KoTextView::textColor() const {
    return m_currentFormat->color();
}

QFont KoTextView::textFont() const {
    QFont fn( m_currentFormat->font() );
    fn.setPointSize( textObject()->docFontSize( m_currentFormat ) ); // "unzoom" the font size
    return fn;
}

QString KoTextView::textFontFamily()const {
    return m_currentFormat->font().family();
}

void KoTextView::setPointSize( int s ){
    KoTextFormat format( *m_currentFormat );
    format.setPointSize( s );
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::Size, true /* zoom the font size */ );
}

void KoTextView::setFamily(const QString &font){
    KoTextFormat format( *m_currentFormat );
    format.setFamily( font );
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::Family );
}

void KoTextView::setFont( const QFont &font, bool _subscript, bool _superscript, const QColor &col, int flags )
{
    KoTextFormat format( *m_currentFormat );
    format.setFont( font );
    format.setColor( col );

    if(!_subscript)
    {
        if(!_superscript)
            format.setVAlign(QTextFormat::AlignNormal);
        else
            format.setVAlign(QTextFormat::AlignSuperScript);
    }
    else
        format.setVAlign(QTextFormat::AlignSubScript);

    textObject()->setFormat( m_cursor, m_currentFormat, &format, flags, true /* zoom the font size */);
}

void KoTextView::setTextColor(const QColor &color) {
    KoTextFormat format( *m_currentFormat );
    format.setColor( color );
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::Color );
}

void KoTextView::setTextSubScript(bool on)
{
    KoTextFormat format( *m_currentFormat );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSubScript);
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::VAlign );
}

void KoTextView::setTextSuperScript(bool on)
{
    KoTextFormat format( *m_currentFormat );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSuperScript);
    textObject()->setFormat( m_cursor, m_currentFormat, &format, QTextFormat::VAlign );
}

void KoTextView::dragStarted()
{
    mightStartDrag = FALSE;
    inDoubleClick = FALSE;
}

void KoTextView::applyStyle( const KoStyle * style )
{
    textObject()->applyStyle( m_cursor, style );
    showCurrentFormat();
}

void KoTextView::showCurrentFormat()
{
    //kdDebug() << "KoTextView::showCurrentFormat currentFormat=" << currentFormat() << " " << currentFormat()->key() << endl;
    KoTextFormat format = *currentFormat();
    format.setPointSize( textObject()->docFontSize( currentFormat() ) ); // "unzoom" the font size
    showFormat( &format );
}

KCommand * KoTextView::setCounterCommand( const KoParagCounter & counter )
{
     return textObject()->setCounterCommand( m_cursor, counter );
}
KCommand * KoTextView::setAlignCommand( int align )
{
     return textObject()->setAlignCommand( m_cursor, align );
}
KCommand * KoTextView::setLineSpacingCommand( double spacing )
{
     return textObject()->setLineSpacingCommand( m_cursor, spacing );
}
KCommand * KoTextView::setBordersCommand( const KoBorder& leftBorder, const KoBorder& rightBorder, const KoBorder& bottomBorder, const KoBorder& topBorder )
{
    return textObject()->setBordersCommand( m_cursor, leftBorder, rightBorder, bottomBorder, topBorder );
}
KCommand * KoTextView::setMarginCommand( QStyleSheetItem::Margin m, double margin )
{
    return textObject()->setMarginCommand( m_cursor, m, margin );
}
KCommand * KoTextView::setTabListCommand( const KoTabulatorList & tabList )
{
    return textObject()->setTabListCommand( m_cursor, tabList );
}

KoTextDocument * KoTextView::textDocument() const
{
    return textObject()->textDocument();
}

#include "kotextview.moc"
