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

using namespace Qt3;
#define QTextCustomItem Qt3::QTextCustomItem
#define QTextFlow Qt3::QTextFlow
#define QTextDocument Qt3::QTextDocument
#define QTextParag Qt3::QTextParag

void QTextCustomItem::setPainter( QPainter*, bool adjust ){ if ( adjust ) width = 0; }

bool QTextCustomItem::enter( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy, bool atEnd )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; Q_UNUSED( atEnd ) return TRUE;

}
bool QTextCustomItem::enterAt( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy, const QPoint & )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::next( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::prev( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::down( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::up( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}

void QTextFlow::setPageSize( int ps ) { pagesize = ps; }
bool QTextFlow::isEmpty() { return leftItems.isEmpty() && rightItems.isEmpty(); }

#ifdef QTEXTTABLE_AVAILABLE
void QTextTableCell::invalidate() { cached_width = -1; cached_sizehint = -1; }

void QTextTable::invalidate() { cachewidth = -1; }
#endif

int QTextCursor::x() const
{
    KoTextStringChar *c = string->at( idx );
    int curx = c->x;
    if ( c->rightToLeft )
        curx += c->width; //string->string()->width( idx );
    return curx;
}

int QTextCursor::y() const
{
    int dummy, line;
    string->lineStartOfChar( idx, &dummy, &line );
    return string->lineY( line );
}

bool QTextDocument::hasSelection( int id ) const
{
    return selections.find( id ) != selections.end();
}

void QTextDocument::setSelectionStart( int id, QTextCursor *cursor )
{
    QTextDocumentSelection sel;
    sel.startCursor = *cursor;
    sel.endCursor = *cursor;
    sel.swapped = FALSE;
    selections[ id ] = sel;
}

QTextParag *QTextDocument::paragAt( int i ) const
{
    QTextParag *s = fParag;
    while ( s ) {
	if ( s->paragId() == i )
	    return s;
	s = s->next();
    }
    return 0;
}

QTextFormat::QTextFormat()
    : fm( QFontMetrics( fn ) ), linkColor( TRUE ), logicalFontSize( 3 ), stdPointSize( qApp->font().pointSize() ),
      painter( 0 ), different( NoFlags )
{
    ref = 0;
    missp = FALSE;
    ha = AlignNormal;
    collection = 0;
    // WYSIWYG works much much better with scalable fonts -> force it to be scalable
    fn.setStyleStrategy( QFont::ForceOutline );
//#ifdef DEBUG_COLLECTION
//    qDebug("QTextFormat simple ctor, no addRef ! %p",this);
//#endif
}

QTextFormat::QTextFormat( const QStyleSheetItem *style )
    : fm( QFontMetrics( fn ) ), linkColor( TRUE ), logicalFontSize( 3 ), stdPointSize( qApp->font().pointSize() ),
      painter( 0 ), different( NoFlags )
{
#ifdef DEBUG_COLLECTION
    qDebug("QTextFormat::QTextFormat( const QStyleSheetItem *style )");
#endif
    ref = 0;
    this->style = style->name();
    missp = FALSE;
    ha = AlignNormal;
    collection = 0;
    fn = QFont( style->fontFamily(),
		style->fontSize(),
		style->fontWeight(),
		style->fontItalic() );
    fn.setUnderline( style->fontUnderline() );
    // WYSIWYG works much much better with scalable fonts -> force it to be scalable
    fn.setStyleStrategy( QFont::ForceOutline );
    col = style->color();
    fm = QFontMetrics( fn );
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.height();
    asc = fm.ascent();
    dsc = fm.descent();
    missp = FALSE;
    ha = AlignNormal;
    memset( widths, 0, 256 * sizeof( ushort ) );
    generateKey();
    addRef();
    updateStyleFlags();
}

QTextFormat::QTextFormat( const QFont &f, const QColor &c, QTextFormatCollection *parent )
    : fn( f ), col( c ), fm( QFontMetrics( f ) ), linkColor( TRUE ),
      logicalFontSize( 3 ), stdPointSize( f.pointSize() ), painter( 0 ),
      different( NoFlags )
{
#ifdef DEBUG_COLLECTION
    qDebug("QTextFormat with font & color & parent (%p), addRef. %p", parent, this);
#endif
    if ( fn.pointSize() == -1 ) // font was set with a pixelsize, we need a pointsize!
        fn.setPointSizeFloat( ( (double)fn.pixelSize() * 72.0 ) / (double)QPaintDevice::x11AppDpiY() );
    // WYSIWYG works much much better with scalable fonts -> force it to be scalable
    fn.setStyleStrategy( QFont::ForceOutline );
    ref = 0;
    collection = parent;
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.height();
    asc = fm.ascent();
    dsc = fm.descent();
    missp = FALSE;
    ha = AlignNormal;
    memset( widths, 0, 256 * sizeof( ushort ) );
    generateKey();
    addRef();
    updateStyleFlags();
}

QTextFormat::QTextFormat( const QTextFormat &f )
    : fm( f.fm )
{
#ifdef DEBUG_COLLECTION
    //qDebug("QTextFormat::QTextFormat %p copy ctor (copying %p). Will addRef.",this,&f);
#endif
    ref = 0;
    collection = 0; // f might be in the collection, but we are not
    fn = f.fn;
    col = f.col;
    painter = f.painter;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset( widths, 0, 256 * sizeof( ushort ) );
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdPointSize = f.stdPointSize;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    style = f.style;
    different = f.different;
    addRef();
}

QTextFormat::~QTextFormat()
{
}

QTextFormat& QTextFormat::operator=( const QTextFormat &f )
{
#ifdef DEBUG_COLLECTION
    qDebug("QTextFormat::operator= %p (copying %p). Will addRef",this,&f);
#endif
    ref = 0;
    collection = 0; // f might be in the collection, but we are not
    fn = f.fn;
    col = f.col;
    fm = f.fm;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset( widths, 0, 256 * sizeof( ushort ) );
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdPointSize = f.stdPointSize;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    style = f.style;
    different = f.different;
    addRef();
    return *this;
}

void QTextFormat::update()
{
    //qDebug("%p QTextFormat::update %s %d",this, fn.family().latin1(),fn.pointSize());
    fn.setStyleStrategy( QFont::ForceOutline );
    fm = QFontMetrics( fn );
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.height();
    asc = fm.ascent();
    dsc = fm.descent();
    memset( widths, 0, 256 * sizeof( ushort ) );
    generateKey();
    updateStyleFlags();
}

int QTextFormat::minLeftBearing() const
{
    if ( !painter || !painter->isActive() )
	return leftBearing;
    painter->setFont( fn );
    return painter->fontMetrics().minLeftBearing();
}

int QTextFormat::minRightBearing() const
{
    if ( !painter || !painter->isActive() )
	return rightBearing;
    painter->setFont( fn );
    return painter->fontMetrics().minRightBearing();
}

int QTextFormat::height() const
{
    if ( !painter || !painter->isActive() )
	return hei;
    painter->setFont( fn );
    return painter->fontMetrics().height();
}

int QTextFormat::ascent() const
{
    if ( !painter || !painter->isActive() )
	return asc;
    painter->setFont( fn );
    return painter->fontMetrics().ascent();
}

int QTextFormat::descent() const
{
    if ( !painter || !painter->isActive() )
	return dsc;
    painter->setFont( fn );
    return painter->fontMetrics().descent();
}

void QTextFormat::generateKey()
{
    k = QString::number( fn.pointSize() );
    k += '/';
    k += QString::number( fn.weight() );
    k += '/';
    k += QString::number( (int)fn.underline() );
    k += '/';
    k += QString::number( (int)fn.strikeOut() );
    k += '/';
    k += QString::number( (int)fn.italic() );
    k += '/';
    if ( col.isValid() ) // just to shorten the key in the common case
        k += QString::number( (uint)col.rgb() );
    k += '/';
    k += fn.family();
    k += '/';
    k += QString::number( (int)isMisspelled() );
    k += '/';
    k += QString::number( (int)vAlign() );
}

QString QTextFormat::getKey( const QFont &fn, const QColor &col, bool misspelled, VerticalAlignment a )
{
    QString k = QString::number( fn.pointSize() );
    k += '/';
    k += QString::number( fn.weight() );
    k += '/';
    k += QString::number( (int)fn.underline() );
    k += '/';
    k += QString::number( (int)fn.strikeOut() );
    k += '/';
    k += QString::number( (int)fn.italic() );
    k += '/';
    if ( col.isValid() ) // just to shorten the key in the common case
        k += QString::number( (uint)col.rgb() );
    k += '/';
    k += fn.family();
    k += '/';
    k += QString::number( (int)misspelled );
    k += '/';
    k += QString::number( (int)a );
    return k;
}

void QTextFormat::addRef()
{
    ref++;
#ifdef DEBUG_COLLECTION
    if ( collection )
        qDebug( "  add ref of '%s' to %d (%p) (coll %p)", k.latin1(), ref, this, collection );
#endif
}

void QTextFormat::removeRef()
{
    ref--;
    if ( !collection )
        return;
#ifdef DEBUG_COLLECTION
    qDebug( "  remove ref of '%s' to %d (%p) (coll %p)", k.latin1(), ref, this, collection );
#endif
    if ( ref == 0 )
        collection->remove( this );
}

void QTextFormat::updateStyle()
{
    if ( !collection || !collection->styleSheet() )
	return;
    QStyleSheetItem *item = collection->styleSheet()->item( style );
    if ( !item )
	return;
    if ( !( different & Color ) && item->color().isValid() )
	col = item->color();
    if ( !( different & Size ) && item->fontSize() != -1 )
	fn.setPointSize( item->fontSize() );
    if ( !( different & Family ) && !item->fontFamily().isEmpty() )
	fn.setFamily( item->fontFamily() );
    if ( !( different & Bold ) && item->fontWeight() != -1 )
	fn.setWeight( item->fontWeight() );
    if ( !( different & Italic ) && item->definesFontItalic() )
	fn.setItalic( item->fontItalic() );
    if ( !( different & Underline ) && item->definesFontUnderline() )
	fn.setUnderline( item->fontUnderline() );
    generateKey();
    update();

}

void QTextFormat::updateStyleFlags()
{
    different = NoFlags;
    if ( !collection || !collection->styleSheet() )
	return;
    QStyleSheetItem *item = collection->styleSheet()->item( style );
    if ( !item )
	return;
    if ( item->color() != col )
	different |= Color;
    if ( item->fontSize() != fn.pointSize() )
	different |= Size;
    if ( item->fontFamily() != fn.family() )
	different |= Family;
    if ( item->fontItalic() != fn.italic() )
	different |= Italic;
    if ( item->fontUnderline() != fn.underline() )
	different |= Underline;
    if ( item->fontWeight() != fn.weight() )
	different |= Bold;
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
#if 0
	// ### workaround so that non-breaking whitespaces are drawn
	// properly, actually this should be fixed in QFont somewhere
	if ( *uc == (char)0xa0 )
	    *uc = 0x20;
	// ### workaround so that \n are not drawn, actually this should be
	// fixed in QFont somewhere (under Windows you get ugly boxes
	// otherwise)
	if ( *uc == '\n' )
	    *uc = 0x20;
#endif
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
#if 0
	// ### workaround so that non-breaking whitespaces are drawn
	// properly, actually this should be fixed in QFont somewhere
	if ( *uc == (char)0xa0 )
	    *uc = 0x20;
	// ### workaround so that \n are not drawn, actually this should be
	// fixed in QFont somewhere (under Windows you get ugly boxes
	// otherwise)
	if ( *uc == '\n' )
	    *uc = 0x20;
#endif
	uc++;
	c--;
    }

    return s;
}

void QTextParag::setSelection( int id, int start, int end )
{
    QMap<int, QTextParagSelection>::ConstIterator it = selections().find( id );
    if ( it != mSelections->end() ) {
	if ( start == ( *it ).start && end == ( *it ).end )
	    return;
    }

    QTextParagSelection sel;
    sel.start = start;
    sel.end = end;
    (*mSelections)[ id ] = sel;
    setChanged( TRUE, TRUE );
}

void QTextParag::removeSelection( int id )
{
    if ( !hasSelection( id ) )
	return;
    if ( mSelections )
	mSelections->remove( id );
    setChanged( TRUE, TRUE );
}

int QTextParag::selectionStart( int id ) const
{
    if ( !mSelections )
	return -1;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).start;
}

int QTextParag::selectionEnd( int id ) const
{
    if ( !mSelections )
	return -1;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).end;
}

bool QTextParag::hasSelection( int id ) const
{
    if ( !mSelections )
	return FALSE;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return FALSE;
    return ( *it ).start != ( *it ).end || length() == 1;
}

bool QTextParag::fullSelected( int id ) const
{
    if ( !mSelections )
	return FALSE;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return FALSE;
    return ( *it ).start == 0 && ( *it ).end == str->length() - 1;
}

int QTextParag::lineY( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineY: line %d out of range!", l );
	return 0;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->y;
}

int QTextParag::lineBaseLine( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineBaseLine: line %d out of range!", l );
	return 10;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->baseLine;
}

int QTextParag::lineHeight( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineHeight: line %d out of range!", l );
	return 15;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->h;
}

void QTextParag::lineInfo( int l, int &y, int &h, int &bl ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineInfo: line %d out of range!", l );
	qDebug( "%d %d", (int)lineStarts.count() - 1, l );
	y = 0;
	h = 15;
	bl = 10;
	return;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    y = ( *it )->y;
    h = ( *it )->h;
    bl = ( *it )->baseLine;
}

int QTextParag::alignment() const
{
    if ( align != -1 )
	return align;
    QStyleSheetItem *item = style();
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
}

QPtrVector<QStyleSheetItem> QTextParag::styleSheetItems() const
{
    QPtrVector<QStyleSheetItem> vec;
    if ( mStyleSheetItemsVec ) {
	vec.resize( mStyleSheetItemsVec->size() );
	for ( int i = 0; i < (int)vec.size(); ++i )
	    vec.insert( i, (*mStyleSheetItemsVec)[ i ] );
    }
    return vec;
}

QStyleSheetItem *QTextParag::style() const
{
    if ( !mStyleSheetItemsVec || mStyleSheetItemsVec->size() == 0 )
	return 0;
    return (*mStyleSheetItemsVec)[ mStyleSheetItemsVec->size() - 1 ];
}

int QTextParag::numberOfSubParagraph() const
{
    if ( list_val != -1 )
	return list_val;
    if ( numSubParag != -1 )
 	return numSubParag;
    int n = 0;
    QTextParag *p = (QTextParag*)this;
    while ( p && ( styleSheetItemsVec().size() >= p->styleSheetItemsVec().size() &&
	    styleSheetItemsVec()[ (int)p->styleSheetItemsVec().size() - 1 ] == p->style() ||
		   p->styleSheetItemsVec().size() >= styleSheetItemsVec().size() &&
		   p->styleSheetItemsVec()[ (int)styleSheetItemsVec().size() - 1 ] == style() ) ) {
	if ( p->style() == style() && listStyle() != p->listStyle()
	     && p->styleSheetItemsVec().size() == styleSheetItemsVec().size() )
	    break;
	if ( p->style()->displayMode() == QStyleSheetItem::DisplayListItem
	     && p->style() != style() || styleSheetItemsVec().size() == p->styleSheetItemsVec().size() )
	    ++n;
	p = p->prev();
    }
    ( (QTextParag*)this )->numSubParag = n;
    return n;
}

void QTextParag::setFormat( QTextFormat *fm )
{
    bool doUpdate = FALSE;
    if (defFormat && (defFormat != formatCollection()->defaultFormat()))
       doUpdate = TRUE;
    defFormat = formatCollection()->format( fm );
    if ( !doUpdate )
	return;
    for ( int i = 0; i < length(); ++i ) {
	if ( at( i )->format()->styleName() == defFormat->styleName() )
	    at( i )->format()->updateStyle();
    }
}

QTextFormatter *QTextParag::formatter() const
{
    if ( doc )
	return doc->formatter();
    if ( pFormatter )
	return pFormatter;
    //return ( ( (QTextParag*)this )->pFormatter = new QTextFormatterBreakWords );
    return 0L;
}

void QTextParag::setFormatter( QTextFormatter *f )
{
    if ( doc ) return;
    if ( pFormatter ) delete pFormatter;
    pFormatter = f;
}

int QTextParag::minimumWidth() const
{
    return doc ? doc->minimumWidth() : 0;
}

void QTextParag::setTabArray( int *a )
{
    delete [] tArray;
    tArray = a;
}

void QTextParag::setTabStops( int tw )
{
    if ( doc )
	doc->setTabStops( tw );
    else
	tabStopWidth = tw;
}

QMap<int, QTextParagSelection> &QTextParag::selections() const
{
    if ( !mSelections )
	((QTextParag *)this)->mSelections = new QMap<int, QTextParagSelection>;
    return *mSelections;
}

QPtrVector<QStyleSheetItem> &QTextParag::styleSheetItemsVec() const
{
    if ( !mStyleSheetItemsVec )
	((QTextParag *)this)->mStyleSheetItemsVec = new QPtrVector<QStyleSheetItem>;
    return *mStyleSheetItemsVec;
}

QPtrList<QTextCustomItem> &QTextParag::floatingItems() const
{
    if ( !mFloatingItems )
	((QTextParag *)this)->mFloatingItems = new QPtrList<QTextCustomItem>;
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

#undef QTextCustomItem
#undef QTextFlow
#undef QTextDocument
#undef QTextParag
