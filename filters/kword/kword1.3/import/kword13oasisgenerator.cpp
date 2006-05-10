/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <QString>
#include <q3dict.h>
#include <QFile>
#include <qbuffer.h>
#include <QColor>
#include <qimage.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <kdebug.h>
#include <ktempfile.h>
#include <kmimetype.h>

#include <kofficeversion.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoDocument.h>

#include "kword13formatother.h"
#include "kword13picture.h"
#include "kword13document.h"

#include "kword13oasisgenerator.h"

KWord13OasisGenerator::KWord13OasisGenerator( void ) : m_kwordDocument( 0 ),  m_store( 0 ), m_manifestWriter( 0 )
{
}

KWord13OasisGenerator::~KWord13OasisGenerator( void )
{
}

void KWord13OasisGenerator::prepareTextFrameset( KWordTextFrameset* frameset )
{
    if ( ! frameset )
    {
        kWarning(30520) << "Tried to prepare a NULL text frameset!" << endl;
        return;
    }

    for ( Q3ValueList<KWord13Paragraph>::Iterator it = frameset->m_paragraphGroup.begin();
        it != frameset->m_paragraphGroup.end(); ++it)
    {
        declareLayout( (*it).m_layout );
        for ( KWord13Format* format = (*it).m_formats.first(); format; format = (*it).m_formats.next() )
        {
            // ### Provisory, as it does not handle id != 1
            KWord13FormatOneData* data = format->getFormatOneData();
            if ( data )
            {
                // Inspired from KoTextParag::saveOasis, macro WRITESPAN
                KoGenStyle gs( KoGenStyle::STYLE_AUTO, "text", (*it).m_layout.m_autoStyleName );
                fillGenStyleWithFormatOne( *data , gs, false );
                data->m_autoStyleName = m_oasisGenStyles.lookup( gs, "T" );
                kDebug(30520) << "Format: Parent " << (*it).m_layout.m_autoStyleName << " => " << data->m_autoStyleName << endl;
            }
        }
    }
}

void KWord13OasisGenerator::preparePageLayout( void )
{
    // Inspired by KoPageLayout::saveOasis
    KoGenStyle style(KoGenStyle::STYLE_PAGELAYOUT);
    style.addPropertyPt("fo:page-width", positiveNumberOrNull ( m_kwordDocument->getProperty( "PAPER:width", "PAPER:ptWidth" ) ) );
    style.addPropertyPt("fo:page-height", positiveNumberOrNull ( m_kwordDocument->getProperty( "PAPER:height", "PAPER:ptHeight" ) ) );
    style.addPropertyPt("fo:margin-left", positiveNumberOrNull ( m_kwordDocument->getProperty( "PAPERBORDERS:left", "PAPERBORDERS:ptLeft" ) ) );
    style.addPropertyPt("fo:margin-right", positiveNumberOrNull ( m_kwordDocument->getProperty( "PAPERBORDERS:right", "PAPERBORDERS:ptRight" ) ) );
    style.addPropertyPt("fo:margin-top", positiveNumberOrNull ( m_kwordDocument->getProperty( "PAPERBORDERS:top", "PAPERBORDERS:ptTop" ) ) );
    style.addPropertyPt("fo:margin-bottom", positiveNumberOrNull ( m_kwordDocument->getProperty( "PAPERBORDERS:bottom", "PAPERBORDERS:ptBottom" ) ) );
    if ( m_kwordDocument->getProperty( "PAPER:orientation" ) == "1" )
    {
        style.addProperty("style:print-orientation", "landscape" );
    }
    else
    {
        style.addProperty("style:print-orientation", "portrait" );
    }
    // end of "inspiration"
    // ### TODO: verify that it fits OASIS spec (as it is OO spec.)
    bool ok = false;
    const int firstPageNumber = m_kwordDocument->getProperty( "VARIABLESETTINGS:startingPageNumber" ).toInt( &ok );
    style.addProperty("style:first-page-number", ( ( ok && firstPageNumber > 1 ) ? firstPageNumber : 1 ) );

    const int columns = m_kwordDocument->getProperty( "PAPER:columns" ).toInt( &ok );
    if ( ok && columns > 1 )
    {
        // ### TODO: test
        QBuffer buffer;
        buffer.open( QIODevice::WriteOnly );
        KoXmlWriter element ( &buffer );
        element.startElement("style:columns");
        element.addAttribute( "fo:column-count", columns );
        element.addAttributePt( "fo:column-gap", positiveNumberOrNull( m_kwordDocument->getProperty( "PAPER:columnspacing", "PAPER:ptColumnspc" ) ) );

        for ( int i=0; i < columns; ++i )
        {
            element.startElement( "style:column" );
            element.addAttribute( "style:rel-width", "1*" );
            element.addAttributePt( "fo:margin-left", 0.0 );
            element.addAttributePt( "fo:margin-right", 0.0 );
            element.endElement();
        }

        element.endElement();
        buffer.close();
        const QString strElement( QString::fromUtf8( buffer.buffer(), buffer.buffer().size() ) );
        style.addChildElement( "style:columns", strElement );
    }
    const QString automaticPageStyle ( m_oasisGenStyles.lookup( style, "pm" ) );
    kDebug(30520) << "Automatic page style: " << automaticPageStyle << endl;
}


bool KWord13OasisGenerator::prepare( KWord13Document& kwordDocument )
{
    if ( m_kwordDocument && ( (void*) m_kwordDocument ) != ( (void*) &kwordDocument ) )
    {
        kWarning(30520) << "KWord Document is different!" <<endl;
    }

    m_kwordDocument = &kwordDocument;

    preparePageLayout();

    // Declare styles
    for ( Q3ValueList<KWord13Layout>::Iterator it = m_kwordDocument->m_styles.begin();
        it != m_kwordDocument->m_styles.end(); ++it)
    {
        declareStyle( *it );
    }

    // Prepare first text frameset
    prepareTextFrameset( m_kwordDocument->m_normalTextFramesetList.first() );

    // ### TODO

    return true;
}

double KWord13OasisGenerator::numberOrNull( const QString& str ) const
{
    bool ok = false;
    const double d = str.toDouble( &ok );
    if ( ok )
        return d;
    else
        return 0.0;
}

double KWord13OasisGenerator::positiveNumberOrNull( const QString& str ) const
{
    bool ok = false;
    const double d = str.toDouble( &ok );
    if ( ok && d >= 0.0 )
        return d;
    else
        return 0.0;
}

// Inspired by KoParagStyle::saveStyle
void KWord13OasisGenerator::declareLayout( KWord13Layout& layout )
{
    KoGenStyle gs( KoGenStyle::STYLE_AUTO, "paragraph", layout.m_name );

    // ### TODO: any display name? gs.addAttribute( "style:display-name", layout.m_name );
#if 0
    // TODO: check that this is correct
    if ( m_paragLayout.counter && m_paragLayout.counter->depth() ) {
        if ( m_bOutline )
            gs.addAttribute( "style:default-outline-level", (int)m_paragLayout.counter->depth() + 1 );
        else
            gs.addAttribute( "style:default-level", (int)m_paragLayout.counter->depth() + 1 );
    }
#endif
    fillGenStyleWithLayout( layout, gs, false );
    fillGenStyleWithFormatOne( layout.m_format , gs, false );

    layout.m_autoStyleName = m_oasisGenStyles.lookup( gs, "P", true );

    kDebug(30520) << "Layout: Parent " << layout.m_name << " => " << layout.m_autoStyleName << endl;
}


// Inspired by KoParagStyle::saveStyle
void KWord13OasisGenerator::declareStyle( KWord13Layout& layout )
{
    KoGenStyle gs( KoGenStyle::STYLE_USER, "paragraph", QString::null );

    gs.addAttribute( "style:display-name", layout.m_name );
#if 0
    // TODO: check that this is correct
    if ( m_paragLayout.counter && m_paragLayout.counter->depth() ) {
        if ( m_bOutline )
            gs.addAttribute( "style:default-outline-level", (int)m_paragLayout.counter->depth() + 1 );
        else
            gs.addAttribute( "style:default-level", (int)m_paragLayout.counter->depth() + 1 );
    }
#endif
    fillGenStyleWithLayout( layout, gs, true );
    fillGenStyleWithFormatOne( layout.m_format , gs, true );

    layout.m_autoStyleName = m_oasisGenStyles.lookup( gs, layout.m_name, false );

    kDebug(30520) << "Style: " << layout.m_name << " => " << layout.m_autoStyleName << endl;
}


// Inspired from KoTextFormat::save but we have not the same data to start with.
void KWord13OasisGenerator::fillGenStyleWithFormatOne( const KWord13FormatOneData& one, KoGenStyle& gs, const bool style ) const
{
    QString str; // helper string

    KoGenStyle::PropertyType tt = KoGenStyle::TextType;
    bool redok = false, greenok = false, blueok = false, ok = false;
    const QColor color(
        one.getProperty( "COLOR:red" ).toInt( &redok ),
        one.getProperty( "COLOR:green" ).toInt( &greenok ),
        one.getProperty( "COLOR:blue" ).toInt( &blueok ) );
    if ( color.isValid() && redok && greenok && blueok )
    {
        gs.addProperty( "fo:color", color.name(), tt );
    }
    else if ( style )
    {
        gs.addProperty( "fo:color", "#000000", tt );
    }
    // TODO declare svg font faces stuff according to the OASIS format;
    str = one.getProperty( "FONT:name" );
    if ( !str.isEmpty() )
    {
        gs.addProperty( "style:font-name", str, tt ); // hack
    }
    // ### TODO What to do if a style does not define a font? Do we leave it empty or do we use our default font?

    double d = numberOrNull( one.getProperty( "SIZE:value" ) );
    if ( d >= 1.0 ) // Sane value?
    {
        gs.addPropertyPt( "fo:font-size", d, tt );
    }
    // ### TODO If not, same question as with font name.

    ok = false;
    const int weight = one.getProperty( "WEIGHT:value" ).toInt( &ok );
    if ( ok && weight >=0 )
    {
        if ( weight == 50 )
        {
            gs.addProperty( "fo:font-weight", "normal", tt );
        }
        else if (weight == 75 )
        {
            gs.addProperty( "fo:font-weight", "bold", tt );
        }
        else
        {
            gs.addProperty( "fo:font-weight", QString::number( weight * 10 ), tt );
        }
    }
    else if ( style )
    {
        gs.addProperty( "fo:font-weight", "normal", tt );
    }

    ok = false;
    const int italic = one.getProperty( "ITALIC:value" ).toInt( &ok );
    if ( ok && ( italic == 1 ) )
    {
        gs.addProperty( "fo:font-style", "italic", tt );
    }
    else if ( ( ok && ! italic ) || style )
    {
        gs.addProperty( "fo:font-style", "normal", tt );
    }

    // ### TODO
#if 0
    
    gs.addProperty( "style:text-underline-mode", d->m_bWordByWord ? "skip-white-space" : "continuous", tt );
    gs.addProperty( "style:text-underline-type", m_underlineType == U_NONE ? "none" :
                    m_underlineType == U_DOUBLE ? "double" : "single", tt );
    QString styleline;
    if ( m_underlineType == U_WAVE )
        styleline = "wave";
    else
        styleline = exportOasisUnderline( m_underlineStyle );
    gs.addProperty( "style:text-underline-style", styleline, tt );
    gs.addProperty( "style:text-underline-color", m_textUnderlineColor.isValid() ? m_textUnderlineColor.name() : "font-color", tt );
    // TODO U_SIMPLE_BOLD
    // TODO style:text-line-through-mode
    gs.addProperty( "style:text-line-through-type", m_strikeOutType == S_NONE ? "none" :
                    m_strikeOutType == S_DOUBLE ? "double" : "single", tt );

    styleline = exportOasisUnderline( (UnderlineStyle) m_strikeOutStyle );
    gs.addProperty( "style:text-line-through-style", styleline, tt );
    //gs.addProperty( "style:text-line-through-color", ...) TODO in kotext

    QString textPos;
    if ( d->m_offsetFromBaseLine != 0 )
        textPos = QString::number( 100 * d->m_offsetFromBaseLine / fn.pointSizeFloat() ) + '%';
    else if ( va == AlignSuperScript ) textPos = "super";
    else if ( va == AlignSubScript ) textPos = "sub";
    else textPos = "0%";
    textPos += ' ';
    textPos += QString::number( d->m_relativeTextSize * 100 );
    textPos += '%';
    gs.addProperty( "style:text-position", textPos, tt );

    if ( m_attributeFont == ATT_SMALL_CAPS )
        gs.addProperty( "fo:font-variant", "small-caps", tt );
    else if ( m_attributeFont == ATT_UPPER )
        gs.addProperty( "fo:text-transform", "uppercase", tt );
    else if ( m_attributeFont == ATT_LOWER )
        gs.addProperty( "fo:text-transform", "lowercase", tt );

    gs.addProperty( "fo:language", m_language == "en_US" ? QString("en") : m_language, tt );
    gs.addProperty( "fo:background-color",
                    m_textBackColor.isValid() ? m_textBackColor.name() : "transparent", tt );
    gs.addProperty( "fo:text-shadow", shadowAsCss(), tt );
    gs.addProperty( "fo:hyphenate", d->m_bHyphenation, tt );
#endif
}

// Inspired from KoParagLayout::saveOasis but we have not the same data to start with.
void KWord13OasisGenerator::fillGenStyleWithLayout( const KWord13Layout& layout, KoGenStyle& gs, const bool style ) const
{
    // ### TODO syntaxVersion < 3

    QString str; // Help string to store each KWord 1.3 layout property

    str = layout.getProperty( "FLOW:align" );
    if ( str.isEmpty() && ! style)
    {
        // Nothing to do!
    }
    else if ( ( str == "left" ) || ( str == "right") || ( str == "center" ) || ( str == "justify" ) )
    {
        gs.addProperty( "fo:text-align", str );
    }
    else // KWord 1.3's "auto" (or empty/unknown string for a style)
    {
        gs.addProperty( "fo:text-align", "start" ); // i.e. direction-dependent
    }

    str = layout.getProperty( "FLOW:dir" );
    if ( str == "R" ) // ### TODO: check the right value
    {
            gs.addProperty( "style:writing-mode", "rl-tb" ); // right-to-left, top-to-bottom
    }
    else if ( style )
    {
            gs.addProperty( "style:writing-mode", "lr-tb" ); // left-to-right, top-to-bottom
    }

    // ### TODO: do not define if it does not exist and ! style
    gs.addPropertyPt( "fo:margin-left", numberOrNull( layout.getProperty( "INDENTS:left" ) ) );
    gs.addPropertyPt( "fo:margin-right", numberOrNull( layout.getProperty( "INDENTS:right" ) ) );
    gs.addPropertyPt( "fo:text-indent", numberOrNull( layout.getProperty( "INDENTS:first" ) ) );
    gs.addPropertyPt( "fo:margin-top", numberOrNull( layout.getProperty( "OFFSETS:before" ) ) );
    gs.addPropertyPt( "fo:margin-bottom", numberOrNull( layout.getProperty( "OFFSETS:after" ) ) );

#if 0
    switch ( lineSpacingType ) {
    case KoParagLayout::LS_SINGLE:
        gs.addProperty( "fo:line-height", "100%" );
        break;
    case KoParagLayout::LS_ONEANDHALF:
        gs.addProperty( "fo:line-height", "150%" );
        break;
    case KoParagLayout::LS_DOUBLE:
        gs.addProperty( "fo:line-height", "200%" );
        break;
    case KoParagLayout::LS_MULTIPLE:
        gs.addProperty( "fo:line-height", QString::number( lineSpacing * 100.0 ) + '%' );
        break;
    case KoParagLayout::LS_FIXED:
        gs.addPropertyPt( "fo:line-height", lineSpacing );
        break;
    case KoParagLayout::LS_CUSTOM:
        gs.addPropertyPt( "style:line-spacing", lineSpacing );
        break;
    case KoParagLayout::LS_AT_LEAST:
        gs.addPropertyPt( "style:line-height-at-least", lineSpacing );
        break;
    }
#endif

#if 0
    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter tabsWriter( &buffer, 4 ); // indent==4: root,autostyle,style,parag-props
    tabsWriter.startElement( "style:tab-stops" );
    KoTabulatorList::ConstIterator it = m_tabList.begin();
    for ( ; it != m_tabList.end() ; it++ )
    {
        tabsWriter.startElement( "style:tab-stop" );
        tabsWriter.addAttributePt( "style:position", (*it).ptPos );

        switch ( (*it).type ) {
        case T_LEFT:
            tabsWriter.addAttribute( "style:type", "left" );
            break;
        case T_CENTER:
            tabsWriter.addAttribute( "style:type", "center" );
            break;
        case T_RIGHT:
            tabsWriter.addAttribute( "style:type", "right" );
            break;
        case T_DEC_PNT:  // "alignment on decimal point"
            tabsWriter.addAttribute( "style:type", "char" );
            tabsWriter.addAttribute( "style:char", QString( (*it).alignChar ) );
            break;
        case T_INVALID: // keep compiler happy, this can't happen
            break;
        }
        switch( (*it).filling ) {
        case TF_BLANK:
            tabsWriter.addAttribute( "style:leader-type", "none" );
            break;
        case TF_LINE:
            tabsWriter.addAttribute( "style:leader-type", "single" );
            tabsWriter.addAttribute( "style:leader-style", "solid" );
            // Give OOo a chance to show something, since it doesn't support lines here.
            tabsWriter.addAttribute( "style:leader-text", "_" );
            break;
        case TF_DOTS:
            tabsWriter.addAttribute( "style:leader-type", "single" );
            tabsWriter.addAttribute( "style:leader-style", "dotted" );
            // Give OOo a chance to show something, since it doesn't support lines here.
            tabsWriter.addAttribute( "style:leader-text", "." );
            break;
        case TF_DASH:
            tabsWriter.addAttribute( "style:leader-type", "single" );
            tabsWriter.addAttribute( "style:leader-style", "dash" );
            // Give OOo a chance to show something, since it doesn't support lines here.
            tabsWriter.addAttribute( "style:leader-text", "_" );
            break;
        case TF_DASH_DOT:
            tabsWriter.addAttribute( "style:leader-type", "single" );
            tabsWriter.addAttribute( "style:leader-style", "dot-dash" );
            // Give OOo a chance to show something, since it doesn't support lines here.
            tabsWriter.addAttribute( "style:leader-text", "." );
            break;
        case TF_DASH_DOT_DOT:
            tabsWriter.addAttribute( "style:leader-type", "single" );
            tabsWriter.addAttribute( "style:leader-style", "dot-dot-dash" );
            // Give OOo a chance to show something, since it doesn't support lines here.
            tabsWriter.addAttribute( "style:leader-text", "." );
            break;
        }
        if ( (*it).filling != TF_BLANK )
            tabsWriter.addAttributePt( "style:leader-width", (*it).ptWidth );
        // If we want to support it, oasis also defines style:leader-color
        tabsWriter.endElement();
    }
    tabsWriter.endElement();
    buffer.close();
    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    gs.addChildElement( "style:tab-stops", elementContents );
#endif

#if 0
    bool fourBordersEqual = leftBorder.penWidth() > 0 &&
               leftBorder == rightBorder && rightBorder == topBorder && topBorder == bottomBorder;
    if ( fourBordersEqual ) {
        gs.addProperty( "fo:border", leftBorder.saveFoBorder() );
    } else {
        if ( leftBorder.penWidth() > 0 )
            gs.addProperty( "fo:border-left", leftBorder.saveFoBorder() );
        if ( rightBorder.penWidth() > 0 )
            gs.addProperty( "fo:border-right", rightBorder.saveFoBorder() );
        if ( topBorder.penWidth() > 0 )
            gs.addProperty( "fo:border-top", topBorder.saveFoBorder() );
        if ( bottomBorder.penWidth() > 0 )
            gs.addProperty( "fo:border-bottom", bottomBorder.saveFoBorder() );
    }
#endif

#if 0
    if ( pageBreaking & KoParagLayout::HardFrameBreakBefore )
        gs.addProperty( "fo:break-before", "column" );
    else if ( pageBreaking & KoParagLayout::HardFrameBreakAfter )
        gs.addProperty( "fo:break-after", "column" );
    if ( pageBreaking & KoParagLayout::KeepLinesTogether )
        gs.addProperty( "fo:keep-together", "always" );
    if ( pageBreaking & KoParagLayout::KeepWithNext )
        gs.addProperty( "fo:keep-with-next", "always" );
#endif
}

// Inspired by KoTextParag::saveOasis
void KWord13OasisGenerator::generateTextFrameset( KoXmlWriter& writer, KWordTextFrameset* frameset, bool /*main*/ )
{
    if ( ! frameset )
    {
        kWarning(30520) << "Tried to generate a NULL text frameset!" << endl;
        return;
    }

    for ( Q3ValueList<KWord13Paragraph>::Iterator it = frameset->m_paragraphGroup.begin();
        it != frameset->m_paragraphGroup.end(); ++it)
    {
        // Write rawly the paragrapgh (see KoTextParag::saveOasis)
        writer.startElement( "text:p", false ); // No indent inside!
        writer.addAttribute( "text:style-name", (*it).m_layout.m_autoStyleName );
#if 1
        const QString paragraphText( (*it).text() );
        int currentPos = 0; // Current position where the next character has to be written

        for ( KWord13Format* format = (*it).m_formats.first(); format; format = (*it).m_formats.next() )
        {
            // Perhaps we have text before the format's position
            const int pos = format->m_pos;
            const int length = format->length();
            if ( currentPos < pos )
            {
                writer.addTextSpan( paragraphText.mid( currentPos, pos - currentPos ) );
                currentPos = pos;
            }
            // Now we have to write the text belonging to the format
            KWord13FormatOneData* data = format->getFormatOneData();
            if ( data && format->m_id == 1 )
            {    // Normal text
                writer.startElement( "text:span" );
                writer.addAttribute( "text:style-name", data->m_autoStyleName );
                writer.addTextSpan( paragraphText.mid( pos, length ) );
                writer.endElement();
            }
            else if ( format->m_id == 3 )
            {    // Old tabulator
                // ### TEMPORARY: do it with KWord13FormatOneData
                writer.addTextSpan("\t"); // Tabulator
            }
            else if ( format->m_id == 4 )
            {    // Variable
                // ### TEMPORARY
                const QString text ( ( (KWord13FormatFour*) format ) -> m_text );
                if ( text.isEmpty() )
                    writer.addTextNode( "#" ); // Placeholder
                else
                    writer.addTextSpan( text );
            }
            else
            {
                // ### TEMPORARY
                writer.addTextNode("#"); // Placeholder
            }
            currentPos += length;
        }
        // We might have still something to write out
        const QString tailText( paragraphText.mid( currentPos ) );
        if ( ! tailText.isEmpty() )
            writer.addTextSpan( tailText );
#else
        writer.addTextSpan( (*it).text() );
#endif
        writer.endElement(); // text:p
    }
}

// Inspired by KWDocument::saveOasisDocumentStyles
void KWord13OasisGenerator::writeStylesXml( void )
{
    if ( !m_store || !m_kwordDocument )
    {
        kError(30520) << "Not possible to generate style.xml" << endl;
        return;
    }

    m_store->open("styles.xml"); // ### TODO: check error!
    KoStoreDevice io ( m_store );
    io.open( QIODevice::WriteOnly );  // ### TODO: check error!

    KoXmlWriter *stylesWriter = KoDocument::createOasisXmlWriter( &io, "office:document-styles" );

    stylesWriter->startElement( "office:styles" );
    Q3ValueList<KoGenStyles::NamedStyle> styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_USER );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, m_oasisGenStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    stylesWriter->endElement(); // office:styles

    stylesWriter->startElement( "office:automatic-styles" );
#if 0
    styles = m_oasisGenStyles.styles( KWDocument::STYLE_FRAME );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, m_oasisGenStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }
#endif

    QString pageLayoutName;
    styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
    Q_ASSERT( styles.count() == 1 );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, m_oasisGenStyles, "style:page-layout", (*it).name, "style:page-layout-properties", false /*don't close*/ );
        //if ( m_pageLayout.columns > 1 ) TODO add columns element. This is a bit of a hack,
        // which only works as long as we have only one page master
        stylesWriter->endElement();
        Q_ASSERT( pageLayoutName.isEmpty() ); // if there's more than one pagemaster we need to rethink all this
        pageLayoutName = (*it).name;
    }



    stylesWriter->endElement(); // office:automatic-styles

    stylesWriter->startElement( "office:master-styles" );
    stylesWriter->startElement( "style:master-page" );
    stylesWriter->addAttribute( "style:name", "Standard" );
    stylesWriter->addAttribute( "style:page-layout-name", pageLayoutName );
    stylesWriter->endElement();
    stylesWriter->endElement(); // office:master-styles

    stylesWriter->endElement(); // root element (office:document-styles)
    stylesWriter->endDocument();
    io.close();
    m_store->close();
    delete stylesWriter;

    if ( m_manifestWriter )
    {
        m_manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    }

}

// Inspired by KWDocument::saveOasis
void KWord13OasisGenerator::writeContentXml(void)
{
    if ( !m_store || !m_kwordDocument )
    {
        kError(30520) << "Not possible to generate content.xml" << endl;
        return;
    }

    m_store->open("content.xml"); // ### TODO: check error!
    KoStoreDevice io ( m_store );
    io.open( QIODevice::WriteOnly );  // ### TODO: check error!

    KoXmlWriter *writer = KoDocument::createOasisXmlWriter( &io, "office:document-content" );


    // Automatic styles
    writer->startElement( "office:automatic-styles" );
    Q3ValueList<KoGenStyles::NamedStyle> styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_AUTO );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it;
    for ( it = styles.begin(); it != styles.end() ; ++it ) {
        (*it).style->writeStyle( writer, m_oasisGenStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_LIST );
    for ( it = styles.begin(); it != styles.end() ; ++it ) {
        (*it).style->writeStyle( writer, m_oasisGenStyles, "text:list-style", (*it).name, 0 );
    }
    writer->endElement(); // office:automatic-styles

    writer->startElement( "office:body" );
    writer->startElement( "office:text" );

    // ### TODO: check that there is at least a normal text frameset
    generateTextFrameset( *writer, m_kwordDocument->m_normalTextFramesetList.first(), true );

    writer->endElement(); // office:text
    writer->endElement(); // office:body


    // ### TODO

    writer->endElement();
    writer->endDocument();
    io.close();
    delete writer;
    m_store->close();

    if ( m_manifestWriter )
    {
        m_manifestWriter->addManifestEntry( "content.xml", "text/xml" );
    }
}

void KWord13OasisGenerator::writeMetaXml(void)
{
    if ( !m_store || !m_kwordDocument )
    {
        kError(30520) << "Not possible to generate meta.xml" << endl;
        return;
    }

    m_store->open("meta.xml"); // ### TODO: check error!
    KoStoreDevice io ( m_store );
    io.open( QIODevice::WriteOnly );  // ### TODO: check error!

    KoXmlWriter *writer = KoDocument::createOasisXmlWriter( &io, "office:document-meta" );

    writer->startElement( "office:meta" );

    // Tell who we are in case that we have a bug in our filter output!
    // According to OASIS spec section 3.1.1, it has to follow section 14.43 of RFC 2616
    writer->startElement( "meta:generator" );
    QString strVersion;
    strVersion += "KWord-OneDotThree-Import-Filter/";
    strVersion += QString( "$Revision$" ).mid( 10 ).remove( '$' ).trimmed();
    strVersion += " KOffice/";
    strVersion += KOFFICE_VERSION_STRING;
    writer->addTextSpan( strVersion );
    writer->endElement();

    QString str; // Helper string

    str = m_kwordDocument->getDocumentInfo( "about:title" );
    if ( !str.isEmpty() )
    {
        writer->startElement( "dc:title" );
        writer->addTextSpan( str );
        writer->endElement();
    }

    str = m_kwordDocument->getDocumentInfo( "about:abstract" );
    if (!str.isEmpty())
    {
        writer->startElement( "dc:description");
        writer->addTextSpan( str );
        writer->endElement();
    }

    str = m_kwordDocument->getDocumentInfo( "author:full-name" );
    if ( !str.isEmpty() )
    {
        writer->startElement( "dc:creator" );
        writer->addTextSpan( str );
        writer->endElement();
    }

    // ### TODO: what about the other document info of KWord 1.3?

    QDateTime dt;

    dt = m_kwordDocument->creationDate();
    if ( dt.isValid() )
    {
        writer->startElement( "meta:creation-date");
        writer->addTextNode( dt.toString( Qt::ISODate) );
        writer->endElement();
    }

    dt = m_kwordDocument->modificationDate();
    if ( dt.isValid() )
    {
        writer->startElement( "dc:date");
        writer->addTextNode( dt.toString( Qt::ISODate) );
        writer->endElement();
    }

    dt = m_kwordDocument->lastPrintingDate();
    if ( dt.isValid() )
    {
        writer->startElement( "meta:print-date");
        writer->addTextNode( dt.toString( Qt::ISODate) );
        writer->endElement();
    }

    writer->startElement( "meta:document-statistic" );

    // KWord files coming from import filters mostly do not have any page count
    const int numPages = m_kwordDocument->getProperty( "PAPER:pages" ).toInt();
    if ( numPages > 0 )
    {
        writer->addAttribute( "meta:page-count", numPages );
    }

#if 0
    zipWriteData( " meta:image-count=\"" ); // This is not specified in the OO specification section 2.1.19, fixed in OASIS (### TODO)
#if 1
    zipWriteData( "0" ); // ### TODO
#else
    zipWriteData( QString::number ( m_pictureNumber ) );
#endif
    zipWriteData( "\"" );

    zipWriteData( " meta:table-count=\"" );
#if 1
    zipWriteData( "0" ); // ### TODO
#else
    zipWriteData( QString::number ( m_tableNumber ) );
#endif
    zipWriteData( "\"" );
#endif
    writer->endElement(); // meta:document-statistic
    writer->endElement(); // office:meta

    writer->endElement();
    writer->endDocument();
    delete writer;

    io.close();
    m_store->close();

    if ( m_manifestWriter )
    {
        m_manifestWriter->addManifestEntry( "meta.xml", "text/xml" );
    }
}

void KWord13OasisGenerator::writePreviewFile(void)
{
    if ( !m_store || !m_kwordDocument )
    {
        kError(30520) << "Not possible to generate preview file" << endl;
        return;
    }

    // Load image
    QImage image( m_kwordDocument->m_previewFile->name() );
    if ( image.isNull() )
    {
        kWarning(30520) << "Could not re-read preview from temp file!" << endl;
        return;
    }

    // We have a 256x256x8 preview and we need a 128x128x32 preview with alpha channel
    QImage preview( image.convertDepth( 32, Qt::ColorOnly ).smoothScale( 128, 128 ) );
    if ( preview.isNull() )
    {
        kWarning(30520) << "Could not create preview!" << endl;
        return;
    }
    if ( !preview.hasAlphaBuffer() )
    {
        preview.setAlphaBuffer( true );
    }
    m_store->open("Thumbnails/thumbnail.png");
    KoStoreDevice io ( m_store );
    io.open( QIODevice::WriteOnly );  // ### TODO: check error!
    preview.save( &io, "PNG", 0 );
    io.close();
    m_store->close();

    // No manifest entry, as it is supposed not to be part of the document.
}

void KWord13OasisGenerator::writePictures( void )
{
    if ( !m_store || !m_kwordDocument )
    {
        kError(30520) << "Not possible to generate preview file" << endl;
        return;
    }

    for ( Q3DictIterator<KWord13Picture> it( m_kwordDocument->m_pictureDict ) ; it.current(); ++it )
    {
        if ( !it.current()->m_valid || !it.current()->m_tempFile )
        {
            kDebug(30520) << "No data for picture: " << it.currentKey() << endl;
            continue;
        }
        const QString fileName( it.current()->m_tempFile->name() );
        const QString oasisName( it.current()->getOasisPictureName() );
        kDebug(30520) << "Copying... " << it.currentKey() << endl << " => " << oasisName << endl;
        QFile file( fileName );
        if ( !file.open( QIODevice::ReadOnly ) )
        {
            kWarning(30520) << "Cannot open: " << fileName << endl;
            continue;
        }
        QByteArray array( file.readAll() );
        if ( array.isNull() )
        {
            kWarning(30520) << "Null picture for " << fileName << endl;
            file.close();
            continue;
        }
        file.close();

        m_store->open( oasisName );
        m_store->write( array );
        m_store->close();

        if ( m_manifestWriter )
        {
            const QString mimeType ( KMimeType::findByContent( array, 0 )->name() );
            if ( mimeType == "application/octet-stream" )
            {
                kWarning(30520) << "Generic mime type for " << it.currentKey() << endl;
                // ### TODO: try harder to find a mime type
            }
            m_manifestWriter->addManifestEntry( oasisName, mimeType );
        }

    }

}

bool KWord13OasisGenerator::generate ( const QString& fileName, KWord13Document& kwordDocument )
{
    if ( m_kwordDocument && ( (void*) m_kwordDocument ) != ( (void*) &kwordDocument ) )
    {
        kWarning(30520) << "KWord Document is different!" <<endl;
    }

    m_kwordDocument = &kwordDocument;

    m_store = KoStore::createStore( fileName, KoStore::Write, "application/vnd.sun.xml.writer", KoStore::Zip );
    if ( ! m_store )
    {
        kError(30520) << "Cannot create output KoStore" << endl;
        return false;
    }
    m_store->disallowNameExpansion();

    // Prepare manifest file - in memory (inspired by KoDocument::saveNativeFormat)
    QByteArray manifestData;
    QBuffer manifestBuffer( &manifestData );
    manifestBuffer.open( QIODevice::WriteOnly );
    m_manifestWriter = new KoXmlWriter( &manifestBuffer );
    m_manifestWriter->startDocument( "manifest:manifest" );
    m_manifestWriter->startElement( "manifest:manifest" );
    m_manifestWriter->addAttribute( "xmlns:manifest", "urn:oasis:names:tc:openoffice:xmlns:manifest:1.0" );

    // ### TODO: check if writing the store is successful

    writeStylesXml();
    writeContentXml();
    writeMetaXml();
    writePictures();

        // Write out manifest file
    m_manifestWriter->endElement();
    m_manifestWriter->endDocument();
    delete m_manifestWriter;
    m_manifestWriter = 0;
    if ( m_store->open( "META-INF/manifest.xml" ) )
    {
        m_store->write( manifestData );
        m_store->close();
    }

    if ( kwordDocument.m_previewFile )
    {
        writePreviewFile();
    }
    else
    {
        kDebug(30520) << "No preview file available to make an OASIS thumbnail!" << endl;
    }


# ifndef NDEBUG // DEBUG (out of specification)
    // No entry in the manifest file, as it is not part of the document
    if ( m_store->open("Debug/debug.xml") )
    {
        KoStoreDevice io ( m_store );
        io.open( QIODevice::WriteOnly );  // ### TODO: check error!
        kwordDocument.xmldump( &io );
        io.close();
        m_store->close();
    }
# endif


    delete m_store;
    m_store = 0;
    return true;
}

