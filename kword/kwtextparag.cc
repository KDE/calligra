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
#include "kwdoc.h"
#include "kwanchor.h"
#include "kwtextimage.h"
#include "kwtextframeset.h"
#include <koVariable.h>
#include <koparagcounter.h>
#include "kwvariable.h"
#include <klocale.h>
#include <kdebug.h>
#include <assert.h>

//#define DEBUG_FORMATTING

// Called by KoTextParag::drawParagString - all params are in pixel coordinates
void KWTextParag::drawFormattingChars( QPainter &painter, const QString & /*s*/, int start, int len,
                                       int /*startX*/, int /*lastY*/, int /*baseLine*/, int /*h*/, // in LU
                                       int /*startX_pix*/, int lastY_pix, int baseLine_pix, int /*bw*/, int h_pix, // in pixels
                                       bool drawSelections,
                                       KoTextFormat *lastFormat, int /*i*/, const QMemArray<int> &selectionStarts,
                                       const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft )
{
    KWTextFrameSet * textfs = kwTextDocument()->textFrameSet();
    if ( textfs )
    {
        bool forPrint = ( painter.device()->devType() == QInternal::Printer );
        KWDocument * doc = textfs->kWordDocument();
        KoZoomHandler * zh = kwTextDocument()->paintingZoomHandler();
        if ( doc && doc->viewFormattingChars() && !forPrint )
        {
            painter.save();
            QPen pen( cg.color( QColorGroup::Highlight ) );
            painter.setPen( pen );
            //kdDebug() << "KWTextParag::drawFormattingChars start=" << start << " len=" << len << " length=" << length() << endl;
            if ( start + len == length() )
            {
                if ( hardFrameBreakAfter() )
                {
                    // keep in sync with KWTextFrameSet::formatVertically
                    QString str = i18n( "--- Frame Break ---" );
                    int width = 0;
                    //for ( int i = 0 ; i < (int)str.length() ; ++i )
                        // width += ...
                    width = lastFormat->screenFontMetrics(zh).width(str);
                    QColorGroup cg2( cg );
                    //cg2.setColor( QColorGroup::Base, Qt::green ); // for debug
                    int last = length() - 1;
                    KoTextStringChar &ch = string()->at( last );
                    int x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj;

                    KoTextFormat format( *lastFormat );
                    format.setColor( pen.color() ); // ### A bit slow, maybe pass the color to drawParagStringInternal ?
                    KoTextParag::drawParagStringInternal(
                        painter, str, 0, str.length(),
                        x, lastY_pix, // startX and lastY
                        zh->layoutUnitToPixelY( ch.ascent() ), // baseline
                        width, zh->layoutUnitToPixelY( ch.height() ), // bw and h
                        drawSelections, &format, last, selectionStarts,
                        selectionEnds, cg2, rightToLeft, zh );
                }
                else
                {
                    // drawing the end of the parag
                    KoTextStringChar &ch = string()->at( length() - 1 );
                    KoTextFormat* format = static_cast<KoTextFormat *>( ch.format() );
                    int w = format->screenFontMetrics(zh).width('X');
                    int size = QMIN( w, h_pix * 3 / 4 );
                    // x,y is the bottom right corner of the ¶
                    //kdDebug() << "startX=" << startX << " bw=" << bw << " w=" << w << endl;
                    int x;
                    if ( rightToLeft )
                        x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj + ch.pixelwidth - 1;
                    else
                        x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj + w;
                    int y = lastY_pix + baseLine_pix;
                    //kdDebug() << "KWTextParag::drawFormattingChars drawing CR at " << x << "," << y << endl;
                    painter.drawLine( (int)(x - size * 0.2), y - size, (int)(x - size * 0.2), y );
                    painter.drawLine( (int)(x - size * 0.5), y - size, (int)(x - size * 0.5), y );
                    painter.drawLine( x, y, (int)(x - size * 0.7), y );
                    painter.drawLine( x, y - size, (int)(x - size * 0.5), y - size);
                    painter.drawArc( x - size, y - size, size, (int)(size / 2), -90*16, -180*16 );
#ifdef DEBUG_FORMATTING
                    painter.setPen( Qt::blue );
                    painter.drawRect( zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj - 1, lastY_pix, ch.pixelwidth, baseLine_pix );
                    QPen pen( cg.color( QColorGroup::Highlight ) );
                    painter.setPen( pen );
#endif
                }
            }

            // Now draw spaces and tabs
            int end = QMIN( start + len, length() - 1 ); // don't look at the trailing space
            for ( int i = start ; i < end ; ++i )
            {
                KoTextStringChar &ch = string()->at(i);
#ifdef DEBUG_FORMATTING
                painter.setPen( (i % 2)? Qt::red: Qt::green );
                painter.drawRect( zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj - 1, lastY_pix, ch.pixelwidth, baseLine_pix );
                QPen pen( cg.color( QColorGroup::Highlight ) );
                painter.setPen( pen );
#endif
                if ( ch.isCustom() )
                    continue;
                if ( ch.c == ' ' )
                {
                    // Don't use ch.pixelwidth here. We want a square with
                    // the same size for all spaces, even the justified ones
                    //int w = zh->layoutUnitToPixelX( string()->width(i) );
                    int w = zh->layoutUnitToPixelX( ch.format()->width( ' ' ) );
                    int height = zh->layoutUnitToPixelY( ch.ascent() );
                    int size = QMAX( 2, QMIN( w/2, height/3 ) ); // Enfore that it's a square, and that it's visible
                    int x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj;
                    painter.drawRect( x + (ch.pixelwidth - size) / 2, lastY_pix + baseLine_pix - (height - size) / 2, size, size );
                }
                else if ( ch.c == '\t' )
                {
                    /*KoTextStringChar &nextch = string()->at(i+1);
                    int nextx = (nextch.x > ch.x) ? nextch.x : rect().width();
                    //kdDebug() << "tab x=" << ch.x << " nextch.x=" << nextch.x
                    //          << " nextx=" << nextx << " startX=" << startX << " bw=" << bw << endl;
                    int availWidth = nextx - ch.x - 1;
                    availWidth=zh->layoutUnitToPixelX(availWidth);*/

                    int availWidth = ch.pixelwidth;

                    KoTextFormat* format = ch.format();
                    int x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj + availWidth / 2;
                    int size = QMIN( availWidth,format->screenFontMetrics(zh).width('W')) / 2 ; // actually the half size
                    int y = lastY_pix + baseLine_pix - zh->layoutUnitToPixelY( ch.ascent()/2 );
                    int arrowsize = zh->zoomItY( 2 );
                    painter.drawLine( x + size, y, x - size, y );
                    if ( rightToLeft )
                    {
                        painter.drawLine( x - size, y, x - size + arrowsize, y - arrowsize );
                        painter.drawLine( x - size, y, x - size + arrowsize, y + arrowsize );
                    }
                    else
                    {
                        painter.drawLine( x + size, y, x + size - arrowsize, y - arrowsize );
                        painter.drawLine( x + size, y, x + size - arrowsize, y + arrowsize );
                    }
                }
                else if ( ch.c == '\n' )
                {
                    // draw line break
                    KoTextFormat* format = static_cast<KoTextFormat *>( ch.format() );
                    int w = format->screenFontMetrics(zh).width('X');
                    int size = QMIN( w, h_pix * 3 / 4 );
                    int arrowsize = zh->zoomItY( 2 );
                    // x,y is the bottom right corner of the reversed L
                    //kdDebug() << "startX=" << startX << " bw=" << bw << " w=" << w << endl;
                    int y = lastY_pix + baseLine_pix - arrowsize;
                    //kdDebug() << "KWTextParag::drawFormattingChars drawing Line Break at " << x << "," << y << endl;
                    if ( rightToLeft )
                    {
                        int x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj + ch.pixelwidth - 1;
                        painter.drawLine( x - size, y - size, x - size, y );
                        painter.drawLine( x - size, y, (int)(x - size * 0.3), y );
                        // Now the arrow
                        painter.drawLine( (int)(x - size * 0.3), y, (int)(x - size * 0.3 - arrowsize), y - arrowsize );
                        painter.drawLine( (int)(x - size * 0.3), y, (int)(x - size * 0.3 - arrowsize), y + arrowsize );
                    }
                    else
                    {
                        int x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj + w - 1;
                        painter.drawLine( x, y - size, x, y );
                        painter.drawLine( x, y, (int)(x - size * 0.7), y );
                        // Now the arrow
                        painter.drawLine( (int)(x - size * 0.7), y, (int)(x - size * 0.7 + arrowsize), y - arrowsize );
                        painter.drawLine( (int)(x - size * 0.7), y, (int)(x - size * 0.7 + arrowsize), y + arrowsize );
                    }
                }
            }
            painter.restore();
        }
    }
}

void KWTextParag::setPageBreaking( int pb )
{
    m_layout.pageBreaking = pb;
    invalidate(0);
    if ( next() && ( pb & KoParagLayout::HardFrameBreakAfter ) )
        next()->invalidate(0);
}

KWTextDocument * KWTextParag::kwTextDocument() const
{
    return static_cast<KWTextDocument *>( document() );
}

//static
QDomElement KWTextParag::saveFormat( QDomDocument & doc, KoTextFormat * curFormat, KoTextFormat * refFormat, int pos, int len )
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
        int size = static_cast<int>( KoTextZoomHandler::layoutUnitPtToPt( curFormat->font().pointSize() ) );
        elem.setAttribute( "value", size );
    }
    if( !refFormat || curFormat->font().italic() != refFormat->font().italic() )
    {
        elem = doc.createElement( "ITALIC" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->font().italic()) );
    }
    if( !refFormat
        || curFormat->underlineLineType() != refFormat->underlineLineType()
        || curFormat->textUnderlineColor() !=refFormat->textUnderlineColor()
        || curFormat->underlineLineStyle() !=refFormat->underlineLineStyle())
    {
        if ( curFormat->underlineLineType()!= KoTextFormat::U_NONE )
        {
            elem = doc.createElement( "UNDERLINE" );
            formatElem.appendChild( elem );
            if ( curFormat->doubleUnderline() )
                elem.setAttribute( "value", "double" );
            else if ( curFormat->underlineLineType() == KoTextFormat::U_SIMPLE_BOLD)
                elem.setAttribute( "value", "single-bold" );
            else
                elem.setAttribute( "value", static_cast<int>(curFormat->underline()) );
            QString strLineType=underlineStyleToString( curFormat->underlineLineStyle() );
            elem.setAttribute( "styleline", strLineType );
            if ( curFormat->textUnderlineColor().isValid() )
                elem.setAttribute( "underlinecolor", curFormat->textUnderlineColor().name() );
        }
    }
    if( !refFormat
        || curFormat->strikeOutLineType() != refFormat->strikeOutLineType()
        || curFormat->strikeOutLineStyle()!= refFormat->strikeOutLineStyle())
    {
        if ( curFormat->strikeOutLineType()!= KoTextFormat::S_NONE )
        {
            elem = doc.createElement( "STRIKEOUT" );
            formatElem.appendChild( elem );
            if ( curFormat->doubleStrikeOut() )
                elem.setAttribute( "value", "double" );
            else
                elem.setAttribute( "value", static_cast<int>(curFormat->strikeOut()) );
            QString strLineType=strikeOutStyleToString( curFormat->strikeOutLineStyle() );
            elem.setAttribute( "styleline", strLineType );
        }
    }
    // ######## Not needed in 3.0?
    /*
    if( !refFormat || curFormat->font().charSet() != refFormat->font().charSet() )
    {
        elem = doc.createElement( "CHARSET" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->font().charSet()) );
    }
    */
    if( !refFormat || curFormat->vAlign() != refFormat->vAlign() )
    {
        elem = doc.createElement( "VERTALIGN" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->vAlign()) );
    }
    if( !refFormat || curFormat->textBackgroundColor() != refFormat->textBackgroundColor())
        if ( curFormat->textBackgroundColor().isValid() )
        {
            elem = doc.createElement( "TEXTBACKGROUNDCOLOR" );
            formatElem.appendChild( elem );
            elem.setAttribute( "red", curFormat->textBackgroundColor().red() );
            elem.setAttribute( "green", curFormat->textBackgroundColor().green() );
            elem.setAttribute( "blue", curFormat->textBackgroundColor().blue() );
        }

    return formatElem;
}

void KWTextParag::save( QDomElement &parentElem, bool saveAnchorsFramesets )
{
    // The QMAX below ensures that although we don't save the trailing space
    // in the normal case, we do save it for empty paragraphs (#30336)
    save( parentElem, 0, QMAX( 0, length()-2 ), saveAnchorsFramesets );
}

void KWTextParag::save( QDomElement &parentElem, int from /* default 0 */,
                        int to /* default length()-2 */,
                        bool saveAnchorsFramesets /* default false */ )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement paragElem = doc.createElement( "PARAGRAPH" );
    parentElem.appendChild( paragElem );
    QDomElement textElem = doc.createElement( "TEXT" );
    textElem.setAttribute("xml:space", "preserve");
    paragElem.appendChild( textElem );
    QString text = string()->toString();
    Q_ASSERT( text.right(1)[0] == ' ' );
    textElem.appendChild( doc.createTextNode( text.mid( from, to - from + 1 ) ) );

    QDomElement formatsElem = doc.createElement( "FORMATS" );
    int startPos = -1;
    int index = 0; // Usually same as 'i' but if from>0, 'i' indexes the parag's text and this one indexes the output
    KoTextFormat *curFormat = paragraphFormat();
    for ( int i = from; i <= to; ++i, ++index )
    {
        KoTextStringChar & ch = string()->at(i);
        KoTextFormat * newFormat = static_cast<KoTextFormat *>( ch.format() );
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
            KoTextCustomItem* customItem = ch.customItem();
            formatElem.setAttribute( "id", customItem->typeId() );
            customItem->save( formatElem );
            startPos = -1;
            curFormat = paragraphFormat();
            // Save the contents of the frameset inside the anchor
            // This is NOT used when saving, but it is used when copying an inline frame
            if ( saveAnchorsFramesets )
            {
                KWFrameSet* inlineFs = 0L;
                if ( dynamic_cast<KWAnchor *>( customItem )  )
                    inlineFs = static_cast<KWAnchor *>( customItem )->frameSet();
                else if ( dynamic_cast<KWFootNoteVariable *>( customItem ) )
                    inlineFs = static_cast<KWFootNoteVariable *>( customItem )->frameSet();

                if ( inlineFs )
                {
                    // Save inline framesets at the toplevel. Necessary when copying a textframeset that
                    // itself includes an inline frameset - we want all frameset tags at the toplevel.
                    QDomElement elem = doc.documentElement();
                    kdDebug() << " saving into " << elem.tagName() << endl;
                    inlineFs->toXML( elem );
                }
            }
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

    saveParagLayout( m_layout, layoutElem );

    // Paragraph's format
    // ## Maybe we should have a "default format" somewhere and
    // pass it instead of 0L, to only save the non-default attributes
    QDomElement paragFormatElement = saveFormat( doc, paragraphFormat(), 0L, 0, to - from + 1 );
    layoutElem.appendChild( paragFormatElement );
}

//static
KoTextFormat KWTextParag::loadFormat( QDomElement &formatElem, KoTextFormat * refFormat, const QFont & defaultFont )
{
    KoTextFormat format;
    QFont font;
    if ( refFormat )
    {
        format = *refFormat;
        format.setCollection( 0 ); // Out of collection copy
        font = format.font();
    }
    else
    {
        font = defaultFont;
    }

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
    {
        double size = elem.attribute("value").toDouble();
        font.setPointSizeFloat( KoTextZoomHandler::ptToLayoutUnitPt( size ) );
    }
    elem = formatElem.namedItem( "ITALIC" ).toElement();
    if ( !elem.isNull() )
        font.setItalic( elem.attribute("value").toInt() == 1 );
    elem = formatElem.namedItem( "UNDERLINE" ).toElement();
    if ( !elem.isNull() ) {
        QString value = elem.attribute("value");
        if ( value == "0" || value == "1" )
            format.setUnderlineLineType( (value.toInt() == 1)?KoTextFormat::U_SIMPLE: KoTextFormat::U_NONE );
        else if ( value == "single" ) // value never used when saving, but why not support it? ;)
            format.setUnderlineLineType ( KoTextFormat::U_SIMPLE);
        else if ( value == "double" )
            format.setUnderlineLineType ( KoTextFormat::U_DOUBLE);
        else if ( value == "single-bold" )
            format.setUnderlineLineType ( KoTextFormat::U_SIMPLE_BOLD);
        if ( elem.hasAttribute("styleline" ))
        {
            QString strLineType = elem.attribute("styleline");
            format.setUnderlineLineStyle( stringToUnderlineStyle( strLineType ));
        }
        if ( elem.hasAttribute("underlinecolor"))
        {
            QColor col( QColor(elem.attribute("underlinecolor")));
            format.setTextUnderlineColor( col );
        }
    }
    elem = formatElem.namedItem( "STRIKEOUT" ).toElement();
    if ( !elem.isNull() )
    {
        QString value = elem.attribute("value");
        if ( value == "0" || value == "1" )
            format.setStrikeOutLineType( (value.toInt() == 1)?KoTextFormat::S_SIMPLE: KoTextFormat::S_NONE );
        else if ( value == "single" ) // value never used when saving, but why not support it? ;)
            format.setStrikeOutLineType ( KoTextFormat::S_SIMPLE);
        else if ( value == "double" )
            format.setStrikeOutLineType ( KoTextFormat::S_DOUBLE);
        if ( elem.hasAttribute("styleline" ))
        {
            QString strLineType = elem.attribute("styleline");
            format.setStrikeOutLineStyle( stringToStrikeOutStyle( strLineType ));
        }

    }
    // ######## Not needed in 3.0?
    /*
    elem = formatElem.namedItem( "CHARSET" ).toElement();
    if ( !elem.isNull() )
        font.setCharSet( (QFont::CharSet) elem.attribute("value").toInt() );
    */
    format.setFont( font );

    elem = formatElem.namedItem( "VERTALIGN" ).toElement();
    if ( !elem.isNull() )
        format.setVAlign( static_cast<KoTextFormat::VerticalAlignment>( elem.attribute("value").toInt() ) );
    elem = formatElem.namedItem( "COLOR" ).toElement();
    if ( !elem.isNull() )
    {
        QColor col( elem.attribute("red").toInt(),
                    elem.attribute("green").toInt(),
                    elem.attribute("blue").toInt() );
        format.setColor( col );
    }
    elem = formatElem.namedItem( "TEXTBACKGROUNDCOLOR" ).toElement();
    if ( !elem.isNull() )
    {
        QColor col( elem.attribute("red").toInt(),
                    elem.attribute("green").toInt(),
                    elem.attribute("blue").toInt() );
        format.setTextBackgroundColor( col );
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
        KoParagLayout paragLayout = loadParagLayout( layout, doc, true );
        setParagLayout( paragLayout );

        // Load default format from style.
        KoTextFormat *defaultFormat = style() ? &style()->format() : 0L;
        QDomElement formatElem = layout.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
        {
            // Load paragraph format
            KoTextFormat f = loadFormat( formatElem, defaultFormat, doc->defaultFont() );
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

    QValueList<int> removeLenList;
    QValueList<int> removePosList;

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
                    KoTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont() );
                    //kdDebug(32002) << "KWTextParag::loadFormatting applying formatting from " << index << " to " << index+len << endl;
                    setFormat( index, len, document()->formatCollection()->format( &f ) );
                    break;
                }
                case 2: // Picture
                {
                    len = 1; // it was missing from old 1.0 files

                    // The character matching this format is probably a QChar(1)
                    // However, as it is an invalid XML character, we must replace it
                    // or it will be written out while save the file.
                    KoTextStringChar& ch = string()->at(index);
                    if (ch.c.unicode()==1)
                    {
                        kdDebug() << "Replacing QChar(1) (in KWTextParag::loadFormatting)" << endl;
                        ch.c='#';
                    }

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
                        QString key = typeElem.attribute( "key" );
                        kdDebug() << "KWTextParag::loadFormatting variable type=" << type << " key=" << key << endl;
                        KoVariableFormat * varFormat = key.isEmpty() ? 0 : doc->variableFormatCollection()->format( key.latin1() );
                        // If varFormat is 0 (no key specified), the default format will be used.
                        KoVariable * var =doc->getVariableCollection()->createVariable( type, -1, doc->variableFormatCollection(), varFormat,kwTextDocument(),doc );
                        var->load( varElem );
                        KoTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont() );
                        setCustomItem( index, var, document()->formatCollection()->format( &f ) );

                        var->recalc();
                        if(len>1) {
                            removePosList.append(index+1);
                            removeLenList.append(len-1);
                        }
                    }
                    break;
                }
                case 6: // Anchor
                {
                    Q_ASSERT( len == 1 );
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
    for(unsigned int i=0; i < removeLenList.count(); i++) {
        remove(*removePosList.at(i), *removeLenList.at(i));
    }
}

void KWTextParag::setParagLayout( const KoParagLayout & layout, int flags )
{
    KoTextParag::setParagLayout( layout, flags );

    if ( flags & KoParagLayout::PageBreaking )
        setPageBreaking( layout.pageBreaking );
}

//////////

// Create a KoParagLayout from XML.
KoParagLayout KWTextParag::loadParagLayout( QDomElement & parentElem, KWDocument *doc, bool findStyle )
{
    KoParagLayout layout;

    // Only when loading paragraphs, not when loading styles
    if ( findStyle )
    {
        KoStyle *style;
        // Name of the style. If there is no style, then we do not supply
        // any default!
        QDomElement element = parentElem.namedItem( "NAME" ).toElement();
        if ( !element.isNull() )
        {
            QString styleName = element.attribute( "value" );
            style = doc->styleCollection()->findStyle( styleName );
            if (!style)
            {
                kdError(32001) << "Cannot find style \"" << styleName << "\" specified in paragraph LAYOUT - using Standard" << endl;
                style = doc->styleCollection()->findStyle( "Standard" );
            }
            //else kdDebug() << "KoParagLayout::KoParagLayout setting style to " << style << " " << style->name() << endl;
        }
        else
        {
            kdError(32001) << "Missing NAME tag in paragraph LAYOUT - using Standard" << endl;
            style = doc->styleCollection()->findStyle( "Standard" );
        }
        Q_ASSERT(style);
        layout.style = style;
    }

    // Load the paragraph tabs - forget about the ones from the style first.
    // We can't apply the 'default comes from the style' in this case, because
    // there is no way to differenciate between "I want no tabs in the parag"
    // and "use default from style".
    //m_tabList.clear();
    KoTabulatorList tabList;
    QDomElement element = parentElem.firstChild().toElement();
    for ( ; !element.isNull() ; element = element.nextSibling().toElement() )
    {
        if ( element.tagName() == "TABULATOR" )
        {
            KoTabulator tab;
            tab.type = static_cast<KoTabulators>( KWDocument::getAttribute( element, "type", T_LEFT ) );
            tab.ptPos = KWDocument::getAttribute( element, "ptpos", 0.0 );
            tab.filling = static_cast<KoTabulatorFilling>( KWDocument::getAttribute( element, "filling", TF_BLANK ) );
            tab.ptWidth = KWDocument::getAttribute( element, "width", 0.5 );
            tabList.append( tab );
        }
    }
    qHeapSort( tabList );
    layout.setTabList( tabList );
    layout.alignment = Qt::AlignLeft;
    element = parentElem.namedItem( "FLOW" ).toElement(); // Flow is what is now called alignment internally
    if ( !element.isNull() )
    {
        QString flow = element.attribute( "align" ); // KWord-1.0 DTD
        if ( !flow.isEmpty() )
        {
            // 'left' is the default and maps to Qt::AlignAuto
            // We don't have support for forcing left-aligned RTL text, this would have
            // to be called something else, e.g. forceLeft. This is the least damaging
            // way. Saving AlignAuto as 'auto' would break the file format.
            layout.alignment = flow=="right" ? (int)Qt::AlignRight : flow=="center" ? (int)Qt::AlignCenter : flow=="justify" ? (int)Qt::AlignJustify : (int)Qt::AlignAuto;
        } else {
            flow = element.attribute( "value" ); // KWord-0.8
            static const int flow2align[] = { Qt::AlignAuto, Qt::AlignRight, Qt::AlignCenter, Qt::AlignJustify };
            layout.alignment = flow2align[flow.toInt()];
        }
    }

    if ( doc->syntaxVersion() < 2 )
    {
        element = parentElem.namedItem( "OHEAD" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginTop] = KWDocument::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "OFOOT" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginBottom] = KWDocument::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "IFIRST" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginFirstLine] = KWDocument::getAttribute( element, "pt", 0.0 );

        element = parentElem.namedItem( "ILEFT" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.margins[QStyleSheetItem::MarginLeft] = KWDocument::getAttribute( element, "pt", 0.0 );
    }

    // KWord-1.0 DTD
    element = parentElem.namedItem( "INDENTS" ).toElement();
    if ( !element.isNull() )
    {
        layout.margins[QStyleSheetItem::MarginFirstLine] = KWDocument::getAttribute( element, "first", 0.0 );
        layout.margins[QStyleSheetItem::MarginLeft] = KWDocument::getAttribute( element, "left", 0.0 );
        layout.margins[QStyleSheetItem::MarginRight] = KWDocument::getAttribute( element, "right", 0.0 );
    }
    element = parentElem.namedItem( "OFFSETS" ).toElement();
    if ( !element.isNull() )
    {
        layout.margins[QStyleSheetItem::MarginTop] = KWDocument::getAttribute( element, "before", 0.0 );
        layout.margins[QStyleSheetItem::MarginBottom] = KWDocument::getAttribute( element, "after", 0.0 );
    }

    if ( doc->syntaxVersion() < 2 )
    {
        element = parentElem.namedItem( "LINESPACE" ).toElement(); // used by KWord-0.8
        if ( !element.isNull() )
            layout.lineSpacing = KWDocument::getAttribute( element, "pt", 0.0 );
    }

    element = parentElem.namedItem( "LINESPACING" ).toElement(); // KWord-1.0 DTD
    if ( !element.isNull() )
    {
        QString value = element.attribute( "value" );
        if ( value == "oneandhalf" )
            layout.lineSpacing = KoParagLayout::LS_ONEANDHALF;
        else if ( value == "double" )
            layout.lineSpacing = KoParagLayout::LS_DOUBLE;
        else
            layout.lineSpacing = value.toDouble();
    }

    int pageBreaking = 0;
    element = parentElem.namedItem( "PAGEBREAKING" ).toElement();
    if ( !element.isNull() )
    {
        if ( element.attribute( "linesTogether" ) == "true" )
            pageBreaking |= KoParagLayout::KeepLinesTogether;
        if ( element.attribute( "hardFrameBreak" ) == "true" )
            pageBreaking |= KoParagLayout::HardFrameBreakBefore;
        if ( element.attribute( "hardFrameBreakAfter" ) == "true" )
            pageBreaking |= KoParagLayout::HardFrameBreakAfter;
    }
    if ( doc->syntaxVersion() < 2 )
    {
        element = parentElem.namedItem( "HARDBRK" ).toElement(); // KWord-0.8
        if ( !element.isNull() )
            pageBreaking |= KoParagLayout::HardFrameBreakBefore;
    }
    layout.pageBreaking = pageBreaking;

    element = parentElem.namedItem( "LEFTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.leftBorder = KoBorder::loadBorder( element );
    else
        layout.leftBorder.setPenWidth(0);

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.rightBorder = KoBorder::loadBorder( element );
    else
        layout.rightBorder.setPenWidth(0);

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        layout.topBorder = KoBorder::loadBorder( element );
    else
        layout.topBorder.setPenWidth(0);

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        layout.bottomBorder = KoBorder::loadBorder( element );
    else
        layout.bottomBorder.setPenWidth(0);

    element = parentElem.namedItem( "COUNTER" ).toElement();
    if ( !element.isNull() )
    {
        layout.counter = new KoParagCounter;
        layout.counter->load( element );
    }

    element = parentElem.namedItem( "SHADOW" ).toElement();
    if ( !element.isNull() )
    {
        layout.shadowDistance=element.attribute("distance").toInt();
        layout.shadowDirection=element.attribute("direction").toInt();
        if ( element.hasAttribute("red") )
        {
            int r = element.attribute("red").toInt();
            int g = element.attribute("green").toInt();
            int b = element.attribute("blue").toInt();
            layout.shadowColor.setRgb( r, g, b );
        }
    }
    return layout;
}

void KWTextParag::saveParagLayout( const KoParagLayout& layout, QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    if ( layout.style )
        element.setAttribute( "value", layout.style->name() );
    else
        kdWarning() << "KWTextParag::saveParagLayout: style==0L!" << endl;

    element = doc.createElement( "FLOW" );
    parentElem.appendChild( element );
    int a = layout.alignment;
    element.setAttribute( "align", a==Qt::AlignRight ? "right" : a==Qt::AlignCenter ? "center" : a==Qt::AlignJustify ? "justify" : "left" );

    if ( layout.margins[QStyleSheetItem::MarginFirstLine] != 0 ||
         layout.margins[QStyleSheetItem::MarginLeft] != 0 ||
         layout.margins[QStyleSheetItem::MarginRight] != 0 )
    {
        element = doc.createElement( "INDENTS" );
        parentElem.appendChild( element );
        if ( layout.margins[QStyleSheetItem::MarginFirstLine] != 0 )
            element.setAttribute( "first", layout.margins[QStyleSheetItem::MarginFirstLine] );
        if ( layout.margins[QStyleSheetItem::MarginLeft] != 0 )
            element.setAttribute( "left", layout.margins[QStyleSheetItem::MarginLeft] );
        if ( layout.margins[QStyleSheetItem::MarginRight] != 0 )
            element.setAttribute( "right", layout.margins[QStyleSheetItem::MarginRight] );
    }

    if ( layout.margins[QStyleSheetItem::MarginTop] != 0 ||
         layout.margins[QStyleSheetItem::MarginBottom] != 0 )
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        if ( layout.margins[QStyleSheetItem::MarginTop] != 0 )
            element.setAttribute( "before", layout.margins[QStyleSheetItem::MarginTop] );
        if ( layout.margins[QStyleSheetItem::MarginBottom] != 0 )
            element.setAttribute( "after", layout.margins[QStyleSheetItem::MarginBottom] );
    }

    if ( layout.lineSpacing != 0 )
    {
        element = doc.createElement( "LINESPACING" );
        parentElem.appendChild( element );
        if ( layout.lineSpacing == KoParagLayout::LS_ONEANDHALF )
            element.setAttribute( "value", "oneandhalf" );
        else if ( layout.lineSpacing == KoParagLayout::LS_DOUBLE )
            element.setAttribute( "value", "double" );
        else
            element.setAttribute( "value", layout.lineSpacing );
    }

    if ( layout.pageBreaking != 0 )
    {
        element = doc.createElement( "PAGEBREAKING" );
        parentElem.appendChild( element );
        if ( layout.pageBreaking & KoParagLayout::KeepLinesTogether )
            element.setAttribute( "linesTogether",  "true" );
        if ( layout.pageBreaking & KoParagLayout::HardFrameBreakBefore )
            element.setAttribute( "hardFrameBreak", "true" );
        if ( layout.pageBreaking & KoParagLayout::HardFrameBreakAfter )
            element.setAttribute( "hardFrameBreakAfter", "true" );
    }

    if ( layout.leftBorder.penWidth() > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        layout.leftBorder.save( element );
    }
    if ( layout.rightBorder.penWidth() > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        layout.rightBorder.save( element );
    }
    if ( layout.topBorder.penWidth() > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        layout.topBorder.save( element );
    }
    if ( layout.bottomBorder.penWidth() > 0 )
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        layout.bottomBorder.save( element );
    }
    if ( layout.counter && layout.counter->numbering() != KoParagCounter::NUM_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        if ( layout.counter )
            layout.counter->save( element );
    }

    KoTabulatorList tabList = layout.tabList();
    KoTabulatorList::Iterator it = tabList.begin();
    for ( ; it != tabList.end() ; it++ )
    {
        element = doc.createElement( "TABULATOR" );
        parentElem.appendChild( element );
        element.setAttribute( "type", (*it).type );
        element.setAttribute( "ptpos", (*it).ptPos );
        element.setAttribute( "filling", (*it).filling );
        element.setAttribute( "width", (*it).ptWidth );
    }
    if(layout.shadowDistance!=0 || layout.shadowDirection!=KoParagLayout::SD_RIGHT_BOTTOM)
    {
        element = doc.createElement( "SHADOW" );
        parentElem.appendChild( element );
        element.setAttribute( "distance", layout.shadowDistance );
        element.setAttribute( "direction", layout.shadowDirection );
        if (layout.shadowColor.isValid())
        {
            element.setAttribute("red", layout.shadowColor.red());
            element.setAttribute("green", layout.shadowColor.green());
            element.setAttribute("blue", layout.shadowColor.blue());
        }
    }
}

void KWTextParag::join( KoTextParag *parag )
{
    m_layout.pageBreaking &= ~(KoParagLayout::HardFrameBreakBefore|KoParagLayout::HardFrameBreakAfter);
    KoTextParag::join( parag );
}

QString KWTextParag::underlineStyleToString( KoTextFormat::UnderlineLineStyle _lineType )
{
    QString strLineType;
    switch ( _lineType )
    {
    case KoTextFormat::U_SOLID:
        strLineType ="solid";
        break;
    case KoTextFormat::U_DASH:
        strLineType ="dash";
        break;
    case KoTextFormat::U_DOT:
        strLineType ="dot";
        break;
    case KoTextFormat::U_DASH_DOT:
        strLineType="dashdot";
        break;
    case KoTextFormat::U_DASH_DOT_DOT:
        strLineType="dashdotdot";
        break;
    }
    return strLineType;
}

QString KWTextParag::strikeOutStyleToString( KoTextFormat::StrikeOutLineStyle _lineType )
{
    QString strLineType;
    switch ( _lineType )
    {
    case KoTextFormat::S_SOLID:
        strLineType ="solid";
        break;
    case KoTextFormat::S_DASH:
        strLineType ="dash";
        break;
    case KoTextFormat::S_DOT:
        strLineType ="dot";
        break;
    case KoTextFormat::S_DASH_DOT:
        strLineType="dashdot";
        break;
    case KoTextFormat::S_DASH_DOT_DOT:
        strLineType="dashdotdot";
        break;
    }
    return strLineType;
}

KoTextFormat::UnderlineLineStyle KWTextParag::stringToUnderlineStyle( const QString & _str )
{
    if ( _str =="solid")
        return KoTextFormat::U_SOLID;
    else if ( _str =="dash" )
        return KoTextFormat::U_DASH;
    else if ( _str =="dot" )
        return KoTextFormat::U_DOT;
    else if ( _str =="dashdot")
        return KoTextFormat::U_DASH_DOT;
    else if ( _str=="dashdotdot")
        return KoTextFormat::U_DASH_DOT_DOT;
    else
        return KoTextFormat::U_SOLID;
}

KoTextFormat::StrikeOutLineStyle KWTextParag::stringToStrikeOutStyle( const QString & _str )
{
    if ( _str =="solid")
        return KoTextFormat::S_SOLID;
    else if ( _str =="dash" )
        return KoTextFormat::S_DASH;
    else if ( _str =="dot" )
        return KoTextFormat::S_DOT;
    else if ( _str =="dashdot")
        return KoTextFormat::S_DASH_DOT;
    else if ( _str=="dashdotdot")
        return KoTextFormat::S_DASH_DOT_DOT;
    else
        return KoTextFormat::S_SOLID;
}
