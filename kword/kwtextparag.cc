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

#include "kwtextparag.h"
#include "kwtextdocument.h"
#include "kwutils.h"
#include "kwstyle.h"
#include "kwdoc.h"
#include "kwformat.h"
#include "kwanchorpos.h"
#include "kwtextimage.h"
#include "kwtextframeset.h"
#include "variable.h"
#include "counter.h"
#include <klocale.h>
#include <kdebug.h>
#include <qdom.h>
#include <qtl.h>
#include <assert.h>

KWTextParag::KWTextParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds)
    : QTextParag( d, pr, nx, updateIds )
{
    //kdDebug() << "KWTextParag::KWTextParag " << this << endl;
    m_item = 0L;
}

KWTextParag::~KWTextParag()
{
    if ( !textDocument()->isDestroying() )
        invalidateCounters();
    //kdDebug() << "KWTextParag::~KWTextParag " << this << endl;
    delete m_item;
}

KWTextDocument * KWTextParag::textDocument() const
{
    return static_cast<KWTextDocument *>( document() );
}

// There is one QStyleSheetItems per paragraph, created on demand,
// in order to set the DisplayMode for counters.
void KWTextParag::checkItem( QStyleSheetItem * & item, const char * name )
{
    if ( !item )
    {
        item = new QStyleSheetItem( 0, QString::fromLatin1(name) /* For debugging purposes only */ );
        QVector<QStyleSheetItem> vec = styleSheetItems();
        vec.resize( vec.size() + 1 );
        vec.insert( vec.size() - 1, item );
        //kdDebug() << "KWTextParag::checkItem inserting QStyleSheetItem " << name << " at position " << vec.size()-1 << endl;
        setStyleSheetItems( vec );
    }
}

// Return the counter associated with this paragraph.
Counter *KWTextParag::counter()
{
    if ( !m_layout.counter )
        return m_layout.counter;

    // Garbage collect unnneeded counters.
    if ( m_layout.counter->numbering() == Counter::NUM_NONE )
        setNoCounter();
    return m_layout.counter;
}

void KWTextParag::setMargin( QStyleSheetItem::Margin m, double _i )
{
    //kdDebug() << "KWTextParag::setMargin " << m << " margin " << _i << endl;
    m_layout.margins[m] = _i;
    if ( m == QStyleSheetItem::MarginTop && prev() )
        prev()->invalidate(0);     // for top margin
    invalidate(0);
}

void KWTextParag::setMargins( const double * margins )
{
    for ( int i = 0 ; i < 5 ; ++i )
        m_layout.margins[i] = margins[i];
    if ( prev() )
        prev()->invalidate(0);     // for top margin
    invalidate(0);
}

void KWTextParag::setAlign( int align )
{
    setAlignment( align );
    m_layout.alignment = align;
}

void KWTextParag::setLineSpacing( double _i )
{
    m_layout.lineSpacing = _i;
    invalidate(0);
}

void KWTextParag::setPageBreaking( int pb )
{
    m_layout.pageBreaking = pb;
    invalidate(0);
    if ( next() && ( pb & KWParagLayout::HardFrameBreakAfter ) )
        next()->invalidate(0);
}

void KWTextParag::setTopBorder( const Border & _brd )
{
    m_layout.topBorder = _brd;
    if ( prev() )
        prev()->invalidate(0);     // for top margin
    invalidate(0);
}

void KWTextParag::setBottomBorder( const Border & _brd )
{
    m_layout.bottomBorder = _brd;
    invalidate(0);
}

void KWTextParag::setNoCounter()
{
    delete m_layout.counter;
    m_layout.counter = 0L;
    invalidateCounters();
}

void KWTextParag::setCounter( const Counter & counter )
{
    // Garbage collect unnneeded counters.
    if ( counter.numbering() == Counter::NUM_NONE )
    {
        setNoCounter();
    }
    else
    {
        delete m_layout.counter;
        m_layout.counter = new Counter( counter );

        checkItem( m_item, "m_item" );
        // Set the display mode (in order for drawLabel to get called by QTextParag)
        m_item->setDisplayMode( QStyleSheetItem::DisplayListItem );
        //m_item->setSelfNesting( FALSE ); // Not sure why this is necessary.. to be investigated

        // Invalidate the counters
        invalidateCounters();
    }
}

void KWTextParag::invalidateCounters()
{
    // Invalidate this paragraph and all the following ones
    // (Numbering may have changed)
    invalidate( 0 );
    if ( m_layout.counter )
        m_layout.counter->invalidate();
    KWTextParag *s = static_cast<KWTextParag *>( next() );
    while ( s ) {
        if ( s->m_layout.counter )
            s->m_layout.counter->invalidate();
        s->invalidate( 0 );
        s = static_cast<KWTextParag *>( s->next() );
    }
}

int KWTextParag::counterWidth() const
{
    if ( !m_layout.counter )
        return 0;

    return m_layout.counter->width( this );
}

// Draw the counter/bullet for a paragraph
void KWTextParag::drawLabel( QPainter* p, int x, int y, int /*w*/, int h, int base, const QColorGroup& /*cg*/ )
{
    if ( !m_layout.counter ) // shouldn't happen
        return;

    if ( m_layout.counter->numbering() == Counter::NUM_NONE )
    {   // Garbage collect unnneeded counter.
        delete m_layout.counter;
        m_layout.counter = 0L;
        return;
    }

    int size = m_layout.counter->width( this );

    // Draw the complete label.
    QTextFormat *format = at( 0 )->format(); // paragFormat();
    p->save();
    QPen tmpPen=p->pen();

    QColor newColor(format->color());
    QFont newFont(format->font());

    p->setFont( newFont );
    tmpPen.setColor( newColor);
    p->setPen(tmpPen);

    // Now draw any bullet that is required over the space left for it.
    if ( m_layout.counter->isBullet() )
    {
        // Modify x offset.
        QString suffix = m_layout.counter->suffix() + ' ' /*the trailing space*/;
        for ( unsigned int i = 0; i < suffix.length(); i++ )
            x -= format->width( suffix, i );
        int width = format->width( ' ' );
        int height = format->height();
        QRect er( x - width, y - h + height / 2 - width / 2, width, width );
        // Draw the bullet.
        switch ( m_layout.counter->style() )
        {
            case Counter::STYLE_DISCBULLET:
                p->setBrush( QBrush(newColor) );
                p->drawEllipse( er );
                p->setBrush( Qt::NoBrush );
                break;
            case Counter::STYLE_SQUAREBULLET:
                p->fillRect( er , QBrush(newColor) );
                break;
            case Counter::STYLE_CIRCLEBULLET:
                p->drawEllipse( er );
                break;
            case Counter::STYLE_CUSTOMBULLET:
                // The user has selected a symbol from a special font. Override the paragraph
                // font with the given family. This conserves the right size etc.
                if ( !m_layout.counter->customBulletFont().isEmpty() )
                {
                    newFont.setFamily( m_layout.counter->customBulletFont() );
                    p->setFont( newFont );
                }
                p->drawText( x - width, y - h + base, m_layout.counter->customBulletCharacter() );
                break;
            default:
                break;
        }
        // TODO draw suffix text ?
    }
    else
    {
        // There are no bullets...any parent bullets have already been suppressed.
        // Just draw the text! Note: one space is always appended.
        QString counterText = m_layout.counter->text( this );
        if ( !counterText.isEmpty() )
            p->drawText( x - size, y - h + base, counterText + ' ' );
    }
    p->restore();
}

int KWTextParag::topMargin() const
{
    KWZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItY( m_layout.margins[ QStyleSheetItem::MarginTop ] )
        + Border::zoomWidthY( m_layout.topBorder.ptWidth, zh, 0 );
}

int KWTextParag::bottomMargin() const
{
    KWZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItY( m_layout.margins[ QStyleSheetItem::MarginBottom ] )
        + Border::zoomWidthY( m_layout.bottomBorder.ptWidth, zh, 0 );
}

int KWTextParag::leftMargin() const
{
    KWZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItX( m_layout.margins[ QStyleSheetItem::MarginLeft ] )
        + Border::zoomWidthX( m_layout.leftBorder.ptWidth, zh, 0 )
        + counterWidth() /* shouldn't be zoomed, it depends on the font sizes */;
}

int KWTextParag::rightMargin() const
{
    KWZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItX( m_layout.margins[ QStyleSheetItem::MarginRight ] )
        + Border::zoomWidthX( m_layout.rightBorder.ptWidth, zh, 0 );
}

int KWTextParag::firstLineMargin() const
{
    return textDocument()->zoomHandler()->zoomItX(
        m_layout.margins[ QStyleSheetItem::MarginFirstLine ] );
}

int KWTextParag::lineSpacing( int line ) const
{
    if ( m_layout.lineSpacing >= 0 )
        return textDocument()->zoomHandler()->zoomItY(
            m_layout.lineSpacing );
    else {
        KWTextParag * that = const_cast<KWTextParag *>(this);
        ASSERT( line < that->lineStartList().count() );
        //kdDebug() << "KWTextParag::lineSpacing line=" << line << " lines=" << that->lineStartList().count() << endl;
        QMap<int, QTextParagLineStart*>::ConstIterator it = that->lineStartList().begin();
        while ( line-- > 0 )
            ++it;
        int height = ( *it )->h;
        //kdDebug() << " line height=" << height << endl;

        if ( m_layout.lineSpacing == KWParagLayout::LS_ONEANDHALF )
        {
            // Tricky. During formatting height doesn't include the linespacing,
            // but afterwards (e.g. when drawing the cursor), it does !
            return isValid() ? height * 2 / 3 : height / 2;
        }
        else if ( m_layout.lineSpacing == KWParagLayout::LS_DOUBLE )
        {
            return isValid() ? height / 2 : height;
        }
    }
}

// Reimplemented from QTextParag
void KWTextParag::paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cursor, bool drawSelections,
                         int clipx, int clipy, int clipw, int cliph )
{
    //qDebug("KWTextParag::paint %p", this);
    QTextParag::paint( painter, cg, cursor, drawSelections, clipx, clipy, clipw, cliph );

    if ( m_layout.topBorder.ptWidth > 0
         || m_layout.bottomBorder.ptWidth > 0
         || m_layout.leftBorder.ptWidth > 0
         || m_layout.rightBorder.ptWidth > 0 )
    {
        KWZoomHandler * zh = textDocument()->zoomHandler();

        QRect r;
        // r.setLeft( leftMargin() ); // breaks with centered text and with counters
        r.setLeft( at( 0 )->x - counterWidth() - 1 );
        r.setRight( rect().width() - rightMargin() - 1 ); /*documentWidth()-1 requires many fixes in QRT*/
        r.setTop( lineY( 0 ) );
        int lastLine = lines() - 1;
        r.setBottom( static_cast<int>( lineY( lastLine ) + lineHeight( lastLine ) - lineSpacing( lastLine ) ) - 1 );

        Border::drawBorders( painter, zh, r, m_layout.leftBorder, m_layout.rightBorder, m_layout.topBorder, m_layout.bottomBorder,
                             0, QPen() );
    }
}

// Reimplemented from QTextParag
void KWTextParag::drawParagString( QPainter &painter, const QString &s, int start, int len, int startX,
                                   int lastY, int baseLine, int bw, int h, bool drawSelections,
                                   QTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                   const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft )
{
    // Resolve the color before calling the default implementation of drawParagString
    if ( lastFormat && !lastFormat->color().isValid() )
    {
        QTextFormat format( *lastFormat );
        format.setColor( KWDocument::defaultTextColor( &painter ) );
        lastFormat = &format;
    }

    QTextParag::drawParagString( painter, s, start, len, startX,
                                 lastY, baseLine, bw, h, drawSelections,
                                 lastFormat, i, selectionStarts,
                                 selectionEnds, cg, rightToLeft );

    KWTextFrameSet * textfs = textDocument()->textFrameSet();
    if ( textfs )
    {
        KWDocument * doc = textfs->kWordDocument();
        if ( doc && doc->viewFormattingChars() && painter.device()->devType() != QInternal::Printer )
        {
            painter.save();
            QPen pen( Qt::red ); // ?
            painter.setPen( pen );
            //kdDebug() << "KWTextParag::drawParagString start=" << start << " len=" << len << " length=" << length() << endl;
            if ( start + len == length() )
            {
                // drawing the end of the parag
                int w = lastFormat->width('x'); // see KWTextFrameSet::adjustFlow
                int size = QMIN( w, h * 3 / 4 );
                int arrowsize = textDocument()->zoomHandler()->zoomItY( 2 );
                // x,y is the bottom right corner of the reversed L
                int x = startX + bw + w - 1;
                int y = lastY + baseLine - arrowsize;
                //kdDebug() << "KWTextParag::drawParagString drawing CR at " << x << "," << y << endl;
                painter.drawLine( x, y - size, x, y );
                painter.drawLine( x, y, x - size, y );
                // Now the arrow
                painter.drawLine( x - size, y, x - size + arrowsize, y - arrowsize );
                painter.drawLine( x - size, y, x - size + arrowsize, y + arrowsize );
            }
            // Now draw spaces and tabs
            int end = QMIN( start + len, length() - 1 ); // don't look at the trailing space
            for ( int i = start ; i < end ; ++i )
            {
                QTextStringChar &ch = string()->at(i);
                if ( ch.c == ' ' )
                {
                    int w = string()->width(i);
                    int size = QMAX( 2, QMIN( w/2, h/3 ) ); // Enfore that it's a square, and that it's visible
                    painter.drawRect( ch.x + (w - size) / 2, lastY + (h - size) / 2, size, size );
                }
                else if ( ch.c == '\t' )
                {
                    QTextStringChar &nextch = string()->at(i+1);
                    //kdDebug() << "tab x=" << ch.x << " next x=" << nextch.x << endl;
                    int availWidth = nextch.x - ch.x - 1;
                    int x = ch.x + availWidth / 2;
                    int size = QMIN( availWidth, lastFormat->width('W') ) / 2; // actually the half size
                    int y = lastY + h/2;
                    int arrowsize = textDocument()->zoomHandler()->zoomItY( 2 );
                    painter.drawLine( x + size, y, x - size, y );
                    painter.drawLine( x + size, y, x + size - arrowsize, y - arrowsize );
                    painter.drawLine( x + size, y, x + size - arrowsize, y + arrowsize );
                }
            }
            painter.restore();
        }
    }
}

// Reimplemented from QTextParag
void KWTextParag::copyParagData( QTextParag *_parag )
{
    KWTextParag * parag = static_cast<KWTextParag *>(_parag);
    // Style of the previous paragraph
    KWStyle * style = parag->style();
    // Obey "following style" setting
    bool styleApplied = false;
    if ( style )
    {
        KWStyle * newStyle = style->followingStyle();
        if ( newStyle && style != newStyle ) // if same style, keep paragraph-specific changes as usual
        {
            setParagLayout( newStyle->paragLayout() );
            KWTextFrameSet * textfs = textDocument()->textFrameSet();
            ASSERT( textfs );
            if ( textfs )
            {
                QTextFormat * format = textfs->zoomFormatFont( &newStyle->format() );
                setFormat( format );
                format->addRef();
                string()->setFormat( 0, format, true ); // prepare format for text insertion
            }
            styleApplied = true;
        }
    }
    else
        kdWarning() << "Paragraph has no style " << paragId() << endl;

    // No "following style" setting, or same style -> copy layout & format of previous paragraph
    if (!styleApplied)
    {
        setParagLayout( parag->paragLayout() );
        // Don't copy the hard-frame-break setting though
        m_layout.pageBreaking &= ~KWParagLayout::HardFrameBreakBefore;
        m_layout.pageBreaking &= ~KWParagLayout::HardFrameBreakAfter;
        // set parag format to the format of the trailing space of the previous parag
        setFormat( parag->at( parag->length()-1 )->format() );
        // QTextCursor::splitAndInsertEmptyParag takes care of setting the format
        // for the chars in the new parag
    }

    // Note: we don't call QTextParag::copyParagData on purpose.
    // We don't want setListStyle to get called - it ruins our stylesheetitems
    // And we don't care about copying the stylesheetitems directly,
    // applying the parag layout will create them
}

void KWTextParag::setCustomItem( int index, KWTextCustomItem * custom, QTextFormat * currentFormat )
{
    kdDebug(32001) << "KWTextParag::setCustomItem " << index << "  " << (void*)custom
                   << "  currentFormat=" << (void*)currentFormat << endl;
    if ( currentFormat )
        setFormat( index, 1, currentFormat );
    at( index )->setCustomItem( custom );
    addCustomItem();
    document()->registerCustomItem( custom, this );
    custom->resize();
    invalidate( 0 );
}

void KWTextParag::removeCustomItem( int index )
{
    ASSERT( at( index )->isCustom() );
    QTextCustomItem * item = at( index )->customItem();
    at( index )->loseCustomItem();
    remove( index, 1 );
    QTextParag::removeCustomItem();
    document()->unregisterCustomItem( item, this );
}

int KWTextParag::findCustomItem( const QTextCustomItem * custom ) const
{
    int len = string()->length();
    for ( int i = 0; i < len; ++i )
    {
        QTextStringChar & ch = string()->at(i);
        if ( ch.isCustom() && ch.customItem() == custom )
            return i;
    }
    kdWarning() << "KWTextParag::findCustomItem custom item " << (void*)custom
              << " not found in paragraph " << paragId() << endl;
    return 0;
}

void KWTextParag::setTabList( const KoTabulatorList &tabList )
{
    KoTabulatorList lst( tabList );
    m_layout.setTabList( lst );
    if ( !tabList.isEmpty() )
    {
        KWZoomHandler * zh = textDocument()->zoomHandler();
        int * tabs = new int[ tabList.count() + 1 ]; // will be deleted by ~QTextParag
        KoTabulatorList::Iterator it = lst.begin();
        unsigned int i = 0;
        for ( ; it != lst.end() ; ++it, ++i )
            tabs[i] = zh->zoomItX( (*it).ptPos );
        tabs[i] = 0;
        assert( i == tabList.count() );
        setTabArray( tabs );
    } else
    {
        setTabArray( 0 );
    }
    invalidate( 0 );
}

int KWTextParag::nextTab( int chnum, int x )
{
    if ( !m_layout.tabList().isEmpty() )
    {
        // Fetch the zoomed and sorted tab positions from QTextParag
        // We stored them there for faster access
        int * tArray = tabArray();
        int i = 0;
        while ( tArray[ i ] ) {
            //kdDebug() << "KWTextParag::nextTab tArray[" << i << "]=" << tArray[i] << " type" << m_layout.tabList()[i].type << endl;
            if ( tArray[ i ] >= x ) {
                int type = m_layout.tabList()[i].type;
                switch ( type ) {
                case T_RIGHT:
                case T_CENTER:
                {
                    // Look for the next tab (or EOL)
                    int c = chnum + 1;
                    int w = 0;
                    while ( c < string()->length()-1 && string()->at( c ).c != '\t' )
                    {
                        QTextStringChar & ch = string()->at( c );
                        // Determine char width (same code as the one in QTextFormatterBreak[In]Words::format())
                        if ( ch.c.unicode() >= 32 || ch.isCustom() )
                            w += string()->width( c );
                        else
                            w += ch.format()->width( ' ' );
                        ++c;
                    }
                    if ( type == T_RIGHT )
                        return tArray[ i ] - w;
                    else // T_CENTER
                        return tArray[ i ] - w/2;
                }
                case T_DEC_PNT:
                {
                    // Look for the next tab (or EOL), and for <digit><dot>
                    // Default to right-aligned if no decimal point found (behavior from msword)
                    int c = chnum + 1;
                    int w = 0;
                    int decimalPoint = KGlobal::locale()->decimalSymbol()[0].unicode();
                    bool digitFound = false;
                    while ( c < string()->length()-1 && string()->at( c ).c != '\t' )
                    {
                        QTextStringChar & ch = string()->at( c );
                        if ( ch.c.isDigit() )
                            digitFound = true;
                        else if ( digitFound && ( ch.c == '.' || ch.c.unicode() == decimalPoint ) )
                        {
                            w += string()->width( c ) / 2; // center around the decimal point
                            break;
                        }
                        else
                            digitFound = false; // The digit has to be right before the dot

                        // Determine char width (same code as the one in QTextFormatterBreak[In]Words::format())
                        if ( ch.c.unicode() >= 32 || ch.isCustom() )
                            w += string()->width( c );
                        else
                            w += ch.format()->width( ' ' );

                        ++c;
                    }
                    return tArray[ i ] - w;
                }
                default: // case T_LEFT:
                    return tArray[ i ];
                }
            }
            ++i;
        }
    } else // No tab list, use tab-stop-width. QTextParag has the code :)
        return QTextParag::nextTab( chnum, x );
}

//static
QDomElement KWTextParag::saveFormat( QDomDocument & doc, KWTextFormat * curFormat, KWTextFormat * refFormat, int pos, int len )
{
    QDomElement formatElem = doc.createElement( "FORMAT" );
    formatElem.setAttribute( "id", 1 ); // text format
    if ( len ) // 0 when saving the format of a style
    {
        formatElem.setAttribute( "pos", pos );
        formatElem.setAttribute( "len", len );
    }
    QDomElement elem;
    if( !refFormat || curFormat->font().weight() != refFormat->font().weight() )
    {
        elem = doc.createElement( "WEIGHT" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", curFormat->font().weight() );
    }
    if( !refFormat || curFormat->color() != refFormat->color() )
        if ( curFormat->color().isValid() )
        {
            elem = doc.createElement( "COLOR" );
            formatElem.appendChild( elem );
            elem.setAttribute( "red", curFormat->color().red() );
            elem.setAttribute( "green", curFormat->color().green() );
            elem.setAttribute( "blue", curFormat->color().blue() );
        }
    if( !refFormat || curFormat->font().family() != refFormat->font().family() )
    {
        elem = doc.createElement( "FONT" );
        formatElem.appendChild( elem );
        elem.setAttribute( "name", curFormat->font().family() );
    }
    if( !refFormat || curFormat->font().pointSize() != refFormat->font().pointSize() )
    {
        elem = doc.createElement( "SIZE" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", curFormat->font().pointSize() );
    }
    if( !refFormat || curFormat->font().italic() != refFormat->font().italic() )
    {
        elem = doc.createElement( "ITALIC" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->font().italic()) );
    }
    if( !refFormat || curFormat->font().underline() != refFormat->font().underline() )
    {
        elem = doc.createElement( "UNDERLINE" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->font().underline()) );
    }
    if( !refFormat || curFormat->font().strikeOut() != refFormat->font().strikeOut() )
    {
        elem = doc.createElement( "STRIKEOUT" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->font().strikeOut()) );
    }
    if( !refFormat || curFormat->vAlign() != refFormat->vAlign() )
    {
        elem = doc.createElement( "VERTALIGN" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->vAlign()) );
    }
    return formatElem;
}

void KWTextParag::save( QDomElement &parentElem, int from /* default 0 */, int to /* default length()-2 */ )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement paragElem = doc.createElement( "PARAGRAPH" );
    parentElem.appendChild( paragElem );
    QDomElement textElem = doc.createElement( "TEXT" );
    paragElem.appendChild( textElem );
    QString text = string()->toString();
    ASSERT( text.right(1)[0] == ' ' );
    textElem.appendChild( doc.createTextNode( text.mid( from, to - from + 1 ) ) );

    QDomElement formatsElem = doc.createElement( "FORMATS" );
    int startPos = -1;
    int index = 0; // Usually same as 'i' but if from>0, 'i' indexes the parag's text and this one indexes the output
    KWTextFormat *curFormat = paragraphFormat();
    for ( int i = from; i <= to; ++i, ++index )
    {
        QTextStringChar & ch = string()->at(i);
        if ( ch.isCustom() )
        {
            if ( startPos > -1 && curFormat) { // Save former format
                QDomElement formatElem = saveFormat( doc, curFormat,
                                                     paragraphFormat(), startPos, index-startPos );
                if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
                    formatsElem.appendChild( formatElem );
            }

            QDomElement formatElem = doc.createElement( "FORMAT" );
            formatsElem.appendChild( formatElem );
            formatElem.setAttribute( "pos", index );
            formatElem.setAttribute( "len", 1 );
            static_cast<KWTextCustomItem *>( ch.customItem() )->save( formatElem );
            startPos = -1;
        }
        else
        {
            KWTextFormat * newFormat = static_cast<KWTextFormat *>( ch.format() );
            if ( newFormat != curFormat )
            {
                // Format changed.
                if ( startPos > -1 && curFormat) { // Save former format
                    QDomElement formatElem = saveFormat( doc, curFormat, paragraphFormat(), startPos, index-startPos );
                    if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
                        formatsElem.appendChild( formatElem );
                }

                // Format different from paragraph's format ?
                if( newFormat != paragFormat() )
                {
                    startPos = index;
                    curFormat = newFormat;
                }
                else
                {
                    startPos = -1;
                    curFormat = paragraphFormat();
                }
            }
        }
    }
    if ( startPos > -1 && index > startPos && curFormat) { // Save last format
        QDomElement formatElem = saveFormat( doc, curFormat, paragraphFormat(), startPos, index-startPos );
        if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
            formatsElem.appendChild( formatElem );
    }

    if (!formatsElem.firstChild().isNull()) // Do we have formats to save ?
        paragElem.appendChild( formatsElem );


    QDomElement layoutElem = doc.createElement( "LAYOUT" );
    paragElem.appendChild( layoutElem );

    m_layout.save( layoutElem );

    // Paragraph's format
    // ## Maybe we should have a "default format" somewhere and
    // pass it instead of 0L, to only save the non-default attributes
    QDomElement paragFormatElement = saveFormat( doc, paragraphFormat(), 0L, 0, to - from + 1 );
    layoutElem.appendChild( paragFormatElement );
}

//static
KWTextFormat KWTextParag::loadFormat( QDomElement &formatElem, KWTextFormat * refFormat, const QFont & defaultFont )
{
    KWTextFormat format;
    if ( refFormat )
        format = *refFormat;
    QFont font = format.font();
    QDomElement elem;
    elem = formatElem.namedItem( "FONT" ).toElement();
    if ( !elem.isNull() )
    {
        font.setFamily( elem.attribute("name") );
    }
    else if ( !refFormat )
    {   // No reference format and no FONT tag -> use default font
        font = defaultFont;
    }
    elem = formatElem.namedItem( "WEIGHT" ).toElement();
    if ( !elem.isNull() )
        font.setWeight( elem.attribute( "value" ).toInt() );
    elem = formatElem.namedItem( "SIZE" ).toElement();
    if ( !elem.isNull() )
        font.setPointSize( elem.attribute("value").toInt() );
    elem = formatElem.namedItem( "ITALIC" ).toElement();
    if ( !elem.isNull() )
        font.setItalic( elem.attribute("value").toInt() == 1 );
    elem = formatElem.namedItem( "UNDERLINE" ).toElement();
    if ( !elem.isNull() )
        font.setUnderline( elem.attribute("value").toInt() == 1 );
    elem = formatElem.namedItem( "STRIKEOUT" ).toElement();
    if ( !elem.isNull() )
        font.setStrikeOut( elem.attribute("value").toInt() == 1 );

    format.setFont( font );

    elem = formatElem.namedItem( "VERTALIGN" ).toElement();
    if ( !elem.isNull() )
        format.setVAlign( static_cast<QTextFormat::VerticalAlignment>( elem.attribute("value").toInt() ) );
    elem = formatElem.namedItem( "COLOR" ).toElement();
    if ( !elem.isNull() )
    {
        QColor col( elem.attribute("red").toInt(),
                    elem.attribute("green").toInt(),
                    elem.attribute("blue").toInt() );
        format.setColor( col );
    }
    //kdDebug() << "KWTextParag::loadFormat format=" << format.key() << endl;
    return format;
}

void KWTextParag::loadLayout( QDomElement & attributes )
{
    QDomElement layout = attributes.namedItem( "LAYOUT" ).toElement();
    if ( !layout.isNull() )
    {
        KWDocument * doc = textDocument()->textFrameSet()->kWordDocument();
        KWParagLayout paragLayout( layout, doc, true );
        setParagLayout( paragLayout );

        // Load default format from style.
        KWTextFormat *defaultFormat = style() ? &style()->format() : 0L;
        QDomElement formatElem = layout.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
        {
            // Load paragraph format
            KWTextFormat f = loadFormat( formatElem, defaultFormat, doc->defaultFont() );
            setFormat( document()->formatCollection()->format( &f ) );
        }
        else // No paragraph format
        {
            if ( defaultFormat ) // -> use the one from the style
                setFormat( document()->formatCollection()->format( defaultFormat ) );
        }
    }
    else
    {
        // Even the simplest import filter should do <LAYOUT><NAME value="Standard"/></LAYOUT>
        kdWarning(32001) << "No LAYOUT tag in PARAGRAPH, dunno what layout to apply" << endl;
    }
}

void KWTextParag::load( QDomElement &attributes )
{
    loadLayout( attributes );

    // Set the text after setting the paragraph format - so that the format applies
    QDomElement element = attributes.namedItem( "TEXT" ).toElement();
    if ( !element.isNull() )
    {
        //kdDebug() << "KWTextParag::load '" << element.text() << "'" << endl;
        append( element.text() );
        // Apply default format - this should be automatic !!
        setFormat( 0, string()->length(), paragFormat(), TRUE );
    }

    loadFormatting( attributes );

    setChanged( true );
    invalidate( 0 );
}

void KWTextParag::loadFormatting( QDomElement &attributes, int offset )
{
    KWDocument * doc = textDocument()->textFrameSet()->kWordDocument();
    QDomElement formatsElem = attributes.namedItem( "FORMATS" ).toElement();
    if ( !formatsElem.isNull() )
    {
        QDomElement formatElem = formatsElem.firstChild().toElement();
        for ( ; !formatElem.isNull() ; formatElem = formatElem.nextSibling().toElement() )
        {
            if ( formatElem.tagName() == "FORMAT" )
            {
                int index = formatElem.attribute( "pos" ).toInt() + offset;
                int len = formatElem.attribute( "len" ).toInt();

                int id = formatElem.attribute( "id" ).toInt();
                switch( id ) {
                case 1: // Normal text
                {
                    KWTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont() );
                    //kdDebug(32002) << "KWTextParag::loadFormatting applying formatting from " << index << " to " << index+len << endl;
                    setFormat( index, len, document()->formatCollection()->format( &f ) );
                    break;
                }
                case 2: // Picture
                {
                    ASSERT( len == 1 );
                    KWTextImage * custom = new KWTextImage( textDocument(), QString::null );
                    kdDebug() << "KWTextParag::loadFormatting insertCustomItem" << endl;
                    paragFormat()->addRef();
                    setCustomItem( index, custom, paragFormat() );
                    custom->load( formatElem );
                    break;
                }
                case 4: // Variable
                {
                    QDomElement varElem = formatElem.namedItem( "VARIABLE" ).toElement();
                    bool oldDoc = false;
                    if ( varElem.isNull() )
                    {
                        // Not found, must be an old document -> the tags were directly
                        // under the FORMAT tag.
                        varElem = formatElem;
                        oldDoc = true;
                    }
                    QDomElement typeElem = varElem.namedItem( "TYPE" ).toElement();
                    if ( typeElem.isNull() )
                        kdWarning(32001) <<
                            ( oldDoc ? "No <TYPE> in <FORMAT> with id=4, for a variable [old document assumed] !"
                              : "No <TYPE> found in <VARIABLE> tag!" ) << endl;
                    else
                    {
                        int type = typeElem.attribute( "type" ).toInt();
                        kdDebug() << "KWTextParag::loadFormatting variable type=" << type << endl;
                        KWVariable * var = KWVariable::createVariable( type, -1, textDocument()->textFrameSet() );
                        var->load( varElem );
                        KWTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont() );
                        setCustomItem( index, var, document()->formatCollection()->format( &f ) );
                        var->recalc();
                    }
                    break;
                }
                case 6: // Anchor
                {
                    ASSERT( len == 1 );
                    QDomElement anchorElem = formatElem.namedItem( "ANCHOR" ).toElement();
                    if ( !anchorElem.isNull() ) {
                        QString type = anchorElem.attribute( "type" );
                        if ( type == "grpMgr" /* old syntax */ || type == "frameset" )
                        {
                            QString framesetName = anchorElem.attribute( "instance" );
                            KWAnchorPosition pos;
                            pos.textfs = textDocument()->textFrameSet();
                            pos.paragId = paragId();
                            pos.index = index;
                            doc->addAnchorRequest( framesetName, pos );
                        }
                        else
                            kdWarning() << "Anchor type not supported: " << type << endl;
                    }
                    else
                        kdWarning() << "Missing ANCHOR tag" << endl;
                    break;
                }
                default:
                    kdWarning() << "KWTextParag::loadFormat id=" << id << " not supported" << endl;
                    break;
                }
            }
        }
    }
}

void KWTextParag::setParagLayout( const KWParagLayout & layout, int flags )
{
    //kdDebug() << "KWTextParag::setParagLayout flags=" << flags << endl;
    if ( flags & KWParagLayout::Alignment )
        setAlign( layout.alignment );
    if ( flags & KWParagLayout::Margins )
         setMargins( layout.margins );
    if ( flags & KWParagLayout::PageBreaking )
        setPageBreaking( layout.pageBreaking );
    if ( flags & KWParagLayout::LineSpacing )
        setLineSpacing( layout.lineSpacing );
    if ( flags & KWParagLayout::Borders )
    {
        setLeftBorder( layout.leftBorder );
        setRightBorder( layout.rightBorder );
        setTopBorder( layout.topBorder );
        setBottomBorder( layout.bottomBorder );
    }
    if ( flags & KWParagLayout::BulletNumber )
        setCounter( layout.counter );
    if ( flags & KWParagLayout::Tabulator )
        setTabList( layout.tabList() );

    if ( flags == KWParagLayout::All )
        // Don't call setStyle from here, it would overwrite any paragraph-specific settings
        setStyle( layout.style );
}

#ifndef NDEBUG
void KWTextParag::printRTDebug( int info )
{
    kdDebug() << "Paragraph " << this << "   (" << paragId() << ") ------------------ " << endl;
    if ( prev() && prev()->paragId() + 1 != paragId() )
        kdWarning() << "  Previous paragraph " << prev() << " has ID " << prev()->paragId() << endl;
    if ( next() && next()->paragId() != paragId() + 1 )
        kdWarning() << "  Next paragraph " << next() << " has ID " << next()->paragId() << endl;
    if ( !next() )
        kdDebug() << "  next is 0L" << endl;
    /*
      static const char * dm[] = { "DisplayBlock", "DisplayInline", "DisplayListItem", "DisplayNone" };
      QVector<QStyleSheetItem> vec = styleSheetItems();
      for ( uint i = 0 ; i < vec.size() ; ++i )
      {
      QStyleSheetItem * item = vec[i];
      kdDebug() << "  StyleSheet Item " << item << " '" << item->name() << "'" << endl;
      kdDebug() << "        italic=" << item->fontItalic() << " underline=" << item->fontUnderline() << " fontSize=" << item->fontSize() << endl;
      kdDebug() << "        align=" << item->alignment() << " leftMargin=" << item->margin(QStyleSheetItem::MarginLeft) << " rightMargin=" << item->margin(QStyleSheetItem::MarginRight) << " topMargin=" << item->margin(QStyleSheetItem::MarginTop) << " bottomMargin=" << item->margin(QStyleSheetItem::MarginBottom) << endl;
      kdDebug() << "        displaymode=" << dm[item->displayMode()] << endl;
      }*/
    kdDebug() << "  Style: " << style() << " " << ( style() ? style()->name().local8Bit().data() : "NO STYLE" ) << endl;
    kdDebug() << "  Text: '" << string()->toString() << "'" << endl;
    if ( info == 0 ) // paragraph info
    {
        if ( counter() )
            kdDebug() << "  Counter style=" << counter()->style()
                      << " numbering=" << counter()->numbering()
                      << " depth=" << counter()->depth()
                      << " text='" << m_layout.counter->text( this ) << "'"
                      << " width=" << m_layout.counter->width( this ) << endl;
        kdDebug() << "  rect() : " << DEBUGRECT( rect() ) << endl;

        kdDebug() << "  topMargin()=" << topMargin() << " bottomMargin()=" << bottomMargin()
                  << " leftMargin()=" << leftMargin() << " rightMargin()=" << rightMargin() << endl;

        static const char * tabtype[] = { "T_LEFT", "T_CENTER", "T_RIGHT", "T_DEC_PNT", "error!!!" };
        KoTabulatorList tabList = m_layout.tabList();
        KoTabulatorList::Iterator it = tabList.begin();
        for ( ; it != tabList.end() ; it++ )
            kdDebug() << "Tab type:" << tabtype[(*it).type] << " at: " << (*it).ptPos << endl;

    } else if ( info == 1 ) // formatting info
    {
        kdDebug() << "  Paragraph format=" << paragFormat() << " " << paragFormat()->key()
                  << " fontsize:" << dynamic_cast<KWTextFormat *>(paragFormat())->pointSizeFloat() << endl;

        QTextString * s = string();
        for ( int i = 0 ; i < s->length() ; ++i )
        {
            QTextStringChar & ch = s->at(i);
            kdDebug() << i << ": '" << QString(ch.c) << "' (" << ch.c.unicode() << ")"
                      << " height=" << ch.height()
                      << " format=" << ch.format()
                      << " \"" << ch.format()->key() << "\" "
                //<< " fontsize:" << dynamic_cast<KWTextFormat *>(ch.format())->pointSizeFloat()
                      << endl;
            if ( ch.isCustom() )
            {
                QTextCustomItem * item = ch.customItem();
                kdDebug() << " - custom item " << item
                          << " ownline=" << item->ownLine()
                          << " size=" << item->width << "x" << item->height
                          << endl;
            }
        }
    }
}
#endif

//////////

// Create a default KWParagLayout.
KWParagLayout::KWParagLayout()
{
    initialise();
}

void KWParagLayout::operator=( const KWParagLayout &layout )
{
    alignment = layout.alignment;
    for ( int i = 0 ; i < 5 ; ++i )
        margins[i] = layout.margins[i];
    pageBreaking = layout.pageBreaking;
    leftBorder = layout.leftBorder;
    rightBorder = layout.rightBorder;
    topBorder = layout.topBorder;
    bottomBorder = layout.bottomBorder;
    if ( layout.counter )
        counter = new Counter( *layout.counter );
    else
        counter = 0L;
    lineSpacing = layout.lineSpacing;
    style = layout.style;
    setTabList( layout.tabList() );
}

int KWParagLayout::compare( const KWParagLayout & layout ) const
{
    int flags = 0;
    if ( alignment != layout.alignment )
        flags |= Alignment;
    for ( int i = 0 ; i < 5 ; ++i )
        if ( margins[i] != layout.margins[i] )
        {
            flags |= Margins;
            break;
        }
    if ( pageBreaking != layout.pageBreaking )
        flags |= PageBreaking;
    if ( leftBorder != layout.leftBorder
         || rightBorder != layout.rightBorder
         || topBorder != layout.topBorder
         || bottomBorder != layout.bottomBorder )
        flags |= Borders;

    if ( layout.counter )
    {
        if ( counter )
        {
            if ( ! ( *layout.counter == *counter ) )
                flags |= BulletNumber;
        } else
            if ( layout.counter->numbering() != Counter::NUM_NONE )
                flags |= BulletNumber;
    }
    else
        if ( counter && counter->numbering() != Counter::NUM_NONE )
            flags |= BulletNumber;

    if ( lineSpacing != layout.lineSpacing )
        flags |= LineSpacing;
    //if ( style != layout.style )
    //    flags |= Style;
    if ( m_tabList != m_tabList )
        flags |= Tabulator;
    return flags;
}

// Create a KWParagLayout from XML.
//
// If a document is supplied, default values are taken from the style in the
// document named by the layout. This allows for simplified import filters,
// and also looks to the day that redundant data can be eliminated from the
// saved XML.
KWParagLayout::KWParagLayout( QDomElement & parentElem, KWDocument *doc, bool useRefStyle )
{
    initialise();

    // Only when loading paragraphs, not when loading styles
    if ( useRefStyle )
    {
        // Name of the style. If there is no style, then we do not supply
        // any default!
        QDomElement element = parentElem.namedItem( "NAME" ).toElement();
        if ( !element.isNull() )
        {
            QString styleName = element.attribute( "value" );
            // Default all the layout stuff from the style.
            style = doc->findStyle( styleName );
            if (style)
            {
                //kdDebug() << "KWParagLayout::KWParagLayout setting style to " << style << " " << style->name() << endl;
                *this = style->paragLayout();
            }
            else
            {
                kdError(32001) << "Cannot find style \"" << styleName << "\"" << endl;
                style = doc->findStyle( "Standard" );
            }
        }
        else
        {
            kdError(32001) << "Missing NAME tag in LAYOUT ( for a paragraph ) -> no style !" << endl;
            style = doc->findStyle( "Standard" );
        }
        ASSERT(style);
    }

    // Load the paragraph tabs - forget about the ones from the style first.
    // We can't apply the 'default comes from the style' in this case, because
    // there is no way to differenciate between "I want no tabs in the parag"
    // and "use default from style".
    QDomElement element;
    m_tabList.clear();
    QDomNodeList listTabs = parentElem.elementsByTagName ( "TABULATOR" );
    unsigned int count = listTabs.count();
    for (unsigned int item = 0; item < count; item++)
    {
        element = listTabs.item( item ).toElement();
        KoTabulator tab;
        tab.type = static_cast<KoTabulators>( KWDocument::getAttribute( element, "type", T_LEFT ) );
        tab.ptPos = KWDocument::getAttribute( element, "ptpos", 0.0 );
        m_tabList.append( tab );
    }
    alignment = Qt::AlignLeft;
    element = parentElem.namedItem( "FLOW" ).toElement(); // Flow is what is now called alignment internally
    if ( !element.isNull() )
    {
        QString flow = element.attribute( "value" ); // KWord-0.8
        if ( !flow.isEmpty() )
        {
            static const int flow2align[] = { Qt::AlignLeft, Qt::AlignRight, Qt::AlignCenter, Qt3::AlignJustify };
            alignment = flow2align[flow.toInt()];
        } else {
            flow = element.attribute( "align" ); // KWord-1.0 DTD
            alignment = flow=="right" ? (int)Qt::AlignRight : flow=="center" ? (int)Qt::AlignCenter : flow=="justify" ? (int)Qt3::AlignJustify : (int)Qt::AlignLeft;
        }
    }

    if ( doc->syntaxVersion() < 2 )
    {
        element = parentElem.namedItem( "OHEAD" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            margins[QStyleSheetItem::MarginTop] = KWDocument::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "OFOOT" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            margins[QStyleSheetItem::MarginBottom] = KWDocument::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "IFIRST" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            margins[QStyleSheetItem::MarginFirstLine] = KWDocument::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "ILEFT" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            margins[QStyleSheetItem::MarginLeft] = KWDocument::getAttribute( element, "pt", 0.0 );
    }

    // KWord-1.0 DTD
    element = parentElem.namedItem( "INDENTS" ).toElement();
    if ( !element.isNull() )
    {
        margins[QStyleSheetItem::MarginFirstLine] = KWDocument::getAttribute( element, "first", 0.0 );
        margins[QStyleSheetItem::MarginLeft] = KWDocument::getAttribute( element, "left", 0.0 );
        margins[QStyleSheetItem::MarginRight] = KWDocument::getAttribute( element, "right", 0.0 );
    }
    element = parentElem.namedItem( "OFFSETS" ).toElement();
    if ( !element.isNull() )
    {
        margins[QStyleSheetItem::MarginTop] = KWDocument::getAttribute( element, "before", 0.0 );
        margins[QStyleSheetItem::MarginBottom] = KWDocument::getAttribute( element, "after", 0.0 );
    }

    if ( doc->syntaxVersion() < 2 )
    {
        element = parentElem.namedItem( "LINESPACE" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            lineSpacing = KWDocument::getAttribute( element, "pt", 0.0 );
    }

    element = parentElem.namedItem( "LINESPACING" ).toElement(); // KWord-1.0 DTD
    if ( !element.isNull() )
    {
        QString value = element.attribute( "value" );
        if ( value == "oneandhalf" )
            lineSpacing = LS_ONEANDHALF;
        else if ( value == "double" )
            lineSpacing = LS_DOUBLE;
        else
            lineSpacing = value.toDouble();
    }

    pageBreaking = 0;
    element = parentElem.namedItem( "PAGEBREAKING" ).toElement();
    if ( !element.isNull() )
    {
        if ( element.attribute( "linesTogether" ) == "true" )
            pageBreaking |= KeepLinesTogether;
        if ( element.attribute( "hardFrameBreak" ) == "true" )
            pageBreaking |= HardFrameBreakBefore;
        if ( element.attribute( "hardFrameBreakAfter" ) == "true" )
            pageBreaking |= HardFrameBreakAfter;
    }
    if ( doc->syntaxVersion() < 2 )
    {
        element = parentElem.namedItem( "HARDBRK" ).toElement(); // KWord-0.8
        if ( !element.isNull() )
            pageBreaking |= HardFrameBreakBefore;
    }

    element = parentElem.namedItem( "LEFTBORDER" ).toElement();
    if ( !element.isNull() )
        leftBorder = Border::loadBorder( element );
    else
        leftBorder.ptWidth = 0;

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        rightBorder = Border::loadBorder( element );
    else
        rightBorder.ptWidth = 0;

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        topBorder = Border::loadBorder( element );
    else
        topBorder.ptWidth = 0;

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        bottomBorder = Border::loadBorder( element );
    else
        bottomBorder.ptWidth = 0;

    element = parentElem.namedItem( "COUNTER" ).toElement();
    if ( !element.isNull() )
    {
        counter = new Counter;
        counter->load( element );
    }
}

void KWParagLayout::initialise()
{
    alignment = Qt::AlignLeft;
    for ( int i = 0 ; i < 5 ; ++i ) // use memset ?
        margins[i] = 0;
    lineSpacing = 0;
    counter = 0L;
    leftBorder.ptWidth = 0;
    rightBorder.ptWidth = 0;
    topBorder.ptWidth = 0;
    bottomBorder.ptWidth = 0;
    pageBreaking = 0;
    style = 0L;
    m_tabList.clear();
}

KWParagLayout::~KWParagLayout()
{
    delete counter;
}

void KWParagLayout::save( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    if ( style )
        element.setAttribute( "value", style->name() );
    else
        kdWarning() << "KWParagLayout::save: style==0L!" << endl;

    element = doc.createElement( "FLOW" );
    parentElem.appendChild( element );
    int a = alignment;
    element.setAttribute( "align", a==Qt::AlignRight ? "right" : a==Qt::AlignCenter ? "center" : a==Qt3::AlignJustify ? "justify" : "left" );
    if ( margins[QStyleSheetItem::MarginFirstLine] != 0 ||
         margins[QStyleSheetItem::MarginLeft] != 0 ||
         margins[QStyleSheetItem::MarginRight] != 0 )
    {
        element = doc.createElement( "INDENTS" );
        parentElem.appendChild( element );
        if ( margins[QStyleSheetItem::MarginFirstLine] != 0 )
            element.setAttribute( "first", margins[QStyleSheetItem::MarginFirstLine] );
        if ( margins[QStyleSheetItem::MarginLeft] != 0 )
            element.setAttribute( "left", margins[QStyleSheetItem::MarginLeft] );
        if ( margins[QStyleSheetItem::MarginRight] != 0 )
            element.setAttribute( "right", margins[QStyleSheetItem::MarginRight] );
    }

    if ( margins[QStyleSheetItem::MarginTop] != 0 ||
         margins[QStyleSheetItem::MarginBottom] != 0 )
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        if ( margins[QStyleSheetItem::MarginTop] != 0 )
            element.setAttribute( "before", margins[QStyleSheetItem::MarginTop] );
        if ( margins[QStyleSheetItem::MarginBottom] != 0 )
            element.setAttribute( "after", margins[QStyleSheetItem::MarginBottom] );
    }

    if ( lineSpacing != 0 )
    {
        element = doc.createElement( "LINESPACING" );
        parentElem.appendChild( element );
        if ( lineSpacing == LS_ONEANDHALF )
            element.setAttribute( "value", "oneandhalf" );
        else if ( lineSpacing == LS_DOUBLE )
            element.setAttribute( "value", "double" );
        else
            element.setAttribute( "value", lineSpacing );
    }

    if ( pageBreaking != 0 )
    {
        element = doc.createElement( "PAGEBREAKING" );
        parentElem.appendChild( element );
        if ( pageBreaking & KeepLinesTogether )
            element.setAttribute( "linesTogether",  "true" );
        if ( pageBreaking & HardFrameBreakBefore )
            element.setAttribute( "hardFrameBreak", "true" );
        if ( pageBreaking & HardFrameBreakAfter )
            element.setAttribute( "hardFrameBreakAfter", "true" );
    }

    if ( leftBorder.ptWidth > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        leftBorder.save( element );
    }
    if ( rightBorder.ptWidth > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        rightBorder.save( element );
    }
    if ( topBorder.ptWidth > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        topBorder.save( element );
    }
    if ( bottomBorder.ptWidth > 0 )
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        bottomBorder.save( element );
    }
    if ( counter && counter->numbering() != Counter::NUM_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        counter->save( element );
    }

    KoTabulatorList::Iterator it = m_tabList.begin();
    for ( ; it != m_tabList.end() ; it++ )
    {
        element = doc.createElement( "TABULATOR" );
        parentElem.appendChild( element );
        element.setAttribute( "type", (*it).type );
        element.setAttribute( "ptpos", (*it).ptPos );
    }
}
