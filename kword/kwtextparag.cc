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

KWTextParag::KWTextParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds)
    : QTextParag( d, pr, nx, updateIds )
{
    //qDebug("KWTextParag::KWTextParag %p", this);
    m_item = 0L;
    m_leftBorder.ptWidth = 0;
    m_rightBorder.ptWidth = 0;
    m_topBorder.ptWidth = 0;
    m_bottomBorder.ptWidth = 0;
    m_counter = 0L;
}

KWTextParag::~KWTextParag()
{
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

void KWTextParag::setNoCounter()
{
    delete m_counter;
    m_counter = 0L;
    invalidateCounters();
}

void KWTextParag::setCounter( const Counter & counter )
{
    //kdDebug() << "KWTextParag::setCounter " << counter.counterType << endl;
    if ( counter.counterType == Counter::CT_NONE )
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

        int counterMargin = 16;
	QString tmpCounter;
        // TODO make this width-determination better (and more complete)
        if ( counter.counterType == Counter::CT_ALPHAB_L ||
             counter.counterType == Counter::CT_ALPHAB_U ||
             counter.counterType == Counter::CT_ROM_NUM_L ||
             counter.counterType == Counter::CT_ROM_NUM_U ||
             counter.counterType == Counter::CT_NUM ||
	     counter.counterType == Counter::CT_CUSTOMBULLET )
	  {
	    tmpCounter = m_counter->counterLeftText +'.' + m_counter->counterRightText + ' ';
	    for (unsigned int i=0;i<tmpCounter.length();i++)
	      counterMargin +=paragFormat()->width(tmpCounter,i);
	    if (counter.counterType == Counter::CT_ALPHAB_L)
	      counterMargin+=paragFormat()->width( 'a' )*m_counter->counterDepth;
	    else if (counter.counterType == Counter::CT_ALPHAB_U)
	      counterMargin+=paragFormat()->width( 'A' )*m_counter->counterDepth;
	    else if (counter.counterType == Counter::CT_ROM_NUM_L)
	      counterMargin+=paragFormat()->width( 'i' )*m_counter->counterDepth;
	    else if (counter.counterType == Counter::CT_ROM_NUM_U)
	      counterMargin+=paragFormat()->width( 'I' )*m_counter->counterDepth;
	    else if (counter.counterType == Counter::CT_NUM)
	      counterMargin+=paragFormat()->width( '1' )*m_counter->counterDepth;

	  }
        m_counter->counterMargin = counterMargin;
        invalidateCounters();
    }
}

void KWTextParag::invalidateCounters()
{
    // Invalidate this paragraph and all the following ones
    // (Numbering may have changed)
    invalidate( 0 );
    if ( m_counter )
        m_counter->numSubParag = -1;
    KWTextParag *s = static_cast<KWTextParag *>(next());
    while ( s ) {
        if ( s->m_counter )
            s->m_counter->numSubParag = -1;
        s->invalidate( 0 );
        s = static_cast<KWTextParag *>(s->next());
    }
}

int KWTextParag::numberOfSubParagraph() const
{
    if ( !m_counter ) // should never happen
        return 0;
    if ( m_counter->numSubParag != -1 ) // cached value
        return m_counter->numSubParag;
    int n = m_counter->startCounter;
    KWTextParag *p = static_cast<KWTextParag*>(prev());
    // Go up while being in the same list/counter, and over more nested lists/counters
    while ( p && p->counter() && (
        ( m_counter->counterType == p->counter()->counterType && m_counter->counterDepth == p->counter()->counterDepth )
        || ( m_counter->counterDepth < p->counter()->counterDepth ) ) )
    {
        if ( m_counter->counterDepth == p->counter()->counterDepth )
            ++n;
        p = static_cast<KWTextParag *>(p->prev());
    }
    const_cast<KWTextParag*>(this)->m_counter->numSubParag = n;
    return n;
}

void KWTextParag::drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg )
{
    if ( !m_counter ) // shouldn't happen
        return;
    QRect r ( x, y, w, h );

    QFont oldFont = p->font();
    QFont newFont = paragFormat()->font();
    // ### Problem is, the paragFormat never changes. It should probably
    // change when we change the format of the whole paragraph ?
    p->setFont( newFont );
    QFontMetrics fm( p->fontMetrics() );
    int size = fm.lineSpacing() / 3;

    switch(m_counter->counterType)
    {
    case Counter::CT_NONE: // shouldn't happen
        break;
    case Counter::CT_DISCBULLET:
        {
            // Code from QStyleSheetItem::ListDisc
            p->setBrush( cg.brush( QColorGroup::Foreground ));
            QRect er( r.right()-size*2, r.top() + base - fm.boundingRect( 'A' ).height()/2 - size/2 - 1, size, size);
            p->drawEllipse( er );
            p->setBrush( Qt::NoBrush );
        }
        break;
    case Counter::CT_SQUAREBULLET:
        {
            // Code from QStyleSheetItem::ListSquare
            QRect er( r.right()-size*2, r.top() + base - fm.boundingRect( 'A' ).height()/2 - size/2 - 1, size, size );
            p->fillRect( er , cg.brush( QColorGroup::Foreground ) );
        }
        break;
    case Counter::CT_CIRCLEBULLET:
        {
            // Code from QStyleSheetItem::ListCircle
            QRect er( r.right()-size*2, r.top() + base - fm.boundingRect( 'A' ).height()/2 - size/2 - 1, size, size);
            p->drawEllipse( er );
        }
        break;
    case Counter::CT_CUSTOMBULLET:
        {
            // The user has selected a symbol from a special font. Override the paragraph font with the
            // given family. This conserves the right size etc.
            if ( !m_counter->bulletFont.isEmpty() )
            {
                newFont.setFamily( m_counter->bulletFont );
                p->setFont( newFont );
            }
            QString l = m_counter->counterBullet;
            l += QString::fromLatin1(". ");
            p->drawText( r.right() - fm.width( l ), r.top() + base, l );
        }
        break;
    case Counter::CT_ALPHAB_L:
    case Counter::CT_ALPHAB_U:
    case Counter::CT_ROM_NUM_L:
    case Counter::CT_ROM_NUM_U:
    case Counter::CT_CUSTOM:
    default: // Counter::CT_NUM

        QString l = paragraphCounterText( numberOfSubParagraph() );

        // Handle chapter-style numbering, calling paragraphCounterText() for each depth above us
        // Now... this works but it's quite slow. Some caching ought to be done.
        if ( m_counter->numberingType == Counter::NT_CHAPTER )
        {
            int depth = static_cast<int>(m_counter->counterDepth) - 1;
            KWTextParag * parag = this;
            //kdDebug() << "Looking for item with depth " << depth << endl;
            for ( ; depth >= 0 ; --depth )
            {
                bool bFound = false;
                for ( ; parag && !bFound ; parag = static_cast<KWTextParag *>(parag->prev()) )
                {
                    const Counter * c = parag->counter();
                    if ( c && c->counterDepth == static_cast<unsigned int>(depth) )
                    {
                        l.prepend( parag->paragraphCounterText( parag->numberOfSubParagraph() ) + '.' );
                        bFound = true;
                    }
                    if ( c && c->counterDepth < static_cast<unsigned int>(depth) )
                        break; // Item's depth is too small, abort here
                }
                if ( !bFound ) // Hmm, strange case. Depth-2 item without a depth-1 parent...
                {
                    kdDebug() << "No item found with depth=" << depth << " above " << richText() << endl;
                    l.prepend( paragraphCounterText( 1 ) + '.' ); // hackish
                }
            }
        }
        l = m_counter->counterLeftText + l + m_counter->counterRightText + ' ';
        p->drawText( r.right() - fm.width( l ), r.top() + base, l );
        break;
    }
    p->setFont( oldFont );
}

QString KWTextParag::paragraphCounterText( int n ) const
{
    QString l;
    switch ( m_counter->counterType )
    {
        case Counter::CT_ALPHAB_L:
            if ( n < 27 )
                l = QChar( ('a' + (char) (n-1)));
            else if(n <= (26*27))
	      {
	        l = QChar( ('a' + (char) (((n-1)/26)-1)));
           	l += QChar( ('a' + (char) ((n-1)%26)));
	      }
	      else
                l.setNum( n );
            break;
        case Counter::CT_ALPHAB_U:
            if ( n < 27 )
                l = QChar( ('A' + (char) (n-1)));
            else if(n <= (26*27) )
	      {
		l = QChar( ('A' + (char) (((n-1)/26)-1)));
		l += QChar( ('A' + (char) ((n-1)%26)));
	      }
	    else
                l.setNum( n );
            break;
        case Counter::CT_ROM_NUM_L:
            l = makeRomanNumber( n ).lower();
            break;
        case Counter::CT_ROM_NUM_U:
            l = makeRomanNumber( n ).upper();
            break;
        case Counter::CT_CUSTOM:
            ////// TODO
        default:  //CT_NUM
            l.setNum( n );
            break;
    }
    return l;
}

int KWTextParag::topMargin() const
{
    return static_cast<int>(m_margins[ QStyleSheetItem::MarginTop ].pt()
                            + m_topBorder.ptWidth); // TODO zoom
}

int KWTextParag::bottomMargin() const
{
    return static_cast<int>(m_margins[ QStyleSheetItem::MarginBottom ].pt()
                            + (int)m_bottomBorder.ptWidth);
}

int KWTextParag::leftMargin() const
{
    return static_cast<int>(m_margins[ QStyleSheetItem::MarginLeft ].pt()
                            + (int)m_leftBorder.ptWidth
                            + ( m_counter ? m_counter->counterMargin : 0 ));
}

int KWTextParag::rightMargin() const
{
    return static_cast<int>(m_margins[ QStyleSheetItem::MarginRight ].pt()
                            + (int)m_rightBorder.ptWidth);
}

int KWTextParag::firstLineMargin() const
{
    return static_cast<int>(m_margins[ QStyleSheetItem::MarginFirstLine ].pt());
}

int KWTextParag::lineSpacing() const
{
    return static_cast<int>(m_lineSpacing.pt());
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
            painter.setPen( Border::borderPen( m_topBorder ) );
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
            setFormat( &newStyle->format() );
            string()->setFormat( 0, &newStyle->format(), true ); // prepare format for text insertion
            styleApplied = true;
        }
    }
    // No "following style" setting, or same style -> copy layout & format of previous paragraph
    if (!styleApplied)
    {
        setParagLayout( parag->createParagLayout() );
        setFormat( parag->paragFormat() );
        string()->setFormat( 0, parag->paragFormat(), true ); // prepare format for text insertion
    }

    // Note: we don't call QTextParag::copyParagData on purpose.
    // We don't want setListStyle to get called - it ruins our stylesheetitems
    // And we don't care about copying the stylesheetitems directly,
    // applying the parag layout will create them
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
#if 0
    if( !refFormat ||  ... )
    {
        elem = doc.createElement( "ANCHOR" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", ... );
    }
#endif
    return formatElem;
}

void KWTextParag::save( QDomElement &parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement paragElem = doc.createElement( "PARAGRAPH" );
    parentElem.appendChild( paragElem );
    QDomElement textElem = doc.createElement( "TEXT" );
    paragElem.appendChild( textElem );
    textElem.appendChild( doc.createTextNode( string()->toString() ) );

    QDomElement formatsElem = doc.createElement( "FORMATS" );
    int startPos = -1;
    QTextFormat *curFormat = paragFormat();
    for ( int i = 0; i < string()->length(); ++i )
    {
        QTextFormat * newFormat = string()->at(i).format();
        if ( newFormat != curFormat )
        {
            // Format changed.
            if ( startPos > 0 && curFormat) { // Save former format
                QDomElement formatElem = saveFormat( doc, curFormat, paragFormat(), startPos, i-startPos );
                if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
                    formatsElem.appendChild( formatElem );
            }

            // Format different from paragraph's format ?
            if( newFormat != paragFormat() )
            {
                startPos = i;
                curFormat = newFormat;
            }
            else
            {
                startPos = -1;
                curFormat = paragFormat();
            }
        }
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
    QDomElement paragFormatElement = saveFormat( doc, paragFormat(), 0L, 0, string()->length() );
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

void KWTextParag::load( QDomElement &attributes )
{
    QString value;
    QDomElement element;

    QDomElement layout = attributes.namedItem( "LAYOUT" ).toElement();
    if ( !layout.isNull() )
    {
        KWParagLayout paragLayout( layout );
        setParagLayout( paragLayout );

        QDomElement formatElem = layout.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
        {
            // Load default format and insert/find it in the collection
            QTextFormat f = loadFormat( formatElem, 0L );
            QTextFormat * defaultFormat = document()->formatCollection()->format( &f );
            setFormat( defaultFormat );
        }
    }

    // Set the text after setting the paragraph format - so that the format applies
    element = attributes.namedItem( "TEXT" ).toElement();
    if ( !element.isNull() )
    {
        append( element.text() );
        // Apply default format - this should be automatic !!
        setFormat( 0, string()->length(), paragFormat(), TRUE );
    }

    QDomElement formatsElem = attributes.namedItem( "FORMATS" ).toElement();
    if ( !formatsElem.isNull() )
    {
        QDomNodeList listFormats = formatsElem.elementsByTagName( "FORMAT" );
        for (unsigned int item = 0; item < listFormats.count(); item++)
        {
            QDomElement formatElem = listFormats.item( item ).toElement();
            QTextFormat f = loadFormat( formatElem, paragFormat() );

            int id = formatElem.attribute( "id" ).toInt();
            if ( id != 1 )
                kdWarning() << "KWTextParag::loadFormat id=" << id << " should be 1" << endl;

            int pos = formatElem.attribute( "pos" ).toInt();
            int len = formatElem.attribute( "len" ).toInt();
            setFormat( pos, len, &f );
        }
    }
}

KWParagLayout KWTextParag::createParagLayout() const
{
    KWParagLayout l;

    // From QTextParag
    l.alignment = alignment();

    // From KWTextParag
    for ( int i = 0 ; i < 5 ; ++i )
        l.margins[i] = m_margins[i];
    l.leftBorder = m_leftBorder;
    l.rightBorder = m_rightBorder;
    l.topBorder = m_topBorder;
    l.bottomBorder = m_bottomBorder;
    if ( m_counter )
        l.counter = *m_counter;
    l.lineSpacing = m_lineSpacing;
    l.styleName = m_styleName;

    return l;
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
    // Don't call setStyle from here, it would overwrite any paragraph-specific settings
    m_styleName = layout.styleName;
}

////

KWParagLayout::KWParagLayout( QDomElement & parentElem )
{
    // Name of the style
    QDomElement element = parentElem.namedItem("NAME").toElement();
    if ( !element.isNull() )
        styleName = element.attribute("value");

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
        counter.counterType = static_cast<Counter::CounterType>( element.attribute("type").toInt() );
        counter.counterDepth = element.attribute("depth").toInt();
        counter.counterBullet = QChar( element.attribute("bullet").toInt() );
        counter.counterLeftText = correctQString( element.attribute("lefttext") );
        counter.counterRightText = correctQString( element.attribute("righttext") );
        QString s = element.attribute("start");
        if ( s[0].isDigit() )
            counter.startCounter = s.toInt();
        else // support for very-old files
            counter.startCounter = s.lower()[0].latin1() - 'a' + 1;
        counter.numberingType = static_cast<Counter::NumType>( element.attribute("numberingtype").toInt() );
        counter.bulletFont = correctQString( element.attribute("bulletfont") );
        counter.customCounterDef = correctQString( element.attribute("customdef") );
    }
}

void KWParagLayout::save( QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    element.setAttribute( "value", styleName );

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
    if ( counter.counterType != Counter::CT_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        element.setAttribute( "type", static_cast<int>( counter.counterType ) );
        element.setAttribute( "depth", counter.counterDepth );
        element.setAttribute( "bullet", counter.counterBullet.unicode() );
        element.setAttribute( "lefttext", counter.counterLeftText );
        element.setAttribute( "righttext", counter.counterRightText );
        element.setAttribute( "start", counter.startCounter );
        element.setAttribute( "numberingtype", static_cast<int>( counter.numberingType ) );
        element.setAttribute( "bulletfont", counter.bulletFont );
        element.setAttribute( "customdef", counter.customCounterDef );
    }
}


#include "kwtextparag.moc"
