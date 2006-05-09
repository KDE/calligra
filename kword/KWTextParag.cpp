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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWTextParag.h"
#include "KWDocument.h"
#include "KWAnchor.h"
#include "KWTextImage.h"
#include "KWTextFrameSet.h"
#include "KWVariable.h"
#include "KWLoadingInfo.h"

#include <KoVariable.h>
#include <KoParagCounter.h>
#include <KoOasisContext.h>
#include <KoXmlNS.h>
#include <KoOasisStyles.h>

#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>
#include <assert.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3MemArray>

//#define DEBUG_FORMATTING
#undef S_NONE // Solaris defines it in sys/signal.h

// Called by KoTextParag::drawParagString - all params are in pixel coordinates
void KWTextParag::drawFormattingChars( QPainter &painter, int start, int len,
                                       int lastY_pix, int baseLine_pix, int h_pix, // in pixels
                                       bool drawSelections,
                                       KoTextFormat *lastFormat, const Q3MemArray<int> &selectionStarts,
                                       const Q3MemArray<int> &selectionEnds, const QColorGroup &cg,
                                       bool rightToLeft, int line, KoTextZoomHandler* zh,
                                       int whichFormattingChars )
{
    KWTextFrameSet * textfs = kwTextDocument()->textFrameSet();
    if ( !textfs )
        return;
    KWDocument * doc = textfs->kWordDocument();
    if ( !doc || !doc->viewFormattingChars() )
        return;
    // We set whichFormattingChars before calling KoTextFormat::drawFormattingChars
    whichFormattingChars = 0;
    if ( doc->viewFormattingSpace() )
        whichFormattingChars |= FormattingSpace;
    if ( doc->viewFormattingBreak() )
        whichFormattingChars |= FormattingBreak;
    if ( doc->viewFormattingEndParag() )
        whichFormattingChars |= FormattingEndParag;
    if ( doc->viewFormattingTabs() )
        whichFormattingChars |= FormattingTabs;

    if ( !whichFormattingChars )
        return;
    if ( start + len == length() && (whichFormattingChars & FormattingBreak) && hardFrameBreakAfter() )
    {
        painter.save();
        QPen pen( KGlobalSettings::linkColor() ); // #101820
        painter.setPen( pen );
        //kDebug() << "KWTextParag::drawFormattingChars start=" << start << " len=" << len << " length=" << length() << endl;
            // keep in sync with KWTextFrameSet::formatVertically
            QString str = i18n( "--- Frame Break ---" );
            int width = 0;
            //width = lastFormat->screenStringWidth( zh, str );
            width = lastFormat->screenFontMetrics( zh ).width( str );
            QColorGroup cg2( cg );
            //cg2.setColor( QColorGroup::Base, Qt::green ); // for debug
            int last = length() - 1;
            KoTextStringChar &ch = string()->at( last );
            int x = zh->layoutUnitToPixelX( ch.x );// + ch.pixelxadj;

            KoTextFormat format( *lastFormat );
            format.setColor( pen.color() ); // ### A bit slow, maybe pass the color to drawParagStringInternal ?
            KoTextParag::drawParagStringInternal(
                painter, str, 0, str.length(),
                x, lastY_pix, // startX and lastY
                zh->layoutUnitToPixelY( ch.ascent() ), // baseline
                width, zh->layoutUnitToPixelY( ch.height() ), // bw and h
                drawSelections, &format, selectionStarts,
                selectionEnds, cg2, rightToLeft, line, zh, false );
        // Clear 'paint end of line' flag, we don't want it overwriting the above
        whichFormattingChars &= ~FormattingEndParag;
        painter.restore();
    }

    KoTextParag::drawFormattingChars( painter, start, len,
                                      lastY_pix, baseLine_pix, h_pix,
                                      drawSelections,
                                      lastFormat, selectionStarts,
                                      selectionEnds, cg, rightToLeft,
                                      line, zh, whichFormattingChars );
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
    //kDebug() << "KWTextParag::saveFormat refFormat=" << (  refFormat ? refFormat->key() : "none" )
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
    {
        if ( curFormat->color().isValid() )
        {
            elem = doc.createElement( "COLOR" );
            formatElem.appendChild( elem );
            elem.setAttribute( "red", curFormat->color().red() );
            elem.setAttribute( "green", curFormat->color().green() );
            elem.setAttribute( "blue", curFormat->color().blue() );
        }
        else
        {
            if ( refFormat )
            {
                elem = doc.createElement( "COLOR" );
                formatElem.appendChild( elem );
                elem.setAttribute( "red", -1 );
                elem.setAttribute( "green", -1 );
                elem.setAttribute( "blue", -1 );
            }
        }
    }
    if( !refFormat || curFormat->font().family() != refFormat->font().family() )
    {
        elem = doc.createElement( "FONT" );
        formatElem.appendChild( elem );
        elem.setAttribute( "name", curFormat->font().family() );
    }
    if( !refFormat || curFormat->pointSize() != refFormat->pointSize() )
    {
        elem = doc.createElement( "SIZE" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", curFormat->pointSize() );
    }
    if( !refFormat || curFormat->font().italic() != refFormat->font().italic() )
    {
        elem = doc.createElement( "ITALIC" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->font().italic()) );
    }
    if( !refFormat
        || curFormat->underlineType() != refFormat->underlineType()
        || curFormat->textUnderlineColor() !=refFormat->textUnderlineColor()
        || curFormat->underlineStyle() !=refFormat->underlineStyle()
        || curFormat->wordByWord() != refFormat->wordByWord())
    {
        elem = doc.createElement( "UNDERLINE" );
        formatElem.appendChild( elem );
        if ( curFormat->doubleUnderline() )
            elem.setAttribute( "value", "double" );
        else if ( curFormat->underlineType() == KoTextFormat::U_SIMPLE_BOLD)
            elem.setAttribute( "value", "single-bold" );
        else if( curFormat->underlineType()==KoTextFormat::U_WAVE)
            elem.setAttribute( "value", "wave" );
        else
            elem.setAttribute( "value", static_cast<int>(curFormat->underline()) );
        QString strLineType=KoTextFormat::underlineStyleToString( curFormat->underlineStyle() );
        elem.setAttribute( "styleline", strLineType );
        if ( curFormat->textUnderlineColor().isValid() )
            elem.setAttribute( "underlinecolor", curFormat->textUnderlineColor().name() );

        elem.setAttribute( "wordbyword" , static_cast<int>(curFormat->wordByWord()));
    }
    if( !refFormat
        || curFormat->strikeOutType() != refFormat->strikeOutType()
        || curFormat->strikeOutStyle()!= refFormat->strikeOutStyle()
        || curFormat->wordByWord() != refFormat->wordByWord())
    {
        elem = doc.createElement( "STRIKEOUT" );
        formatElem.appendChild( elem );
        if ( curFormat->doubleStrikeOut() )
            elem.setAttribute( "value", "double" );
        else if ( curFormat->strikeOutType() == KoTextFormat::S_SIMPLE_BOLD)
            elem.setAttribute( "value", "single-bold" );
        else
            elem.setAttribute( "value", static_cast<int>(curFormat->strikeOut()) );
        QString strLineType=KoTextFormat::strikeOutStyleToString( curFormat->strikeOutStyle() );
        elem.setAttribute( "styleline", strLineType );
        elem.setAttribute( "wordbyword" , static_cast<int>(curFormat->wordByWord()));
    }
    if( !refFormat || (curFormat->vAlign() != refFormat->vAlign())
        || (curFormat->relativeTextSize() != refFormat->relativeTextSize()))
    {
        elem = doc.createElement( "VERTALIGN" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", static_cast<int>(curFormat->vAlign()) );
        if ( curFormat->relativeTextSize()!=0.66)
            elem.setAttribute( "relativetextsize", curFormat->relativeTextSize() );
    }
    if( !refFormat || curFormat->textBackgroundColor() != refFormat->textBackgroundColor() )
    {
        if ( curFormat->textBackgroundColor().isValid())
        {
            elem = doc.createElement( "TEXTBACKGROUNDCOLOR" );
            formatElem.appendChild( elem );
            elem.setAttribute( "red", curFormat->textBackgroundColor().red() );
            elem.setAttribute( "green", curFormat->textBackgroundColor().green() );
            elem.setAttribute( "blue", curFormat->textBackgroundColor().blue() );
        }
        else
        {
            if ( refFormat )
            {
                elem = doc.createElement( "TEXTBACKGROUNDCOLOR" );
                formatElem.appendChild( elem );
                elem.setAttribute( "red", -1 );
                elem.setAttribute( "green", -1 );
                elem.setAttribute( "blue", -1 );
            }
        }
    }
    if( !refFormat ||
        ( curFormat->shadowDistanceX() != refFormat->shadowDistanceX()
          || ( curFormat->shadowDistanceY() != refFormat->shadowDistanceY() )
          || ( curFormat->shadowColor() != refFormat->shadowColor() ) ) )
    {
        elem = doc.createElement( "SHADOW" );
        formatElem.appendChild( elem );
        elem.setAttribute( "text-shadow", curFormat->shadowAsCss() );
    }
    if( !refFormat || curFormat->offsetFromBaseLine() != refFormat->offsetFromBaseLine())
    {
        elem = doc.createElement( "OFFSETFROMBASELINE" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", curFormat->offsetFromBaseLine() );
    }
    if( !refFormat || curFormat->attributeFont() != refFormat->attributeFont())
    {
        elem = doc.createElement( "FONTATTRIBUTE" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", KoTextFormat::attributeFontToString(curFormat->attributeFont()) );
    }
    if( !refFormat || curFormat->language() != refFormat->language())
    {
        elem = doc.createElement( "LANGUAGE" );
        formatElem.appendChild( elem );
        elem.setAttribute( "value", curFormat->language() );
    }
    return formatElem;
}

void KWTextParag::save( QDomElement &parentElem, bool saveAnchorsFramesets )
{
    // The qMax below ensures that although we don't save the trailing space
    // in the normal case, we do save it for empty paragraphs (#30336)
    save( parentElem, 0, qMax( 0, length()-2 ), saveAnchorsFramesets );
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
    if ( partOfTableOfContents() )
        paragElem.setAttribute( "toc", "true" );
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
                    kDebug() << " saving into " << elem.tagName() << endl;
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

    // save with the real alignment (left or right, not auto)
    m_layout.saveParagLayout( layoutElem, resolveAlignment() );

    // Paragraph's format
    // ## Maybe we should have a "default format" somewhere and
    // pass it instead of 0L, to only save the non-default attributes
    // But this would break all export filters again.
    QDomElement paragFormatElement = saveFormat( doc, paragraphFormat(), 0L, 0, to - from + 1 );
    layoutElem.appendChild( paragFormatElement );
}

//static
KoTextFormat KWTextParag::loadFormat( QDomElement &formatElem, KoTextFormat * refFormat, const QFont & defaultFont, const QString & defaultLanguage, bool hyphanation )
{
    KoTextFormat format;
    //todo fixme !!!!!!!!!!!!
    format.setHyphenation( hyphanation );
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
        font.setPointSize( elem.attribute("value").toInt() );
    elem = formatElem.namedItem( "ITALIC" ).toElement();
    if ( !elem.isNull() )
        font.setItalic( elem.attribute("value").toInt() == 1 );
    elem = formatElem.namedItem( "UNDERLINE" ).toElement();
    if ( !elem.isNull() ) {
        QString value = elem.attribute("value");
        if ( value == "0" || value == "1" )
            format.setUnderlineType( (value.toInt() == 1)?KoTextFormat::U_SIMPLE: KoTextFormat::U_NONE );
        else if ( value == "single" ) // value never used when saving, but why not support it? ;)
            format.setUnderlineType ( KoTextFormat::U_SIMPLE);
        else if ( value == "double" )
            format.setUnderlineType ( KoTextFormat::U_DOUBLE);
        else if ( value == "single-bold" )
            format.setUnderlineType ( KoTextFormat::U_SIMPLE_BOLD);
        else if( value =="wave")
            format.setUnderlineType ( KoTextFormat::U_WAVE);
        if ( elem.hasAttribute("styleline" ))
        {
            QString strLineType = elem.attribute("styleline");
            format.setUnderlineStyle( KoTextFormat::stringToUnderlineStyle( strLineType ));
        }
        if ( elem.hasAttribute("underlinecolor"))
        {
            QColor col( QColor(elem.attribute("underlinecolor")));
            format.setTextUnderlineColor( col );
        }
        if ( elem.hasAttribute( "wordbyword" ))
            format.setWordByWord( elem.attribute("wordbyword").toInt()==1);
    }
    elem = formatElem.namedItem( "STRIKEOUT" ).toElement();
    if ( !elem.isNull() )
    {
        QString value = elem.attribute("value");
        if ( value == "0" || value == "1" )
            format.setStrikeOutType( (value.toInt() == 1)?KoTextFormat::S_SIMPLE: KoTextFormat::S_NONE );
        else if ( value == "single" ) // value never used when saving, but why not support it? ;)
            format.setStrikeOutType ( KoTextFormat::S_SIMPLE);
        else if ( value == "double" )
            format.setStrikeOutType ( KoTextFormat::S_DOUBLE);
        else if ( value =="single-bold" )
            format.setStrikeOutType ( KoTextFormat::S_SIMPLE_BOLD);

        if ( elem.hasAttribute("styleline" ))
        {
            QString strLineType = elem.attribute("styleline");
            format.setStrikeOutStyle( KoTextFormat::stringToStrikeOutStyle( strLineType ));
        }
        if ( elem.hasAttribute( "wordbyword" ))
            format.setWordByWord( elem.attribute("wordbyword").toInt()==1);
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
    {
        format.setVAlign( static_cast<KoTextFormat::VerticalAlignment>( elem.attribute("value").toInt() ) );
        if (elem.hasAttribute("relativetextsize"))
            format.setRelativeTextSize(elem.attribute("relativetextsize").toDouble());
    }
    elem = formatElem.namedItem( "COLOR" ).toElement();
    if ( !elem.isNull() )
    {
        int red = elem.attribute("red").toInt();
        int green = elem.attribute("green").toInt();
        int blue = elem.attribute("blue").toInt();
        if ( red == -1 && blue == -1 && green == -1 )
            format.setColor( QColor() );
        else
            format.setColor( QColor(red,green,blue) );
    }
    elem = formatElem.namedItem( "TEXTBACKGROUNDCOLOR" ).toElement();
    if ( !elem.isNull() )
    {
        int red = elem.attribute("red").toInt();
        int green = elem.attribute("green").toInt();
        int blue = elem.attribute("blue").toInt();
        if ( red == -1 && blue == -1 && green == -1 )
            format.setTextBackgroundColor( QColor() );
        else
            format.setTextBackgroundColor( QColor(red,green,blue) );
    }
    elem = formatElem.namedItem( "SHADOW" ).toElement();
    if ( !elem.isNull() )
    {
        format.parseShadowFromCss( elem.attribute( "text-shadow" ) );
    } else {
        // Compat with koffice-1.2
        elem = formatElem.namedItem( "SHADOWTEXT" ).toElement();
        if ( !elem.isNull() && elem.attribute("value").toInt() && KoParagLayout::shadowCssCompat )
        {
            // Retrieve shadow attributes from KoParagLayout
            // We don't have its pointer, so shadowCssCompat is static.
            format.parseShadowFromCss( *KoParagLayout::shadowCssCompat );
        }
    }

    elem = formatElem.namedItem( "OFFSETFROMBASELINE" ).toElement();
    if ( !elem.isNull() )
        format.setOffsetFromBaseLine( elem.attribute("value").toInt());

    elem = formatElem.namedItem( "FONTATTRIBUTE" ).toElement();
    if ( !elem.isNull() )
        format.setAttributeFont( KoTextFormat::stringToAttributeFont(elem.attribute("value")));

    elem = formatElem.namedItem( "LANGUAGE" ).toElement();
    if ( !elem.isNull() )
        format.setLanguage( elem.attribute("value") );
    else if ( !refFormat )// No reference format and no LANGUAGE tag -> use default font
        format.setLanguage( defaultLanguage );

    //kDebug() << "KWTextParag::loadFormat format=" << format.key() << endl;
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
            KoTextFormat f = loadFormat( formatElem, defaultFormat, doc->defaultFont(), doc->globalLanguage(), doc->globalHyphenation() );
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
        kWarning(32001) << "No LAYOUT tag in PARAGRAPH, dunno what layout to apply" << endl;
    }
}

void KWTextParag::load( QDomElement &attributes )
{
    loadLayout( attributes );

    // Set the text after setting the paragraph format - so that the format applies
    QDomElement element = attributes.namedItem( "TEXT" ).toElement();
    if ( !element.isNull() )
    {
        //kDebug() << "KWTextParag::load '" << element.text() << "'" << endl;
        append( element.text() );
        // Apply default format - this should be automatic !!
        setFormat( 0, string()->length(), paragFormat(), true );
    }

    if ( attributes.attribute( "toc" ) == "true" )
        setPartOfTableOfContents( true );

    loadFormatting( attributes );

    setChanged( true );
    invalidate( 0 );
}

void KWTextParag::loadFormatting( QDomElement &attributes, int offset, bool loadFootNote )
{

    Q3ValueList<int> removeLenList;
    Q3ValueList<int> removePosList;

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
                    KoTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont(),doc->globalLanguage(), doc->globalHyphenation() );
                    //kDebug(32002) << "KWTextParag::loadFormatting applying formatting from " << index << " to " << index+len << endl;
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
                        kDebug() << "Replacing QChar(1) (in KWTextParag::loadFormatting)" << endl;
                        ch.c='#';
                    }

                    KWTextImage * custom = new KWTextImage( kwTextDocument(), QString::null );
                    kDebug() << "KWTextParag::loadFormatting insertCustomItem" << endl;
                    paragFormat()->addRef();
                    setCustomItem( index, custom, paragFormat() );
                    custom->load( formatElem );
                    break;
                }
                case 3: // Tabulator
                {
                    len = 1; // it was missing from old 1.0 files

                    // We have to replace the # or QChar(1) by a tabulator
                    KoTextStringChar& ch = string()->at(index);
                    ch.c='\t';

                    // I assume that we need the same treatment as for id == 1
                    KoTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont(),doc->globalLanguage(), doc->globalHyphenation() );
                    //kDebug(32002) << "KWTextParag::loadFormatting applying formatting from " << index << " to " << index+len << endl;
                    setFormat( index, len, document()->formatCollection()->format( &f ) );
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
                        kWarning(32001) <<
                            ( oldDoc ? "No <TYPE> in <FORMAT> with id=4, for a variable [old document assumed] !"
                              : "No <TYPE> found in <VARIABLE> tag!" ) << endl;
                    else
                    {
                        int type = typeElem.attribute( "type" ).toInt();
                        QString key = typeElem.attribute( "key" );
                        kDebug() << "KWTextParag::loadFormatting variable type=" << type << " key=" << key << endl;
                        KoVariableFormat * varFormat = key.isEmpty() ? 0 : doc->variableFormatCollection()->format( key.latin1() );
                        // If varFormat is 0 (no key specified), the default format will be used.
                        int correct = 0;
                        if (typeElem.hasAttribute( "correct" ))
                            correct = typeElem.attribute("correct").toInt();
                        KoVariable * var =doc->variableCollection()->createVariable( type, -1, doc->variableFormatCollection(), varFormat,kwTextDocument(),doc, correct, true , loadFootNote);
                        if ( var )
                        {
                            var->load( varElem );
                            KoTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont(),doc->globalLanguage(), doc->globalHyphenation() );
                            setCustomItem( index, var, document()->formatCollection()->format( &f ) );
                        }
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
                            kWarning() << "Anchor type not supported: " << type << endl;
                    }
                    else
                        kWarning() << "Missing ANCHOR tag" << endl;
                    break;
                }
                default:
                    kWarning() << "KWTextParag::loadFormatting id=" << id << " not supported" << endl;
                    break;
                }
            }
        }
    }
    for(int i=0; i < removeLenList.count(); i++) {
        remove(*removePosList.at(i), *removeLenList.at(i));
    }
}

void KWTextParag::setParagLayout( const KoParagLayout & layout, int flags, int marginIndex )
{
    KoTextParag::setParagLayout( layout, flags, marginIndex );

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
        KoParagStyle *style;
        // Name of the style. If there is no style, then we do not supply
        // any default!
        QDomElement element = parentElem.namedItem( "NAME" ).toElement();
        if ( !element.isNull() )
        {
            QString styleName = element.attribute( "value" );
            style = doc->styleCollection()->findStyle( styleName );
            if (!style)
            {
                kError(32001) << "Cannot find style \"" << styleName << "\" specified in paragraph LAYOUT - using Standard" << endl;
                style = doc->styleCollection()->findStyle( "Standard" );
            }
            //else kDebug() << "KoParagLayout::KoParagLayout setting style to " << style << " " << style->name() << endl;
        }
        else
        {
            kError(32001) << "Missing NAME tag in paragraph LAYOUT - using Standard" << endl;
            style = doc->styleCollection()->findStyle( "Standard" );
        }
        Q_ASSERT(style);
        layout.style = style;
    }

    KoParagLayout::loadParagLayout( layout, parentElem, doc->syntaxVersion() );

    return layout;
}

void KWTextParag::join( KoTextParag *parag )
{
    m_layout.pageBreaking &= ~(KoParagLayout::HardFrameBreakBefore|KoParagLayout::HardFrameBreakAfter);
    KoTextParag::join( parag );
}

void KWTextParag::loadOasis( const QDomElement& paragElement, KoOasisContext& context, KoStyleCollection *styleCollection, uint& pos )
{
    KoTextParag::loadOasis( paragElement, context, styleCollection, pos );

    KWTextFrameSet* textfs = kwTextDocument()->textFrameSet();
    if ( textfs->isMainFrameset() && textfs->kWordDocument()->isLoading() /*not during copy/paste*/ )
    {
        KWDocument * doc = textfs->kWordDocument();
        QString& currentMasterPageRef = doc->loadingInfo()->m_currentMasterPage;
        const QString styleName = paragElement.attributeNS( KoXmlNS::text, "style-name", QString::null );
        if ( !styleName.isEmpty() )
        {
            const QDomElement* paragraphStyle = context.oasisStyles().findStyle( styleName, "paragraph" );
            QString masterPageName = paragraphStyle ? paragraphStyle->attributeNS( KoXmlNS::style, "master-page-name", QString::null ) : QString::null;

            // In KWord we don't support sections so the first paragraph is the one that determines the page layout.
            if ( prev() == 0 ) {
                if ( masterPageName.isEmpty() )
                    masterPageName = "Standard"; // Seems to be a builtin name for the default layout...
                currentMasterPageRef = masterPageName; // do this first to avoid recursion
                context.styleStack().save();
                context.styleStack().setTypeProperties( "paragraph" );
                context.addStyles( paragraphStyle, "paragraph" );
                // This is quite ugly... OOo stores the starting page-number in the first paragraph style...
                QString pageNumber = context.styleStack().attributeNS( KoXmlNS::style, "page-number" );
                if ( !pageNumber.isEmpty() )
                    doc->variableCollection()->variableSetting()->setStartingPageNumber( pageNumber.toInt() );
                context.styleStack().restore();

                doc->loadOasisPageLayout( masterPageName, context ); // page layout
            }
            else if ( !masterPageName.isEmpty() // empty means no change
                      && masterPageName != currentMasterPageRef )
            {
                // Detected a change in the master page -> this means we have to use a new page layout
                // and insert a frame break if not on the first paragraph.
                kDebug(32001) << "KWTextParag::loadOasis: change of master page detected: from " << currentMasterPageRef << " to " << masterPageName << " -> inserting page break" << endl;
                currentMasterPageRef = masterPageName;
                // [see also KoParagLayout for the 'normal' way to insert page breaks]
                m_layout.pageBreaking |= KoParagLayout::HardFrameBreakBefore;
                // We have no way to load/use the new page layout, KWord doesn't have "sections".
            }
        }
    }
}

void KWTextParag::saveOasis( KoXmlWriter& writer, KoSavingContext& context,
                             int from, int to, bool saveAnchorsFramesets ) const
{
    // Special case for inline tables that are alone in their paragraph:
    // save <table> instead of <p>.
    if ( string()->length() == 2 /*&& saveAnchorsFramesets*/ ) {
        KoTextStringChar &ch = string()->at( 0 );
        if ( ch.isCustom() && dynamic_cast<KWAnchor*>( ch.customItem() )) {
            KWFrameSet* fs = static_cast<KWAnchor *>( ch.customItem() )->frameSet();
            if ( fs->type() == FT_TABLE ) {
                // TODO maybe save parag style? extract a common method out of KoTextStringChar::saveOasis
                fs->saveOasis( writer, context, true );
                return;
            }
        }
    }
    KoTextParag::saveOasis( writer, context, from, to, saveAnchorsFramesets );
}
