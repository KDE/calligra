/****************************************************************************
** $Id$
**
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

#include "qrichtext_p.h"
#include <assert.h>

//void KoTextCustomItem::setPainter( QPainter*, bool adjust ){ if ( adjust ) width = 0; }
void KoTextCustomItem::setPainter( QPainter*, bool ){ resize(); } // changed for kotext

bool KoTextCustomItem::enter( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy, bool atEnd )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; Q_UNUSED( atEnd ) return TRUE;

}
bool KoTextCustomItem::enterAt( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy, const QPoint & )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool KoTextCustomItem::next( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool KoTextCustomItem::prev( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool KoTextCustomItem::down( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool KoTextCustomItem::up( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}

void KoTextFlow::setPageSize( int ps ) { pagesize = ps; }
bool KoTextFlow::isEmpty() { return leftItems.isEmpty() && rightItems.isEmpty(); }

#ifdef QTEXTTABLE_AVAILABLE
void KoTextTableCell::invalidate() { cached_width = -1; cached_sizehint = -1; }

void KoTextTable::invalidate() { cachewidth = -1; }
#endif

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

bool KoTextDocument::hasSelection( int id, bool visible ) const
{
    return ( selections.find( id ) != selections.end() &&
             ( !visible ||
               ( (KoTextDocument*)this )->selectionStartCursor( id ) !=
               ( (KoTextDocument*)this )->selectionEndCursor( id ) ) );
}

void KoTextDocument::setSelectionStart( int id, KoTextCursor *cursor )
{
    KoTextDocumentSelection sel;
    sel.startCursor = *cursor;
    sel.endCursor = *cursor;
    sel.swapped = FALSE;
    selections[ id ] = sel;
}

KoTextParag *KoTextDocument::paragAt( int i ) const
{
    KoTextParag *s = fParag;
    while ( s ) {
	if ( s->paragId() == i )
	    return s;
	s = s->next();
    }
    return 0;
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
	qWarning( "KoTextParag::lineY: line %d out of range!", l );
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
	qWarning( "KoTextParag::lineBaseLine: line %d out of range!", l );
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
	qWarning( "KoTextParag::lineHeight: line %d out of range!", l );
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
	qWarning( "KoTextParag::lineInfo: line %d out of range!", l );
	qDebug( "%d %d", (int)lineStarts.count() - 1, l );
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

int KoTextParag::alignment() const
{
//    if ( align != -1 )
	return align;
#if 0
    QStyleSheetItem *item = qstyle();
    if ( !item )
	return Qt::AlignAuto;
    if ( mStyleSheetItemsVec ) {
	for ( int i = 0; i < (int)mStyleSheetItemsVec->size(); ++i ) {
	    item = (*mStyleSheetItemsVec)[ i ];
	    if ( item->alignment() != QStyleSheetItem::Undefined )
		return item->alignment();
	}
    }
    return Qt::AlignAuto;
#endif
}

QPtrVector<QStyleSheetItem> KoTextParag::styleSheetItems() const
{
    QPtrVector<QStyleSheetItem> vec;
    if ( mStyleSheetItemsVec ) {
	vec.resize( mStyleSheetItemsVec->size() );
	for ( int i = 0; i < (int)vec.size(); ++i )
	    vec.insert( i, (*mStyleSheetItemsVec)[ i ] );
    }
    return vec;
}

QStyleSheetItem *KoTextParag::qstyle() const
{
    if ( !mStyleSheetItemsVec || mStyleSheetItemsVec->size() == 0 )
	return 0;
    return (*mStyleSheetItemsVec)[ mStyleSheetItemsVec->size() - 1 ];
}

int KoTextParag::numberOfSubParagraph() const
{
    if ( list_val != -1 )
	return list_val;
    if ( numSubParag != -1 )
 	return numSubParag;
    int n = 0;
    KoTextParag *p = (KoTextParag*)this;
    while ( p && ( styleSheetItemsVec().size() >= p->styleSheetItemsVec().size() &&
	    styleSheetItemsVec()[ (int)p->styleSheetItemsVec().size() - 1 ] == p->qstyle() ||
		   p->styleSheetItemsVec().size() >= styleSheetItemsVec().size() &&
		   p->styleSheetItemsVec()[ (int)styleSheetItemsVec().size() - 1 ] == qstyle() ) ) {
	if ( p->qstyle() == qstyle() && listStyle() != p->listStyle()
	     && p->styleSheetItemsVec().size() == styleSheetItemsVec().size() )
	    break;
	if ( p->qstyle()->displayMode() == QStyleSheetItem::DisplayListItem
	     && p->qstyle() != qstyle() || styleSheetItemsVec().size() == p->styleSheetItemsVec().size() )
	    ++n;
	p = p->prev();
    }
    ( (KoTextParag*)this )->numSubParag = n;
    return n;
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
    if ( pFormatter )
	return pFormatter;
    //return ( ( (KoTextParag*)this )->pFormatter = new KoTextFormatterBaseBreakWords );
    return 0L;
}

void KoTextParag::setFormatter( KoTextFormatterBase *f )
{
    if ( doc ) return;
    if ( pFormatter ) delete pFormatter;
    pFormatter = f;
}

int KoTextParag::minimumWidth() const
{
    return doc ? doc->minimumWidth() : 0;
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
    else
	tabStopWidth = tw;
}

QMap<int, KoTextParagSelection> &KoTextParag::selections() const
{
    if ( !mSelections )
	((KoTextParag *)this)->mSelections = new QMap<int, KoTextParagSelection>;
    return *mSelections;
}

QPtrVector<QStyleSheetItem> &KoTextParag::styleSheetItemsVec() const
{
    if ( !mStyleSheetItemsVec )
	((KoTextParag *)this)->mStyleSheetItemsVec = new QPtrVector<QStyleSheetItem>;
    return *mStyleSheetItemsVec;
}

QPtrList<KoTextCustomItem> &KoTextParag::floatingItems() const
{
    if ( !mFloatingItems )
	((KoTextParag *)this)->mFloatingItems = new QPtrList<KoTextCustomItem>;
    return *mFloatingItems;
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

#include "qrichtext_p.moc"
