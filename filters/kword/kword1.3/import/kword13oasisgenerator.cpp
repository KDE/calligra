//

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qstring.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <kzip.h>

#include <kofficeversion.h>
#include <koStore.h>
#include <koStoreDevice.h>
#include <koxmlwriter.h>
#include <koGenStyles.h>

#include "kword13document.h"

#include "kword13oasisgenerator.h"

KWord13OasisGenerator::KWord13OasisGenerator( void ) : m_kwordDocument( 0 ), m_zip( 0 ), m_store( 0 ), m_streamOut( 0 )
{
}

KWord13OasisGenerator::~KWord13OasisGenerator( void )
{
}

void KWord13OasisGenerator::prepareTextFrameset( KWordTextFrameset* frameset )
{
    if ( ! frameset )
    {
        kdWarning(30520) << "Tried to prepare a NULL text frameset!" << endl;
        return;
    }
    
    for ( QValueList<KWordParagraph>::Iterator it = frameset->m_paragraphGroup.begin();
        it != frameset->m_paragraphGroup.end(); ++it)
    {
        declareLayout( (*it).m_layout );
    }
}

bool KWord13OasisGenerator::prepare( KWord13Document& kwordDocument )
{
    if ( !m_kwordDocument )
    {
        kdError(30520) << "No document declared!" << endl;
    }
    else if ( m_kwordDocument && ( (void*) m_kwordDocument ) != ( (void*) &kwordDocument ) )
    {
        kdWarning(30520) << "KWord Document is different!" <<endl;
    }
    
    m_kwordDocument = &kwordDocument;

    // Declare styles
    for ( QValueList<KWord13Layout>::Iterator it = m_kwordDocument->m_styles.begin();
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
    fillGenStyleWithLayout( layout, gs, true );
    fillGenStyleWithFormatOne( layout.m_format , gs, true );

    layout.m_autoStyleName = m_oasisGenStyles.lookup( gs, "P", true );
    
    kdDebug(30520) << "Layout: Parent " << layout.m_name << " => " << layout.m_autoStyleName << endl;
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
    
    kdDebug(30520) << "Style: " << layout.m_name << " => " << layout.m_autoStyleName << endl;
}


// Inspired from KoTextFormat::save but we have not the same data to start with.
void KWord13OasisGenerator::fillGenStyleWithFormatOne( const KWord13FormatOne& one, KoGenStyle& gs, const bool style ) const
{
    QString str; // helper string
    
    KoGenStyle::PropertyType tt = KoGenStyle::TextType;
#if 0
    gs.addProperty( "fo:color", col.isValid() ? col.name() : "#000000", tt );
#endif
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
    
#if 0
    int w = fn.weight();
    gs.addProperty( "fo:font-weight", w == 50 ? "normal" : w == 75 ? "bold" : QString::number( w * 10 ), tt );
    gs.addProperty( "fo:font-style", fn.italic() ? "italic" : "normal", tt );
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
    buffer.open( IO_WriteOnly );
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


QString KWord13OasisGenerator::escapeOOText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Escape apostrophs (allowed by XML)

    QString strReturn;
    QChar ch;

    for (uint i=0; i<strText.length(); i++)
    {
        ch=strText[i];
        switch (ch.unicode())
        {
        case 38: // &
            {
                strReturn+="&amp;";
                break;
            }
        case 60: // <
            {
                strReturn+="&lt;";
                break;
            }
        case 62: // >
            {
                strReturn+="&gt;";
                break;
            }
        case 34: // "
            {
                strReturn+="&quot;";
                break;
            }
        case 39: // '
            {
                strReturn+="&apos;";
                break;
            }
        default:
            {
                strReturn+=ch;
                break;
            }
        }
    }

    return strReturn;
}


QString KWord13OasisGenerator::escapeOOSpan(const QString& strText) const
// We need not only to escape the classical XML stuff but also to take care of spaces and tabs.
// Also we must take care about not falling into the rules in XML about white space between 2 opening tags or between 2 closing tags
{
    QString strReturn;
    QChar ch;
    int spaceNumber = 0; // How many spaces should be written
    uint spaceSequenceStart = 9999; // Where does the space sequence start (start value must be non-null)

    for (uint i=0; i<strText.length(); i++)
    {
        ch=strText[i];

        if (ch!=' ')
        {
            // The next character is not a space (anymore)
            if ( spaceNumber > 0 )
            {
                if ( spaceSequenceStart )
                {   // Generate a real space only if we are not at start

                    strReturn += ' ';
                    --spaceNumber;
                }
                if ( spaceNumber > 1 )
                {
                    strReturn += "<text:s text:c=\"";
                    strReturn += QString::number( spaceNumber );
                    strReturn += "\"/>";
                }
            }
            spaceNumber = 0;
        }

        // ### TODO: would be switch/case or if/elseif the best?
        switch (ch.unicode())
        {
        case 9: // Tab
            {
                strReturn+="<text:tab-stop/>";
                break;
            }
        case 10: // Line-feed
            {
                strReturn+="<text:line-break/>";
                break;
            }
        case 32: // Space
            {
                if ( spaceNumber > 0 )
                {
                    ++spaceNumber;
                }
                else
                {
                    spaceNumber = 1;
                    spaceSequenceStart = i;
                }
                break;
            }
        case 38: // &
            {
                strReturn+="&amp;";
                break;
            }
        case 60: // <
            {
                strReturn+="&lt;";
                break;
            }
        case 62: // >
            {
                strReturn+="&gt;";
                break;
            }
        case 34: // "
            {
                strReturn+="&quot;";
                break;
            }
        case 39: // '
            {
                strReturn+="&apos;";
                break;
            }
        case 1: // (Non-XML-compatible) replacement character from KWord 0.8
            {
                strReturn += '#'; //use KWord 1.[123] replacement character instead
                break;
            }
        // Following characters are not allowed in XML (but some files from KWord 0.8 have some of them.)
        case  0: case  2 ... 8: case 11: case 12: case 14 ... 31:
            {
                kdWarning(30518) << "Not allowed XML character: " << ch.unicode() << endl;
                strReturn += '?';
                break;
            }
        case 13: // ### TODO: what to do with it?
        default:
            {
                strReturn+=ch;
                break;
            }
        }
    }

    if ( spaceNumber > 0 )
    {
        // The last characters were spaces
        // We do not care about writing a real space (also to avoid to have <tag> </tag>)
        strReturn+="<text:s text:c=\"";
        strReturn+=QString::number(spaceNumber);
        strReturn+="\"/>";
    }

    return strReturn;
}

bool KWord13OasisGenerator::zipPrepareWriting(const QString& name)
{
    if (!m_zip)
        return false;
    m_size=0;
    return m_zip->prepareWriting(name, QString::null, QString::null, 0);
}

bool KWord13OasisGenerator::zipDoneWriting(void)
{
    if (!m_zip)
        return false;
    return m_zip->doneWriting(m_size);
}

bool KWord13OasisGenerator::zipWriteData(const char* str)
{
    if (!m_zip)
        return false;
    const uint size=strlen(str);
    m_size+=size;
    return m_zip->writeData(str,size);
}

bool KWord13OasisGenerator::zipWriteData(const QByteArray& array)
{
    if (!m_zip)
        return false;
    const uint size=array.size();
    m_size+=size;
    return m_zip->writeData(array.data(),size);
}

bool KWord13OasisGenerator::zipWriteData(const QCString& cstr)
{
    if (!m_zip)
        return false;
    const uint size=cstr.length();
    m_size+=size;
    return m_zip->writeData(cstr.data(),size);
}

bool KWord13OasisGenerator::zipWriteData(const QString& str)
{
    return zipWriteData(str.utf8());
}

void KWord13OasisGenerator::writeStartOfFile(const QString& type)
{
    const bool noType=type.isEmpty();
    zipWriteData("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    zipWriteData("<!DOCTYPE office:document");
    if (!noType)
    {
        // No type might happen for raw XML documents (which this filter does not support yet.)
        zipWriteData("-");
        zipWriteData(type);
    }
    zipWriteData(" PUBLIC \"-//OpenOffice.org//DTD OfficeDocument 1.0//EN\"");
    zipWriteData(" \"office.dtd\"");
    zipWriteData(">\n");

    zipWriteData("<office:document");
    if (!noType)
    {
        zipWriteData("-");
        zipWriteData(type);
    }

    // The name spaces used by OOWriter (those not used by this filter are commented out)

    // General namespaces
    zipWriteData(" xmlns:office=\"http://openoffice.org/2000/office\"");
    zipWriteData(" xmlns:xlink=\"http://www.w3.org/1999/xlink\"");

    // Namespaces for context.xml and style.xml
    if ( type == "content" || type == "styles" || type.isEmpty() )
    {
        zipWriteData(" xmlns:style=\"http://openoffice.org/2000/style\"");
        zipWriteData(" xmlns:text=\"http://openoffice.org/2000/text\"");
        zipWriteData(" xmlns:table=\"http://openoffice.org/2000/table\"");
        zipWriteData(" xmlns:draw=\"http://openoffice.org/2000/drawing\"");
        zipWriteData(" xmlns:fo=\"http://www.w3.org/1999/XSL/Format\"");

        //zipWriteData(" xmlns:number=\"http://openoffice.org/2000/datastyle\"");
        zipWriteData(" xmlns:svg=\"http://www.w3.org/2000/svg\"");
        //zipWriteData(" xmlns:chart=\"http://openoffice.org/2000/chart\"");
        //zipWriteData(" xmlns:dr3d=\"http://openoffice.org/2000/dr3d\"");
        //zipWriteData(" xmlns:math=\"http://www.w3.org/1998/Math/MathML"");
        //zipWriteData(" xmlns:form=\"http://openoffice.org/2000/form\"");
        //zipWriteData(" xmlns:script=\"http://openoffice.org/2000/script\"");
    }

    // Namespaces For meta.xml
    if ( type == "meta" || type.isEmpty() )
    {
        zipWriteData(" xmlns:dc=\"http://purl.org/dc/elements/1.1/\"");
        zipWriteData(" xmlns:meta=\"http://openoffice.org/2000/meta\"");
    }


    zipWriteData(" office:class=\"text\"");

#ifdef STRICT_OOWRITER_VERSION_1
    zipWriteData(" office:version=\"1.0\"");
#else
    // We are using an (rejected draft OASIS) extension compared to version 1.0, so we cannot write the version string.
    // (We do not even write it for context.xml and meta.xml, as OOWriter 1.0.1 does not like it in this case.)
#endif

    zipWriteData(">\n");
}

void KWord13OasisGenerator::writeStylesXml( void )
#if 1
// Inspired by KWDocument::saveOasisDocumentStyles
{
    if ( !m_store || !m_kwordDocument )
        return;

    m_store->open("meta.xml"); // ### TODO: check error!
    KoStoreDevice io ( m_store );
    io.open( IO_WriteOnly );  // ### TODO: check error!
    
    KoXmlWriter stylesWriter( &io, "office:document-styles" );

    stylesWriter.startElement( "office:styles" );
    QValueList<KoGenStyles::NamedStyle> styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_USER );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &stylesWriter, m_oasisGenStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    stylesWriter.endElement(); // office:styles

    stylesWriter.startElement( "office:automatic-styles" );
#if 0
    styles = m_oasisGenStyles.styles( KWDocument::STYLE_FRAME );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &stylesWriter, m_oasisGenStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }
#endif

    QString pageLayoutName;
    styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
    Q_ASSERT( styles.count() == 1 );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &stylesWriter, m_oasisGenStyles, "style:page-layout", (*it).name, "style:page-layout-properties", false /*don't close*/ );
        //if ( m_pageLayout.columns > 1 ) TODO add columns element. This is a bit of a hack,
        // which only works as long as we have only one page master
        stylesWriter.endElement();
        Q_ASSERT( pageLayoutName.isEmpty() ); // if there's more than one pagemaster we need to rethink all this
        pageLayoutName = (*it).name;
    }



    stylesWriter.endElement(); // office:automatic-styles

    stylesWriter.startElement( "office:master-styles" );
    stylesWriter.startElement( "style:master-page" );
    stylesWriter.addAttribute( "style:name", "Standard" );
    stylesWriter.addAttribute( "style:page-layout-name", pageLayoutName );
    stylesWriter.endElement();
    stylesWriter.endElement(); // office:master-styles

    stylesWriter.endElement(); // root element (office:document-styles)
    stylesWriter.endDocument();
    io.close();
    m_store->close();
}
#else
{
    if (!m_zip && !m_kwordDocument)
        return;

    zipPrepareWriting("styles.xml");

    writeStartOfFile("styles");

    // ### TODO writeFontDeclaration();

    // ### TODO zipWriteData(m_styles);

    zipWriteData(" <office:automatic-styles>\n");
    zipWriteData("  <style:page-master style:name=\"pm1\">\n"); // ### TODO: verify if style name is unique

    zipWriteData( "   <style:properties" );
    zipWriteData( " style:page-usage=\"all\"" ); // ### TODO: check

    zipWriteData(" fo:page-width=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPER:width", "PAPER:ptWidth" ) );
    zipWriteData("pt\" fo:page-height=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPER:height", "PAPER:ptHeight" ) );
    zipWriteData("pt\" ");

    zipWriteData("style:print-orientation=\"");
    if ( m_kwordDocument->getProperty( "PAPER:orientation" ) == "1" )
    {
        zipWriteData("landscape");
    }
    else
    {
        zipWriteData("portrait");
    }
    
    const int firstPageNumber = m_kwordDocument->getProperty( "VARIABLESETTINGS:startingPageNumber" ).toInt();

    zipWriteData("\" fo:margin-top=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:top", "PAPERBORDERS:ptTop" ) );
    zipWriteData("pt\" fo:margin-bottom=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:bottom", "PAPERBORDERS:ptBottom" ) );
    zipWriteData("pt\" fo:margin-left=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:left", "PAPERBORDERS:ptLeft" ) );
    zipWriteData("pt\" fo:margin-right=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:right", "PAPERBORDERS:ptRight" ) );
    zipWriteData("pt\" style:first-page-number=\"");
    zipWriteData(QString::number( ( firstPageNumber > 1 ) ? firstPageNumber : 1 ) );
    zipWriteData( "\">\n" );

    const int columns = m_kwordDocument->getProperty( "PAPER:columns" ).toInt();
    if ( columns > 1 )
    {
        zipWriteData( "    <style:columns" );
        zipWriteData( " fo:column-count=\"" );
        zipWriteData( QString::number( columns ) );
        zipWriteData( "\" fo:column-gap=\"" );
        zipWriteData( m_kwordDocument->getProperty( "PAPER:columnspacing", "PAPER:ptColumnspc" ) );
        zipWriteData( "pt\">\n" );

        for (int i=0; i < columns; ++i)
        {
            zipWriteData( "     <style:column style:rel-width=\"1*\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\"/>\n" );
        }

        zipWriteData( "    </style:columns>\n" );
    }

    zipWriteData("   </style:properties>\n");
    zipWriteData("  </style:page-master>\n");
    zipWriteData(" </office:automatic-styles>\n");

    zipWriteData(" <office:master-styles>\n");
    zipWriteData("  <style:master-page style:name=\"Standard\" style:page-master-name=\"pm1\" />\n");
    zipWriteData(" </office:master-styles>\n");

    zipWriteData( "</office:document-styles>\n" );

    zipDoneWriting();
}
#endif

// Inspired by KWDocument::saveOasis
void KWord13OasisGenerator::writeContentXml(void)
{
#if 1
    if ( !m_store || !m_kwordDocument )
        return;

    m_store->open("content.xml"); // ### TODO: check error!
    KoStoreDevice io ( m_store );
    io.open( IO_WriteOnly );  // ### TODO: check error!

    KoXmlWriter writer( &io, "office:document-content" );
    
    
    // Automatic styles
    writer.startElement( "office:automatic-styles" );
    QValueList<KoGenStyles::NamedStyle> styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_AUTO );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it;
    for ( it = styles.begin(); it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &writer, m_oasisGenStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    styles = m_oasisGenStyles.styles( KoGenStyle::STYLE_LIST );
    for ( it = styles.begin(); it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &writer, m_oasisGenStyles, "text:list-style", (*it).name, 0 );
    }
    writer.endElement(); // office:automatic-styles

    writer.startElement( "office:body" );
    writer.startElement( "office:text" );

    // ### TODO: write text document!
    
    writer.endElement(); // office:text
    writer.endElement(); // office:body

    
    // ### TODO
        
    writer.endElement();
    writer.endDocument();
    io.close();
    m_store->close();
#else
    if (!m_zip)
        return;

    kdDebug(30520) << "content.xml: preparing..." << endl;
    zipPrepareWriting("content.xml");

    kdDebug(30520) << "content.xml: start file..." << endl;
    writeStartOfFile("content");

    kdDebug(30520) << "content.xml: declare fonts..." << endl;
    // ### TODO writeFontDeclaration();

    kdDebug(30520) << "content.xml: writing automatical styles..." << endl;
    zipWriteData(" <office:automatic-styles>\n");

    zipWriteData(m_contentAutomaticStyles);
    m_contentAutomaticStyles = QString::null; // Release memory

    zipWriteData(" </office:automatic-styles>\n");

    kdDebug(30520) << "content.xml: writing body..." << endl;
    zipWriteData("<office:body>\n");
    zipWriteData(m_contentBody);
    m_contentBody.resize( 0 ); // Release memory
    zipWriteData("</office:body>\n");

    zipWriteData( "</office:document-content>\n" );

    kdDebug(30520) << "content.xml: closing file..." << endl;
    zipDoneWriting();
    kdDebug(30520) << "content.xml: done!" << endl;
#endif
}

void KWord13OasisGenerator::writeMetaXml(void)
{
    if ( !m_store || !m_kwordDocument )
        return;

    m_store->open("meta.xml"); // ### TODO: check error!
    KoStoreDevice io ( m_store );
    io.open( IO_WriteOnly );  // ### TODO: check error!

    KoXmlWriter writer( &io, "office:document-meta" );
    
    writer.startElement( "office:meta" );
    
    // Tell who we are in case that we have a bug in our filter output!
    writer.startElement( "meta:generator" );
    QString strVersion = "KOffice ";
    strVersion += KOFFICE_VERSION_STRING;
    strVersion += " / KWord's OOWriter Export Filter ";
    strVersion += QString( "$Revision$" ).remove( '$' );
    writer.addTextSpan( strVersion );
    writer.endElement();
        
#if 0
    if (!m_docInfo.title.isEmpty())
    {
        writer.startElement( "dc:title" );
        writer.addTextSpan( m_docInfo.title );
        writer.endElement();
    }
    if (!m_docInfo.abstract.isEmpty())
    {
        writer.startElement( "dc:description");
        writer.addTextSpan( m_docInfo.abstract );
        writer.endElement();
    }
#endif
    
    // ### TODO

    QDateTime dt;
    
    dt = m_kwordDocument->creationDate();
    if ( dt.isValid() )
    {
        writer.startElement( "meta:creation-date");
        writer.addTextSpan( dt.toString( Qt::ISODate) );
        writer.endElement();
    }

    dt = m_kwordDocument->modificationDate();
    if ( dt.isValid() )
    {
        writer.startElement( "dc:date");
        writer.addTextSpan( dt.toString( Qt::ISODate) );
        writer.endElement();
    }

    dt = m_kwordDocument->lastPrintingDate();
    if ( dt.isValid() )
    {
        writer.startElement( "meta:print-date");
        writer.addTextSpan( dt.toString( Qt::ISODate) );
        writer.endElement();
    }
    
#if 0
    zipWriteData( "  <meta:document-statistic" );

    // KWord files coming from import filters mostly do not have no page count
    const int numPages = m_kwordDocument->getProperty( "PAPER:pages" ).toInt();
    if ( numPages > 0 )
    {
        zipWriteData( " meta:page-count=\"" );
        zipWriteData( QString::number ( numPages ) );
        zipWriteData( "\"" );
    }

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
    zipWriteData( "/>\n" ); // meta:document-statistic
#endif    
    writer.endElement();
    writer.endDocument();
    
    io.close();
    m_store->close();
}

bool KWord13OasisGenerator::generate ( const QString& fileName, KWord13Document& kwordDocument )
{
#if 1
    if ( m_kwordDocument && ( (void*) m_kwordDocument ) != ( (void*) &kwordDocument ) )
    {
        kdWarning(30520) << "KWord Document is different!" <<endl;
    }
    
    m_kwordDocument = &kwordDocument;
    
    KoStore* m_store = KoStore::createStore( fileName, KoStore::Write, "application/vnd.sun.xml.writer", KoStore::Zip );
    if ( ! m_store )
    {
        kdError(30520) << "Cannot create output KoStore" << endl;
        return false;
    }
    m_store->disallowNameExpansion();
    
    writeStylesXml();
    writeContentXml();
    writeMetaXml();
    
# if 1 // DEBUG
    m_store->open("debug.xml"); // ### TODO: check error!
    KoStoreDevice io ( m_store );
    io.open( IO_WriteOnly );  // ### TODO: check error!
    kwordDocument.xmldump( &io );
    io.close();
    m_store->close();
# endif
    
    
    delete m_store;
    m_store = 0;
#else
    m_streamOut = new QTextStream( m_contentBody, IO_WriteOnly );
    m_streamOut->setEncoding( QTextStream::UnicodeUTF8 );
    
    // ### TODO
    
    if ( m_kwordDocument && ( (void*) m_kwordDocument ) != ( (void*) &kwordDocument ) )
    {
        kdWarning(30520) << "KWord Document is different!" <<endl;
    }
    
    m_kwordDocument = &kwordDocument;
    
    m_zip = new KZip( fileName ); // How to check failure?

    if (!m_zip->open(IO_WriteOnly))
    {
        kdError(30520) << "Could not open ZIP file for writing! Aborting!" << endl;
        delete m_zip;
        m_zip=NULL;
        return false;
    }

    m_zip->setCompression( KZip::NoCompression );
    m_zip->setExtraField( KZip::NoExtraField );

    const QCString appId( "application/vnd.sun.xml.writer" );

    m_zip->writeFile( "mimetype", QString::null, QString::null, appId.length(), appId.data() );

    m_zip->setCompression( KZip::DeflateCompression );

    // ### TODO
    if (m_zip)
    {
        kdDebug(30520) << "Writing content..." << endl;
        writeContentXml();
        kdDebug(30520) << "Writing meta..." << endl;
    // ### TODO        writeMetaXml();
        kdDebug(30520) << "Writing styles..." << endl;
    // ### TODO        writeStylesXml();
        kdDebug(30520) << "Closing ZIP..." << endl;
        m_zip->close();
    }
    kdDebug(30520) << "Deleting ZIP..." << endl;
    delete m_zip;
    m_zip=NULL;
#endif    
    return true;
}

