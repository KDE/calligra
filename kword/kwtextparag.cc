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
#include "kwutils.h"
#include "kwdoc.h"
#include "kwtextframeset.h"
#include <kdebug.h>
#include <qdom.h>

#undef getPointBasedAttribute
#define getPointBasedAttribute(struc, attribute, element, attributeName, defaultValue) \
do \
{ \
    struc pt##attribute = KWDocument::getAttribute( element, attributeName, defaultValue ); \
    struc mm##attribute = POINT_TO_MM( struc pt##attribute ); \
    struc inch##attribute = POINT_TO_INCH( struc pt##attribute ); \
} while (0)

static KWTextParag * const INVALID_PARAG = (KWTextParag *)-1;

Counter::Counter()
{
    m_numbering = NUM_NONE;
    m_style = STYLE_NONE;
    m_depth = 0;
    m_startNumber = 1;
    m_prefix = QString::null;
    m_suffix = '.';
    m_customBullet.character = QChar( '-' );
    m_customBullet.font = QString::null;
    invalidate();
}

bool Counter::operator==( const Counter & c2 ) const
{
    return (m_numbering==c2.m_numbering &&
            m_style==c2.m_style &&
            m_depth==c2.m_depth &&
            m_startNumber==c2.m_startNumber &&
            m_prefix==c2.m_prefix &&
            m_suffix==c2.m_suffix &&
            m_customBullet.character==c2.m_customBullet.character &&
            m_customBullet.font==c2.m_customBullet.font &&
            m_custom==c2.m_custom);

}

QString Counter::custom() const
{
    return m_custom;
}

QChar Counter::customBulletCharacter() const
{
    return m_customBullet.character;
}

QString Counter::customBulletFont() const
{
    return m_customBullet.font;
}

unsigned int Counter::depth() const
{
    return m_depth;
}

void Counter::invalidate()
{
    m_cache.number = -1;
    m_cache.text = QString::null;
    m_cache.width = -1;
    m_cache.parent = INVALID_PARAG;
}

bool Counter::isBullet() const
{
    switch ( style() )
    {
    case STYLE_DISCBULLET:
    case STYLE_SQUAREBULLET:
    case STYLE_CIRCLEBULLET:
    case STYLE_CUSTOMBULLET:
        return true;
    default:
        return false;
    }
}

void Counter::load( QDomElement & element )
{
    m_numbering = static_cast<Numbering>( element.attribute("numberingtype").toInt() );
    m_style = static_cast<Style>( element.attribute("type").toInt() );
    m_depth = element.attribute("depth").toInt();
    m_customBullet.character = QChar( element.attribute("bullet").toInt() );
    m_prefix = correctQString( element.attribute("lefttext") );
    m_suffix = correctQString( element.attribute("righttext") );
    QString s = element.attribute("start");
    if ( s[0].isDigit() )
        m_startNumber = s.toInt();
    else // support for very-old files
        m_startNumber = s.lower()[0].latin1() - 'a' + 1;
    m_customBullet.font = correctQString( element.attribute("bulletfont") );
    m_custom = correctQString( element.attribute("customdef") );
    invalidate();
}

int Counter::number( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( m_cache.number != -1 )
        return m_cache.number;

    // Go looking for another paragraph at the same level or higher level.
    KWTextParag *otherParagraph = static_cast<KWTextParag *>( paragraph->prev() );
    Counter *otherCounter;

    switch ( m_numbering )
    {
    case NUM_NONE:
        // This should not occur!
        m_cache.number = 0;
        break;
    case NUM_CHAPTER:
        m_cache.number = m_startNumber;
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter &&                                        // ...numbered paragraphs.
                ( otherCounter->m_numbering == NUM_CHAPTER ) &&         // ...same number type.
                ( otherCounter->m_depth <= m_depth ) )        // ...same or higher level.
            {
                if ( ( otherCounter->m_depth == m_depth ) &&
                   ( otherCounter->m_style == m_style ) )
                {
                    // Found a preceeding paragraph of exactly our type!
                    m_cache.number = otherCounter->number( otherParagraph ) + 1;
                }
                else
                {
                    // Found a preceeding paragraph of higher level!
                    m_cache.number = m_startNumber;
                }
                break;
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    case NUM_LIST:
        m_cache.number = m_startNumber;
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter )                                         // ...numbered paragraphs.
            {
                if ( ( otherCounter->m_numbering == NUM_LIST ) &&       // ...same number type.
                    ( otherCounter->m_depth <= m_depth ) )    // ...same or higher level.
                {
                    if ( ( otherCounter->m_depth == m_depth ) &&
                       ( otherCounter->m_style == m_style ) )
                    {
                        // Found a preceeding paragraph of exactly our type!
                        m_cache.number = otherCounter->number( otherParagraph ) + 1;
                    }
                    else
                    {
                        // Found a preceeding paragraph of higher level!
                        m_cache.number = m_startNumber;
                    }
                    break;
                }
                else
                if ( otherCounter->m_numbering == NUM_CHAPTER )        // ...heading number type.
                {
                    m_cache.number = m_startNumber;
                    break;
                }
            }
            else
            {
                // There is no counter at all.
                m_cache.number = m_startNumber;
                break;
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    }
    return m_cache.number;
}

Counter::Numbering Counter::numbering() const
{
    return m_numbering;
}

// Go looking for another paragraph at a higher level.
KWTextParag *Counter::parent( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( m_cache.parent != INVALID_PARAG )
        return m_cache.parent;

    KWTextParag *otherParagraph = static_cast<KWTextParag *>( paragraph->prev() );
    Counter *otherCounter;

    switch ( m_numbering )
    {
    case NUM_NONE:
        // This should not occur!
        otherParagraph = 0L;
        break;
    case NUM_CHAPTER:
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter &&                                        // ...numbered paragraphs.
                ( otherCounter->m_numbering == NUM_CHAPTER ) &&         // ...same number type.
                ( otherCounter->m_depth < m_depth ) )         // ...higher level.
            {
                break;
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    case NUM_LIST:
        // Go upwards while...
        while ( otherParagraph )
        {
            otherCounter = otherParagraph->counter();
            if ( otherCounter )                                         // ...numbered paragraphs.
            {
                if ( ( otherCounter->m_numbering == NUM_LIST ) &&       // ...same number type.
                    ( otherCounter->m_depth < m_depth ) )     // ...higher level.
                {
                    break;
                }
                else
                if ( otherCounter->m_numbering == NUM_CHAPTER )         // ...heading number type.
                {
                    otherParagraph = 0L;
                    break;
                }
            }
            otherParagraph = static_cast<KWTextParag *>( otherParagraph->prev() );
        }
        break;
    }
    m_cache.parent = otherParagraph;
    return m_cache.parent;
}

QString Counter::prefix() const
{
    return m_prefix;
}

void Counter::save( QDomElement & element )
{
    element.setAttribute( "type", static_cast<int>( m_style ) );
    element.setAttribute( "depth", m_depth );
    element.setAttribute( "bullet", m_customBullet.character.unicode() );
    element.setAttribute( "lefttext", m_prefix );
    element.setAttribute( "righttext", m_suffix );
    element.setAttribute( "start", m_startNumber );
    element.setAttribute( "numberingtype", static_cast<int>( m_numbering ) );
    element.setAttribute( "bulletfont", m_customBullet.font );
    element.setAttribute( "customdef", m_custom );
}

void Counter::setCustom( QString c )
{
    m_custom = c;
    invalidate();
}

void Counter::setCustomBulletCharacter( QChar c )
{
    m_customBullet.character = c;
    invalidate();
}

void Counter::setCustomBulletFont( QString f )
{
    m_customBullet.font = f;
    invalidate();
}

void Counter::setDepth( unsigned int d )
{
    m_depth = d;
    invalidate();
}

void Counter::setNumbering( Numbering n )
{
    m_numbering = n;
    invalidate();
}

void Counter::setPrefix( QString p )
{
    m_prefix = p;
    invalidate();
}
void Counter::setStartNumber( int s )
{
    m_startNumber = s;
    invalidate();
}

void Counter::setStyle( Style s )
{
    m_style = s;
    invalidate();
}

void Counter::setSuffix( QString s )
{
    m_suffix = s;
    invalidate();
}

int Counter::startNumber() const
{
    return m_startNumber;
}

Counter::Style Counter::style() const
{
    switch ( m_style )
    {
    case STYLE_DISCBULLET:
    case STYLE_SQUAREBULLET:
    case STYLE_CIRCLEBULLET:
    case STYLE_CUSTOMBULLET:
        if ( m_numbering == NUM_CHAPTER )
        {
            // Shome mishtake surely!
            return STYLE_NUM;
        }
        break;
    default:
        break;
    }
    return m_style;
}

QString Counter::suffix() const
{
    return m_suffix;
}

QString Counter::text( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( m_cache.text != QString::null )
        return m_cache.text;

    // Recurse to find the text of the preceeding level.
    if ( parent( paragraph ) )
    {
        m_cache.text = m_cache.parent->counter()->text( m_cache.parent );

        // If the preceeding level is a bullet, replace it with blanks.
        if ( m_cache.parent->counter()->isBullet() )
            for ( unsigned i = 0; i < m_cache.text.length(); i++ )
                m_cache.text.at( i ) = ' ';
    }

    // Ensure paragraph number is valid.
    number( paragraph );

    // Now convert to text.
    QString tmp;
    int n;
    char bottomDigit;
    switch ( style() )
    {
    case STYLE_NONE:
        if ( m_numbering == NUM_LIST )
            tmp = ' ';
        break;
    case STYLE_NUM:
        tmp.setNum( m_cache.number );
        break;
    case STYLE_ALPHAB_L:
        n = m_cache.number;
        while ( n >= 26 )
        {
            bottomDigit = n % 26;
            n = n / 26;
            tmp.prepend( QChar( 'a' + bottomDigit - 1 ) );
        }
        tmp.prepend( QChar( 'a' + n - 1 ) );
        break;
    case STYLE_ALPHAB_U:
        n = m_cache.number;
        while ( n >= 26 )
        {
            bottomDigit = n % 26;
            n = n / 26;
            tmp.prepend( QChar( 'A' + bottomDigit - 1 ) );
        }
        tmp.prepend( QChar( 'A' + n - 1 ) );
        break;
    case STYLE_ROM_NUM_L:
        tmp = makeRomanNumber( m_cache.number ).lower();
        break;
    case STYLE_ROM_NUM_U:
        tmp = makeRomanNumber( m_cache.number ).upper();
        break;
    case STYLE_CUSTOM:
        ////// TODO
        tmp.setNum( m_cache.number );
        break;
    case Counter::STYLE_DISCBULLET:
    case Counter::STYLE_SQUAREBULLET:
    case Counter::STYLE_CIRCLEBULLET:
    case Counter::STYLE_CUSTOMBULLET:
        // Allow space for bullet!
        tmp = ' ';
        break;
    }
    tmp.prepend( prefix() );
    tmp.append( suffix() );

    // Find the number of missing parents, and add dummy text for them.
    int missingParents;
    if ( parent( paragraph ) )
    {
        missingParents = m_depth - m_cache.parent->counter()->m_depth - 1;
    }
    else
    {
        missingParents = m_depth;
    }
    while ( missingParents )
    {
        // Each missing level adds a "0." prefix.
        tmp.prepend( "0." );
        missingParents--;
    }
    m_cache.text.append( tmp );
    return m_cache.text;
}

int Counter::width( const KWTextParag *paragraph )
{
    // Return cached value if possible.
    if ( m_cache.width != -1 )
        return m_cache.width;

    // Ensure paragraph text is valid.
    text( paragraph );

    // Now calculate width.
    QTextFormat *format = paragraph->paragFormat();
    m_cache.width = 0;
    for ( unsigned int i = 0; i < m_cache.text.length(); i++ )
        m_cache.width += format->width( m_cache.text, i );
    return m_cache.width;
}

KWTextParag::KWTextParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds)
    : QTextParag( d, pr, nx, updateIds )
{
    //kdDebug() << "KWTextParag::KWTextParag " << this << endl;
    m_item = 0L;
    m_leftBorder.ptWidth = 0;
    m_rightBorder.ptWidth = 0;
    m_topBorder.ptWidth = 0;
    m_bottomBorder.ptWidth = 0;
    m_counter = 0L;
}

KWTextParag::~KWTextParag()
{
    //kdDebug() << "KWTextParag::~KWTextParag " << this << endl;
    delete m_item;
    delete m_counter;
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
    if ( !m_counter )
        return m_counter;

    // Garbage collect unnneeded counters.
    if ( m_counter->numbering() == Counter::NUM_NONE )
        setNoCounter();
    return m_counter;
}

void KWTextParag::setMargin( QStyleSheetItem::Margin m, KWUnit _i )
{
    kdDebug() << "KWTextParag::setMargin " << m << " margin " << _i.pt() << endl;
    m_margins[m] = _i;
    if ( m == QStyleSheetItem::MarginTop && prev() )
        prev()->invalidate(0);     // for top margin
    invalidate(0);
}

void KWTextParag::setMargins( const KWUnit * margins )
{
    for ( int i = 0 ; i < 5 ; ++i )
        m_margins[i] = margins[i];
    if ( prev() )
        prev()->invalidate(0);     // for top margin
    invalidate(0);
}


void KWTextParag::setLineSpacing( KWUnit _i )
{
    m_lineSpacing = _i;
    invalidate(0);
}

void KWTextParag::setTopBorder( const Border & _brd )
{
    m_topBorder = _brd;
    invalidate(0);
}

void KWTextParag::setBottomBorder( const Border & _brd )
{
    m_bottomBorder = _brd;
    invalidate(0);
}

void  KWTextParag::setTabList( const QList<KoTabulator> *tabList )
{
    m_tabList.clear();
    QListIterator<KoTabulator> it( *tabList );
    for ( it.toFirst(); it.current(); ++it ) {
        KoTabulator *t = new KoTabulator;
        t->type = it.current()->type;
        t->mmPos = it.current()->mmPos;
        t->inchPos = it.current()->inchPos;
        t->ptPos = it.current()->ptPos;
        m_tabList.append( t );
    }
}

void KWTextParag::setNoCounter()
{
    delete m_counter;
    m_counter = 0L;
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
        delete m_counter;
        m_counter = new Counter( counter );

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
    if ( m_counter )
        m_counter->invalidate();
    KWTextParag *s = static_cast<KWTextParag *>( next() );
    while ( s ) {
        if ( s->m_counter )
            s->m_counter->invalidate();
        s->invalidate( 0 );
        s = static_cast<KWTextParag *>( s->next() );
    }
}

int KWTextParag::counterWidth() const
{
    if ( !m_counter )
        return 0;

    return m_counter->width( this );
}

// Draw the counter/bullet for a paragraph
void KWTextParag::drawLabel( QPainter* p, int x, int y, int /*w*/, int h, int base, const QColorGroup& cg )
{
    if ( !m_counter ) // shouldn't happen
        return;

    int size = m_counter->width( this );

    // Draw the complete label.
    QTextFormat *format = paragFormat();
    QFont oldFont = p->font();
    QFont newFont = format->font();
    // ### Problem is, the paragFormat never changes. It should probably
    // change when we change the format of the whole paragraph ?
    p->setFont( newFont );

    // Now draw any bullet that is required over the space left for it.
    if ( m_counter->isBullet() )
    {
        // Modify x offset.
        for ( unsigned int i = 0; i < m_counter->suffix().length(); i++ )
            x -= format->width( m_counter->suffix(), i );
        int width = format->width( ' ' );
        int height = format->height();
        QRect er( x - width, y - h + height / 2 - width / 2, width, width );

        // Draw the bullet.
        switch ( m_counter->style() )
        {
            case Counter::STYLE_DISCBULLET:
                p->setBrush( cg.brush( QColorGroup::Foreground ) );
                p->drawEllipse( er );
                p->setBrush( Qt::NoBrush );
                break;
            case Counter::STYLE_SQUAREBULLET:
                p->fillRect( er , cg.brush( QColorGroup::Foreground ) );
                break;
            case Counter::STYLE_CIRCLEBULLET:
                p->drawEllipse( er );
                break;
            case Counter::STYLE_CUSTOMBULLET:
                // The user has selected a symbol from a special font. Override the paragraph
                // font with the given family. This conserves the right size etc.
                if ( !m_counter->customBulletFont().isEmpty() )
                {
                    newFont.setFamily( m_counter->customBulletFont() );
                    p->setFont( newFont );
                }
                p->drawText( x - width, y - h + base, m_counter->customBulletCharacter() );
                break;
            default:
                break;
        }
    }
    else
    {
        // There are no bullets...any parent bullets have already been suppressed.
        // Just draw the text!
        p->drawText( x - size, y - h + base, m_counter->text( this ) );
    }
    p->setFont( oldFont );
}

int KWTextParag::topMargin() const
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
    return static_cast<int>( textdoc->textFrameSet()->kWordDocument()->zoomItY(
        m_margins[ QStyleSheetItem::MarginTop ].pt()
        + m_topBorder.ptWidth ) );
}

int KWTextParag::bottomMargin() const
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
    return static_cast<int>( textdoc->textFrameSet()->kWordDocument()->zoomItY(
        m_margins[ QStyleSheetItem::MarginBottom ].pt()
        + m_bottomBorder.ptWidth ) );
}

int KWTextParag::leftMargin() const
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
    return static_cast<int>( textdoc->textFrameSet()->kWordDocument()->zoomItX(
        m_margins[ QStyleSheetItem::MarginLeft ].pt()
        + m_leftBorder.ptWidth
        + counterWidth() ) );
}

int KWTextParag::rightMargin() const
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
    return static_cast<int>( textdoc->textFrameSet()->kWordDocument()->zoomItX(
        m_margins[ QStyleSheetItem::MarginRight ].pt()
        + m_rightBorder.ptWidth ) );
}

int KWTextParag::firstLineMargin() const
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
    return static_cast<int>( textdoc->textFrameSet()->kWordDocument()->zoomItX(
        m_margins[ QStyleSheetItem::MarginFirstLine ].pt() ) );
}

int KWTextParag::lineSpacing() const
{
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
    return static_cast<int>( textdoc->textFrameSet()->kWordDocument()->zoomItY(
        m_lineSpacing.pt() ) );
}

// Reimplemented from QTextParag
void KWTextParag::paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cursor, bool drawSelections,
                         int clipx, int clipy, int clipw, int cliph )
{
    //qDebug("KWTextParag::paint %p", this);
    QTextParag::paint( painter, cg, cursor, drawSelections, clipx, clipy, clipw, cliph );

    if ( m_topBorder.ptWidth > 0
         || m_bottomBorder.ptWidth > 0
         || m_leftBorder.ptWidth > 0
         || m_rightBorder.ptWidth > 0 )
    {
        int leftX = 0;
        int rightX = documentWidth()-1;
        int topY = lineY( 0 ); // Maybe this is always 0. Not sure.
        int bottomY = static_cast<int>( lineY( lines() -1 ) + lineHeight( lines() -1 ) - m_lineSpacing.pt() );
        //kdDebug() << "KWTextParag::paint bottomY=" << bottomY << endl;
        if ( m_topBorder.ptWidth > 0 )
        {
            painter.setPen( Border::borderPen( m_topBorder ) ); // ### in theory we should zoomIt(ptWidth)
            painter.drawLine( leftX, topY, rightX, topY );
        }
        if ( m_bottomBorder.ptWidth > 0 )
        {
            painter.setPen( Border::borderPen( m_bottomBorder ) );
            painter.drawLine( leftX, bottomY, rightX, bottomY );
        }
        if ( m_leftBorder.ptWidth > 0 )
        {
            painter.setPen( Border::borderPen( m_leftBorder ) );
            painter.drawLine( leftX, topY, leftX, bottomY );
        }
        if ( m_rightBorder.ptWidth > 0 )
        {
            painter.setPen( Border::borderPen( m_rightBorder ) );
            painter.drawLine( rightX, topY, rightX, bottomY );
        }
    }
}

void KWTextParag::copyParagData( QTextParag *_parag )
{
    KWTextParag * parag = static_cast<KWTextParag *>(_parag);
    KWTextDocument * textdoc = static_cast<KWTextDocument *>(parag->document());
    KWDocument * doc = textdoc->textFrameSet()->kWordDocument();
    // Style of the previous paragraph
    KWStyle * style = doc->findStyle( parag->styleName() );
    // Obey "following style" setting
    bool styleApplied = false;
    if ( style && !style->followingStyle().isEmpty() )
    {
        KWStyle * newStyle = doc->findStyle( style->followingStyle() );
        if (!newStyle)
            kdWarning() << "Style " << style->followingStyle() << " not found" << endl;
        else if ( style != newStyle ) // if same style, keep paragraph-specific changes as usual
        {
            setParagLayout( newStyle->paragLayout() );
            QTextFormat * format = textdoc->formatCollection()->format( &newStyle->format() );
            setFormat( format );
            string()->setFormat( 0, format, true ); // prepare format for text insertion
            styleApplied = true;
        }
    }
    // No "following style" setting, or same style -> copy layout & format of previous paragraph
    if (!styleApplied)
    {
        setParagLayout( parag->createParagLayout() );
        setFormat( parag->paragFormat() );
        // QTextCursor::splitAndInsertEmptyParag takes care of setting the format
        // for the chars in the new parag
    }

    // Note: we don't call QTextParag::copyParagData on purpose.
    // We don't want setListStyle to get called - it ruins our stylesheetitems
    // And we don't care about copying the stylesheetitems directly,
    // applying the parag layout will create them
}

void KWTextParag::setCustomItem( int index, QTextCustomItem * custom, QTextFormat * currentFormat )
{
    setFormat( index, 1, currentFormat );
    at( index )->setCustomItem( custom );
    addCustomItem();
    document()->registerCustomItem( custom, this );
    invalidate( 0 );
}

//static
QDomElement KWTextParag::saveFormat( QDomDocument & doc, QTextFormat * curFormat, QTextFormat * refFormat, int pos, int len )
{
    QDomElement formatElem = doc.createElement( "FORMAT" );
    formatElem.setAttribute( "id", 1 ); // text format
    formatElem.setAttribute( "pos", pos );
    formatElem.setAttribute( "len", len );
    QDomElement elem;
    if( !refFormat || curFormat->font().weight() != refFormat->font().weight() )
    {
        elem = doc.createElement( "WEIGHT" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", curFormat->font().weight() );
    }
    if( !refFormat || curFormat->color() != refFormat->color() )
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
    if( !refFormat || curFormat->vAlign() != refFormat->vAlign() )
    {
        elem = doc.createElement( "VERTALIGN" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->vAlign()) );
    }
    return formatElem;
}

void KWTextParag::save( QDomElement &parentElem, int from /* default 0 */, int to /* default -1 */ )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement paragElem = doc.createElement( "PARAGRAPH" );
    parentElem.appendChild( paragElem );
    QDomElement textElem = doc.createElement( "TEXT" );
    paragElem.appendChild( textElem );
    QString text = string()->toString();
    ASSERT( text.right(1)[0] == ' ' );
    if ( to == -1 )
        to = string()->length()-2; // 'to' is inclusive, and length-1 is the trailing space
    textElem.appendChild( doc.createTextNode( text.mid( from, to - from + 1 ) ) );

    QDomElement formatsElem = doc.createElement( "FORMATS" );
    int startPos = -1;
    int index = 0; // Usually same as 'i' but if from>0, 'i' indexes the parag's text and this one indexes the output
    QTextFormat *curFormat = paragFormat();
    for ( int i = from; i <= to; ++i, ++index )
    {
        QTextStringChar & ch = string()->at(i);
        if ( ch.isCustom() )
        {
            if ( startPos > -1 && curFormat) { // Save former format
                QDomElement formatElem = saveFormat( doc, curFormat, paragFormat(), startPos, index-startPos );
                if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
                    formatsElem.appendChild( formatElem );
            }

            QDomElement formatElem = doc.createElement( "FORMAT" );
            formatsElem.appendChild( formatElem );
            formatElem.setAttribute( "pos", index );
            formatElem.setAttribute( "len", 1 );
            // Is it an image ?
            KWTextImage * ti = dynamic_cast<KWTextImage *>( ch.customItem() );
            if ( ti )
            {
                formatElem.setAttribute( "id", 2 ); // code for a picture
                QDomElement imageElem = parentElem.ownerDocument().createElement( "IMAGE" );
                formatElem.appendChild( imageElem );
                QDomElement elem = parentElem.ownerDocument().createElement( "FILENAME" );
                imageElem.appendChild( elem );
                elem.setAttribute( "value", ti->image().key() );
                startPos = -1;
            }
            else
            {
                // ... TODO
            }
        }
        else
        {
            QTextFormat * newFormat = ch.format();
            if ( newFormat != curFormat )
            {
                // Format changed.
                if ( startPos > -1 && curFormat) { // Save former format
                    QDomElement formatElem = saveFormat( doc, curFormat, paragFormat(), startPos, index-startPos );
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
                    curFormat = paragFormat();
                }
            }
        }
    }
    if ( startPos > -1 && index > startPos && curFormat) { // Save last format
        QDomElement formatElem = saveFormat( doc, curFormat, paragFormat(), startPos, index-startPos );
        if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
            formatsElem.appendChild( formatElem );
    }

    if (!formatsElem.firstChild().isNull()) // Do we have formats to save ?
        paragElem.appendChild( formatsElem );


    QDomElement layoutElem = doc.createElement( "LAYOUT" );
    paragElem.appendChild( layoutElem );

    KWParagLayout layout = createParagLayout(); // slightly slow method but allows to reuse code
    layout.save( layoutElem );

    // Paragraph's format
    // ## Maybe we should have a "default format" somewhere and
    // pass it instead of 0L, to only save the non-default attributes
    QDomElement paragFormatElement = saveFormat( doc, paragFormat(), 0L, 0, to - from + 1 );
    layoutElem.appendChild( paragFormatElement );

    // TODO TABULATOR
}

//static
QTextFormat KWTextParag::loadFormat( QDomElement &formatElem, QTextFormat * refFormat )
{
    QTextFormat format;
    if ( refFormat )
        format = *refFormat;
    QFont font = format.font();
    // Since most fields might be missing, maybe it would be faster
    // to iterate over the children... Not sure it makes any real difference though.
    QDomElement elem = formatElem.namedItem( "WEIGHT" ).toElement();
    if ( !elem.isNull() )
        font.setWeight( elem.attribute( "value" ).toInt() );
    elem = formatElem.namedItem( "COLOR" ).toElement();
    if ( !elem.isNull() )
    {
        QColor col( elem.attribute("red").toInt(),
                    elem.attribute("green").toInt(),
                    elem.attribute("blue").toInt() );
        format.setColor( col );
    }
    elem = formatElem.namedItem( "FONT" ).toElement();
    if ( !elem.isNull() )
        font.setFamily( elem.attribute("name") );
    elem = formatElem.namedItem( "SIZE" ).toElement();
    if ( !elem.isNull() )
        font.setPointSize( elem.attribute("value").toInt() );
    elem = formatElem.namedItem( "ITALIC" ).toElement();
    if ( !elem.isNull() )
        font.setItalic( elem.attribute("value").toInt() == 1 );
    elem = formatElem.namedItem( "UNDERLINE" ).toElement();
    if ( !elem.isNull() )
        font.setUnderline( elem.attribute("value").toInt() == 1 );
    elem = formatElem.namedItem( "VERTALIGN" ).toElement();
    if ( !elem.isNull() )
        format.setVAlign( static_cast<QTextFormat::VerticalAlignment>( elem.attribute("value").toInt() ) );

    format.setFont( font );
    return format;
}

void KWTextParag::loadLayout( QDomElement & attributes )
{
    QDomElement layout = attributes.namedItem( "LAYOUT" ).toElement();
    if ( !layout.isNull() )
    {
        KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
        KWDocument * doc = textdoc->textFrameSet()->kWordDocument();
        KWParagLayout paragLayout( layout, doc );
        setParagLayout( paragLayout );

        // Load default format from style.
        KWStyle *existingStyle = doc->findStyle( m_styleName );
        QTextFormat *defaultFormat = existingStyle ? &existingStyle->format() : 0L;
        QDomElement formatElem = layout.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
        {
            // Load paragraph format
            QTextFormat f = loadFormat( formatElem, defaultFormat );
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
        kdError(32001) << "Missing LAYOUT tag" << endl;
    }
}

void KWTextParag::load( QDomElement &attributes )
{
    loadLayout( attributes );

    // Set the text after setting the paragraph format - so that the format applies
    QDomElement element = attributes.namedItem( "TEXT" ).toElement();
    if ( !element.isNull() )
    {
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
    QDomElement formatsElem = attributes.namedItem( "FORMATS" ).toElement();
    if ( !formatsElem.isNull() )
    {
        //QTextFormatCollection *fc = formatCollection();
        QDomNodeList listFormats = formatsElem.elementsByTagName( "FORMAT" );
        for (unsigned int item = 0; item < listFormats.count(); item++)
        {
            QDomElement formatElem = listFormats.item( item ).toElement();
            int index = formatElem.attribute( "pos" ).toInt() + offset;
            int len = formatElem.attribute( "len" ).toInt();

            int id = formatElem.attribute( "id" ).toInt();
            switch( id ) {
            case 1: // Normal text
            {
                QTextFormat f = loadFormat( formatElem, paragFormat() );
                //kdDebug(32002) << "KWTextParag::loadFormatting applying formatting from " << index << " to " << index+len << endl;
                setFormat( index, len, document()->formatCollection()->format( &f ) );
                break;
            }
            case 2: // Picture
            {
                ASSERT( len == 1 );
                KWTextDocument * textdoc = static_cast<KWTextDocument *>(document());
                KWDocument * doc = textdoc->textFrameSet()->kWordDocument();
                KWTextImage * custom = new KWTextImage( textdoc, QString::null );
                kdDebug() << "KWTextParag::loadFormatting insertCustomItem" << endl;
                setCustomItem( index, custom, paragFormat() );
                // <IMAGE>
                QDomElement image = formatElem.namedItem( "IMAGE" ).toElement();
                if ( !image.isNull() ) {
                    // <FILENAME>
                    QDomElement filenameElement = image.namedItem( "FILENAME" ).toElement();
                    if ( !filenameElement.isNull() )
                    {
                        QString filename = filenameElement.attribute( "value" );
                        doc->addImageRequest( filename, custom );
                    }
                    else
                        kdError(32001) << "Missing FILENAME tag in IMAGE" << endl;
                } else
                    kdError(32001) << "Missing IMAGE tag in FORMAT wth id=2" << endl;

                break;
            }
            default:
                kdWarning() << "KWTextParag::loadFormat id=" << id << " not supported" << endl;
                break;
            }
        }
    }
}

KWParagLayout KWTextParag::createParagLayout()
{
    return KWParagLayout( *this );
}

void KWTextParag::setParagLayout( const KWParagLayout & layout )
{
    //kdDebug() << "KWTextParag " << this << " setParagLayout" << endl;
    setAlignment( layout.alignment );
    setMargins( layout.margins );
    setLineSpacing( layout.lineSpacing );
    setLeftBorder( layout.leftBorder );
    setRightBorder( layout.rightBorder );
    setTopBorder( layout.topBorder );
    setBottomBorder( layout.bottomBorder );
    setCounter( layout.counter );

    setTabList( layout.tabList() );
    // Don't call setStyle from here, it would overwrite any paragraph-specific settings
    m_styleName = layout.styleName();
}

// Create a default KWParagLayout.
KWParagLayout::KWParagLayout()
{
    initialise();
}

// Create a KWParagLayout from an existing paragraph's layout.
KWParagLayout::KWParagLayout( KWTextParag &parag )
{
    initialise();

    // From QTextParag
    alignment = parag.alignment();

    // From KWTextParag
    for ( int i = 0 ; i < 5 ; ++i )
        margins[i] = parag.margins()[i];
    leftBorder = parag.leftBorder();
    rightBorder = parag.rightBorder();
    topBorder = parag.topBorder();
    bottomBorder = parag.bottomBorder();
    if ( parag.counter() )
        counter = *parag.counter();
    lineSpacing = parag.kwLineSpacing();
    m_styleName = parag.styleName();
    setTabList( parag.tabList() );
}

// Create a KWParagLayout from XML.
//
// If a document is supplied, default values are taken from the style in the
// document named by the layout. This allows for simplified import filters,
// and also looks to the day that redundant data can be eliminated from the
// saved XML.
KWParagLayout::KWParagLayout( QDomElement & parentElem, KWDocument *doc )
{
    initialise();

    // Name of the style. If there is no style, then we do not supply
    // any default!
    QDomElement element = parentElem.namedItem( "NAME" ).toElement();
    if ( !element.isNull() )
    {
        m_styleName = element.attribute( "value" );
        if ( doc )
        {
            // Default all the layout stuff from the style.
            KWStyle *existingStyle = doc->findStyle( m_styleName );
            if (existingStyle)
            {
                *this = existingStyle->paragLayout();
            }
            else
            {
                kdError(32001) << "Cannot find style \"" << m_styleName << "\"" << endl;
            }
        }
    }
    else
    {
        kdError(32001) << "Missing NAME tag in LAYOUT" << endl;
    }

    element = parentElem.namedItem( "TABULATOR" ).toElement();
    if ( !element.isNull() )
    {
        KoTabulator *tab = new KoTabulator;
        tab->type = static_cast<KoTabulators>( KWDocument::getAttribute( element, "type", T_LEFT ) );
        getPointBasedAttribute( tab->, Pos, element, "ptpos", 0.0 );
        //m_tabList.append( tab ); TODO
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
            flow = element.attribute( "align" ); // KWord-v2 DTD
            alignment = flow=="right" ? Qt::AlignRight : flow=="center" ? Qt::AlignCenter : flow=="justify" ? Qt3::AlignJustify : Qt::AlignLeft;
        }
    }

    element = parentElem.namedItem( "OHEAD" ).toElement(); // Should have been in KWord-v1 DTD, used by KWord-0.8
    if ( !element.isNull() )
        margins[QStyleSheetItem::MarginTop].setPT( KWDocument::getAttribute( element, "pt", 0.0 ) );

    element = parentElem.namedItem( "OFOOT" ).toElement(); // Should have been in KWord-v1 DTD, used by KWord-0.8
    if ( !element.isNull() )
        margins[QStyleSheetItem::MarginBottom].setPT( KWDocument::getAttribute( element, "pt", 0.0 ) );

    element = parentElem.namedItem( "IFIRST" ).toElement(); // Should have been in KWord-v1 DTD, used by KWord-0.8
    if ( !element.isNull() )
        margins[QStyleSheetItem::MarginFirstLine].setPT( KWDocument::getAttribute( element, "pt", 0.0 ) );

    element = parentElem.namedItem( "ILEFT" ).toElement(); // Should have been in KWord-v1 DTD, used by KWord-0.8
    if ( !element.isNull() )
        margins[QStyleSheetItem::MarginLeft].setPT( KWDocument::getAttribute( element, "pt", 0.0 ) );

    // KWord-v2 DTD
    element = parentElem.namedItem( "INDENTS" ).toElement();
    if ( !element.isNull() )
    {
        margins[QStyleSheetItem::MarginFirstLine].setPT( KWDocument::getAttribute( element, "first", 0.0 ) );
        margins[QStyleSheetItem::MarginLeft].setPT( KWDocument::getAttribute( element, "left", 0.0 ) );
        margins[QStyleSheetItem::MarginRight].setPT( KWDocument::getAttribute( element, "right", 0.0 ) );
    }
    element = parentElem.namedItem( "OFFSETS" ).toElement();
    if ( !element.isNull() )
    {
        margins[QStyleSheetItem::MarginTop].setPT( KWDocument::getAttribute( element, "before", 0.0 ) );
        margins[QStyleSheetItem::MarginBottom].setPT( KWDocument::getAttribute( element, "after", 0.0 ) );
    }

    element = parentElem.namedItem( "LINESPACE" ).toElement(); // not in KWord-v1 DTD, used by KWord-0.8
    if ( !element.isNull() )
        lineSpacing.setPT( KWDocument::getAttribute( element, "pt", 0.0 ) );

    element = parentElem.namedItem( "LINESPACING" ).toElement(); // KWord-v2 DTD
    if ( !element.isNull() )
        lineSpacing.setPT( KWDocument::getAttribute( element, "value", 0.0 ) );

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
        counter.load( element );
    }
}

void KWParagLayout::initialise()
{
    alignment = Qt::AlignLeft;
    leftBorder.ptWidth = 0;
    rightBorder.ptWidth = 0;
    topBorder.ptWidth = 0;
    bottomBorder.ptWidth = 0;
}

void KWParagLayout::save( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", m_styleName );

    element = doc.createElement( "FLOW" );
    parentElem.appendChild( element );
    int a = alignment;
    element.setAttribute( "align", a==Qt::AlignRight ? "right" : a==Qt::AlignCenter ? "center" : a==Qt3::AlignJustify ? "justify" : "left" );
    if ( margins[QStyleSheetItem::MarginFirstLine].pt() != 0 ||
         margins[QStyleSheetItem::MarginLeft].pt() != 0 ||
         margins[QStyleSheetItem::MarginRight].pt() != 0 )
    {
        element = doc.createElement( "INDENTS" );
        parentElem.appendChild( element );
        if ( margins[QStyleSheetItem::MarginFirstLine].pt() != 0 )
            element.setAttribute( "first", margins[QStyleSheetItem::MarginFirstLine].pt() );
        if ( margins[QStyleSheetItem::MarginLeft].pt() != 0 )
            element.setAttribute( "left", margins[QStyleSheetItem::MarginLeft].pt() );
        if ( margins[QStyleSheetItem::MarginRight].pt() != 0 )
            element.setAttribute( "right", margins[QStyleSheetItem::MarginRight].pt() );
    }

    if ( margins[QStyleSheetItem::MarginTop].pt() != 0 ||
         margins[QStyleSheetItem::MarginBottom].pt() != 0 )
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        if ( margins[QStyleSheetItem::MarginTop].pt() != 0 )
            element.setAttribute( "before", margins[QStyleSheetItem::MarginTop].pt() );
        if ( margins[QStyleSheetItem::MarginBottom].pt() != 0 )
            element.setAttribute( "after", margins[QStyleSheetItem::MarginBottom].pt() );
    }

    if ( lineSpacing.pt() != 0 )
    {
        element = doc.createElement( "LINESPACING" );
        parentElem.appendChild( element );
        element.setAttribute( "value", lineSpacing.pt() );
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
    if ( counter.numbering() != Counter::NUM_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        counter.save( element );
    }
}

void KWParagLayout::setStyleName( const QString &styleName )
{
    m_styleName = styleName;
}

void KWParagLayout::setTabList( const QList<KoTabulator> *tabList )
{
    m_tabList.clear();
    QListIterator<KoTabulator> it( *tabList);
    for ( it.toFirst(); it.current(); ++it ) {
        KoTabulator *t = new KoTabulator;
        t->type = it.current()->type;
        t->mmPos = it.current()->mmPos;
        t->inchPos = it.current()->inchPos;
        t->ptPos = it.current()->ptPos;
        m_tabList.append( t );
    }
}
#include "kwtextparag.moc"
