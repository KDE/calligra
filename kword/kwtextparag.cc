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
#include "kwanchor.h"
#include "kwtextimage.h"
#include "kwtextframeset.h"
#include "kwviewmode.h"
#include "variable.h"
#include "counter.h"
#include <klocale.h>
#include <kdebug.h>
#include <qdom.h>
#include <qtl.h>
#include <assert.h>

KoTextParag::KoTextParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds)
    : QTextParag( d, pr, nx, updateIds )
{
    //kdDebug() << "KoTextParag::KoTextParag " << this << endl;
    m_item = 0L;
}

KoTextParag::~KoTextParag()
{
    if ( !textDocument()->isDestroying() )
        invalidateCounters();
    //kdDebug() << "KoTextParag::~KoTextParag " << this << endl;
    delete m_item;
}

KoTextDocument * KoTextParag::textDocument() const
{
    return static_cast<KoTextDocument *>( document() );
}

// There is one QStyleSheetItems per paragraph, created on demand,
// in order to set the DisplayMode for counters.
void KoTextParag::checkItem( QStyleSheetItem * & item, const char * name )
{
    if ( !item )
    {
        item = new QStyleSheetItem( 0, QString::fromLatin1(name) /* For debugging purposes only */ );
        QVector<QStyleSheetItem> vec = styleSheetItems();
        vec.resize( vec.size() + 1 );
        vec.insert( vec.size() - 1, item );
        //kdDebug() << "KoTextParag::checkItem inserting QStyleSheetItem " << name << " at position " << vec.size()-1 << endl;
        setStyleSheetItems( vec );
    }
}

// Return the counter associated with this paragraph.
KoParagCounter *KoTextParag::counter()
{
    if ( !m_layout.counter )
        return 0L;

    // Garbage collect unnneeded counters.
    if ( m_layout.counter->numbering() == KoParagCounter::NUM_NONE )
        setNoCounter();
    return m_layout.counter;
}

void KoTextParag::setMargin( QStyleSheetItem::Margin m, double _i )
{
    //kdDebug() << "KoTextParag::setMargin " << m << " margin " << _i << endl;
    m_layout.margins[m] = _i;
    if ( m == QStyleSheetItem::MarginTop && prev() )
        prev()->invalidate(0);     // for top margin (post-1.1: remove this, not necessary anymore)
    invalidate(0);
}

void KoTextParag::setMargins( const double * margins )
{
    for ( int i = 0 ; i < 5 ; ++i )
        m_layout.margins[i] = margins[i];
    if ( prev() )
        prev()->invalidate(0);     // for top margin (post-1.1: remove this, not necessary anymore)
    invalidate(0);
}

void KoTextParag::setAlign( int align )
{
    setAlignment( align );
    m_layout.alignment = align;
}

void KoTextParag::setLineSpacing( double _i )
{
    m_layout.lineSpacing = _i;
    invalidate(0);
}

void KoTextParag::setTopBorder( const Border & _brd )
{
    m_layout.topBorder = _brd;
    if ( prev() )
        prev()->invalidate(0);     // for top margin (post-1.1: remove this, not necessary anymore)
    invalidate(0);
}

void KoTextParag::setBottomBorder( const Border & _brd )
{
    m_layout.bottomBorder = _brd;
    invalidate(0);
}

void KoTextParag::setNoCounter()
{
    delete m_layout.counter;
    m_layout.counter = 0L;
    invalidateCounters();
}

void KoTextParag::setCounter( const KoParagCounter & counter )
{
    // Garbage collect unnneeded counters.
    if ( counter.numbering() == KoParagCounter::NUM_NONE )
    {
        setNoCounter();
    }
    else
    {
        delete m_layout.counter;
        m_layout.counter = new KoParagCounter( counter );

        checkItem( m_item, "m_item" );
        // Set the display mode (in order for drawLabel to get called by QTextParag)
        m_item->setDisplayMode( QStyleSheetItem::DisplayListItem );

        // Invalidate the counters
        invalidateCounters();
    }
}

void KoTextParag::invalidateCounters()
{
    // Invalidate this paragraph and all the following ones
    // (Numbering may have changed)
    invalidate( 0 );
    if ( m_layout.counter )
        m_layout.counter->invalidate();
    KoTextParag *s = static_cast<KoTextParag *>( next() );
    while ( s ) {
        if ( s->m_layout.counter )
            s->m_layout.counter->invalidate();
        s->invalidate( 0 );
        s = static_cast<KoTextParag *>( s->next() );
    }
}

int KoTextParag::counterWidth() const
{
    if ( !m_layout.counter )
        return 0;

    return m_layout.counter->width( this );
}

// Draw the counter/bullet for a paragraph
void KoTextParag::drawLabel( QPainter* p, int x, int y, int /*w*/, int h, int base, const QColorGroup& /*cg*/ )
{
    if ( !m_layout.counter ) // shouldn't happen
        return;

    if ( m_layout.counter->numbering() == KoParagCounter::NUM_NONE )
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
            case KoParagCounter::STYLE_DISCBULLET:
                p->setBrush( QBrush(newColor) );
                p->drawEllipse( er );
                p->setBrush( Qt::NoBrush );
                break;
            case KoParagCounter::STYLE_SQUAREBULLET:
                p->fillRect( er , QBrush(newColor) );
                break;
            case KoParagCounter::STYLE_CIRCLEBULLET:
                p->drawEllipse( er );
                break;
            case KoParagCounter::STYLE_CUSTOMBULLET:
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
        {
            //code from qt3stuff
            if (format->vAlign() == QTextFormat::AlignSuperScript )
            {
                QFont tmpFont( p->font() );
                tmpFont.setPointSize( ( tmpFont.pointSize() * 2 ) / 3 );
                p->setFont( tmpFont );
                base=base*2/3;
            }
            else if ( format->vAlign() == QTextFormat::AlignSubScript )
            {
                QFont tmpFont( p->font() );
                tmpFont.setPointSize( ( tmpFont.pointSize() * 2 ) / 3 );
                p->setFont( tmpFont );
            }
            p->drawText( x - size, y - h + base, counterText + ' ' );
        }
    }
    p->restore();
}

int KoTextParag::topMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItY( m_layout.margins[ QStyleSheetItem::MarginTop ] )
        + Border::zoomWidthY( m_layout.topBorder.ptWidth, zh, 0 );
}

int KoTextParag::bottomMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItY( m_layout.margins[ QStyleSheetItem::MarginBottom ] )
        + Border::zoomWidthY( m_layout.bottomBorder.ptWidth, zh, 0 );
}

int KoTextParag::leftMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItX( m_layout.margins[ QStyleSheetItem::MarginLeft ] )
        + Border::zoomWidthX( m_layout.leftBorder.ptWidth, zh, 0 )
        + counterWidth() /* shouldn't be zoomed, it depends on the font sizes */;
}

int KoTextParag::rightMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->zoomItX( m_layout.margins[ QStyleSheetItem::MarginRight ] )
        + Border::zoomWidthX( m_layout.rightBorder.ptWidth, zh, 0 );
}

int KoTextParag::firstLineMargin() const
{
    return textDocument()->zoomHandler()->zoomItX(
        m_layout.margins[ QStyleSheetItem::MarginFirstLine ] );
}

int KoTextParag::lineSpacing( int line ) const
{
    if ( m_layout.lineSpacing >= 0 )
        return textDocument()->zoomHandler()->zoomItY(
            m_layout.lineSpacing );
    else {
        KoTextParag * that = const_cast<KoTextParag *>(this);
        if( line >= (int)that->lineStartList().count() )
        {
            kdError() << "KoTextParag::lineSpacing assert(line<lines) failed: line=" << line << " lines=" << that->lineStartList().count() << endl;
            return 0;
        }
        QMap<int, QTextParagLineStart*>::ConstIterator it = that->lineStartList().begin();
        while ( line-- > 0 )
            ++it;
        int height = ( *it )->h;
        //kdDebug() << " line height=" << height << " valid=" << isValid() << endl;

        if ( m_layout.lineSpacing == KoParagLayout::LS_ONEANDHALF )
        {
            // Tricky. During formatting height doesn't include the linespacing,
            // but afterwards (e.g. when drawing the cursor), it does !
            return isValid() ? height / 3 : height / 2;
        }
        else if ( m_layout.lineSpacing == KoParagLayout::LS_DOUBLE )
        {
            return isValid() ? height / 2 : height;
        }
    }
    kdWarning() << "Unhandled linespacing value : " << m_layout.lineSpacing << endl;
    return 0;
}

// Reimplemented from QTextParag
void KoTextParag::paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cursor, bool drawSelections,
                         int clipx, int clipy, int clipw, int cliph )
{
    //qDebug("KoTextParag::paint %p", this);
    QTextParag::paint( painter, cg, cursor, drawSelections, clipx, clipy, clipw, cliph );

    if ( m_layout.hasBorder() )
    {
        KoZoomHandler * zh = textDocument()->zoomHandler();

        QRect r;
        // Old solution: stick to the text
        //r.setLeft( at( 0 )->x - counterWidth() - 1 );
        //r.setRight( rect().width() - rightMargin() - 1 );

        // New solution: occupy the full width
        r.setLeft( Border::zoomWidthX( m_layout.leftBorder.ptWidth, zh, 0 ) );
        // ## documentWidth breaks with variable width. Maybe use currentDrawnFrame() ?
        r.setRight( documentWidth() - 2 - Border::zoomWidthX( m_layout.rightBorder.ptWidth, zh, 0 ) );
        r.setTop( lineY( 0 ) );
        int lastLine = lines() - 1;
        r.setBottom( static_cast<int>( lineY( lastLine ) + lineHeight( lastLine ) ) );
        // If we don't have a bottom border, we need go as low as possible ( to touch the next parag's border ).
        // If we have a bottom border, then we rather exclude the linespacing. Just looks nicer IMHO.
        if ( m_layout.bottomBorder.ptWidth > 0 )
            r.rBottom() -= lineSpacing( lastLine ) + 1;

        //kdDebug() << "KoTextParag::paint documentWidth=" << documentWidth() << " r=" << DEBUGRECT( r ) << endl;
        Border::drawBorders( painter, zh, r,
                             m_layout.leftBorder, m_layout.rightBorder, m_layout.topBorder, m_layout.bottomBorder,
                             0, QPen() );
    }
}

// Reimplemented from QTextParag
void KWTextParag::drawParagString( QPainter &painter, const QString &s, int start, int len, int startX,
                                   int lastY, int baseLine, int bw, int h, bool drawSelections,
                                   QTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                   const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft )
{
    QTextFormat * localFormat = 0L;
    // Resolve the color before calling the default implementation of drawParagString
    if ( lastFormat && !lastFormat->color().isValid() )
    {
        localFormat = new QTextFormat( *lastFormat );
        localFormat->setColor( KWDocument::defaultTextColor( &painter ) );
        lastFormat = localFormat;
    }

    KWTextFrameSet * textfs = kwTextDocument()->textFrameSet();
    if ( textfs )
    {
        KWDocument * doc = textfs->kWordDocument();
        drawSelections=textfs->currentViewMode()->drawSelections();
        if ( doc && doc->viewFormattingChars() && painter.device()->devType() != QInternal::Printer )
        {
            painter.save();
            QPen pen( cg.color( QColorGroup::Highlight ) ); // ## maybe make configurable ?
            painter.setPen( pen );
            //draw hardframebreak (string) if it is the last line
            //in paragraph before we draw "frame Break"
            //several time.
            if ( hardFrameBreakAfter()&& lineY( lines()-1 )==lastY)
            {
                QTextFormat format = *lastFormat;
                format.setColor( pen.color() );
                // keep in sync with KWTextFrameSet::adjustFlow
                QString str = i18n( "--- Frame Break ---" );
                int width = 0;
                for ( int i = 0 ; i < (int)str.length() ; ++i )
                    width += lastFormat->width( str, i );
                QColorGroup cg2( cg );
                cg2.setColor( QColorGroup::Base, Qt::green );
                int last = length() - 1;
                QTextParag::drawParagString( painter, str, 0, str.length(), at( last )->x,
                                             lastY, at( last )->ascent(), width, lastFormat->height(),
                                             drawSelections, &format, last, selectionStarts,
                                             selectionEnds, cg2, rightToLeft );
            }
            else
            {
                //kdDebug() << "KoTextParag::drawParagString start=" << start << " len=" << len << " length=" << length() << endl;
                if ( start + len == length() )
                {
                    // drawing the end of the parag
                    QTextFormat * format = at( length() - 1 )->format();
                    int w = format->width('x'); // see KWTextFrameSet::adjustFlow
                    int size = QMIN( w, h * 3 / 4 );
                    int arrowsize = textDocument()->zoomHandler()->zoomItY( 2 );
                    // x,y is the bottom right corner of the reversed L
                    int x = startX + bw + w - 1;
                    int y = lastY + baseLine - arrowsize;
                    //kdDebug() << "KoTextParag::drawParagString drawing CR at " << x << "," << y << endl;
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
                    if ( ch.isCustom() )
                        continue;
                    if ( ch.c == ' ' )
                    {
                        int w = string()->width(i);
                        int height = ch.ascent();
                        int size = QMAX( 2, QMIN( w/2, height/3 ) ); // Enfore that it's a square, and that it's visible
                        painter.drawRect( ch.x + (w - size) / 2, lastY + baseLine - (height - size) / 2, size, size );
                    }
                    else if ( ch.c == '\t' )
                    {
                        QTextStringChar &nextch = string()->at(i+1);
                        int nextx = (nextch.x > ch.x) ? nextch.x : rect().width();
                        //kdDebug() << "tab x=" << ch.x << " nextch.x=" << nextch.x
                        //          << " nextx=" << nextx << " startX=" << startX << " bw=" << bw << endl;
                        int availWidth = nextx - ch.x - 1;
                        int x = ch.x + availWidth / 2;
                        int size = QMIN( availWidth, ch.format()->width('W') ) / 2; // actually the half size
                        int y = lastY + baseLine - ch.ascent()/2;
                        int arrowsize = textDocument()->zoomHandler()->zoomItY( 2 );
                        painter.drawLine( x + size, y, x - size, y );
                        painter.drawLine( x + size, y, x + size - arrowsize, y - arrowsize );
                        painter.drawLine( x + size, y, x + size - arrowsize, y + arrowsize );
                    }
                }
            }
            painter.restore();
        }
    }
     QTextParag::drawParagString( painter, s, start, len, startX,
                                 lastY, baseLine, bw, h, drawSelections,
                                 lastFormat, i, selectionStarts,
                                 selectionEnds, cg, rightToLeft );


    if ( localFormat )
        delete localFormat;
}

void KoTextParag::setTabList( const KoTabulatorList &tabList )
{
    KoTabulatorList lst( tabList );
    m_layout.setTabList( lst );
    if ( !tabList.isEmpty() )
    {
        KoZoomHandler * zh = textDocument()->zoomHandler();
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

int KoTextParag::nextTab( int chnum, int x )
{
    if ( !m_layout.tabList().isEmpty() )
    {
        // Fetch the zoomed and sorted tab positions from QTextParag
        // We stored them there for faster access
        int * tArray = tabArray();
        int i = 0;
        while ( tArray[ i ] ) {
            //kdDebug() << "KoTextParag::nextTab tArray[" << i << "]=" << tArray[i] << " type " << m_layout.tabList()[i].type << endl;
            if ( tArray[ i ] >= x ) {
                int type = m_layout.tabList()[i].type;
                switch ( type ) {
                case T_RIGHT:
                case T_CENTER:
                {
                    // Look for the next tab (or EOL)
                    int c = chnum + 1;
                    int w = 0;
                    // We include the trailing space in the calculation because QRT actually formats it
                    while ( c < string()->length() - 1 && string()->at( c ).c != '\t' )
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
                        return tArray[ i ] - w - 1; // -1 is due to qrt's nx-x+1
                    else // T_CENTER
                        return tArray[ i ] - w/2 - 1; // -1 is due to qrt's nx-x+1
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
                    return tArray[ i ] - w - 1; // -1 is due to qrt's nx-x+1
                }
                default: // case T_LEFT:
                    return tArray[ i ] - 1; // -1 is due to qrt's nx-x+1
                }
            }
            ++i;
        }
    }
    // No tab list (or no more tabs), use tab-stop-width. QTextParag has the code :)
    return QTextParag::nextTab( chnum, x );
}

// Reimplemented from QTextParag
void KWTextParag::copyParagData( QTextParag *_parag )
{
    KoTextParag * parag = static_cast<KoTextParag *>(_parag);
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
            KWTextFrameSet * textfs = kwTextDocument()->textFrameSet();
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
        m_layout.pageBreaking &= ~KoParagLayout::HardFrameBreakBefore;
        m_layout.pageBreaking &= ~KoParagLayout::HardFrameBreakAfter;
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

void KWTextParag::setPageBreaking( int pb )
{
    m_layout.pageBreaking = pb;
    invalidate(0);
    if ( next() && ( pb & KoParagLayout::HardFrameBreakAfter ) )
        next()->invalidate(0);
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

KWTextDocument * KWTextParag::kwTextDocument() const
{
    return static_cast<KWTextDocument *>( document() );
}

//static
QDomElement KWTextParag::saveFormat( QDomDocument & doc, KWTextFormat * curFormat, KWTextFormat * refFormat, int pos, int len )
{
    //kdDebug() << "KWTextParag::saveFormat refFormat=" << (  refFormat ? refFormat->key() : "none" )
    //          << " curFormat=" << curFormat->key()
    //          << " pos=" << pos << " len=" << len << endl;
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
    if( !refFormat || curFormat->font().charSet() != refFormat->font().charSet() )
    {
        elem = doc.createElement( "CHARSET" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->font().charSet()) );
    }
    if( !refFormat || curFormat->vAlign() != refFormat->vAlign() )
    {
        elem = doc.createElement( "VERTALIGN" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->vAlign()) );
    }
    return formatElem;
}

void KWTextParag::save( QDomElement &parentElem, int from /* default 0 */,
                        int to /* default length()-2 */,
                        bool saveAnchorsFramesets /* default false */ )
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
        KWTextFormat * newFormat = static_cast<KWTextFormat *>( ch.format() );
        if ( ch.isCustom() )
        {
            if ( startPos > -1 && curFormat) { // Save former format
                QDomElement formatElem = saveFormat( doc, curFormat,
                                                     paragraphFormat(), startPos, index-startPos );
                if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
                    formatsElem.appendChild( formatElem );
            }

            QDomElement formatElem = saveFormat( doc, newFormat, paragraphFormat(), index, 1 );
            formatsElem.appendChild( formatElem );
            static_cast<KWTextCustomItem *>( ch.customItem() )->save( formatElem );
            startPos = -1;
            curFormat = paragraphFormat();
            // Save the contents of the frameset inside the anchor
            // This is NOT used when saving, but it is used when copying an inline frame
            if ( saveAnchorsFramesets && dynamic_cast<KWAnchor *>( ch.customItem() ) )
                static_cast<KWAnchor *>( ch.customItem() )->frameSet()->toXML( parentElem );
        }
        else
        {
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
    elem = formatElem.namedItem( "CHARSET" ).toElement();
    if ( !elem.isNull() )
        font.setCharSet( (QFont::CharSet) elem.attribute("value").toInt() );

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
        KWDocument * doc = kwTextDocument()->textFrameSet()->kWordDocument();
        KoParagLayout paragLayout( layout, doc, true );
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
    KWDocument * doc = kwTextDocument()->textFrameSet()->kWordDocument();
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
                    KWTextImage * custom = new KWTextImage( kwTextDocument(), QString::null );
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
                        KWVariable * var = KWVariable::createVariable( type, -1, kwTextDocument()->textFrameSet() );
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
                            pos.textfs = kwTextDocument()->textFrameSet();
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

void KoTextParag::applyStyle( KWStyle *style )
{
    setParagLayout( style->paragLayout() );
    KWTextFormat *newFormat = &style->format();
    setFormat( 0, string()->length(), newFormat, true );
    setFormat( newFormat );
}

void KoTextParag::setParagLayout( const KoParagLayout & layout, int flags )
{
    //kdDebug() << "KoTextParag::setParagLayout flags=" << flags << endl;
    if ( flags & KoParagLayout::Alignment )
        setAlign( layout.alignment );
    if ( flags & KoParagLayout::Margins )
         setMargins( layout.margins );
    if ( flags & KoParagLayout::LineSpacing )
        setLineSpacing( layout.lineSpacing );
    if ( flags & KoParagLayout::Borders )
    {
        setLeftBorder( layout.leftBorder );
        setRightBorder( layout.rightBorder );
        setTopBorder( layout.topBorder );
        setBottomBorder( layout.bottomBorder );
    }
    if ( flags & KoParagLayout::BulletNumber )
        setCounter( layout.counter );
    if ( flags & KoParagLayout::Tabulator )
        setTabList( layout.tabList() );

    if ( flags == KoParagLayout::All )
        // Don't call setStyle from here, it would overwrite any paragraph-specific settings
        setStyle( layout.style );
}

void KWTextParag::setParagLayout( const KoParagLayout & layout, int flags )
{
    KoTextParag::setParagLayout( layout, flags );

    if ( flags & KoParagLayout::PageBreaking )
        setPageBreaking( layout.pageBreaking );
}

#ifndef NDEBUG
void KWTextParag::printRTDebug( int info )
{
    kdDebug() << "Paragraph " << this << "   (" << paragId() << ") [changed="
              << hasChanged() << ", valid=" << isValid() << "] ------------------ " << endl;
    if ( prev() && prev()->paragId() + 1 != paragId() )
        kdWarning() << "  Previous paragraph " << prev() << " has ID " << prev()->paragId() << endl;
    if ( next() && next()->paragId() != paragId() + 1 )
        kdWarning() << "  Next paragraph " << next() << " has ID " << next()->paragId() << endl;
    if ( !next() )
        kdDebug() << "  next is 0L" << endl;
    if ( isLastInFrame() )
        kdDebug() << "  Is last in frame" << endl;
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
                  << " leftMargin()=" << leftMargin() << " firstLineMargin()=" << firstLineMargin()
                  << " rightMargin()=" << rightMargin() << endl;

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

// Create a default KoParagLayout.
KoParagLayout::KoParagLayout()
{
    initialise();
}

void KoParagLayout::operator=( const KoParagLayout &layout )
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
        counter = new KoParagCounter( *layout.counter );
    else
        counter = 0L;
    lineSpacing = layout.lineSpacing;
    style = layout.style;
    setTabList( layout.tabList() );
}

int KoParagLayout::compare( const KoParagLayout & layout ) const
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
            if ( layout.counter->numbering() != KoParagCounter::NUM_NONE )
                flags |= BulletNumber;
    }
    else
        if ( counter && counter->numbering() != KoParagCounter::NUM_NONE )
            flags |= BulletNumber;

    if ( lineSpacing != layout.lineSpacing )
        flags |= LineSpacing;
    //if ( style != layout.style )
    //    flags |= Style;
    if ( m_tabList != m_tabList )
        flags |= Tabulator;
    return flags;
}

// Create a KoParagLayout from XML.
//
// If a document is supplied, default values are taken from the style in the
// document named by the layout. This allows for simplified import filters,
// and also looks to the day that redundant data can be eliminated from the
// saved XML.
KoParagLayout::KoParagLayout( QDomElement & parentElem, KWDocument *doc, bool useRefStyle )
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
                //kdDebug() << "KoParagLayout::KoParagLayout setting style to " << style << " " << style->name() << endl;
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
    m_tabList.clear();
    QDomElement element = parentElem.firstChild().toElement();
    for ( ; !element.isNull() ; element = element.nextSibling().toElement() )
    {
        if ( element.tagName() == "TABULATOR" )
        {
            KoTabulator tab;
            tab.type = static_cast<KoTabulators>( KWDocument::getAttribute( element, "type", T_LEFT ) );
            tab.ptPos = KWDocument::getAttribute( element, "ptpos", 0.0 );
            m_tabList.append( tab );
        }
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
        counter = new KoParagCounter;
        counter->load( element );
    }
}

void KoParagLayout::initialise()
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

KoParagLayout::~KoParagLayout()
{
    delete counter;
}

void KoParagLayout::save( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    if ( style )
        element.setAttribute( "value", style->name() );
    else
        kdWarning() << "KoParagLayout::save: style==0L!" << endl;

    element = doc.createElement( "FLOW" );
    parentElem.appendChild( element );
    int a = alignment;
    element.setAttribute( "align", a==Qt::AlignRight ? "right" : a==Qt::AlignCenter ? "center" : a==Qt3::AlignJustify ? "justify" : "left" );

    // Disabled the tests, because when loading the default is the style,
    // not 0. So if someone puts e.g. a margin in a style and removes it
    // for a given paragraph, the margin of the style would be applied when
    // re-loading the document.
    // The other option, omitting when saving if it matches the style's layout,
    // would be consistent with the loading code, but would make writing filters
    // more difficult (e.g. plain text and html filters don't care about styles)

    /*if ( margins[QStyleSheetItem::MarginFirstLine] != 0 ||
         margins[QStyleSheetItem::MarginLeft] != 0 ||
         margins[QStyleSheetItem::MarginRight] != 0 )*/
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

    /*if ( margins[QStyleSheetItem::MarginTop] != 0 ||
         margins[QStyleSheetItem::MarginBottom] != 0 )*/
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        if ( margins[QStyleSheetItem::MarginTop] != 0 )
            element.setAttribute( "before", margins[QStyleSheetItem::MarginTop] );
        if ( margins[QStyleSheetItem::MarginBottom] != 0 )
            element.setAttribute( "after", margins[QStyleSheetItem::MarginBottom] );
    }

    /*if ( lineSpacing != 0 )*/
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

    /*if ( pageBreaking != 0 )*/
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

    /*if ( leftBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        leftBorder.save( element );
    }
    /*if ( rightBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        rightBorder.save( element );
    }
    /*if ( topBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        topBorder.save( element );
    }
    /*if ( bottomBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        bottomBorder.save( element );
    }
    /*if ( counter && counter->numbering() != KoParagCounter::NUM_NONE )*/
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        if ( counter )
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
