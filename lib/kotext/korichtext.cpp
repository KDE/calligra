/****************************************************************************
** Implementation of the internal Qt classes dealing with rich text
**
** Created : 990101
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
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

// ### TODO: make it depend on Qt version
#define INDIC

#include "korichtext.h"
#include "kotextformat.h"

#include <qpaintdevicemetrics.h>
#include "qdrawutil.h" // for KoTextHorizontalLine

#include <stdlib.h>
#include "koparagcounter.h"
#include "kotextdocument.h"
#include <kdebug.h>
#include <kdeversion.h>
#if ! KDE_IS_VERSION(3,1,90)
#include <kdebugclasses.h>
#endif
#include <kglobal.h>
#include <klocale.h>
#ifdef INDIC
#include <private/qtextengine_p.h>
#endif

//#define PARSER_DEBUG
//#define DEBUG_COLLECTION
//#define DEBUG_TABLE_RENDERING

//static KoTextFormatCollection *qFormatCollection = 0;

#if defined(PARSER_DEBUG)
static QString debug_indent;
#endif

static bool is_printer( QPainter *p )
{
    return p && p->device() && p->device()->devType() == QInternal::Printer;
}

static inline int scale( int value, QPainter *painter )
{
    if ( is_printer( painter ) ) {
	QPaintDeviceMetrics metrics( painter->device() );
#if defined(Q_WS_X11)
	value = value * metrics.logicalDpiY() / QPaintDevice::x11AppDpiY();
#elif defined (Q_WS_WIN)
	int gdc = GetDeviceCaps( GetDC( 0 ), LOGPIXELSY );
	if ( gdc )
	    value = value * metrics.logicalDpiY() / gdc;
#elif defined (Q_WS_MAC)
	value = value * metrics.logicalDpiY() / 75; // ##### FIXME
#elif defined (Q_WS_QWS)
	value = value * metrics.logicalDpiY() / 75;
#endif
    }
    return value;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void KoTextDocCommandHistory::addCommand( KoTextDocCommand *cmd )
{
    if ( current < (int)history.count() - 1 ) {
	QPtrList<KoTextDocCommand> commands;
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
}

KoTextCursor *KoTextDocCommandHistory::undo( KoTextCursor *c )
{
    if ( current > -1 ) {
	KoTextCursor *c2 = history.at( current )->unexecute( c );
	--current;
	return c2;
    }
    return 0;
}

KoTextCursor *KoTextDocCommandHistory::redo( KoTextCursor *c )
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

bool KoTextDocCommandHistory::isUndoAvailable()
{
    return current > -1;
}

bool KoTextDocCommandHistory::isRedoAvailable()
{
   return current > -1 && current < (int)history.count() - 1 || current == -1 && history.count() > 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KoTextDocDeleteCommand::KoTextDocDeleteCommand( KoTextDocument *d, int i, int idx, const QMemArray<KoTextStringChar> &str )
    : KoTextDocCommand( d ), id( i ), index( idx ), parag( 0 ), text( str )
{
    for ( int j = 0; j < (int)text.size(); ++j ) {
	if ( text[ j ].format() )
	    text[ j ].format()->addRef();
    }
}

/*KoTextDocDeleteCommand::KoTextDocDeleteCommand( KoTextParag *p, int idx, const QMemArray<KoTextStringChar> &str )
    : KoTextDocCommand( 0 ), id( -1 ), index( idx ), parag( p ), text( str )
{
    for ( int i = 0; i < (int)text.size(); ++i ) {
	if ( text[ i ].format() )
	    text[ i ].format()->addRef();
    }
}*/

KoTextDocDeleteCommand::~KoTextDocDeleteCommand()
{
    for ( int i = 0; i < (int)text.size(); ++i ) {
	if ( text[ i ].format() )
	    text[ i ].format()->removeRef();
    }
    text.resize( 0 );
}

KoTextCursor *KoTextDocDeleteCommand::execute( KoTextCursor *c )
{
    KoTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
	kdWarning(32500) << "can't locate parag at " << id << ", last parag: " << doc->lastParag()->paragId() << endl;
	return 0;
    }

    cursor.setParag( s );
    cursor.setIndex( index );
    int len = text.size();
    if ( c )
	*c = cursor;
    if ( doc ) {
	doc->setSelectionStart( KoTextDocument::Temp, &cursor );
	for ( int i = 0; i < len; ++i )
	    cursor.gotoNextLetter();
	doc->setSelectionEnd( KoTextDocument::Temp, &cursor );
	doc->removeSelectedText( KoTextDocument::Temp, &cursor );
	if ( c )
	    *c = cursor;
    } else {
	s->remove( index, len );
    }

    return c;
}

KoTextCursor *KoTextDocDeleteCommand::unexecute( KoTextCursor *c )
{
    KoTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
	kdWarning(32500) << "can't locate parag at " << id << ", last parag: " << doc->lastParag()->paragId() << endl;
	return 0;
    }

    cursor.setParag( s );
    cursor.setIndex( index );
    QString str = KoTextString::toString( text );
    cursor.insert( str, TRUE, &text );
    cursor.setParag( s );
    cursor.setIndex( index );
    if ( c ) {
	c->setParag( s );
	c->setIndex( index );
	for ( int i = 0; i < (int)text.size(); ++i )
	    c->gotoNextLetter();
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

KoTextDocFormatCommand::KoTextDocFormatCommand( KoTextDocument *d, int sid, int sidx, int eid, int eidx,
					const QMemArray<KoTextStringChar> &old, const KoTextFormat *f, int fl )
    : KoTextDocCommand( d ), startId( sid ), startIndex( sidx ), endId( eid ), endIndex( eidx ), oldFormats( old ), flags( fl )
{
    format = d->formatCollection()->format( f );
    for ( int j = 0; j < (int)oldFormats.size(); ++j ) {
	if ( oldFormats[ j ].format() )
	    oldFormats[ j ].format()->addRef();
    }
}

KoTextDocFormatCommand::~KoTextDocFormatCommand()
{
    format->removeRef();
    for ( int j = 0; j < (int)oldFormats.size(); ++j ) {
	if ( oldFormats[ j ].format() )
	    oldFormats[ j ].format()->removeRef();
    }
}

KoTextCursor *KoTextDocFormatCommand::execute( KoTextCursor *c )
{
    KoTextParag *sp = doc->paragAt( startId );
    KoTextParag *ep = doc->paragAt( endId );
    if ( !sp || !ep )
	return c;

    KoTextCursor start( doc );
    start.setParag( sp );
    start.setIndex( startIndex );
    KoTextCursor end( doc );
    end.setParag( ep );
    end.setIndex( endIndex );

    doc->setSelectionStart( KoTextDocument::Temp, &start );
    doc->setSelectionEnd( KoTextDocument::Temp, &end );
    doc->setFormat( KoTextDocument::Temp, format, flags );
    doc->removeSelection( KoTextDocument::Temp );
    if ( endIndex == ep->length() ) // ### Not in QRT - report sent. Description at http://bugs.kde.org/db/34/34556.html
        end.gotoLeft();
    *c = end;
    return c;
}

KoTextCursor *KoTextDocFormatCommand::unexecute( KoTextCursor *c )
{
    KoTextParag *sp = doc->paragAt( startId );
    KoTextParag *ep = doc->paragAt( endId );
    if ( !sp || !ep )
	return 0;

    int idx = startIndex;
    int fIndex = 0;
    if( !oldFormats.isEmpty()) // ## not in QRT. Not sure how it can happen.
    {
    for ( ;; ) {
	if ( oldFormats.at( fIndex ).c == '\n' ) {
	    if ( idx > 0 ) {
		if ( idx < sp->length() && fIndex > 0 )
		    sp->setFormat( idx, 1, oldFormats.at( fIndex - 1 ).format() );
		if ( sp == ep )
		    break;
		sp = sp->next();
		idx = 0;
	    }
	    fIndex++;
	}
	if ( oldFormats.at( fIndex ).format() )
	    sp->setFormat( idx, 1, oldFormats.at( fIndex ).format() );
	idx++;
	fIndex++;
	if ( fIndex >= (int)oldFormats.size() )
	    break;
	if ( idx >= sp->length() ) {
	    if ( sp == ep )
		break;
	    sp = sp->next();
	    idx = 0;
	}
    }
    }
    KoTextCursor end( doc );
    end.setParag( ep );
    end.setIndex( endIndex );
    if ( endIndex == ep->length() )
        end.gotoLeft();
    *c = end;
    return c;
}

KoTextAlignmentCommand::KoTextAlignmentCommand( KoTextDocument *d, int fParag, int lParag, int na, const QMemArray<int> &oa )
    : KoTextDocCommand( d ), firstParag( fParag ), lastParag( lParag ), newAlign( na ), oldAligns( oa )
{
}

KoTextCursor *KoTextAlignmentCommand::execute( KoTextCursor *c )
{
    KoTextParag *p = doc->paragAt( firstParag );
    if ( !p )
	return c;
    while ( p ) {
	p->setAlignment( newAlign );
	if ( p->paragId() == lastParag )
	    break;
	p = p->next();
    }
    return c;
}

KoTextCursor *KoTextAlignmentCommand::unexecute( KoTextCursor *c )
{
    KoTextParag *p = doc->paragAt( firstParag );
    if ( !p )
	return c;
    int i = 0;
    while ( p ) {
	if ( i < (int)oldAligns.size() )
	    p->setAlignment( oldAligns.at( i ) );
	if ( p->paragId() == lastParag )
	    break;
	p = p->next();
	++i;
    }
    return c;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KoTextCursor::KoTextCursor( KoTextDocument *d )
    : doc( d ), ox( 0 ), oy( 0 )
{
    nested = FALSE;
    idx = 0;
    string = doc ? doc->firstParag() : 0;
    tmpIndex = -1;
}

KoTextCursor::KoTextCursor()
{
}

KoTextCursor::KoTextCursor( const KoTextCursor &c )
{
    doc = c.doc;
    ox = c.ox;
    oy = c.oy;
    nested = c.nested;
    idx = c.idx;
    string = c.string;
    tmpIndex = c.tmpIndex;
    indices = c.indices;
    parags = c.parags;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;
}

KoTextCursor &KoTextCursor::operator=( const KoTextCursor &c )
{
    doc = c.doc;
    ox = c.ox;
    oy = c.oy;
    nested = c.nested;
    idx = c.idx;
    string = c.string;
    tmpIndex = c.tmpIndex;
    indices = c.indices;
    parags = c.parags;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;

    return *this;
}

bool KoTextCursor::operator==( const KoTextCursor &c ) const
{
    return doc == c.doc && string == c.string && idx == c.idx;
}

int KoTextCursor::totalOffsetX() const
{
    if ( !nested )
	return 0;
    QValueStack<int>::ConstIterator xit = xOffsets.begin();
    int xoff = ox;
    for ( ; xit != xOffsets.end(); ++xit )
	xoff += *xit;
    return xoff;
}

int KoTextCursor::totalOffsetY() const
{
    if ( !nested )
	return 0;
    QValueStack<int>::ConstIterator yit = yOffsets.begin();
    int yoff = oy;
    for ( ; yit != yOffsets.end(); ++yit )
	yoff += *yit;
    return yoff;
}

void KoTextCursor::gotoIntoNested( const QPoint &globalPos )
{
    if ( !doc )
	return;
    push();
    ox = 0;
    int bl, y;
    string->lineHeightOfChar( idx, &bl, &y );
    oy = y + string->rect().y();
    nested = TRUE;
    QPoint p( globalPos.x() - offsetX(), globalPos.y() - offsetY() );
    Q_ASSERT( string->at( idx )->isCustom() );
    ox = string->at( idx )->x;
    string->at( idx )->customItem()->enterAt( this, doc, string, idx, ox, oy, p );
}

void KoTextCursor::invalidateNested()
{
    if ( nested ) {
	QValueStack<KoTextParag*>::Iterator it = parags.begin();
	QValueStack<int>::Iterator it2 = indices.begin();
	for ( ; it != parags.end(); ++it, ++it2 ) {
	    if ( *it == string )
		continue;
	    (*it)->invalidate( 0 );
	    if ( (*it)->at( *it2 )->isCustom() )
		(*it)->at( *it2 )->customItem()->invalidate();
	}
    }
}

void KoTextCursor::insert( const QString &str, bool checkNewLine, QMemArray<KoTextStringChar> *formatting )
{
    string->invalidate( idx );
    tmpIndex = -1;
    bool justInsert = TRUE;
    QString s( str );
#if defined(Q_WS_WIN)
    if ( checkNewLine )
	s = s.replace( QRegExp( "\\r" ), "" );
#endif
    if ( checkNewLine )
	justInsert = s.find( '\n' ) == -1;
    if ( justInsert ) {
	string->insert( idx, s );
	if ( formatting ) {
	    for ( int i = 0; i < (int)s.length(); ++i ) {
		if ( formatting->at( i ).format() ) {
		    formatting->at( i ).format()->addRef();
		    string->string()->setFormat( idx + i, formatting->at( i ).format(), TRUE );
		}
	    }
	}
	idx += s.length();
    } else {
	QStringList lst = QStringList::split( '\n', s, TRUE );
	QStringList::Iterator it = lst.begin();
	//int y = string->rect().y() + string->rect().height();
	int lastIndex = 0;
	KoTextFormat *lastFormat = 0;
	for ( ; it != lst.end(); ) {
	    if ( it != lst.begin() ) {
		splitAndInsertEmptyParag( FALSE, TRUE );
		//string->setEndState( -1 );
#if 0 // no!
		string->prev()->format( -1, FALSE );
#endif
		if ( lastFormat && formatting && string->prev() ) {
		    lastFormat->addRef();
		    string->prev()->string()->setFormat( string->prev()->length() - 1, lastFormat, TRUE );
		}
	    }
	    lastFormat = 0;
	    QString s = *it;
	    ++it;
	    if ( !s.isEmpty() )
		string->insert( idx, s );
            else
                string->invalidate( 0 );

	    if ( formatting ) {
		int len = s.length();
		for ( int i = 0; i < len; ++i ) {
		    if ( formatting->at( i + lastIndex ).format() ) {
			formatting->at( i + lastIndex ).format()->addRef();
			string->string()->setFormat( i + idx, formatting->at( i + lastIndex ).format(), TRUE );
		    }
		}
		if ( it != lst.end() )
		    lastFormat = formatting->at( len + lastIndex ).format();
		++len;
		lastIndex += len;
	    }

	    idx += s.length();
	}
#if 0  //// useless and wrong. We'll format things and move them down correctly in KoTextObject::insert().
	string->format( -1, FALSE );
	int dy = string->rect().y() + string->rect().height() - y;
#endif
	KoTextParag *p = string;
	p->setParagId( p->prev()->paragId() + 1 );
	p = p->next();
	while ( p ) {
	    p->setParagId( p->prev()->paragId() + 1 );
	    //p->move( dy );
	    p->invalidate( 0 );
	    p = p->next();
	}
    }

#if 0  //// useless and slow
    int h = string->rect().height();
    string->format( -1, TRUE );
    if ( h != string->rect().height() )
	invalidateNested();
    else if ( doc && doc->parent() )
	doc->nextDoubleBuffered = TRUE;
#endif
#ifdef INDIC
	fixCursorPosition();
#endif
}

void KoTextCursor::gotoLeft()
{
    if ( string->string()->isRightToLeft() )
	gotoNextLetter();
    else
	gotoPreviousLetter();
}

void KoTextCursor::gotoPreviousLetter()
{
    tmpIndex = -1;

    if ( idx > 0 ) {
#ifndef INDIC
	idx--;
#else
	idx = string->string()->previousCursorPosition( idx );
#endif
    } else if ( string->prev() ) {
	string = string->prev();
	while ( !string->isVisible() )
	    string = string->prev();
	idx = string->length() - 1;
#ifndef INDIC
    } else {
	if ( nested ) {
	    pop();
	    processNesting( Prev );
	    if ( idx == -1 ) {
		pop();
		if ( idx > 0 ) {
		    idx--;
		} else if ( string->prev() ) {
		    string = string->prev();
		    idx = string->length() - 1;
		}
	    }
	}
#endif
    }

    const KoTextStringChar *tsc = string->at( idx );
    if ( tsc && tsc->isCustom() && tsc->customItem()->isNested() ) {
	processNesting( EnterEnd );
    }
}

void KoTextCursor::push()
{
    indices.push( idx );
    parags.push( string );
    xOffsets.push( ox );
    yOffsets.push( oy );
    nestedStack.push( nested );
}

void KoTextCursor::pop()
{
    if ( !doc )
	return;
    idx = indices.pop();
    string = parags.pop();
    ox = xOffsets.pop();
    oy = yOffsets.pop();
    //if ( doc->parent() )
    //doc = doc->parent();
    nested = nestedStack.pop();
}

void KoTextCursor::restoreState()
{
    while ( !indices.isEmpty() )
	pop();
}

bool KoTextCursor::place( const QPoint &p, KoTextParag *s, bool link, int *customItemIndex )
{
    if ( customItemIndex )
        *customItemIndex = -1;
    QPoint pos( p );
    QRect r;
    if ( pos.y() < s->rect().y() )
	pos.setY( s->rect().y() );
    while ( s ) {
	r = s->rect();
	r.setWidth( doc ? doc->width() : QWIDGETSIZE_MAX );
	if ( !s->next() || ( pos.y() >= r.y() && pos.y() < s->next()->rect().y() ) )
	    break;
	s = s->next();
    }

    if ( !s )
	return FALSE;

    setParag( s, FALSE );
    int y = s->rect().y();
    int lines = s->lines();
    KoTextStringChar *chr = 0;
    int index = 0;
    int i = 0;
    int cy = 0;
    //int ch = 0;
    for ( ; i < lines; ++i ) {
	chr = s->lineStartOfLine( i, &index );
	cy = s->lineY( i );
	//ch = s->lineHeight( i );
	if ( !chr )
	    return FALSE;
	if ( i < lines - 1 && pos.y() >= y + cy && pos.y() <= y + s->lineY( i+1 ) )
	    break;
    }
    int nextLine;
    if ( i < lines - 1 )
	s->lineStartOfLine( i+1, &nextLine );
    else
	nextLine = s->length();
    i = index;
    int x = s->rect().x();
    if ( pos.x() < x )
	pos.setX( x + 1 );
    int cw;
    int curpos = s->length()-1;
    int dist = 10000000;
    bool inCustom = FALSE;
    while ( i < nextLine ) {
	chr = s->at(i);
	int cpos = x + chr->x;
	cw = chr->width; //s->string()->width( i );
	if ( chr->isCustom() ) {
             if ( pos.x() >= cpos && pos.x() <= cpos + cw &&
                  pos.y() >= y + cy && pos.y() <= y + cy + chr->height() ) {
                if ( customItemIndex )
                    *customItemIndex = i;
                if ( chr->customItem()->isNested() )
                {
                    curpos = i;
                    inCustom = TRUE;
                    break;
                }
	    }
	}
        if( chr->rightToLeft )
            cpos += cw;
        int d = cpos - pos.x();
        bool dm = d < 0 ? !chr->rightToLeft : chr->rightToLeft;
#ifndef INDIC
        if ( QABS( d ) < dist || (dist == d && dm == TRUE ) ) {
#else
        if ( (QABS( d ) < dist || (dist == d && dm == TRUE )) && string->string()->validCursorPosition( i ) ) {
#endif
            dist = QABS( d );
            if ( !link || pos.x() >= x + chr->x ) {
                curpos = i;
            }
        }
	i++;
    }
    setIndex( curpos, FALSE );

#ifndef INDIC
    if ( inCustom && doc && parag()->at( curpos )->isCustom() && parag()->at( curpos )->customItem()->isNested() ) {
	KoTextDocument *oldDoc = doc;
	pos.setX( pos.x() - parag()->at( curpos )->x );
	gotoIntoNested( pos );
	if ( oldDoc == doc )
	    return TRUE;
	QPoint p( pos.x() - offsetX(), pos.y() - offsetY() );
	if ( !place( p, document()->firstParag() ) )
	    pop();
    }
#endif
    return TRUE;
}

void KoTextCursor::processNesting( Operation op )
{
    if ( !doc )
	return;
    push();
    ox = string->at( idx )->x;
    int bl, y;
    string->lineHeightOfChar( idx, &bl, &y );
    oy = y + string->rect().y();
    nested = TRUE;
    bool ok = FALSE;

    switch ( op ) {
    case EnterBegin:
	ok = string->at( idx )->customItem()->enter( this, doc, string, idx, ox, oy );
	break;
    case EnterEnd:
	ok = string->at( idx )->customItem()->enter( this, doc, string, idx, ox, oy, TRUE );
	break;
    case Next:
	ok = string->at( idx )->customItem()->next( this, doc, string, idx, ox, oy );
	break;
    case Prev:
	ok = string->at( idx )->customItem()->prev( this, doc, string, idx, ox, oy );
	break;
    case Down:
	ok = string->at( idx )->customItem()->down( this, doc, string, idx, ox, oy );
	break;
    case Up:
	ok = string->at( idx )->customItem()->up( this, doc, string, idx, ox, oy );
	break;
    }
    if ( !ok )
	pop();
}

void KoTextCursor::gotoRight()
{
    if ( string->string()->isRightToLeft() )
	gotoPreviousLetter();
    else
	gotoNextLetter();
}

void KoTextCursor::gotoNextLetter()
{
    tmpIndex = -1;

#ifdef INDIC
    int len = string->length() - 1;
#endif
    const KoTextStringChar *tsc = string->at( idx );
    if ( tsc && tsc->isCustom() && tsc->customItem()->isNested() ) {
	processNesting( EnterBegin );
	return;
    }

#ifndef INDIC
    if ( idx < string->length() - 1 ) {
	idx++;
#else
    if ( idx < len ) {
        idx = string->string()->nextCursorPosition( idx );
#endif
    } else if ( string->next() ) {
	string = string->next();
	while ( !string->isVisible() )
	    string = string->next();
	idx = 0;
#ifndef INDIC
    } else {
	if ( nested ) {
	    pop();
	    processNesting( Next );
	    if ( idx == -1 ) {
		pop();
		if ( idx < string->length() - 1 ) {
		    idx++;
		} else if ( string->next() ) {
		    string = string->next();
		    idx = 0;
		}
	    }
	}
#endif
    }
}

void KoTextCursor::gotoUp()
{
    int indexOfLineStart;
    int line;
    KoTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( indexOfLineStart == 0 ) {
	if ( !string->prev() ) {
	    if ( !nested )
		return;
	    pop();
	    processNesting( Up );
	    if ( idx == -1 ) {
		pop();
		if ( !string->prev() )
		    return;
		idx = tmpIndex = 0;
	    } else {
		tmpIndex = -1;
		return;
	    }
	}
	string = string->prev();
	while ( !string->isVisible() )
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
#ifdef INDIC
    fixCursorPosition();
#endif
}

void KoTextCursor::gotoDown()
{
    int indexOfLineStart;
    int line;
    KoTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( line == string->lines() - 1 ) {
	if ( !string->next() ) {
	    if ( !nested )
		return;
	    pop();
	    processNesting( Down );
	    if ( idx == -1 ) {
		pop();
		if ( !string->next() )
		    return;
		idx = tmpIndex = 0;
	    } else {
		tmpIndex = -1;
		return;
	    }
	}
	string = string->next();
	while ( !string->isVisible() )
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
#ifdef INDIC
    fixCursorPosition();
#endif
}

void KoTextCursor::gotoLineEnd()
{
    tmpIndex = -1;
    int indexOfLineStart;
    int line;
    KoTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
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

void KoTextCursor::gotoLineStart()
{
    tmpIndex = -1;
    int indexOfLineStart;
    int line;
    KoTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    idx = indexOfLineStart;
}

void KoTextCursor::gotoHome()
{
    tmpIndex = -1;
    if ( doc )
	string = doc->firstParag();
    idx = 0;
}

void KoTextCursor::gotoEnd()
{
    if ( doc && !doc->lastParag()->isValid() )
    {
	kdDebug(32500) << "Last parag, " << doc->lastParag()->paragId() << ", is invalid - aborting gotoEnd() !" << endl;
	return;
    }

    tmpIndex = -1;
    if ( doc )
	string = doc->lastParag();
    idx = string->length() - 1;
}

void KoTextCursor::gotoPageUp( int visibleHeight )
{
    tmpIndex = -1;
    KoTextParag *s = string;
    int h = visibleHeight;
    int y = s->rect().y();
    while ( s ) {
	if ( y - s->rect().y() >= h )
	    break;
	s = s->prev();
    }

    if ( !s && doc )
	s = doc->firstParag();

    string = s;
    idx = 0;
}

void KoTextCursor::gotoPageDown( int visibleHeight )
{
    tmpIndex = -1;
    KoTextParag *s = string;
    int h = visibleHeight;
    int y = s->rect().y();
    while ( s ) {
	if ( s->rect().y() - y >= h )
	    break;
	s = s->next();
    }

    if ( !s && doc ) {
	s = doc->lastParag();
	string = s;
	idx = string->length() - 1;
	return;
    }

    if ( !s->isValid() )
	return;

    string = s;
    idx = 0;
}

void KoTextCursor::gotoWordRight()
{
    if ( string->string()->isRightToLeft() )
	gotoPreviousWord();
    else
	gotoNextWord();
}

void KoTextCursor::gotoWordLeft()
{
    if ( string->string()->isRightToLeft() )
	gotoNextWord();
    else
	gotoPreviousWord();
}

void KoTextCursor::gotoPreviousWord()
{
    gotoPreviousLetter();
    tmpIndex = -1;
    KoTextString *s = string->string();
    bool allowSame = FALSE;
    if ( idx == ( (int)s->length()-1 ) )
        return;
    for ( int i = idx; i >= 0; --i ) {
	if ( s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
	     s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';' ) {
	    if ( !allowSame )
		continue;
	    idx = i + 1;
	    return;
	}
	if ( !allowSame && !( s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
			      s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';'  ) )
	    allowSame = TRUE;
    }
    idx = 0;
}

void KoTextCursor::gotoNextWord()
{
    tmpIndex = -1;
    KoTextString *s = string->string();
    bool allowSame = FALSE;
    for ( int i = idx; i < (int)s->length(); ++i ) {
	if ( ! ( s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
	     s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';' ) ) {
	    if ( !allowSame )
		continue;
	    idx = i;
	    return;
	}
	if ( !allowSame && ( s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
			      s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';'  ) )
	    allowSame = TRUE;
    }

    if ( idx < ((int)s->length()-1) ) {
        gotoLineEnd();
    } else if ( string->next() ) {
	string = string->next();
	while ( !string->isVisible() )
	    string = string->next();
	idx = 0;
    } else {
	gotoLineEnd();
    }
}

bool KoTextCursor::atParagStart() const
{
    return idx == 0;
}

bool KoTextCursor::atParagEnd() const
{
    return idx == string->length() - 1;
}

void KoTextCursor::splitAndInsertEmptyParag( bool ind, bool updateIds )
{
    if ( !doc )
	return;
    tmpIndex = -1;
    KoTextFormat *f = 0;
    if ( doc->useFormatCollection() ) {
	f = string->at( idx )->format();
	if ( idx == string->length() - 1 && idx > 0 )
	    f = string->at( idx - 1 )->format();
	if ( f->isMisspelled() ) {
            KoTextFormat fNoMisspelled( *f );
            fNoMisspelled.setMisspelled( false );
	    f = doc->formatCollection()->format( &fNoMisspelled );
	}
    }

    if ( atParagEnd() ) {
	KoTextParag *n = string->next();
	KoTextParag *s = doc->createParag( doc, string, n, updateIds );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->copyParagData( string );
	if ( ind ) {
	    int oi, ni;
	    s->indent( &oi, &ni );
	    string = s;
	    idx = ni;
	} else {
	    string = s;
	    idx = 0;
	}
    } else if ( atParagStart() ) {
	KoTextParag *p = string->prev();
	KoTextParag *s = doc->createParag( doc, p, string, updateIds );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->copyParagData( string );
	if ( ind ) {
	    s->indent();
	    s->format();
	    indent();
	    string->format();
	}
    } else {
	QString str = string->string()->toString().mid( idx, 0xFFFFFF );
	KoTextParag *n = string->next();
	KoTextParag *s = doc->createParag( doc, string, n, updateIds );
	s->copyParagData( string );
	s->remove( 0, 1 );
	s->append( str, TRUE );
	for ( uint i = 0; i < str.length(); ++i ) {
            KoTextStringChar* tsc = string->at( idx + i );
	    s->setFormat( i, 1, tsc->format(), TRUE );
	    if ( tsc->isCustom() ) {
		KoTextCustomItem * item = tsc->customItem();
		s->at( i )->setCustomItem( item );
		tsc->loseCustomItem();
#if 0
		s->addCustomItem();
		string->removeCustomItem();
#endif
		doc->unregisterCustomItem( item, string );
		doc->registerCustomItem( item, s );
	    }
	}
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

    invalidateNested();
}

#ifdef INDIC
bool KoTextCursor::removePreviousChar()
{
    tmpIndex = -1;
    if ( !atParagStart() ) {
	string->remove( idx-1, 1 );
	int h = string->rect().height();
	idx--;
	// shouldn't be needed, just to make sure.
	fixCursorPosition();
	string->format( -1, TRUE );
	if ( h != string->rect().height() )
	    invalidateNested();
	//else if ( string->document() && string->document()->parent() )
	//    string->document()->nextDoubleBuffered = TRUE;
	return FALSE;
    } else if ( string->prev() ) {
	string = string->prev();
	string->join( string->next() );
	string->invalidateCounters();
	invalidateNested();
	return TRUE;
    }
    return FALSE;
}

#endif
bool KoTextCursor::remove()
{
    tmpIndex = -1;
    if ( !atParagEnd() ) {
#ifndef INDIC
	string->remove( idx, 1 );
#else
	int next = string->string()->nextCursorPosition( idx );
	string->remove( idx, next-idx );
#endif
	int h = string->rect().height();
	string->format( -1, TRUE );
	if ( h != string->rect().height() )
	    invalidateNested();
	//else if ( doc && doc->parent() )
	//    doc->nextDoubleBuffered = TRUE;
	return FALSE;
    } else if ( string->next() ) {
	if ( string->length() == 1 ) {
	    string->next()->setPrev( string->prev() );
	    if ( string->prev() )
		string->prev()->setNext( string->next() );
	    KoTextParag *p = string->next();
	    delete string;
	    string = p;
	    string->invalidate( 0 );
            //// kotext
            string->invalidateCounters();
            ////
	    KoTextParag *s = string;
	    while ( s ) {
		s->id = s->p ? s->p->id + 1 : 0;
		//s->state = -1;
		//s->needPreProcess = TRUE;
		s->changed = TRUE;
		s = s->n;
	    }
	    string->format();
	} else {
	    string->join( string->next() );
	}
	invalidateNested();
	return TRUE;
    }
    return FALSE;
}

void KoTextCursor::killLine()
{
    if ( atParagEnd() )
	return;
    string->remove( idx, string->length() - idx - 1 );
    int h = string->rect().height();
    string->format( -1, TRUE );
    if ( h != string->rect().height() )
	invalidateNested();
    //else if ( doc && doc->parent() )
    //doc->nextDoubleBuffered = TRUE;
}

void KoTextCursor::indent()
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

void KoTextCursor::setDocument( KoTextDocument *d )
{
    doc = d;
    string = d->firstParag();
    idx = 0;
    nested = FALSE;
    restoreState();
    tmpIndex = -1;
}


int KoTextCursor::x() const
{
    KoTextStringChar *c = string->at( idx );
    int curx = c->x;
    if ( c->rightToLeft )
        curx += c->width; //string->string()->width( idx );
    return curx;
}

int KoTextCursor::y() const
{
    int dummy, line;
    string->lineStartOfChar( idx, &dummy, &line );
    return string->lineY( line );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TODO: move to kotextdocument.cpp

void KoTextDocument::init()
{
#if defined(PARSER_DEBUG)
    kdDebug(32500) << debug_indent + "new KoTextDocument (%p)", this << endl;
#endif
    //oTextValid = TRUE;
    //if ( par )
//	par->insertChild( this );
    //pProcessor = 0;
    useFC = TRUE;
    pFormatter = 0;
    indenter = 0;
    fParag = 0;
    m_pageBreakEnabled = false;
    //minw = 0;
    align = Qt::AlignAuto;
    nSelections = 1;
    addMargs = FALSE;

#if 0
    preferRichText = FALSE;
    txtFormat = Qt::AutoText;
    focusIndicator.parag = 0;
    minwParag = 0;
    sheet_ = QStyleSheet::defaultSheet();
    factory_ = QMimeSourceFactory::defaultFactory();
    contxt = QString::null;
    fCollection->setStyleSheet( sheet_ );
#endif

    underlLinks = TRUE;
    backBrush = 0;
    buf_pixmap = 0;
    //nextDoubleBuffered = FALSE;

    //if ( par )
//	withoutDoubleBuffer = par->withoutDoubleBuffer;
//    else
	withoutDoubleBuffer = FALSE;

    lParag = fParag = createParag( this, 0, 0 );
    tmpCursor = 0;

    //cx = 0;
    //cy = 2;
    //if ( par )
	cx = cy = 0;
    //cw = 600; // huh?
    //vw = 0;
    flow_ = new KoTextFlow;
    //flow_->setWidth( cw );

    leftmargin = 0; // 4 in QRT
    rightmargin = 0; // 4 in QRT

    selectionColors[ Standard ] = QApplication::palette().color( QPalette::Active, QColorGroup::Highlight );
    selectionText[ Standard ] = TRUE;
    commandHistory = new KoTextDocCommandHistory( 100 );
    tStopWidth = formatCollection()->defaultFormat()->width( 'x' ) * 8;
}

KoTextDocument::~KoTextDocument()
{
    //if ( par )
//	par->removeChild( this );
    //// kotext
    m_bDestroying = true;
    clear( false );
    ////
    delete commandHistory;
    delete flow_;
    //if ( !par )
	delete pFormatter;
    delete fCollection;
    //delete pProcessor;
    delete buf_pixmap;
    delete indenter;
    delete backBrush;
    if ( tArray )
	delete [] tArray;
}

void KoTextDocument::clear( bool createEmptyParag )
{
    if ( flow_ )
	flow_->clear();
    while ( fParag ) {
	KoTextParag *p = fParag->next();
	delete fParag;
	fParag = p;
    }
    fParag = lParag = 0;
    if ( createEmptyParag )
	fParag = lParag = createParag( this );
    selections.clear();
}

/*
   // Looks slow!
int KoTextDocument::widthUsed() const
{
    KoTextParag *p = fParag;
    int w = 0;
    while ( p ) {
	int a = p->alignment();
	p->setAlignment( Qt::AlignLeft );
	p->invalidate( 0 );
	p->format();
	w = QMAX( w, p->rect().width() );
	p->setAlignment( a );
	p->invalidate( 0 );
	p = p->next();
    }
    return w;
}
*/

int KoTextDocument::height() const
{
    int h = 0;
    if ( lParag )
	h = lParag->rect().top() + lParag->rect().height() + 1;
    //int fh = flow_->boundingRect().height();
    //return QMAX( h, fh );
    return h;
}



KoTextParag *KoTextDocument::createParag( KoTextDocument *d, KoTextParag *pr, KoTextParag *nx, bool updateIds )
{
    return new KoTextParag( d, pr, nx, updateIds );
}

#if 0
bool KoTextDocument::setMinimumWidth( int w, KoTextParag *p )
{
    if ( w == -1 ) {
	minw = 0;
	p = 0;
    }
    if ( p == minwParag ) {
	minw = w;
	emit minimumWidthChanged( minw );
    } else if ( w > minw ) {
	minw = w;
	minwParag = p;
	emit minimumWidthChanged( minw );
    }
    cw = QMAX( minw, cw );
    return TRUE;
}
#endif

void KoTextDocument::setPlainText( const QString &text )
{
    clear();
    //preferRichText = FALSE;
    //oTextValid = TRUE;
    //oText = text;

    int lastNl = 0;
    int nl = text.find( '\n' );
    if ( nl == -1 ) {
	lParag = createParag( this, lParag, 0 );
	if ( !fParag )
	    fParag = lParag;
	QString s = text;
	if ( !s.isEmpty() ) {
	    if ( s[ (int)s.length() - 1 ] == '\r' )
		s.remove( s.length() - 1, 1 );
	    lParag->append( s );
	}
    } else {
	for (;;) {
	    lParag = createParag( this, lParag, 0 );
	    if ( !fParag )
		fParag = lParag;
	    QString s = text.mid( lastNl, nl - lastNl );
	    if ( !s.isEmpty() ) {
		if ( s[ (int)s.length() - 1 ] == '\r' )
		    s.remove( s.length() - 1, 1 );
		lParag->append( s );
	    }
	    if ( nl == 0xffffff )
		break;
	    lastNl = nl + 1;
	    nl = text.find( '\n', nl + 1 );
	    if ( nl == -1 )
		nl = 0xffffff;
	}
    }
    if ( !lParag )
	lParag = fParag = createParag( this, 0, 0 );
}

void KoTextDocument::setText( const QString &text, const QString & /*context*/ )
{
    //focusIndicator.parag = 0;
    selections.clear();
#if 0
    if ( txtFormat == Qt::AutoText && QStyleSheet::mightBeRichText( text ) ||
	 txtFormat == Qt::RichText )
	setRichText( text, context );
    else
#endif
	setPlainText( text );
}

QString KoTextDocument::plainText( KoTextParag *p ) const
{
    if ( !p ) {
	QString buffer;
	QString s;
	KoTextParag *p = fParag;
	while ( p ) {
	    s = p->string()->toString();
	    s.remove( s.length() - 1, 1 );
	    if ( p->next() )
		s += "\n";
	    buffer += s;
	    p = p->next();
	}
	return buffer;
    } else {
	return p->string()->toString();
    }
}

QString KoTextDocument::richText( KoTextParag * ) const
{
    QString s;
    // TODO update from QRT if this code is needed
    return s;
}

QString KoTextDocument::text() const
{
    if ( plainText().simplifyWhiteSpace().isEmpty() )
	return QString("");
    //if ( txtFormat == Qt::AutoText && preferRichText || txtFormat == Qt::RichText )
    //    return richText();
    return plainText( 0 );
}

QString KoTextDocument::text( int parag ) const
{
    KoTextParag *p = paragAt( parag );
    if ( !p )
	return QString::null;

    //if ( txtFormat == Qt::AutoText && preferRichText || txtFormat == Qt::RichText )
    //    return richText( p );
    //else
	return plainText( p );
}

void KoTextDocument::invalidate()
{
    KoTextParag *s = fParag;
    while ( s ) {
	s->invalidate( 0 );
	s = s->next();
    }
}

void KoTextDocument::informParagraphDeleted( KoTextParag* parag )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.begin();
    for ( ; it != selections.end(); ++it )
    {
        if ( (*it).startCursor.parag() == parag ) {
            if ( parag->prev() ) {
                KoTextParag* prevP = parag->prev();
                (*it).startCursor.setParag( prevP );
                (*it).startCursor.setIndex( prevP->length()-1 );
            } else
                (*it).startCursor.setParag( parag->next() ); // sets index to 0
        }
        if ( (*it).endCursor.parag() == parag ) {
            if ( parag->prev() ) {
                KoTextParag* prevP = parag->prev();
                (*it).endCursor.setParag( prevP );
                (*it).endCursor.setIndex( prevP->length()-1 );
            } else
                (*it).endCursor.setParag( parag->next() ); // sets index to 0
        }
    }
    emit paragraphDeleted( parag );
}

void KoTextDocument::selectionStart( int id, int &paragId, int &index )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;
    KoTextDocumentSelection &sel = *it;
    paragId = !sel.swapped ? sel.startCursor.parag()->paragId() : sel.endCursor.parag()->paragId();
    index = !sel.swapped ? sel.startCursor.index() : sel.endCursor.index();
}

KoTextCursor KoTextDocument::selectionStartCursor( int id)
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return KoTextCursor( this );
    KoTextDocumentSelection &sel = *it;
    if ( sel.swapped )
	return sel.endCursor;
    return sel.startCursor;
}

KoTextCursor KoTextDocument::selectionEndCursor( int id)
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return KoTextCursor( this );
    KoTextDocumentSelection &sel = *it;
    if ( !sel.swapped )
	return sel.endCursor;
    return sel.startCursor;
}

void KoTextDocument::selectionEnd( int id, int &paragId, int &index )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;
    KoTextDocumentSelection &sel = *it;
    paragId = sel.swapped ? sel.startCursor.parag()->paragId() : sel.endCursor.parag()->paragId();
    index = sel.swapped ? sel.startCursor.index() : sel.endCursor.index();
}

bool KoTextDocument::isSelectionSwapped( int id )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return false;
    KoTextDocumentSelection &sel = *it;
    return sel.swapped;
}

KoTextParag *KoTextDocument::selectionStart( int id )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return 0;
    KoTextDocumentSelection &sel = *it;
    if ( sel.startCursor.parag()->paragId() <  sel.endCursor.parag()->paragId() )
	return sel.startCursor.parag();
    return sel.endCursor.parag();
}

KoTextParag *KoTextDocument::selectionEnd( int id )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return 0;
    KoTextDocumentSelection &sel = *it;
    if ( sel.startCursor.parag()->paragId() >  sel.endCursor.parag()->paragId() )
	return sel.startCursor.parag();
    return sel.endCursor.parag();
}

void KoTextDocument::addSelection( int id )
{
    nSelections = QMAX( nSelections, id + 1 );
}

static void setSelectionEndHelper( int id, KoTextDocumentSelection &sel, KoTextCursor &start, KoTextCursor &end )
{
    KoTextCursor c1 = start;
    KoTextCursor c2 = end;
    if ( sel.swapped ) {
	c1 = end;
	c2 = start;
    }

    c1.parag()->removeSelection( id );
    c2.parag()->removeSelection( id );
    if ( c1.parag() != c2.parag() ) {
	c1.parag()->setSelection( id, c1.index(), c1.parag()->length() - 1 );
	c2.parag()->setSelection( id, 0, c2.index() );
    } else {
	c1.parag()->setSelection( id, QMIN( c1.index(), c2.index() ), QMAX( c1.index(), c2.index() ) );
    }

    sel.startCursor = start;
    sel.endCursor = end;
    if ( sel.startCursor.parag() == sel.endCursor.parag() )
	sel.swapped = sel.startCursor.index() > sel.endCursor.index();
}

bool KoTextDocument::setSelectionEnd( int id, KoTextCursor *cursor )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return FALSE;
    KoTextDocumentSelection &sel = *it;

    KoTextCursor start = sel.startCursor;
    KoTextCursor end = *cursor;

    if ( start == end ) {
	removeSelection( id );
	setSelectionStart( id, cursor );
	return TRUE;
    }

    if ( sel.endCursor.parag() == end.parag() ) {
	setSelectionEndHelper( id, sel, start, end );
	return TRUE;
    }

    bool inSelection = FALSE;
    KoTextCursor c( this );
    KoTextCursor tmp = sel.startCursor;
    if ( sel.swapped )
	tmp = sel.endCursor;
    tmp.restoreState();
    KoTextCursor tmp2 = *cursor;
    tmp2.restoreState();
    c.setParag( tmp.parag()->paragId() < tmp2.parag()->paragId() ? tmp.parag() : tmp2.parag() );
    KoTextCursor old;
    bool hadStart = FALSE;
    bool hadEnd = FALSE;
    bool hadStartParag = FALSE;
    bool hadEndParag = FALSE;
    bool hadOldStart = FALSE;
    bool hadOldEnd = FALSE;
    bool leftSelection = FALSE;
    sel.swapped = FALSE;
    for ( ;; ) {
	if ( c == start )
	    hadStart = TRUE;
	if ( c == end )
	    hadEnd = TRUE;
	if ( c.parag() == start.parag() )
	    hadStartParag = TRUE;
	if ( c.parag() == end.parag() )
	    hadEndParag = TRUE;
	if ( c == sel.startCursor )
	    hadOldStart = TRUE;
	if ( c == sel.endCursor )
	    hadOldEnd = TRUE;

	if ( !sel.swapped &&
	     ( hadEnd && !hadStart ||
	       hadEnd && hadStart && start.parag() == end.parag() && start.index() > end.index() ) )
	    sel.swapped = TRUE;

	if ( c == end && hadStartParag ||
	     c == start && hadEndParag ) {
	    KoTextCursor tmp = c;
	    tmp.restoreState();
	    if ( tmp.parag() != c.parag() ) {
		int sstart = tmp.parag()->selectionStart( id );
		tmp.parag()->removeSelection( id );
		tmp.parag()->setSelection( id, sstart, tmp.index() );
	    }
	}

	if ( inSelection &&
	     ( c == end && hadStart || c == start && hadEnd ) )
	     leftSelection = TRUE;
	else if ( !leftSelection && !inSelection && ( hadStart || hadEnd ) )
	    inSelection = TRUE;

	bool noSelectionAnymore = hadOldStart && hadOldEnd && leftSelection && !inSelection && !c.parag()->hasSelection( id ) && c.atParagEnd();
	c.parag()->removeSelection( id );
	if ( inSelection ) {
	    if ( c.parag() == start.parag() && start.parag() == end.parag() ) {
		c.parag()->setSelection( id, QMIN( start.index(), end.index() ), QMAX( start.index(), end.index() ) );
	    } else if ( c.parag() == start.parag() && !hadEndParag ) {
		c.parag()->setSelection( id, start.index(), c.parag()->length() - 1 );
	    } else if ( c.parag() == end.parag() && !hadStartParag ) {
		c.parag()->setSelection( id, end.index(), c.parag()->length() - 1 );
	    } else if ( c.parag() == end.parag() && hadEndParag ) {
		c.parag()->setSelection( id, 0, end.index() );
	    } else if ( c.parag() == start.parag() && hadStartParag ) {
		c.parag()->setSelection( id, 0, start.index() );
	    } else {
		c.parag()->setSelection( id, 0, c.parag()->length() - 1 );
	    }
	}

	if ( leftSelection )
	    inSelection = FALSE;

	old = c;
	c.gotoNextLetter();
	if ( old == c || noSelectionAnymore )
	    break;
    }

    if ( !sel.swapped )
	sel.startCursor.parag()->setSelection( id, sel.startCursor.index(), sel.startCursor.parag()->length() - 1 );

    sel.startCursor = start;
    sel.endCursor = end;
    if ( sel.startCursor.parag() == sel.endCursor.parag() )
	sel.swapped = sel.startCursor.index() > sel.endCursor.index();

    setSelectionEndHelper( id, sel, start, end );

    return TRUE;
}

void KoTextDocument::selectAll( int id )
{
    removeSelection( id );

    KoTextDocumentSelection sel;
    sel.swapped = FALSE;
    KoTextCursor c( this );

    c.setParag( fParag );
    c.setIndex( 0 );
    sel.startCursor = c;

    c.setParag( lParag );
    c.setIndex( lParag->length() - 1 );
    sel.endCursor = c;

    KoTextParag *p = fParag;
    while ( p ) {
	p->setSelection( id, 0, p->length() - 1 );
#ifdef QTEXTTABLE_AVAILABLE
	for ( int i = 0; i < (int)p->length(); ++i ) {
	    if ( p->at( i )->isCustom() && p->at( i )->customItem()->isNested() ) {
		KoTextTable *t = (KoTextTable*)p->at( i )->customItem();
		QPtrList<KoTextTableCell> tableCells = t->tableCells();
		for ( KoTextTableCell *c = tableCells.first(); c; c = tableCells.next() )
		    c->richText()->selectAll( id );
	    }
	}
#endif
	p = p->next();
    }

    selections.insert( id, sel );
}

bool KoTextDocument::removeSelection( int id )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return FALSE;

    KoTextDocumentSelection &sel = *it;

    KoTextCursor c( this );
    KoTextCursor tmp = sel.startCursor;
    if ( sel.swapped )
	tmp = sel.endCursor;
    tmp.restoreState();
    c.setParag( tmp.parag() );
    KoTextCursor old;
    bool hadStart = FALSE;
    bool hadEnd = FALSE;
    KoTextParag *lastParag = 0;
    bool leftSelection = FALSE;
    bool inSelection = FALSE;
    sel.swapped = FALSE;
    for ( ;; ) {
	if ( !hadStart && c.parag() == sel.startCursor.parag() )
	    hadStart = TRUE;
	if ( !hadEnd && c.parag() == sel.endCursor.parag() )
	    hadEnd = TRUE;

        if ( !leftSelection && !inSelection && ( c.parag() == sel.startCursor.parag() || c.parag() == sel.endCursor.parag() ) )
	    inSelection = TRUE;

	if ( inSelection &&
	     ( c == sel.endCursor && hadStart || c == sel.startCursor && hadEnd ) ) {
	     leftSelection = TRUE;
             inSelection = FALSE;
        }

	bool noSelectionAnymore = leftSelection && !inSelection && !c.parag()->hasSelection( id ) && c.atParagEnd();

	if ( lastParag != c.parag() )
	    c.parag()->removeSelection( id );

	old = c;
	lastParag = c.parag();
	c.gotoNextLetter();
	if ( old == c || noSelectionAnymore )
	    break;
    }

    selections.remove( id );
    return TRUE;
}

QString KoTextDocument::selectedText( int id, bool withCustom ) const
{
    // ######## TODO: look at textFormat() and return rich text or plain text (like the text() method!)
    QMap<int, KoTextDocumentSelection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
	return QString::null;

    KoTextDocumentSelection sel = *it;


    KoTextCursor c1 = sel.startCursor;
    KoTextCursor c2 = sel.endCursor;
    if ( sel.swapped ) {
	c2 = sel.startCursor;
	c1 = sel.endCursor;
    }

    c2.restoreState();
    c1.restoreState();

    if ( c1.parag() == c2.parag() ) {
	QString s;
	KoTextParag *p = c1.parag();
	int end = c2.index();
	if ( p->at( QMAX( 0, end - 1 ) )->isCustom() )
	    ++end;
	if ( !withCustom || !p->customItems() ) {
	    s += p->string()->toString().mid( c1.index(), end - c1.index() );
	} else {
	    for ( int i = c1.index(); i < end; ++i ) {
		if ( p->at( i )->isCustom() ) {
#ifdef QTEXTTABLE_AVAILABLE
		    if ( p->at( i )->customItem()->isNested() ) {
			s += "\n";
			KoTextTable *t = (KoTextTable*)p->at( i )->customItem();
			QPtrList<KoTextTableCell> cells = t->tableCells();
			for ( KoTextTableCell *c = cells.first(); c; c = cells.next() )
			    s += c->richText()->plainText() + "\n";
			s += "\n";
		    }
#endif
		} else {
		    s += p->at( i )->c;
		}
		s += "\n";
	    }
	}
	return s;
    }

    QString s;
    KoTextParag *p = c1.parag();
    int start = c1.index();
    while ( p ) {
	int end = p == c2.parag() ? c2.index() : p->length() - 1;
	if ( p == c2.parag() && p->at( QMAX( 0, end - 1 ) )->isCustom() )
	    ++end;
	if ( !withCustom || !p->customItems() ) {
	    s += p->string()->toString().mid( start, end - start );
	    if ( p != c2.parag() )
		s += "\n";
	} else {
	    for ( int i = start; i < end; ++i ) {
		if ( p->at( i )->isCustom() ) {
#ifdef QTEXTTABLE_AVAILABLE
		    if ( p->at( i )->customItem()->isNested() ) {
			s += "\n";
			KoTextTable *t = (KoTextTable*)p->at( i )->customItem();
			QPtrList<KoTextTableCell> cells = t->tableCells();
			for ( KoTextTableCell *c = cells.first(); c; c = cells.next() )
			    s += c->richText()->plainText() + "\n";
			s += "\n";
		    }
#endif
		} else {
		    s += p->at( i )->c;
		}
		s += "\n";
	    }
	}
	start = 0;
	if ( p == c2.parag() )
	    break;
	p = p->next();
    }
    return s;
}

void KoTextDocument::setFormat( int id, const KoTextFormat *f, int flags )
{
    QMap<int, KoTextDocumentSelection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    KoTextDocumentSelection sel = *it;

    KoTextCursor c1 = sel.startCursor;
    KoTextCursor c2 = sel.endCursor;
    if ( sel.swapped ) {
	c2 = sel.startCursor;
	c1 = sel.endCursor;
    }

    c2.restoreState();
    c1.restoreState();

    if ( c1.parag() == c2.parag() ) {
	c1.parag()->setFormat( c1.index(), c2.index() - c1.index(), f, TRUE, flags );
	return;
    }

    c1.parag()->setFormat( c1.index(), c1.parag()->length() - c1.index(), f, TRUE, flags );
    KoTextParag *p = c1.parag()->next();
    while ( p && p != c2.parag() ) {
	p->setFormat( 0, p->length(), f, TRUE, flags );
	p = p->next();
    }
    c2.parag()->setFormat( 0, c2.index(), f, TRUE, flags );
}

/*void KoTextDocument::copySelectedText( int id )
{
#ifndef QT_NO_CLIPBOARD
    if ( !hasSelection( id ) )
	return;

    QApplication::clipboard()->setText( selectedText( id ) );
#endif
}*/

void KoTextDocument::removeSelectedText( int id, KoTextCursor *cursor )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    KoTextDocumentSelection sel = *it;

    KoTextCursor c1 = sel.startCursor;
    KoTextCursor c2 = sel.endCursor;
    if ( sel.swapped ) {
	c2 = sel.startCursor;
	c1 = sel.endCursor;
    }

    // ### no support for editing tables yet
    if ( c1.nestedDepth() || c2.nestedDepth() )
	return;

    c2.restoreState();
    c1.restoreState();

    *cursor = c1;
    removeSelection( id );

    if ( c1.parag() == c2.parag() ) {
	c1.parag()->remove( c1.index(), c2.index() - c1.index() );
	return;
    }

    // ## Qt has a strange setValid/isValid on QTextCursor, only used in the few lines below !?!?
    bool valid = true;
    if ( c1.parag() == fParag && c1.index() == 0 &&
         c2.parag() == lParag && c2.index() == lParag->length() - 1 )
        valid = FALSE;

    bool didGoLeft = FALSE;
    if (  c1.index() == 0 && c1.parag() != fParag ) {
	cursor->gotoPreviousLetter();
        if ( valid )
            didGoLeft = TRUE;
    }

    c1.parag()->remove( c1.index(), c1.parag()->length() - 1 - c1.index() );
    KoTextParag *p = c1.parag()->next();
    int dy = 0;
    KoTextParag *tmp;
    while ( p && p != c2.parag() ) {
	tmp = p->next();
	dy -= p->rect().height();
	delete p;
	p = tmp;
    }
    c2.parag()->remove( 0, c2.index() );
    while ( p ) {
	p->move( dy );
        //// kotext
        if ( p->paragLayout().counter )
            p->paragLayout().counter->invalidate();
        ////
	p->invalidate( 0 );
	//p->setEndState( -1 );
	p = p->next();
    }

    c1.parag()->join( c2.parag() );

    if ( didGoLeft )
	cursor->gotoNextLetter();
}

void KoTextDocument::indentSelection( int id )
{
    QMap<int, KoTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    KoTextDocumentSelection sel = *it;
    KoTextParag *startParag = sel.startCursor.parag();
    KoTextParag *endParag = sel.endCursor.parag();
    if ( sel.endCursor.parag()->paragId() < sel.startCursor.parag()->paragId() ) {
	endParag = sel.startCursor.parag();
	startParag = sel.endCursor.parag();
    }

    KoTextParag *p = startParag;
    while ( p && p != endParag ) {
	p->indent();
	p = p->next();
    }
}

void KoTextDocument::addCommand( KoTextDocCommand *cmd )
{
    commandHistory->addCommand( cmd );
}

KoTextCursor *KoTextDocument::undo( KoTextCursor *c )
{
    return commandHistory->undo( c );
}

KoTextCursor *KoTextDocument::redo( KoTextCursor *c )
{
    return commandHistory->redo( c );
}

bool KoTextDocument::find( const QString &expr, bool cs, bool wo, bool forward,
			  int *parag, int *index, KoTextCursor *cursor )
{
    KoTextParag *p = forward ? fParag : lParag;
    if ( parag )
	p = paragAt( *parag );
    else if ( cursor )
	p = cursor->parag();
    bool first = TRUE;

    while ( p ) {
	QString s = p->string()->toString();
	s.remove( s.length() - 1, 1 ); // get rid of trailing space
	int start = forward ? 0 : s.length() - 1;
	if ( first && index )
	    start = *index;
	else if ( first )
	    start = cursor->index();
	if ( !forward && first ) {
	    start -= expr.length() + 1;
	    if ( start < 0 ) {
		first = FALSE;
		p = p->prev();
		continue;
	    }
	}
	first = FALSE;

	for ( ;; ) {
	    int res = forward ? s.find( expr, start, cs ) : s.findRev( expr, start, cs );
	    if ( res == -1 )
		break;

	    bool ok = TRUE;
	    if ( wo ) {
		int end = res + expr.length();
		if ( ( res == 0 || s[ res - 1 ].isSpace() || s[ res - 1 ].isPunct() ) &&
		     ( end == (int)s.length() || s[ end ].isSpace() || s[ end ].isPunct() ) )
		    ok = TRUE;
		else
		    ok = FALSE;
	    }
	    if ( ok ) {
		cursor->setParag( p );
		cursor->setIndex( res );
		setSelectionStart( Standard, cursor );
		cursor->setIndex( res + expr.length() );
		setSelectionEnd( Standard, cursor );
		if ( parag )
		    *parag = p->paragId();
		if ( index )
		    *index = res;
		return TRUE;
	    }
	    if ( forward ) {
		start = res + 1;
	    } else {
		if ( res == 0 )
		    break;
		start = res - 1;
	    }
	}
	p = forward ? p->next() : p->prev();
    }

    return FALSE;
}

#if 0
void KoTextDocument::setTextFormat( Qt::TextFormat f )
{
    txtFormat = f;
}

Qt::TextFormat KoTextDocument::textFormat() const
{
    return txtFormat;
}
#endif

bool KoTextDocument::inSelection( int selId, const QPoint &pos ) const
{
    QMap<int, KoTextDocumentSelection>::ConstIterator it = selections.find( selId );
    if ( it == selections.end() )
	return FALSE;

    KoTextDocumentSelection sel = *it;
    KoTextParag *startParag = sel.startCursor.parag();
    KoTextParag *endParag = sel.endCursor.parag();
    if ( sel.startCursor.parag() == sel.endCursor.parag() &&
	 sel.startCursor.parag()->selectionStart( selId ) == sel.endCursor.parag()->selectionEnd( selId ) )
	return FALSE;
    if ( sel.endCursor.parag()->paragId() < sel.startCursor.parag()->paragId() ) {
	endParag = sel.startCursor.parag();
	startParag = sel.endCursor.parag();
    }

    KoTextParag *p = startParag;
    while ( p ) {
	if ( p->rect().contains( pos ) ) {
	    bool inSel = FALSE;
	    int selStart = p->selectionStart( selId );
	    int selEnd = p->selectionEnd( selId );
	    int y = 0;
	    int h = 0;
	    for ( int i = 0; i < p->length(); ++i ) {
		if ( i == selStart )
		    inSel = TRUE;
		if ( i == selEnd )
		    break;
		if ( p->at( i )->lineStart ) {
		    y = (*p->lineStarts.find( i ))->y;
		    h = (*p->lineStarts.find( i ))->h;
		}
		if ( pos.y() - p->rect().y() >= y && pos.y() - p->rect().y() <= y + h ) {
		    if ( inSel && pos.x() >= p->at( i )->x &&
			 pos.x() <= p->at( i )->x + p->at( i )->width /*p->at( i )->format()->width( p->at( i )->c )*/ )
			return TRUE;
		}
	    }
	}
	if ( pos.y() < p->rect().y() )
	    break;
	if ( p == endParag )
	    break;
	p = p->next();
    }

    return FALSE;
}

#if 0
void KoTextDocument::doLayout( QPainter *p, int w )
{
    if ( !is_printer( p ) )
	p = 0;
    withoutDoubleBuffer = ( p != 0 );
    flow_->setWidth( w );
    cw = w;
    vw = w;
    fCollection->setPainter( p );
    KoTextParag *parag = fParag;
    while ( parag ) {
	parag->invalidate( 0 );
	parag->setPainter( p, TRUE );
	parag->format();
	parag = parag->next();
    }

    fCollection->setPainter( 0 );
    parag = fParag;
    while ( parag ) {
	parag->setPainter( 0, FALSE );
	parag = parag->next();
    }
}
#endif

QPixmap *KoTextDocument::bufferPixmap( const QSize &s )
{
    if ( !buf_pixmap ) {
	int w = QABS( s.width() );
	int h = QABS( s.height() );
	buf_pixmap = new QPixmap( w, h );
    } else {
	if ( buf_pixmap->width() < s.width() ||
	     buf_pixmap->height() < s.height() ) {
	    buf_pixmap->resize( QMAX( s.width(), buf_pixmap->width() ),
				QMAX( s.height(), buf_pixmap->height() ) );
	}
    }

    return buf_pixmap;
}

#if 0
void KoTextDocument::draw( QPainter *p, const QRect &rect, const QColorGroup &cg, const QBrush *paper )
{
    if ( !firstParag() )
	return;

    QBrush bgBrush = paper ? *paper : cg.brush( QColorGroup::Base ); // ## QRT doesn't use cg.brush(Base)
    {
	p->setBrushOrigin( -int( p->translationX() ),
			   -int( p->translationY() ) );
	p->fillRect( rect, bgBrush );
    }

#if 0 // strange code found in QRT - I don't want all my colors to go away !
    if ( formatCollection()->defaultFormat()->color() != cg.text() ) {
	QDict<KoTextFormat> formats = formatCollection()->dict();
	QDictIterator<KoTextFormat> it( formats );
	while ( it.current() ) {
	    if ( it.current() == formatCollection()->defaultFormat() ) {
		++it;
		continue;
	    }
	    it.current()->setColor( cg.text() );
	    ++it;
	}
	formatCollection()->defaultFormat()->setColor( cg.text() );
    }
#endif

    KoTextParag *parag = firstParag();
    while ( parag ) {
	if ( !parag->isValid() )
	    parag->format();
	int y = parag->rect().y();
	QRect pr( parag->rect() );
	pr.setX( 0 );
	pr.setWidth( QWIDGETSIZE_MAX );
	if ( !rect.isNull() && !rect.intersects( pr ) ) {
	    parag = parag->next();
	    continue;
	}
	p->translate( 0, y );
	if ( rect.isValid() )
	    parag->paint( *p, cg, 0, FALSE, rect.x(), rect.y(), rect.width(), rect.height() );
	else
	    parag->paint( *p, cg, 0, FALSE );
	p->translate( 0, -y );
	parag = parag->next();
	if ( !flow()->isEmpty() )
	    flow()->drawFloatingItems( p, rect.x(), rect.y(), rect.width(), rect.height(), cg, FALSE );
    }
}

void KoTextDocument::drawParag( QPainter *p, KoTextParag *parag, int cx, int cy, int cw, int ch,
			       QPixmap *&doubleBuffer, const QColorGroup &cg,
			       bool drawCursor, KoTextCursor *cursor, bool resetChanged )
{
    QPainter *painter = 0;
    if ( resetChanged )
	parag->setChanged( FALSE );
    QRect ir( parag->rect() );
    bool useDoubleBuffer = true;
    //bool useDoubleBuffer = !parag->document()->parent();
    //if ( !useDoubleBuffer && parag->document()->nextDoubleBuffered )
    //useDoubleBuffer = TRUE;
    if ( p->device()->devType() == QInternal::Printer )
	useDoubleBuffer = FALSE;

    if ( useDoubleBuffer  ) {
	if ( cx >= 0 && cy >= 0 )
        {
	    ir = ir.intersect( QRect( cx, cy, cw, ch ) );
            if (ir.isEmpty())
                useDoubleBuffer = FALSE;
        }
    }

    if ( useDoubleBuffer  ) {
	painter = new QPainter;
	if ( !doubleBuffer ||
	     ir.width() > doubleBuffer->width() ||
	     ir.height() > doubleBuffer->height() ) {
	    doubleBuffer = bufferPixmap( ir.size() );
	    painter->begin( doubleBuffer );
	} else {
	    painter->begin( doubleBuffer );
	}
    } else {
	painter = p;
	painter->translate( ir.x(), ir.y() );
    }

    painter->setBrushOrigin( -ir.x(), -ir.y() );

    if ( useDoubleBuffer || is_printer( painter ) ) {
	if ( !parag->backgroundColor() )
	    painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ),
			       cg.brush( QColorGroup::Base ) );
	else
	    painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ),
			       *parag->backgroundColor() );
    } else {
	if ( cursor && cursor->parag() == parag ) {
	    if ( !parag->backgroundColor() )
		painter->fillRect( QRect( parag->at( cursor->index() )->x, 0, 2, ir.height() ),
				   cg.brush( QColorGroup::Base ) );
	    else
		painter->fillRect( QRect( parag->at( cursor->index() )->x, 0, 2, ir.height() ),
				   *parag->backgroundColor() );
	}
    }

    painter->translate( -( ir.x() - parag->rect().x() ),
		       -( ir.y() - parag->rect().y() ) );
    parag->paint( *painter, cg, drawCursor ? cursor : 0, TRUE, cx, cy, cw, ch );
    if ( !flow()->isEmpty() ) {
	painter->translate( 0, -parag->rect().y() );
	QRect cr( cx, cy, cw, ch );
	cr = cr.intersect( QRect( 0, parag->rect().y(), parag->rect().width(), parag->rect().height() ) );
	flow()->drawFloatingItems( painter, cr.x(), cr.y(), cr.width(), cr.height(), cg, FALSE );
	painter->translate( 0, +parag->rect().y() );
    }

    if ( useDoubleBuffer ) {
	delete painter;
	painter = 0;
	p->drawPixmap( ir.topLeft(), *doubleBuffer, QRect( QPoint( 0, 0 ), ir.size() ) );
    } else {
	painter->translate( -ir.x(), -ir.y() );
    }

    if ( useDoubleBuffer ) {
        QRect rect = parag->rect();
        if ( rect.x() + rect.width() < parag->document()->x() + parag->document()->width() ) {
            p->fillRect( rect.x() + rect.width(), rect.y(),
                         ( parag->document()->x() + parag->document()->width() ) -
                         ( rect.x() + rect.width() ),
                         rect.height(), cg.brush( QColorGroup::Base ) );
    }

    //parag->document()->nextDoubleBuffered = FALSE;
}

KoTextParag *KoTextDocument::draw( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
				 bool onlyChanged, bool drawCursor, KoTextCursor *cursor, bool resetChanged )
{
    if ( withoutDoubleBuffer || par && par->withoutDoubleBuffer ) {
	withoutDoubleBuffer = TRUE;
	QRect crect( cx, cy, cw, ch );
	draw( p, crect, cg );
	return 0;
    }
    withoutDoubleBuffer = FALSE;

    if ( !firstParag() )
	return 0;

    if ( drawCursor && cursor )
	tmpCursor = cursor;
    if ( cx < 0 && cy < 0 ) {
	cx = 0;
	cy = 0;
	cw = width();
	ch = height();
    }

    KoTextParag *lastFormatted = 0;
    KoTextParag *parag = firstParag();

    QPixmap *doubleBuffer = 0;
    QPainter painter;
    QRect crect( cx, cy, cw, ch );

    // Space above first parag
    if ( isPageBreakEnabled() && parag && cy <= parag->rect().y() && parag->rect().y() > 0 ) {
	QRect r( 0, 0,
		 parag->document()->x() + parag->document()->width(),
		 parag->rect().y() );
	r &= crect;
	if ( !r.isEmpty() )
	    p->fillRect( r, cg.brush( QColorGroup::Base ) );
    }

    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	QRect ir = parag->rect();
	if ( isPageBreakEnabled() && parag->next() )
	    if ( ir.y() + ir.height() < parag->next()->rect().y() ) {
		QRect r( 0, ir.y() + ir.height(),
			 parag->document()->x() + parag->document()->width(),
			 parag->next()->rect().y() - ( ir.y() + ir.height() ) );
		r &= crect;
		if ( !r.isEmpty() )
		    p->fillRect( r, cg.brush( QColorGroup::Base ) );
	    }

	if ( !ir.intersects( crect ) ) {
	    ir.setWidth( parag->document()->width() );
	    if ( ir.intersects( crect ) )
		p->fillRect( ir.intersect( crect ), cg.brush( QColorGroup::Base ) );
	    if ( ir.y() > cy + ch ) {
		tmpCursor = 0;
		if ( buf_pixmap && buf_pixmap->height() > 300 ) {
		    delete buf_pixmap;
		    buf_pixmap = 0;
		}
		return lastFormatted;
	    }
	    parag = parag->next();
	    continue;
	}

	if ( !parag->hasChanged() && onlyChanged ) {
	    parag = parag->next();
	    continue;
	}

	drawParag( p, parag, cx, cy, cw, ch, doubleBuffer, cg, drawCursor, cursor, resetChanged );
	parag = parag->next();
    }

    parag = lastParag();
    if ( parag->rect().y() + parag->rect().height() < parag->document()->height() ) {
	if ( !parag->document()->parent() ) { // !useDoubleBuffer
	    p->fillRect( 0, parag->rect().y() + parag->rect().height(), parag->document()->width(),
			 parag->document()->height() - ( parag->rect().y() + parag->rect().height() ),
			 cg.brush( QColorGroup::Base ) );
	}
	if ( !flow()->isEmpty() ) {
	    QRect cr( cx, cy, cw, ch );
	    cr = cr.intersect( QRect( 0, parag->rect().y() + parag->rect().height(), parag->document()->width(),
				      parag->document()->height() - ( parag->rect().y() + parag->rect().height() ) ) );
	    flow()->drawFloatingItems( p, cr.x(), cr.y(), cr.width(), cr.height(), cg, FALSE );
	}
    }

    if ( buf_pixmap && buf_pixmap->height() > 300 ) {
	delete buf_pixmap;
	buf_pixmap = 0;
    }

    tmpCursor = 0;
    return lastFormatted;
}
#endif

#if 0
void KoTextDocument::setDefaultFont( const QFont &f )
{
    updateFontSizes( f.pointSize() );
}
#endif

void KoTextDocument::registerCustomItem( KoTextCustomItem *i, KoTextParag *p )
{
    if ( i && i->placement() != KoTextCustomItem::PlaceInline )
	flow_->registerFloatingItem( i );
    p->registerFloatingItem( i );
    i->setParagraph( p );
    //kdDebug(32500) << "KoTextDocument::registerCustomItem " << (void*)i << endl;
    customItems.append( i );
}

void KoTextDocument::unregisterCustomItem( KoTextCustomItem *i, KoTextParag *p )
{
    flow_->unregisterFloatingItem( i );
    p->unregisterFloatingItem( i );
    i->setParagraph( 0 );
    customItems.removeRef( i );
}

// unused in kotext, and needs KoTextStringChar::isAnchor
#if 0
bool KoTextDocument::hasFocusParagraph() const
{
    return !!focusIndicator.parag;
}

QString KoTextDocument::focusHref() const
{
    return focusIndicator.href;
}

bool KoTextDocument::focusNextPrevChild( bool next )
{
    if ( !focusIndicator.parag ) {
	if ( next ) {
	    focusIndicator.parag = fParag;
	    focusIndicator.start = 0;
	    focusIndicator.len = 0;
	} else {
	    focusIndicator.parag = lParag;
	    focusIndicator.start = lParag->length();
	    focusIndicator.len = 0;
	}
    } else {
	focusIndicator.parag->setChanged( TRUE );
    }
    focusIndicator.href = QString::null;

    if ( next ) {
	KoTextParag *p = focusIndicator.parag;
	int index = focusIndicator.start + focusIndicator.len;
	while ( p ) {
	    for ( int i = index; i < p->length(); ++i ) {
		if ( p->at( i )->isAnchor() ) {
		    p->setChanged( TRUE );
		    focusIndicator.parag = p;
		    focusIndicator.start = i;
		    focusIndicator.len = 0;
		    focusIndicator.href = p->at( i )->format()->anchorHref();
		    while ( i < p->length() ) {
			if ( !p->at( i )->format()->isAnchor() )
			    return TRUE;
			focusIndicator.len++;
			i++;
		    }
		} else if ( p->at( i )->isCustom() ) {
#ifdef QTEXTTABLE_AVAILABLE
		    if ( p->at( i )->customItem()->isNested() ) {
			KoTextTable *t = (KoTextTable*)p->at( i )->customItem();
			QPtrList<KoTextTableCell> cells = t->tableCells();
			// first try to continue
			KoTextTableCell *c;
			bool resetCells = TRUE;
			for ( c = cells.first(); c; c = cells.next() ) {
			    if ( c->richText()->hasFocusParagraph() ) {
				if ( c->richText()->focusNextPrevChild( next ) ) {
				    p->setChanged( TRUE );
				    focusIndicator.parag = p;
				    focusIndicator.start = i;
				    focusIndicator.len = 0;
				    focusIndicator.href = c->richText()->focusHref();
				    return TRUE;
				} else {
				    resetCells = FALSE;
				    c = cells.next();
				    break;
				}
			    }
			}
			// now really try
			if ( resetCells )
			    c = cells.first();
			for ( ; c; c = cells.next() ) {
			    if ( c->richText()->focusNextPrevChild( next ) ) {
				p->setChanged( TRUE );
				focusIndicator.parag = p;
				focusIndicator.start = i;
				focusIndicator.len = 0;
				focusIndicator.href = c->richText()->focusHref();
				return TRUE;
			    }
			}
		    }
#endif
		}
	    }
	    index = 0;
	    p = p->next();
	}
    } else {
	KoTextParag *p = focusIndicator.parag;
	int index = focusIndicator.start - 1;
	if ( focusIndicator.len == 0 && index < focusIndicator.parag->length() - 1 )
	    index++;
	while ( p ) {
	    for ( int i = index; i >= 0; --i ) {
		if ( p->at( i )->format()->isAnchor() ) {
		    p->setChanged( TRUE );
		    focusIndicator.parag = p;
		    focusIndicator.start = i;
		    focusIndicator.len = 0;
		    focusIndicator.href = p->at( i )->format()->anchorHref();
		    while ( i >= -1 ) {
			if ( i < 0 || !p->at( i )->format()->isAnchor() ) {
			    focusIndicator.start++;
			    return TRUE;
			}
			if ( i < 0 )
			    break;
			focusIndicator.len++;
			focusIndicator.start--;
			i--;
		    }
		} else if ( p->at( i )->isCustom() ) {
#ifdef QTEXTTABLE_AVAILABLE
		    if ( p->at( i )->customItem()->isNested() ) {
			KoTextTable *t = (KoTextTable*)p->at( i )->customItem();
			QPtrList<KoTextTableCell> cells = t->tableCells();
			// first try to continue
			KoTextTableCell *c;
			bool resetCells = TRUE;
			for ( c = cells.last(); c; c = cells.prev() ) {
			    if ( c->richText()->hasFocusParagraph() ) {
				if ( c->richText()->focusNextPrevChild( next ) ) {
				    p->setChanged( TRUE );
				    focusIndicator.parag = p;
				    focusIndicator.start = i;
				    focusIndicator.len = 0;
				    focusIndicator.href = c->richText()->focusHref();
				    return TRUE;
				} else {
				    resetCells = FALSE;
				    c = cells.prev();
				    break;
				}
			    }
			    if ( cells.at() == 0 )
				break;
			}
			// now really try
			if ( resetCells )
			    c = cells.last();
			for ( ; c; c = cells.prev() ) {
			    if ( c->richText()->focusNextPrevChild( next ) ) {
				p->setChanged( TRUE );
				focusIndicator.parag = p;
				focusIndicator.start = i;
				focusIndicator.len = 0;
				focusIndicator.href = c->richText()->focusHref();
				return TRUE;
			    }
			    if ( cells.at() == 0 )
				break;
			}
		    }
#endif
		}
	    }
	    p = p->prev();
	    if ( p )
		index = p->length() - 1;
	}
    }

    focusIndicator.parag = 0;

    return FALSE;
}
#endif

int KoTextDocument::length() const
{
    int l = 0;
    KoTextParag *p = fParag;
    while ( p ) {
	l += p->length() - 1; // don't count trailing space
	p = p->next();
    }
    return l;
}

#ifdef INDIC

void KoTextCursor::fixCursorPosition()
{
    // searches for the closest valid cursor position
    if ( string->string()->validCursorPosition( idx ) )
 	return;

    int lineIdx;
    KoTextStringChar *start = string->lineStartOfChar( idx, &lineIdx, 0 );
    int x = string->string()->at( idx ).x;
    int diff = QABS(start->x - x);
    int best = lineIdx;

    KoTextStringChar *c = start;
    ++c;

    KoTextStringChar *end = &string->string()->at( string->length()-1 );
    while ( c <= end && !c->lineStart ) {
 	int xp = c->x;
 	if ( c->rightToLeft )
 	    xp += c->pixelwidth; //string->string()->width( lineIdx + (c-start) );
 	int ndiff = QABS(xp - x);
 	if ( ndiff < diff && string->string()->validCursorPosition(lineIdx + (c-start)) ) {
 	    diff = ndiff;
 	    best = lineIdx + (c-start);
 	}
 	++c;
    }
    idx = best;
}

#endif
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KoTextString::KoTextString()
{
    bidiDirty = TRUE;
    bNeedsSpellCheck = true;
    bidi = FALSE;
    rightToLeft = FALSE;
    dir = QChar::DirON;
}

KoTextString::KoTextString( const KoTextString &s )
{
    bidiDirty = s.bidiDirty;
    bNeedsSpellCheck = s.bNeedsSpellCheck;
    bidi = s.bidi;
    rightToLeft = s.rightToLeft;
    dir = s.dir;
    data = s.data;
    data.detach();
    for ( int i = 0; i < (int)data.size(); ++i ) {
        KoTextFormat *f = data[i].format();
        if ( f )
            f->addRef();
    }
}

void KoTextString::insert( int index, const QString &s, KoTextFormat *f )
{
    int os = data.size();
    data.resize( data.size() + s.length() );
    if ( index < os ) {
	memmove( data.data() + index + s.length(), data.data() + index,
		 sizeof( KoTextStringChar ) * ( os - index ) );
    }
    for ( int i = 0; i < (int)s.length(); ++i ) {
	KoTextStringChar &ch = data[ (int)index + i ];
	ch.x = 0;
	ch.pixelxadj = 0;
	ch.pixelwidth = 0;
	ch.width = 0;
	ch.lineStart = 0;
	ch.d.format = 0;
	ch.type = KoTextStringChar::Regular;
	ch.rightToLeft = 0;
	ch.startOfRun = 0;
        ch.c = s[ i ];
#ifdef DEBUG_COLLECTION
	kdDebug(32500) << "KoTextString::insert setting format " << f << " to character " << (int)index+i << endl;
#endif
	ch.setFormat( f );
    }
    bidiDirty = TRUE;
    bNeedsSpellCheck = true;
}

KoTextString::~KoTextString()
{
    clear();
}

void KoTextString::insert( int index, KoTextStringChar *c )
{
    int os = data.size();
    data.resize( data.size() + 1 );
    if ( index < os ) {
	memmove( data.data() + index + 1, data.data() + index,
		 sizeof( KoTextStringChar ) * ( os - index ) );
    }
    KoTextStringChar &ch = data[ (int)index ];
    ch.c = c->c;
    ch.x = 0;
    ch.pixelxadj = 0;
    ch.pixelwidth = 0;
    ch.width = 0;
    ch.lineStart = 0;
    ch.rightToLeft = 0;
    ch.d.format = 0;
    ch.type = KoTextStringChar::Regular;
    ch.setFormat( c->format() );
    bidiDirty = TRUE;
    bNeedsSpellCheck = true;
}

void KoTextString::truncate( int index )
{
    index = QMAX( index, 0 );
    index = QMIN( index, (int)data.size() - 1 );
    if ( index < (int)data.size() ) {
	for ( int i = index + 1; i < (int)data.size(); ++i ) {
	    KoTextStringChar &ch = data[ i ];
	    if ( ch.isCustom() ) {
		delete ch.customItem();
		if ( ch.d.custom->format )
		    ch.d.custom->format->removeRef();
		delete ch.d.custom;
		ch.d.custom = 0;
	    } else if ( ch.format() ) {
		ch.format()->removeRef();
	    }
	}
    }
    data.truncate( index );
    bidiDirty = TRUE;
    bNeedsSpellCheck = true;
}

void KoTextString::remove( int index, int len )
{
    for ( int i = index; i < (int)data.size() && i - index < len; ++i ) {
	KoTextStringChar &ch = data[ i ];
	if ( ch.isCustom() ) {
	    delete ch.customItem();
	    if ( ch.d.custom->format )
		ch.d.custom->format->removeRef();
            delete ch.d.custom;
	    ch.d.custom = 0;
	} else if ( ch.format() ) {
	    ch.format()->removeRef();
	}
    }
    memmove( data.data() + index, data.data() + index + len,
	     sizeof( KoTextStringChar ) * ( data.size() - index - len ) );
    data.resize( data.size() - len, QGArray::SpeedOptim );
    bidiDirty = TRUE;
    bNeedsSpellCheck = true;
}

void KoTextString::clear()
{
    for ( int i = 0; i < (int)data.count(); ++i ) {
	KoTextStringChar &ch = data[ i ];
	if ( ch.isCustom() ) {
	    delete ch.customItem();
	    if ( ch.d.custom->format )
		ch.d.custom->format->removeRef();
	    delete ch.d.custom;
	    ch.d.custom = 0;
	} else if ( ch.format() ) {
	    ch.format()->removeRef();
	}
    }
    data.resize( 0 );
}

void KoTextString::setFormat( int index, KoTextFormat *f, bool useCollection )
{
    KoTextStringChar &ch = data[ index ];
//    kdDebug(32500) << "KoTextString::setFormat index=" << index << " f=" << f << endl;
    if ( useCollection && ch.format() )
    {
	//kdDebug(32500) << "KoTextString::setFormat removing ref on old format " << ch.format() << endl;
	ch.format()->removeRef();
    }
    ch.setFormat( f );
}

void KoTextString::checkBidi() const
{
#ifndef INDIC
    bool rtlKnown = FALSE;
#else
    KoTextString *that = (KoTextString *)this;
    that->bidiDirty = FALSE;
    int length = data.size();
    if ( !length ) {
        that->bidi = FALSE;
        that->rightToLeft = dir == QChar::DirR;
        return;
    }
    const KoTextStringChar *start = data.data();
    const KoTextStringChar *end = start + length;

    // determines the properties we need for layouting
    QTextEngine textEngine( toString(), 0 );
    textEngine.direction = (QChar::Direction) dir;
    textEngine.itemize(QTextEngine::SingleLine);
    const QCharAttributes *ca = textEngine.attributes() + length-1;
    KoTextStringChar *ch = (KoTextStringChar *)end - 1;
    QScriptItem *item = &textEngine.items[textEngine.items.size()-1];
    unsigned char bidiLevel = item->analysis.bidiLevel;
    if ( bidiLevel )
        that->bidi = TRUE;
    int pos = length-1;
    while ( ch >= start ) {
        if ( item->position > pos ) {
            --item;
            Q_ASSERT( item >= &textEngine.items[0] );
            Q_ASSERT( item < &textEngine.items[textEngine.items.size()] );
            bidiLevel = item->analysis.bidiLevel;
            if ( bidiLevel )
                that->bidi = TRUE;
        }
        ch->softBreak = ca->softBreak;
        ch->whiteSpace = ca->whiteSpace;
        ch->charStop = ca->charStop;
        ch->wordStop = ca->wordStop;
        //ch->bidiLevel = bidiLevel;
        ch->rightToLeft = (bidiLevel%2);
        --ch;
        --ca;
        --pos;
    }

#endif
    if ( dir == QChar::DirR ) {
#ifndef INDIC
	((KoTextString *)this)->bidi = TRUE;
	((KoTextString *)this)->rightToLeft = TRUE;
	((KoTextString *)this)->bidiDirty = FALSE;
	return;
#else
        that->bidi = TRUE;
        that->rightToLeft = TRUE;
#endif
    } else if ( dir == QChar::DirL ) {
#ifndef INDIC
	((KoTextString *)this)->rightToLeft = FALSE;
	rtlKnown = TRUE;
#else
        that->rightToLeft = FALSE;
#endif
    } else {
#ifndef INDIC
	((KoTextString *)this)->rightToLeft = FALSE;
    }

    int len = data.size();
    const KoTextStringChar *c = data.data();
    ((KoTextString *)this)->bidi = FALSE;
    while( len ) {
	if ( !rtlKnown ) {
	    switch( c->c.direction() )
	    {
		case QChar::DirL:
		case QChar::DirLRO:
		case QChar::DirLRE:
		    ((KoTextString *)this)->rightToLeft = FALSE;
		    rtlKnown = TRUE;
		    break;
		case QChar::DirR:
		case QChar::DirAL:
		case QChar::DirRLO:
		case QChar::DirRLE:
		    ((KoTextString *)this)->rightToLeft = TRUE;
		    rtlKnown = TRUE;
		    break;
		default:
		    break;
	    }
	}
	uchar row = c->c.row();
	if( (row > 0x04 && row < 0x09) || ( row > 0xfa && row < 0xff ) ) {
	    ((KoTextString *)this)->bidi = TRUE;
            if ( rtlKnown )
                break;
	}
	len--;
	++c;
#else
        that->rightToLeft = (textEngine.direction == QChar::DirR);
#endif
    }
#ifndef INDIC
    ((KoTextString *)this)->bidiDirty = FALSE;
#endif
}

QMemArray<KoTextStringChar> KoTextString::subString( int start, int len ) const
{
    if ( len == 0xFFFFFF )
	len = data.size();
    QMemArray<KoTextStringChar> a;
    a.resize( len );
    for ( int i = 0; i < len; ++i ) {
	KoTextStringChar *c = &data[ i + start ];
	a[ i ].c = c->c;
	a[ i ].x = 0;
	a[ i ].pixelxadj = 0;
	a[ i ].pixelwidth = 0;
	a[ i ].width = 0;
	a[ i ].lineStart = 0;
	a[ i ].rightToLeft = 0;
	a[ i ].d.format = 0;
	a[ i ].type = KoTextStringChar::Regular;
	a[ i ].setFormat( c->format() );
	if ( c->format() )
	    c->format()->addRef();
    }
    return a;
}

QString KoTextString::mid( int start, int len ) const
{
    if ( len == 0xFFFFFF )
	len = data.size();
    QString res;
    res.setLength( len );
    for ( int i = 0; i < len; ++i ) {
	KoTextStringChar *c = &data[ i + start ];
	res[ i ] = c->c;
    }
    return res;
}

QString KoTextString::toString( const QMemArray<KoTextStringChar> &data )
{
    QString s;
    int l = data.size();
    s.setUnicode( 0, l );
    KoTextStringChar *c = data.data();
    QChar *uc = (QChar *)s.unicode();
    while ( l-- ) {
	*uc = c->c;
	uc++;
	c++;
    }

    return s;
}

QString KoTextString::toReverseString() const
{
    QString s;
    int l = length();
    s.setUnicode(0, l);
    KoTextStringChar *c = data.data() + (l-1);
    QChar *uc = (QChar *)s.unicode();
    while ( l-- ) {
	*uc = c->c;
	uc++;
	c--;
    }

    return s;
}

#ifdef INDIC
int KoTextString::nextCursorPosition( int next )
{
    if ( bidiDirty )
        checkBidi();

    const KoTextStringChar *c = data.data();
    int len = length();

    if ( next < len - 1 ) {
        next++;
        while ( next < len - 1 && !c[next].charStop )
            next++;
    }
    return next;
}

int KoTextString::previousCursorPosition( int prev )
{
    if ( bidiDirty )
        checkBidi();

    const KoTextStringChar *c = data.data();

    if ( prev ) {
        prev--;
        while ( prev && !c[prev].charStop )
            prev--;
    }
    return prev;
}

bool KoTextString::validCursorPosition( int idx )
{
    if ( bidiDirty )
        checkBidi();

    return (at( idx ).charStop);
}

////
#endif

void KoTextStringChar::setFormat( KoTextFormat *f )
{
    if ( type == Regular ) {
	d.format = f;
    } else {
	if ( !d.custom ) {
	    d.custom = new CustomData;
	    d.custom->custom = 0;
	}
	d.custom->format = f;
        if ( d.custom->custom )
            d.custom->custom->setFormat( f );
    }
}

void KoTextStringChar::setCustomItem( KoTextCustomItem *i )
{
    if ( type == Regular ) {
	KoTextFormat *f = format();
	d.custom = new CustomData;
	d.custom->format = f;
	type = Custom;
    } else {
	delete d.custom->custom;
    }
    d.custom->custom = i;
}

void KoTextStringChar::loseCustomItem() // setRegular() might be a better name
{
    if ( isCustom() ) {
	KoTextFormat *f = d.custom->format;
	d.custom->custom = 0;
	delete d.custom;
	type = Regular;
	d.format = f;
    }
}

KoTextStringChar::~KoTextStringChar()
{
    if ( format() )
	format()->removeRef();
    switch ( type ) {
	case Custom:
	    delete d.custom; break;
	default:
	    break;
    }
}

#ifndef INDIC
KoTextStringChar *KoTextStringChar::clone() const
{
    KoTextStringChar *chr = new KoTextStringChar;
    chr->c = c;
    chr->x = 0;
    chr->pixelxadj = 0;
    chr->pixelwidth = 0;
    chr->width = 0;
    chr->lineStart = 0;
    chr->rightToLeft = 0;
    chr->d.format = 0;
    chr->type = KoTextStringChar::Regular;
    chr->setFormat( format() );
    if ( chr->format() )
	chr->format()->addRef();
    return chr;
}

#endif
int KoTextStringChar::height() const
{
    return !isCustom() ? format()->height() : ( customItem()->placement() == KoTextCustomItem::PlaceInline ? customItem()->height : 0 );
}

int KoTextStringChar::ascent() const
{
    return !isCustom() ? format()->ascent() : ( customItem()->placement() == KoTextCustomItem::PlaceInline ? customItem()->ascent() : 0 );
}

int KoTextStringChar::descent() const
{
    return !isCustom() ? format()->descent() : 0;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KoTextParag::KoTextParag( KoTextDocument *d, KoTextParag *pr, KoTextParag *nx, bool updateIds )
    : invalid( 0 ), p( pr ), n( nx ), doc( d ),
      changed( FALSE ),
      fullWidth( TRUE ),
      newLinesAllowed( TRUE ), // default in kotext
      visible( TRUE ), breakable( TRUE ), movedDown( FALSE ),
      align( 0 ),
      m_lineChanged( -1 ),
      m_wused( 0 ),
      mSelections( 0 ),
      mFloatingItems( 0 ),
      tArray( 0 )
{
    defFormat = formatCollection()->defaultFormat();
    /*if ( !doc ) {
	tabStopWidth = defFormat->width( 'x' ) * 8;
	commandHistory = new KoTextDocCommandHistory( 100 );
    }*/
#if defined(PARSER_DEBUG)
    kdDebug(32500) << debug_indent + "new KoTextParag" << endl;
#endif

    if ( p ) {
	p->n = this;
#ifdef QTEXTTABLE_AVAILABLE
	if ( p->tc )
	    tc = p->tc;
#endif
    }
    if ( n ) {
	n->p = this;
#ifdef QTEXTTABLE_AVAILABLE
	if ( n->tc )
	    tc = n->tc;
#endif
    }

#ifdef QTEXTTABLE_AVAILABLE
    if ( !tc && d && d->tableCell() )
	tc = d->tableCell();
#endif

    if ( !p && doc )
	doc->setFirstParag( this );
    if ( !n && doc )
	doc->setLastParag( this );

    //firstFormat = TRUE; //// unused
    //firstPProcess = TRUE;
    //state = -1;
    //needPreProcess = FALSE;

    if ( p )
	id = p->id + 1;
    else
	id = 0;
    if ( n && updateIds ) {
	KoTextParag *s = n;
	while ( s ) {
	    s->id = s->p->id + 1;
	    //s->lm = s->rm = s->tm = s->bm = -1, s->flm = -1;
	    s = s->n;
	}
    }

    str = new KoTextString();
    str->insert( 0, " ", formatCollection()->defaultFormat() );
}

KoTextParag::~KoTextParag()
{
    //kdDebug(32500) << "KoTextParag::~KoTextParag " << this << " id=" << paragId() << endl;
    delete str;
//    if ( doc && p == doc->minwParag ) {
//	doc->minwParag = 0;
//	doc->minw = 0;
//    }
    if ( !doc ) {
	//delete pFormatter;
	//delete commandHistory;
    }
    delete [] tArray;
    //delete eData;
    QMap<int, KoTextParagLineStart*>::Iterator it = lineStarts.begin();
    for ( ; it != lineStarts.end(); ++it )
	delete *it;
    if ( mSelections ) delete mSelections;
    if ( mFloatingItems ) delete mFloatingItems;

    if (p)
       p->setNext(n);
    if (n)
       n->setPrev(p);

    //// kotext
    if ( doc && !doc->isDestroying() )
    {
        doc->informParagraphDeleted( this );
    }
    //kdDebug(32500) << "KoTextParag::~KoTextParag " << this << endl;
    ////
}

void KoTextParag::setNext( KoTextParag *s )
{
    n = s;
    if ( !n && doc )
	doc->setLastParag( this );
}

void KoTextParag::setPrev( KoTextParag *s )
{
    p = s;
    if ( !p && doc )
	doc->setFirstParag( this );
}

void KoTextParag::invalidate( int chr )
{
    if ( invalid < 0 )
	invalid = chr;
    else
	invalid = QMIN( invalid, chr );
#if 0 /// strange code!
    if ( mFloatingItems ) {
	for ( KoTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() )
	    i->move( 0, -1 );
    }
#endif
    //lm = rm = bm = tm = flm = -1;
}

void KoTextParag::setChanged( bool b, bool /*recursive*/ )
{
    changed = b;
    m_lineChanged = -1; // all
    //if ( recursive ) {
//	if ( doc && doc->parentParag() )
//	    doc->parentParag()->setChanged( b, recursive );
//    }
}

void KoTextParag::setLineChanged( short int line )
{
    if ( m_lineChanged == -1 ) {
        if ( !changed ) // only if the whole parag wasn't "changed" already
            m_lineChanged = line;
    }
    else
        m_lineChanged = QMIN( m_lineChanged, line ); // also works if line=-1
    changed = true;
    //kdDebug(32500) << "KoTextParag::setLineChanged line=" << line << " -> m_lineChanged=" << m_lineChanged << endl;
}

void KoTextParag::insert( int index, const QString &s )
{
#if 0
    if ( doc && !doc->useFormatCollection() && doc->preProcessor() )
	str->insert( index, s,
		     doc->preProcessor()->format( KoTextPreProcessor::Standard ) );
    else
#endif
	str->insert( index, s, formatCollection()->defaultFormat() );
    invalidate( index );
    //needPreProcess = TRUE;
}

void KoTextParag::truncate( int index )
{
    str->truncate( index );
    insert( length(), " " );
    //needPreProcess = TRUE;
}

void KoTextParag::remove( int index, int len )
{
    if ( index + len - str->length() > 0 )
	return;
    for ( int i = index; i < index + len; ++i ) {
	KoTextStringChar *c = at( i );
	if ( doc && c->isCustom() ) {
	    doc->unregisterCustomItem( c->customItem(), this );
	    //removeCustomItem();
	}
    }
    str->remove( index, len );
    invalidate( 0 );
    //needPreProcess = TRUE;
}

void KoTextParag::join( KoTextParag *s )
{
    //kdDebug(32500) << "KoTextParag::join this=" << paragId() << " (length " << length() << ") with " << s->paragId() << " (length " << s->length() << ")" << endl;
    int oh = r.height() + s->r.height();
    n = s->n;
    if ( n )
	n->p = this;
    else if ( doc )
	doc->setLastParag( this );

    int start = str->length();
    if ( length() > 0 && at( length() - 1 )->c == ' ' ) {
	remove( length() - 1, 1 );
	--start;
    }
    append( s->str->toString(), TRUE );

    for ( int i = 0; i < s->length(); ++i ) {
	if ( !doc || doc->useFormatCollection() ) {
	    s->str->at( i ).format()->addRef();
	    str->setFormat( i + start, s->str->at( i ).format(), TRUE );
	}
	if ( s->str->at( i ).isCustom() ) {
	    KoTextCustomItem * item = s->str->at( i ).customItem();
	    str->at( i + start ).setCustomItem( item );
	    s->str->at( i ).loseCustomItem();
	    doc->unregisterCustomItem( item, s ); // ### missing in QRT
	    doc->registerCustomItem( item, this );
	}
    }
    Q_ASSERT(str->at(str->length()-1).c == ' ');

    /*if ( !extraData() && s->extraData() ) {
	setExtraData( s->extraData() );
	s->setExtraData( 0 );
    } else if ( extraData() && s->extraData() ) {
	extraData()->join( s->extraData() );
        }*/
    delete s;
    invalidate( 0 );
    //// kotext
    invalidateCounters();
    ////
    r.setHeight( oh );
    //needPreProcess = TRUE;
    if ( n ) {
	KoTextParag *s = n;
	while ( s ) {
	    s->id = s->p->id + 1;
	    //s->state = -1;
	    //s->needPreProcess = TRUE;
	    s->changed = TRUE;
	    s = s->n;
	}
    }
    format();
    //state = -1;
}

void KoTextParag::move( int &dy )
{
    //kdDebug(32500) << "KoTextParag::move paragId=" << paragId() << " dy=" << dy << endl;
    if ( dy == 0 )
	return;
    changed = TRUE;
    r.moveBy( 0, dy );
    if ( mFloatingItems ) {
	for ( KoTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() ) {
		i->finalize();
	}
    }
    //if ( p )
    //    p->lastInFrame = TRUE; // Qt does this, but the loop at the end of format() calls move a lot!

    movedDown = FALSE;

    // do page breaks if required
    if ( doc && doc->isPageBreakEnabled() ) {
	int shift;
	if ( ( shift = doc->formatter()->formatVertically(  doc, this ) ) ) {
	    if ( p )
		p->setChanged( TRUE );
	    dy += shift;
	}
    }
}

void KoTextParag::format( int start, bool doMove )
{
    if ( !str || str->length() == 0 || !formatter() )
	return;

#if 0
    if ( doc &&
	 doc->preProcessor() &&
	 ( needPreProcess || state == -1 ) )
	doc->preProcessor()->process( doc, this, invalid <= 0 ? 0 : invalid );
    needPreProcess = FALSE;
#endif

    if ( invalid == -1 )
	return;

    //kdDebug(32500) << "KoTextParag::format " << this << " id:" << paragId() << endl;

    r.moveTopLeft( QPoint( documentX(), p ? p->r.y() + p->r.height() : documentY() ) );
    //if ( p )
    //    p->lastInFrame = FALSE;

    movedDown = FALSE;
    bool formattedAgain = FALSE;

 formatAgain:
    r.setWidth( documentWidth() );

    // Not really useful....
    if ( doc && mFloatingItems ) {
	for ( KoTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() ) {
	    if ( i->placement() == KoTextCustomItem::PlaceRight )
		i->move( r.x() + r.width() - i->width, r.y() );
	    else
		i->move( i->x(), r.y() );
	}
    }
    QMap<int, KoTextParagLineStart*> oldLineStarts = lineStarts;
    lineStarts.clear();
    int y;
    bool formatterWorked = formatter()->format( doc, this, start, oldLineStarts, y, m_wused );

    // It can't happen that width < minimumWidth -- hopefully.
    //r.setWidth( QMAX( r.width(), formatter()->minimumWidth() ) );
    //m_minw = formatter()->minimumWidth();

    QMap<int, KoTextParagLineStart*>::Iterator it = oldLineStarts.begin();

    for ( ; it != oldLineStarts.end(); ++it )
	delete *it;

/*    if ( hasBorder() || string()->isRightToLeft() )
        ////kotext: border extends to doc width
        ////        and, bidi parags might have a counter, which will be right-aligned...
    {
        setWidth( textDocument()->width() - 1 );
    }
    else*/
    {
        if ( lineStarts.count() == 1 ) { //&& ( !doc || doc->flow()->isEmpty() ) ) {
// kotext: for proper parag borders, we want all parags to be as wide as linestart->w
/*            if ( !string()->isBidi() ) {
                KoTextStringChar *c = &str->at( str->length() - 1 );
                r.setWidth( c->x + c->width );
            } else*/ {
                r.setWidth( lineStarts[0]->w );
            }
        }
        if ( newLinesAllowed ) {
            it = lineStarts.begin();
            int usedw = 0; int lineid = 0;
            for ( ; it != lineStarts.end(); ++it, ++lineid ) {
                usedw = QMAX( usedw, (*it)->w );
            }
            if ( r.width() <= 0 ) {
                // if the user specifies an invalid rect, this means that the
                // bounding box should grow to the width that the text actually
                // needs
                r.setWidth( usedw );
            } else {
                r.setWidth( QMIN( usedw, r.width() ) );
            }
        }
    }

    if ( y != r.height() )
	r.setHeight( y );

    if ( !visible )
	r.setHeight( 0 );

    // do page breaks if required
    if ( doc && doc->isPageBreakEnabled() ) {
        int shift = doc->formatter()->formatVertically( doc, this );
        //kdDebug(32500) << "formatVertically returned shift=" << shift << endl;
        if ( shift && !formattedAgain ) {
            formattedAgain = TRUE;
            goto formatAgain;
        }
    }

    if ( doc )
        doc->formatter()->postFormat( this );

    if ( n && doMove && n->invalid == -1 && r.y() + r.height() != n->r.y() ) {
        //kdDebug(32500) << "r=" << r << " n->r=" << n->r << endl;
	int dy = ( r.y() + r.height() ) - n->r.y();
	KoTextParag *s = n;
	bool makeInvalid = false; //p && p->lastInFrame;
	//kdDebug(32500) << "might move of dy=" << dy << ". previous's lastInFrame (=makeInvalid): " << makeInvalid << endl;
	while ( s && dy ) {
            if ( s->movedDown ) { // (not in QRT) : moved down -> invalidate and stop moving down
                s->invalidate( 0 ); // (there is no point in moving down a parag that has a frame break...)
                break;
            }
	    if ( !s->isFullWidth() )
		makeInvalid = TRUE;
	    if ( makeInvalid )
		s->invalidate( 0 );
	    s->move( dy );
	    //if ( s->lastInFrame )
            //    makeInvalid = TRUE;
  	    s = s->n;
	}
    }

//#define DEBUG_CI_PLACEMENT
    if ( mFloatingItems ) {
#ifdef DEBUG_CI_PLACEMENT
        kdDebug(32500) << lineStarts.count() << " lines" << endl;
#endif
        // Place custom items - after the formatting is finished
        int len = length();
        int line = -1;
        int lineY = 0; // the one called "cy" in other algos
        int baseLine = 0;
        QMap<int, KoTextParagLineStart*>::Iterator it = lineStarts.begin();
        for ( int i = 0 ; i < len; ++i ) {
            KoTextStringChar *chr = &str->at( i );
            if ( chr->lineStart ) {
                ++line;
                if ( line > 0 )
                    ++it;
                lineY = (*it)->y;
                baseLine = (*it)->baseLine;
#ifdef DEBUG_CI_PLACEMENT
                kdDebug(32500) << "New line (" << line << "): lineStart=" << (*it) << " lineY=" << lineY << " baseLine=" << baseLine << " height=" << (*it)->h << endl;
#endif
            }
            if ( chr->isCustom() ) {
                int x = chr->x;
                KoTextCustomItem* item = chr->customItem();
                Q_ASSERT( baseLine >= item->ascent() ); // something went wrong in KoTextFormatter if this isn't the case
                int y = lineY + baseLine - item->ascent();
#ifdef DEBUG_CI_PLACEMENT
                kdDebug(32500) << "Custom item: i=" << i << " x=" << x << " lineY=" << lineY << " baseLine=" << baseLine << " ascent=" << item->ascent() << " -> y=" << y << endl;
#endif
                item->move( x, y );
                item->finalize();
            }
        }
    }

    //firstFormat = FALSE; //// unused
    if ( formatterWorked > 0 ) // only if it worked, i.e. we had some width to format it
    {
        invalid = -1;
    }
    changed = TRUE;
    //####   string()->setTextChanged( FALSE );
}

int KoTextParag::lineHeightOfChar( int i, int *bl, int *y ) const
{
    if ( !isValid() )
	( (KoTextParag*)this )->format();

    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.end();
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

    kdWarning(32500) << "KoTextParag::lineHeightOfChar: couldn't find lh for " << i << endl;
    return 15;
}

KoTextStringChar *KoTextParag::lineStartOfChar( int i, int *index, int *line ) const
{
    if ( !isValid() )
	( (KoTextParag*)this )->format();

    int l = (int)lineStarts.count() - 1;
    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.end();
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

    kdWarning(32500) << "KoTextParag::lineStartOfChar: couldn't find " << i << endl;
    return 0;
}

int KoTextParag::lines() const
{
    if ( !isValid() )
	( (KoTextParag*)this )->format();

    return (int)lineStarts.count();
}

KoTextStringChar *KoTextParag::lineStartOfLine( int line, int *index ) const
{
    if ( !isValid() )
	( (KoTextParag*)this )->format();

    if ( line >= 0 && line < (int)lineStarts.count() ) {
	QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.begin();
	while ( line-- > 0 )
	    ++it;
	int i = it.key();
	if ( index )
	    *index = i;
	return &str->at( i );
    }

    kdWarning(32500) << "KoTextParag::lineStartOfLine: couldn't find " << line << endl;
    return 0;
}

int KoTextParag::leftGap() const
{
    if ( !isValid() )
	( (KoTextParag*)this )->format();

    int line = 0;
    int x = str->at(0).x;  /* set x to x of first char */
    if ( str->isBidi() ) {
	for ( int i = 1; i < str->length(); ++i )
	    x = QMIN(x, str->at(i).x);
	return x;
    }

    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while (line < (int)lineStarts.count()) {
	int i = it.key(); /* char index */
	x = QMIN(x, str->at(i).x);
	++it;
	++line;
    }
    return x;
}

void KoTextParag::setFormat( int index, int len, const KoTextFormat *_f, bool useCollection, int flags )
{
    Q_ASSERT( useCollection ); // just for info
    if ( index < 0 )
	index = 0;
    if ( index > str->length() - 1 )
	index = str->length() - 1;
    if ( index + len >= str->length() )
	len = str->length() - index;

    KoTextFormatCollection *fc = 0;
    if ( useCollection )
	fc = formatCollection();
    KoTextFormat *of;
    for ( int i = 0; i < len; ++i ) {
	of = str->at( i + index ).format();
	if ( !changed && _f->key() != of->key() )
	    changed = TRUE;
        // Check things that need the textformatter to run
        // (e.g. not color changes)
        // ######## Is this test exhaustive?
	if ( invalid == -1 &&
	     ( _f->font().family() != of->font().family() ||
	       _f->pointSize() != of->pointSize() ||
	       _f->font().weight() != of->font().weight() ||
	       _f->font().italic() != of->font().italic() ||
	       _f->vAlign() != of->vAlign() ||
               _f->relativeTextSize() != of->relativeTextSize() ||
               _f->offsetFromBaseLine() != of->offsetFromBaseLine() ||
               _f->wordByWord() != of->wordByWord()  ||
               _f->attributeFont() != of->attributeFont() ||
               _f->language() != of->language() ||
               _f->hyphenation() != of->hyphenation() ||
               _f->shadowDistanceX() != of->shadowDistanceX() ||
               _f->shadowDistanceY() != of->shadowDistanceY()
                 ) ) {
	    invalidate( 0 );
	}
	if ( flags == -1 || flags == KoTextFormat::Format || !fc ) {
#ifdef DEBUG_COLLECTION
	    kdDebug(32500) << " KoTextParag::setFormat, will use format(f) " << f << " " << _f->key() << endl;
#endif
            KoTextFormat* f = fc ? fc->format( _f ) : const_cast<KoTextFormat *>( _f );
	    str->setFormat( i + index, f, useCollection );
	} else {
#ifdef DEBUG_COLLECTION
	    kdDebug(32500) << " KoTextParag::setFormat, will use format(of,f,flags) of=" << of << " " << of->key() << ", f=" << _f << " " << _f->key() << endl;
#endif
	    KoTextFormat *fm = fc->format( of, _f, flags );
#ifdef DEBUG_COLLECTION
	    kdDebug(32500) << " KoTextParag::setFormat, format(of,f,flags) returned " << fm << " " << fm->key() << " " << endl;
#endif
	    str->setFormat( i + index, fm, useCollection );
	}
    }
}

void KoTextParag::indent( int *oldIndent, int *newIndent )
{
    if ( !doc || !doc->indent() /*|| isListItem() TODO*/ ) {
	if ( oldIndent )
	    *oldIndent = 0;
	if ( newIndent )
	    *newIndent = 0;
	if ( oldIndent && newIndent )
	    *newIndent = *oldIndent;
	return;
    }
    doc->indent()->indent( doc, this, oldIndent, newIndent );
}

void KoTextParag::drawCursorDefault( QPainter &painter, KoTextCursor *cursor, int curx, int cury, int curh, const QColorGroup &cg )
{
    painter.fillRect( QRect( curx, cury, 1, curh ), cg.color( QColorGroup::Text ) );
    painter.save();
    if ( string()->isBidi() ) {
        const int d = 4;
        if ( at( cursor->index() )->rightToLeft ) {
            painter.setPen( Qt::black );
            painter.drawLine( curx, cury, curx - d / 2, cury + d / 2 );
            painter.drawLine( curx, cury + d, curx - d / 2, cury + d / 2 );
        } else {
            painter.setPen( Qt::black );
            painter.drawLine( curx, cury, curx + d / 2, cury + d / 2 );
            painter.drawLine( curx, cury + d, curx + d / 2, cury + d / 2 );
        }
    }
    painter.restore();
}

int *KoTextParag::tabArray() const
{
    int *ta = tArray;
    if ( !ta && doc )
	ta = doc->tabArray();
    return ta;
}

int KoTextParag::nextTabDefault( int, int x )
{
    int *ta = tArray;
    //if ( doc ) {
	if ( !ta )
	    ta = doc->tabArray();
	int tabStopWidth = doc->tabStopWidth();
    //}
    if ( tabStopWidth != 0 )
	return tabStopWidth*(x/tabStopWidth+1);
    else
        return x;
}

/*void KoTextParag::setPainter( QPainter *p, bool adjust  )
{
    pntr = p;
    for ( int i = 0; i < length(); ++i ) {
	if ( at( i )->isCustom() )
	    at( i )->customItem()->setPainter( p, adjust  );
    }
}*/

KoTextFormatCollection *KoTextParag::formatCollection() const
{
    if ( doc )
	return doc->formatCollection();
    //if ( !qFormatCollection )
    //    qFormatCollection = new KoTextFormatCollection;
    //return qFormatCollection;
    return 0L;
}

QString KoTextParag::richText() const
{
    QString s;
#if 0
    KoTextStringChar *formatChar = 0;
    QString spaces;
    for ( int i = 0; i < length()-1; ++i ) {
	KoTextStringChar *c = &str->at( i );
#endif
#if 0
        if ( c->isAnchor() && !c->anchorName().isEmpty() ) {
            if ( c->anchorName().contains( '#' ) ) {
                QStringList l = QStringList::split( '#', c->anchorName() );
                for ( QStringList::ConstIterator it = l.begin(); it != l.end(); ++it )
                    s += "<a name=\"" + *it + "\"></a>";
            } else {
                s += "<a name=\"" + c->anchorName() + "\"></a>";
            }
        }
#endif
#if 0
        if ( !formatChar ) {
            s += c->format()->makeFormatChangeTags( 0, QString::null, QString::null /*c->anchorHref()*/ );
            formatChar = c;
        } else if ( ( formatChar->format()->key() != c->format()->key() && c->c != ' ' ) /* ||
                  (formatChar->isAnchor() != c->isAnchor() &&
                  (!c->anchorHref().isEmpty() || !formatChar->anchorHref().isEmpty() ) ) */ )  {// lisp was here
            s += c->format()->makeFormatChangeTags( formatChar->format(), QString::null /*formatChar->anchorHref()*/,
                                                    QString::null /*c->anchorHref()*/ );
            formatChar = c;
        }

        if ( c->c == ' ' || c->c == '\t' ) {
            spaces += c->c;
            continue;
        } else if ( !spaces.isEmpty() ) {
            if ( spaces.length() > 1 || spaces[0] == '\t' )
                s += "<wsp>" + spaces + "</wsp>";
            else
                s += spaces;
            spaces = QString::null;
        }

        if ( c->c == '<' ) {
            s += "&lt;";
        } else if ( c->c == '>' ) {
            s += "&gt;";
        } else if ( c->isCustom() ) {
            s += c->customItem()->richText();
        } else {
            s += c->c;
        }
    }
    if ( !spaces.isEmpty() ) {
        if ( spaces.length() > 1 || spaces[0] == '\t' )
                s += "<wsp>" + spaces + "</wsp>";
        else
            s += spaces;
    }

    if ( formatChar )
        s += formatChar->format()->makeFormatEndTags( QString::null /*formatChar->anchorHref()*/ );
#endif
    return s;
}

/*void KoTextParag::addCommand( KoTextDocCommand *cmd )
{
    if ( !doc )
	commandHistory->addCommand( cmd );
    else
	doc->commands()->addCommand( cmd );
}

KoTextCursor *KoTextParag::undo( KoTextCursor *c )
{
    if ( !doc )
	return commandHistory->undo( c );
    return doc->commands()->undo( c );
}

KoTextCursor *KoTextParag::redo( KoTextCursor *c )
{
    if ( !doc )
	return commandHistory->redo( c );
    return doc->commands()->redo( c );
}*/

void KoTextParag::show()
{
    if ( visible || !doc )
	return;
    visible = TRUE;
}

void KoTextParag::hide()
{
    if ( !visible || !doc )
	return;
    visible = FALSE;
}

void KoTextParag::setDirection( QChar::Direction d )
{
    if ( str && str->direction() != d ) {
	str->setDirection( d );
	invalidate( 0 );
        //// kotext
        m_layout.direction = d;
       invalidateCounters(); // #47178
        ////
    }
}

QChar::Direction KoTextParag::direction() const
{
    return (str ? str->direction() : QChar::DirON );
}

void KoTextParag::setSelection( int id, int start, int end )
{
    QMap<int, KoTextParagSelection>::ConstIterator it = selections().find( id );
    if ( it != mSelections->end() ) {
	if ( start == ( *it ).start && end == ( *it ).end )
	    return;
    }

    KoTextParagSelection sel;
    sel.start = start;
    sel.end = end;
    (*mSelections)[ id ] = sel;
    setChanged( TRUE, TRUE );
}

void KoTextParag::removeSelection( int id )
{
    if ( !hasSelection( id ) )
	return;
    if ( mSelections )
	mSelections->remove( id );
    setChanged( TRUE, TRUE );
}

int KoTextParag::selectionStart( int id ) const
{
    if ( !mSelections )
	return -1;
    QMap<int, KoTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).start;
}

int KoTextParag::selectionEnd( int id ) const
{
    if ( !mSelections )
	return -1;
    QMap<int, KoTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).end;
}

bool KoTextParag::hasSelection( int id ) const
{
    if ( !mSelections )
	return FALSE;
    QMap<int, KoTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return FALSE;
    return ( *it ).start != ( *it ).end || length() == 1;
}

bool KoTextParag::fullSelected( int id ) const
{
    if ( !mSelections )
	return FALSE;
    QMap<int, KoTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return FALSE;
    return ( *it ).start == 0 && ( *it ).end == str->length() - 1;
}

int KoTextParag::lineY( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	kdWarning(32500) << "KoTextParag::lineY: line " << l << " out of range!" << endl;
	return 0;
    }

    if ( !isValid() )
	( (KoTextParag*)this )->format();

    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->y;
}

int KoTextParag::lineBaseLine( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	kdWarning(32500) << "KoTextParag::lineBaseLine: line " << l << " out of range!" << endl;
	return 10;
    }

    if ( !isValid() )
	( (KoTextParag*)this )->format();

    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->baseLine;
}

int KoTextParag::lineHeight( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	kdWarning(32500) << "KoTextParag::lineHeight: line " << l << " out of range!" << endl;
	return 15;
    }

    if ( !isValid() )
	( (KoTextParag*)this )->format();

    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->h;
}

void KoTextParag::lineInfo( int l, int &y, int &h, int &bl ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	kdWarning(32500) << "KoTextParag::lineInfo: line " << l << " out of range!" << endl;
	kdDebug(32500) << (int)lineStarts.count() - 1 << " " << l << endl;
	y = 0;
	h = 15;
	bl = 10;
	return;
    }

    if ( !isValid() )
	( (KoTextParag*)this )->format();

    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    y = ( *it )->y;
    h = ( *it )->h;
    bl = ( *it )->baseLine;
}

uint KoTextParag::alignment() const
{
    return align;
}

void KoTextParag::setFormat( KoTextFormat *fm )
{
#if 0
    bool doUpdate = FALSE;
    if (defFormat && (defFormat != formatCollection()->defaultFormat()))
       doUpdate = TRUE;
#endif
    defFormat = formatCollection()->format( fm );
#if 0
    if ( !doUpdate )
	return;
    for ( int i = 0; i < length(); ++i ) {
	if ( at( i )->format()->styleName() == defFormat->styleName() )
	    at( i )->format()->updateStyle();
    }
#endif
}

KoTextFormatterBase *KoTextParag::formatter() const
{
    if ( doc )
	return doc->formatter();
    //if ( pFormatter )
    //    return pFormatter;
    //return ( ( (KoTextParag*)this )->pFormatter = new KoTextFormatterBaseBreakWords );
    return 0L;
}

/*void KoTextParag::setFormatter( KoTextFormatterBase *f )
{
    if ( doc ) return;
    if ( pFormatter ) delete pFormatter;
    pFormatter = f;
}*/

/*int KoTextParag::minimumWidth() const
{
    //return doc ? doc->minimumWidth() : 0;
    return m_minw;
}*/

int KoTextParag::widthUsed() const
{
    return m_wused;
}

void KoTextParag::setTabArray( int *a )
{
    delete [] tArray;
    tArray = a;
}

void KoTextParag::setTabStops( int tw )
{
    if ( doc )
	doc->setTabStops( tw );
    //else
    //    tabStopWidth = tw;
}

QMap<int, KoTextParagSelection> &KoTextParag::selections() const
{
    if ( !mSelections )
	((KoTextParag *)this)->mSelections = new QMap<int, KoTextParagSelection>;
    return *mSelections;
}

QPtrList<KoTextCustomItem> &KoTextParag::floatingItems() const
{
    if ( !mFloatingItems )
	((KoTextParag *)this)->mFloatingItems = new QPtrList<KoTextCustomItem>;
    return *mFloatingItems;
}

void KoTextCursor::setIndex( int i, bool restore )
{
    if ( restore )
	restoreState();
// Note: QRT doesn't allow to position the cursor at string->length
// However we need it, when applying a style to a paragraph, so that
// the trailing space gets the style change applied as well.
// Obviously "right of the trailing space" isn't a good place for a real
// cursor, but this needs to be checked somewhere else.
    if ( i < 0 || i > string->length() ) {
#if defined(QT_CHECK_RANGE)
	kdWarning(32500) << "KoTextCursor::setIndex: " << i << " out of range" << endl;
        //abort();
#endif
	i = i < 0 ? 0 : string->length() - 1;
    }

    tmpIndex = -1;
    idx = i;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KoTextFormatterBase::KoTextFormatterBase()
    : wrapColumn( -1 ), wrapEnabled( TRUE ),
      m_bViewFormattingChars( false ),
      biw( true /*default in kotext*/ )
{
}

// See KoTextFormatter
#if 0
KoTextParagLineStart *KoTextFormatterBase::formatLine( KoTextParag *parag, KoTextString *string, KoTextParagLineStart *line,
						   KoTextStringChar *startChar, KoTextStringChar *lastChar, int align, int space )
{
#ifndef QT_NO_COMPLEXTEXT
    if( string->isBidi() )
	return bidiReorderLine( parag, string, line, startChar, lastChar, align, space );
#endif
    space = QMAX( space, 0 ); // #### with nested tables this gets negative because of a bug I didn't find yet, so workaround for now. This also means non-left aligned nested tables do not work at the moment
    int start = (startChar - &string->at(0));
    int last = (lastChar - &string->at(0) );
    // do alignment Auto == Left in this case
    if ( align & Qt::AlignHCenter || align & Qt::AlignRight ) {
	if ( align & Qt::AlignHCenter )
	    space /= 2;
	for ( int j = start; j <= last; ++j )
	    string->at( j ).x += space;
    } else if ( align & AlignJustify ) {
	int numSpaces = 0;
	for ( int j = start; j < last; ++j ) {
	    if( isBreakable( string, j ) ) {
		numSpaces++;
	    }
	}
	int toAdd = 0;
	for ( int k = start + 1; k <= last; ++k ) {
	    if( isBreakable( string, k ) && numSpaces ) {
		int s = space / numSpaces;
		toAdd += s;
		space -= s;
		numSpaces--;
	    }
	    string->at( k ).x += toAdd;
	}
    }

    if ( last >= 0 && last < string->length() )
	line->w = string->at( last ).x + string->width( last );
    else
	line->w = 0;

    return new KoTextParagLineStart();
}
#endif

#ifdef BIDI_DEBUG
#include <iostream>
#endif

// collects one line of the paragraph and transforms it to visual order
KoTextParagLineStart *KoTextFormatterBase::bidiReorderLine( KoTextParag * /*parag*/, KoTextString *text, KoTextParagLineStart *line,
							KoTextStringChar *startChar, KoTextStringChar *lastChar, int align, int space )
{
    int start = (startChar - &text->at(0));
    int last = (lastChar - &text->at(0) );
    //kdDebug(32500) << "doing BiDi reordering from " << start << " to " << last << "!" << endl;

    KoBidiControl *control = new KoBidiControl( line->context(), line->status );
    QString str;
    str.setUnicode( 0, last - start + 1 );
    // fill string with logically ordered chars.
    KoTextStringChar *ch = startChar;
    QChar *qch = (QChar *)str.unicode();
    while ( ch <= lastChar ) {
	*qch = ch->c;
	qch++;
	ch++;
    }
    int x = startChar->x;

    QPtrList<KoTextRun> *runs;
    runs = KoComplexText::bidiReorderLine(control, str, 0, last - start + 1,
					 (text->isRightToLeft() ? QChar::DirR : QChar::DirL) );

    // now construct the reordered string out of the runs...

    int numSpaces = 0;
    // set the correct alignment. This is a bit messy....
    if( align == Qt::AlignAuto ) {
	// align according to directionality of the paragraph...
	if ( text->isRightToLeft() )
	    align = Qt::AlignRight;
    }

    if ( align & Qt::AlignHCenter )
	x += space/2;
    else if ( align & Qt::AlignRight )
	x += space;
    else if ( align & Qt::AlignJustify ) {
	for ( int j = start; j < last; ++j ) {
	    if( isBreakable( text, j ) ) {
		numSpaces++;
	    }
	}
    }
    int toAdd = 0;
    bool first = TRUE;
    KoTextRun *r = runs->first();
    int xmax = -0xffffff;
    while ( r ) {
	if(r->level %2) {
	    // odd level, need to reverse the string
	    int pos = r->stop + start;
	    while(pos >= r->start + start) {
		KoTextStringChar *c = &text->at(pos);
		if( numSpaces && !first && isBreakable( text, pos ) ) {
		    int s = space / numSpaces;
		    toAdd += s;
		    space -= s;
		    numSpaces--;
		} else if ( first ) {
		    first = FALSE;
		    if ( c->c == ' ' )
			x -= c->format()->width( ' ' );
		}
		c->x = x + toAdd;
		c->rightToLeft = TRUE;
		c->startOfRun = FALSE;
		int ww = 0;
		if ( c->c.unicode() >= 32 || c->c == '\t' || c->c == '\n' || c->isCustom() ) {
		    ww = c->width;
		} else {
		    ww = c->format()->width( ' ' );
		}
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
		pos--;
	    }
	} else {
	    int pos = r->start + start;
	    while(pos <= r->stop + start) {
		KoTextStringChar* c = &text->at(pos);
		if( numSpaces && !first && isBreakable( text, pos ) ) {
		    int s = space / numSpaces;
		    toAdd += s;
		    space -= s;
		    numSpaces--;
		} else if ( first ) {
		    first = FALSE;
		    if ( c->c == ' ' )
			x -= c->format()->width( ' ' );
		}
		c->x = x + toAdd;
		c->rightToLeft = FALSE;
		c->startOfRun = FALSE;
		int ww = 0;
		if ( c->c.unicode() >= 32 || c->c == '\t' || c->isCustom() ) {
		    ww = c->width;
		} else {
		    ww = c->format()->width( ' ' );
		}
		//kdDebug(32500) << "setting char " << pos << " at pos " << x << endl;
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
		pos++;
	    }
	}
	text->at( r->start + start ).startOfRun = TRUE;
	r = runs->next();
    }

    line->w = xmax + 10;
    KoTextParagLineStart *ls = new KoTextParagLineStart( control->context, control->status );
    delete control;
    delete runs;
    return ls;
}

bool KoTextFormatterBase::isStretchable( KoTextString *string, int pos ) const
{
    if ( string->at( pos ).c == QChar(160) ) //non-breaking space
	return true;
#ifndef INDIC
    return isBreakable( string, pos );
#else
    KoTextStringChar& chr = string->at( pos );
    return chr.whiteSpace;
    //return isBreakable( string, pos );
#endif
}

bool KoTextFormatterBase::isBreakable( KoTextString *string, int pos ) const
{
#ifndef INDIC
    const QChar &c = string->at( pos ).c;
    char ch = c.latin1();
    if ( c.isSpace() && ch != '\n' && c.unicode() != 0x00a0U )
#else
    //if (string->at(pos).nobreak)
    //    return FALSE;
    return (pos < string->length()-1 && string->at(pos+1).softBreak);

#if 0
    const QChar &c = string->at(pos).c;
    if ( c.isSpace() && c.unicode() != '\n' && c.unicode() != 0x00a0U )
#endif
	return TRUE;
    if ( c == '-' || c.unicode() == 0xad ) // hyphen or soft hyphen
	return TRUE;
    if ( !ch ) {
	// not latin1, need to do more sophisticated checks for other scripts
	uchar row = c.row();
	if ( row == 0x0e ) {
	    // 0e00 - 0e7f == Thai
	    if ( c.cell() < 0x80 ) {
#ifdef HAVE_THAI_BREAKS
		// check for thai
		if( string != cachedString ) {
		    // build up string of thai chars
		    QTextCodec *thaiCodec = QTextCodec::codecForMib(2259);
		    if ( !thaiCache )
			thaiCache = new QCString;
		    if ( !thaiIt )
			thaiIt = ThBreakIterator::createWordInstance();
		    *thaiCache = thaiCodec->fromUnicode( s->string() );
		}
		thaiIt->setText(thaiCache->data());
		for(int i = thaiIt->first(); i != thaiIt->DONE; i = thaiIt->next() ) {
		    if( i == pos )
			return TRUE;
		    if( i > pos )
			return FALSE;
		}
		return FALSE;
#else
		// if we don't have a thai line breaking lib, allow
		// breaks everywhere except directly before punctuation.
		return TRUE;
#endif
	    } else
		return FALSE;
	}
	if ( row < 0x11 ) // no asian font
	    return FALSE;
	if ( row > 0x2d && row < 0xfb || row == 0x11 )
	    // asian line breaking. Everywhere allowed except directly
	    // in front of a punctuation character.
	    return TRUE;
    }
    return FALSE;
#endif
}

void KoTextParag::insertLineStart( int index, KoTextParagLineStart *ls )
{
    // This tests if we break at the same character in more than one line,
    // i.e. there no space even for _one_ char in a given line.
    // However this shouldn't happen, KoTextFormatter prevents it, otherwise
    // we could loop forever (e.g. if one char is wider than the page...)
#ifndef NDEBUG
    QMap<int, KoTextParagLineStart*>::Iterator it;
    if ( ( it = lineStarts.find( index ) ) == lineStarts.end() ) {
	lineStarts.insert( index, ls );
    } else {
        kdWarning(32500) << "insertLineStart: there's already a line for char index=" << index << endl;
	delete *it;
	lineStarts.remove( it );
	lineStarts.insert( index, ls );
    }
#else // non-debug code, take the fast route
    lineStarts.insert( index, ls );
#endif
}


/* Standard pagebreak algorithm using KoTextFlow::adjustFlow. Returns
 the shift of the paragraphs bottom line.
 */
int KoTextFormatterBase::formatVertically( KoTextDocument* doc, KoTextParag* parag )
{
    int oldHeight = parag->rect().height();
    QMap<int, KoTextParagLineStart*>& lineStarts = parag->lineStartList();
    QMap<int, KoTextParagLineStart*>::Iterator it = lineStarts.begin();
    int h = doc->addMargins() ? parag->topMargin() : 0;
    for ( ; it != lineStarts.end() ; ++it  ) {
	KoTextParagLineStart * ls = it.data();
	ls->y = h;
	KoTextStringChar *c = &parag->string()->at(it.key());
	if ( c && c->customItem() && c->customItem()->ownLine() ) {
	    int h = c->customItem()->height;
	    c->customItem()->pageBreak( parag->rect().y() + ls->y + ls->baseLine - h, doc->flow() );
	    int delta = c->customItem()->height - h;
	    ls->h += delta;
	    if ( delta )
		parag->setMovedDown( TRUE );
	} else {
	    int shift = doc->flow()->adjustFlow( parag->rect().y() + ls->y, ls->w, ls->h );
	    ls->y += shift;
	    if ( shift )
		parag->setMovedDown( TRUE );
	}
	h = ls->y + ls->h;
    }
    int m = parag->bottomMargin();
    if ( parag->next() && doc && !doc->addMargins() )
	m = QMAX( m, parag->next()->topMargin() );
    //if ( parag->next() && parag->next()->isLineBreak() )
    //	m = 0;
    h += m;
    parag->setHeight( h );
    return h - oldHeight;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KoTextIndent::KoTextIndent()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

KoTextCustomItem::KoTextCustomItem( KoTextDocument *p )
      :  width(-1), height(0), parent(p), xpos(0), ypos(-1), parag(0)
{
    m_deleted = false; // added for kotext
}

KoTextCustomItem::~KoTextCustomItem()
{
}

KoTextFlow::KoTextFlow()
{
    w = 0;
    leftItems.setAutoDelete( FALSE );
    rightItems.setAutoDelete( FALSE );
}

KoTextFlow::~KoTextFlow()
{
}

void KoTextFlow::clear()
{
    leftItems.clear();
    rightItems.clear();
}

// Called by KoTextDocument::setWidth
void KoTextFlow::setWidth( int width )
{
    w = width;
}

void KoTextFlow::adjustMargins( int, int, int, int&, int&, int& pageWidth, KoTextParag* )
{
    pageWidth = w;
}

#if 0
int KoTextFlow::adjustLMargin( int yp, int, int margin, int space, KoTextParag* )
{
    for ( KoTextCustomItem* item = leftItems.first(); item; item = leftItems.next() ) {
	if ( item->y() == -1 )
	    continue;
	if ( yp >= item->y() && yp < item->y() + item->height )
	    margin = QMAX( margin, item->x() + item->width + space );
    }
    return margin;
}

int KoTextFlow::adjustRMargin( int yp, int, int margin, int space, KoTextParag* )
{
    for ( KoTextCustomItem* item = rightItems.first(); item; item = rightItems.next() ) {
	if ( item->y() == -1 )
	    continue;
	if ( yp >= item->y() && yp < item->y() + item->height )
	    margin = QMAX( margin, w - item->x() - space );
    }
    return margin;
}
#endif

int KoTextFlow::adjustFlow( int /*y*/, int, int /*h*/ )
{
#if 0
    if ( pagesize > 0 ) { // check pages
	int yinpage = y % pagesize;
	if ( yinpage <= 2 )
	    return 2 - yinpage;
	else
	    if ( yinpage + h > pagesize - 2 )
		return ( pagesize - yinpage ) + 2;
    }
#endif
    return 0;
}

void KoTextFlow::unregisterFloatingItem( KoTextCustomItem* item )
{
    leftItems.removeRef( item );
    rightItems.removeRef( item );
}

void KoTextFlow::registerFloatingItem( KoTextCustomItem* item )
{
    if ( item->placement() == KoTextCustomItem::PlaceRight ) {
	if ( !rightItems.contains( item ) )
	    rightItems.append( item );
    } else if ( item->placement() == KoTextCustomItem::PlaceLeft &&
		!leftItems.contains( item ) ) {
	leftItems.append( item );
    }
}

#if 0
QRect KoTextFlow::boundingRect() const
{
    QRect br;
    QPtrListIterator<KoTextCustomItem> l( leftItems );
    while( l.current() ) {
	br = br.unite( l.current()->geometry() );
	++l;
    }
    QPtrListIterator<KoTextCustomItem> r( rightItems );
    while( r.current() ) {
	br = br.unite( r.current()->geometry() );
	++r;
    }
    return br;
}
#endif

int KoTextFlow::availableHeight() const
{
    return -1; // no limit
}

void KoTextFlow::drawFloatingItems( QPainter* p, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    KoTextCustomItem *item;
    for ( item = leftItems.first(); item; item = leftItems.next() ) {
	if ( item->x() == -1 || item->y() == -1 )
	    continue;
	item->draw( p, item->x(), item->y(), cx, cy, cw, ch, cg, selected );
    }

    for ( item = rightItems.first(); item; item = rightItems.next() ) {
	if ( item->x() == -1 || item->y() == -1 )
	    continue;
	item->draw( p, item->x(), item->y(), cx, cy, cw, ch, cg, selected );
    }
}

//void KoTextFlow::setPageSize( int ps ) { pagesize = ps; }
bool KoTextFlow::isEmpty() { return leftItems.isEmpty() && rightItems.isEmpty(); }

