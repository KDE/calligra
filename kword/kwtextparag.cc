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
#include "kwdoc.h"
#include "kwanchorpos.h"
#include "kwanchor.h"
#include "kwtextimage.h"
#include "kwtextframeset.h"
#include "kwviewmode.h"
#include "variable.h"
#include <koparagcounter.h>
#include <kotextobject.h>
#include <klocale.h>
#include <kdebug.h>
#include <qdom.h>
#include <qtl.h>
#include <assert.h>

// Called by KoTextParag::drawParagString - all params are in pixel coordinates
void KWTextParag::drawFormattingChars( QPainter &painter, const QString & /*s*/, int start, int len, int startX,
                                       int lastY, int baseLine, int bw, int h, bool drawSelections,
                                       QTextFormat *lastFormat, int /*i*/, const QMemArray<int> &selectionStarts,
                                       const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft )
{
    KWTextFrameSet * textfs = kwTextDocument()->textFrameSet();
    if ( textfs )
    {
        bool forPrint = ( painter.device()->devType() == QInternal::Printer );
        KWDocument * doc = textfs->kWordDocument();
        KoZoomHandler * zh = doc;
        drawSelections=textfs->currentViewMode()->drawSelections();
        if ( doc && doc->viewFormattingChars() && !forPrint )
        {
            // Calculate startX in pixels (using the xadj value of the corresponding char)
            startX = zh->layoutUnitToPixelX( startX ) + at( rightToLeft ? start+len-1 : start )->pixelxadj;
            lastY = zh->layoutUnitToPixelY( lastY );
            baseLine = zh->layoutUnitToPixelY( baseLine );
            // already in pixels: bw = zh->layoutUnitToPixelX( bw );
            h = zh->layoutUnitToPixelY( h );
            // From now on lastFormat, startX, lastY etc. have zoom-related (pixel) sizes.

            painter.save();
            QPen pen( cg.color( QColorGroup::Highlight ) );
            painter.setPen( pen );
            //kdDebug() << "KWTextParag::drawFormattingChars start=" << start << " len=" << len << " length=" << length() << endl;
            if ( start + len == length() )
            {
                if ( hardFrameBreakAfter() )
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
                    QTextParag::drawParagString( painter, str, 0, str.length(),
                                                 last, lastY, at( last )->ascent(),
                                                 width, lastFormat->height(),
                                                 drawSelections, &format, last, selectionStarts,
                                                 selectionEnds, cg2, rightToLeft );
                }
                else
                {
                    // drawing the end of the parag
                    QTextFormat format( * at( length() - 1 )->format() );
                    format.setPointSize( qRound( zh->layoutUnitToFontSize( format.font().pointSize(), forPrint ) ) );
                    int w = format.width('x'); // see KWTextFrameSet::adjustFlow
                    int size = QMIN( w, h * 3 / 4 );
                    int arrowsize = zh->zoomItY( 2 );
                    // x,y is the bottom right corner of the reversed L
                    //kdDebug() << "startX=" << startX << " bw=" << bw << " w=" << w << endl;
                    int x = ( startX + bw ) + w - 1;
                    int y = lastY + baseLine - arrowsize;
                    //kdDebug() << "KWTextParag::drawFormattingChars drawing CR at " << x << "," << y << endl;
                    painter.drawLine( x, y - size, x, y );
                    painter.drawLine( x, y, x - size, y );
                    // Now the arrow
                    painter.drawLine( x - size, y, x - size + arrowsize, y - arrowsize );
                    painter.drawLine( x - size, y, x - size + arrowsize, y + arrowsize );
                }
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
                    // Don't use ch.pixelwidth here. We want a square with
                    // the same size for all spaces, even the justified ones
                    int w = zh->layoutUnitToPixelX( string()->width(i) );
                    int height = zh->layoutUnitToPixelY( ch.ascent() );
                    int size = QMAX( 2, QMIN( w/2, height/3 ) ); // Enfore that it's a square, and that it's visible
                    int x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj;
                    painter.drawRect( x + (ch.pixelwidth - size) / 2, lastY + baseLine - (height - size) / 2, size, size );
                }
                else if ( ch.c == '\t' )
                {
                    /*QTextStringChar &nextch = string()->at(i+1);
                    int nextx = (nextch.x > ch.x) ? nextch.x : rect().width();
                    //kdDebug() << "tab x=" << ch.x << " nextch.x=" << nextch.x
                    //          << " nextx=" << nextx << " startX=" << startX << " bw=" << bw << endl;
                    int availWidth = nextx - ch.x - 1;
                    availWidth=zh->layoutUnitToPixelX(availWidth);*/

                    int availWidth = ch.pixelwidth;

                    int x = zh->layoutUnitToPixelX( ch.x ) + ch.pixelxadj + availWidth / 2;
                    int size = QMIN( availWidth,zh->layoutUnitToPixelX(ch.format()->width('W')) ) / 2 ; // actually the half size
                    int y = lastY + baseLine - zh->layoutUnitToPixelY( ch.ascent()/2 );
                    int arrowsize = zh->zoomItY( 2 );
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
                QTextFormat * format = textfs->textObject()->zoomFormatFont( &newStyle->format() );
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

KWTextDocument * KWTextParag::kwTextDocument() const
{
    return static_cast<KWTextDocument *>( document() );
}

//static
QDomElement KWTextParag::saveFormat( QDomDocument & doc, KoTextFormat * curFormat, KoTextFormat * refFormat, int pos, int len, KoZoomHandler *zh )
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
        int size = static_cast<int>( zh->layoutUnitToPt( curFormat->font().pointSize() ) );
        elem.setAttribute( "value", size );
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
    KoTextFormat *curFormat = paragraphFormat();
    for ( int i = from; i <= to; ++i, ++index )
    {
        QTextStringChar & ch = string()->at(i);
        KoTextFormat * newFormat = static_cast<KoTextFormat *>( ch.format() );
        if ( ch.isCustom() )
        {
            if ( startPos > -1 && curFormat) { // Save former format
                QDomElement formatElem = saveFormat( doc, curFormat,
                                                     paragraphFormat(), startPos, index-startPos,
                                                     textDocument()->zoomHandler() );
                if ( !formatElem.firstChild().isNull() ) // Don't save an empty format tag
                    formatsElem.appendChild( formatElem );
            }

            QDomElement formatElem = saveFormat( doc, newFormat, paragraphFormat(), index, 1, textDocument()->zoomHandler() );
            formatsElem.appendChild( formatElem );
            static_cast<KoTextCustomItem *>( ch.customItem() )->save( formatElem );
            startPos = -1;
            curFormat = paragraphFormat();
            // Save the contents of the frameset inside the anchor
            // This is NOT used when saving, but it is used when copying an inline frame
            if ( saveAnchorsFramesets && dynamic_cast<KWAnchor *>( ch.customItem() ) )
            {
                KWFrameSet* inlineFs = static_cast<KWAnchor *>( ch.customItem() )->frameSet();
                //inlineFs->toXML( parentElem );
                // Save inline framesets at the toplevel. Necessary when copying a textframeset that
                // itself includes an inline frameset - we want all frameset tags at the toplevel.

                // Qt bug! (found by Simon)   QDomElement elem = doc.documentElement();
                // Workaround:
                QDomNode n = parentElem;
                while ( !n.isDocument() && !n.isNull() )
                    n = n.parentNode();
                QDomElement elem = n.toDocument().documentElement();
                kdDebug() << " saving into " << elem.tagName() << endl;
                inlineFs->toXML( elem );
            }
        }
        else
        {
            if ( newFormat != curFormat )
            {
                // Format changed.
                if ( startPos > -1 && curFormat) { // Save former format
                    QDomElement formatElem = saveFormat( doc, curFormat, paragraphFormat(), startPos, index-startPos, textDocument()->zoomHandler() );
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
        QDomElement formatElem = saveFormat( doc, curFormat, paragraphFormat(), startPos, index-startPos, textDocument()->zoomHandler() );
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
    QDomElement paragFormatElement = saveFormat( doc, paragraphFormat(), 0L, 0, to - from + 1, textDocument()->zoomHandler() );
    layoutElem.appendChild( paragFormatElement );
}

//static
KoTextFormat KWTextParag::loadFormat( QDomElement &formatElem, KoTextFormat * refFormat, const QFont & defaultFont, KoZoomHandler *zh )
{
    KoTextFormat format;
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
    {
        int size = elem.attribute("value").toInt();
        font.setPointSize( zh->ptToLayoutUnit( size ) );
    }
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
        KoParagLayout paragLayout = loadParagLayout( layout, doc, true );
        setParagLayout( paragLayout );

        // Load default format from style.
        KoTextFormat *defaultFormat = style() ? &style()->format() : 0L;
        QDomElement formatElem = layout.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
        {
            // Load paragraph format
            KoTextFormat f = loadFormat( formatElem, defaultFormat, doc->defaultFont(), textDocument()->zoomHandler() );
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
                    KoTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont(), textDocument()->zoomHandler() );
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
                        QString key = typeElem.attribute( "key" );
                        kdDebug() << "KWTextParag::loadFormatting variable type=" << type << " key=" << key << endl;
                        KWVariableFormat * varFormat = key.isEmpty() ? 0 : doc->variableFormatCollection()->format( key.latin1() );
                        // If varFormat is 0 (no key specified), the default format will be used.
                        KWVariable * var = KWVariable::createVariable( type, -1, kwTextDocument()->textFrameSet(), varFormat );
                        var->load( varElem );
                        KoTextFormat f = loadFormat( formatElem, paragraphFormat(), doc->defaultFont(), textDocument()->zoomHandler() );
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
                  << " fontsize:" << dynamic_cast<KoTextFormat *>(paragFormat())->pointSizeFloat() << endl;

        QTextString * s = string();
        for ( int i = 0 ; i < s->length() ; ++i )
        {
            QTextStringChar & ch = s->at(i);
            kdDebug() << i << ": '" << QString(ch.c) << "' (" << ch.c.unicode() << ")"
                      << " x(LU)=" << ch.x
                      << " w(LU)=" << ch.width//s->width(i)
                      << " x(PIX)=" << textDocument()->zoomHandler()->layoutUnitToPixelX( ch.x )
                + ch.pixelxadj
                      << " (xadj=" << + ch.pixelxadj << ")"
                      << " w(PIX)=" << ch.pixelwidth
                      << " height=" << ch.height()
                //      << " format=" << ch.format()
                //      << " \"" << ch.format()->key() << "\" "
                //<< " fontsize:" << dynamic_cast<KoTextFormat *>(ch.format())->pointSizeFloat()
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

// Create a KoParagLayout from XML.
//
// If a document is supplied, default values are taken from the style in the
// document named by the layout. This allows for simplified import filters,
// and also looks to the day that redundant data can be eliminated from the
// saved XML.
KoParagLayout KWTextParag::loadParagLayout( QDomElement & parentElem, KWDocument *doc, bool useRefStyle )
{
    KoParagLayout layout;

    // Only when loading paragraphs, not when loading styles
    if ( useRefStyle )
    {
        KoStyle *style;
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
                layout = style->paragLayout();
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
            tabList.append( tab );
        }
    }
    layout.setTabList( tabList );
    layout.alignment = Qt::AlignLeft;
    element = parentElem.namedItem( "FLOW" ).toElement(); // Flow is what is now called alignment internally
    if ( !element.isNull() )
    {
        QString flow = element.attribute( "value" ); // KWord-0.8
        if ( !flow.isEmpty() )
        {
            static const int flow2align[] = { Qt::AlignLeft, Qt::AlignRight, Qt::AlignCenter, Qt3::AlignJustify };
            layout.alignment = flow2align[flow.toInt()];
        } else {
            flow = element.attribute( "align" ); // KWord-1.0 DTD
            layout.alignment = flow=="right" ? (int)Qt::AlignRight : flow=="center" ? (int)Qt::AlignCenter : flow=="justify" ? (int)Qt3::AlignJustify : (int)Qt::AlignLeft;
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
        layout.leftBorder.ptWidth = 0;

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.rightBorder = KoBorder::loadBorder( element );
    else
        layout.rightBorder.ptWidth = 0;

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        layout.topBorder = KoBorder::loadBorder( element );
    else
        layout.topBorder.ptWidth = 0;

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        layout.bottomBorder = KoBorder::loadBorder( element );
    else
        layout.bottomBorder.ptWidth = 0;

    element = parentElem.namedItem( "COUNTER" ).toElement();
    if ( !element.isNull() )
    {
        layout.counter = new KoParagCounter;
        layout.counter->load( element );
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
        //if ( layout.margins[QStyleSheetItem::MarginFirstLine] != 0 )
            element.setAttribute( "first", layout.margins[QStyleSheetItem::MarginFirstLine] );
        //if ( layout.margins[QStyleSheetItem::MarginLeft] != 0 )
            element.setAttribute( "left", layout.margins[QStyleSheetItem::MarginLeft] );
        //if ( layout.margins[QStyleSheetItem::MarginRight] != 0 )
            element.setAttribute( "right", layout.margins[QStyleSheetItem::MarginRight] );
    }

    /*if ( margins[QStyleSheetItem::MarginTop] != 0 ||
         margins[QStyleSheetItem::MarginBottom] != 0 )*/
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        //if ( layout.margins[QStyleSheetItem::MarginTop] != 0 )
            element.setAttribute( "before", layout.margins[QStyleSheetItem::MarginTop] );
        //if ( layout.margins[QStyleSheetItem::MarginBottom] != 0 )
            element.setAttribute( "after", layout.margins[QStyleSheetItem::MarginBottom] );
    }

    /*if ( lineSpacing != 0 )*/
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

    /*if ( pageBreaking != 0 )*/
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

    /*if ( leftBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        layout.leftBorder.save( element );
    }
    /*if ( rightBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        layout.rightBorder.save( element );
    }
    /*if ( topBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        layout.topBorder.save( element );
    }
    /*if ( bottomBorder.ptWidth > 0 )*/
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        layout.bottomBorder.save( element );
    }
    /*if ( counter && counter->numbering() != KoParagCounter::NUM_NONE )*/
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
    }
}
