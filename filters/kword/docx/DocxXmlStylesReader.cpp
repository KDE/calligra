/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "DocxXmlStylesReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

#define MSOOXML_CURRENT_NS "w"
#define BIND_READ_CLASS MSOOXML::MsooXmlStylesReader
#define MSOOXML_CURRENT_CLASS DocxXmlStylesReader

#include <MsooXmlReader_p.h>

DocxXmlStylesReader::DocxXmlStylesReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlReader(writers)
{
}

DocxXmlStylesReader::DocxXmlStylesReader(QIODevice* io, KoOdfWriters *writers)
    : MSOOXML::MsooXmlReader(io, writers)
{
}

DocxXmlStylesReader::~DocxXmlStylesReader()
{
}

KoFilter::ConversionStatus DocxXmlStylesReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    Q_UNUSED(context)
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    //w:document
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("w:styles")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }
/*
    const QXmlStreamAttributes attrs( attributes() );
    for (int i=0; i<attrs.count(); i++) {
        kDebug() << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
    }*/    

    QXmlStreamNamespaceDeclarations namespaces( namespaceDeclarations() );
    for (int i=0; i<namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri() 
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains( QXmlStreamNamespaceDeclaration( "w", MSOOXML::Schemas::wordprocessingml ) )) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::wordprocessingml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

//! @todo use KoStyleManager::saveOdfDefaultStyles()
    m_defaultParagraphStyle = KoGenStyle(KoGenStyle::StyleUser, "paragraph");
    m_defaultParagraphStyle.setDefaultStyle(true);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(docDefaults)
        }
        BREAK_IF_END_OF(styles)
    }

    if (!expectElEnd("w:styles")) {
        return KoFilter::WrongFormat;
    }

    // add styles:
    mainStyles->lookup(m_defaultParagraphStyle);
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL docDefaults
//! w:docDefaults handler (Document Default Paragraph and Run Properties)
/*! ECMA-376, 17.5.5.1, p.723.
 Parent elements:
 - styles
 Child elements:
 - [done] pPrDefault
 - [done] rPrDefault

 CASE #850
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_docDefaults()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(pPrDefault)
            ELSE_TRY_READ_IF(rPrDefault)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPrDefault
//! w:pPrDefault handler (Default Paragraph Properties))
/*! ECMA-376, 17.7.5.3, p.726.
 Parent elements:
 - [done] docDefault
 Child elements:
 - [done] pPr

 CASE #850
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_pPrDefault()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(pPr)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPrDefault
//! w:rPrDefault handler (Default Run Properties)
/*! ECMA-376, 17.7.5.5, p.729.
 Parent elements:
 - [done] docDefault
 Child elements:
 - [done] rPr

 CASE #850
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_rPrDefault()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(rPr)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPr
//! w:pPr  handler (Paragraph Properties)
/*! ECMA-376, 17.7.5.3, p.726.
 Parent elements:
 - [done] pPrDefault
 Child elements:
 -

 CASE #850 -> CASE #853
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlStylesReader::read_pPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPr
//! w:rPr handler (Run Properties)
/*! ECMA-376, 17.7.5.4, p.727.
 Parent elements:
 - [done] rPrDefault
 Child elements:
 - b
 - bCs
 - bdr
 - caps
 - color
 - cs
 - dstrike
 - eastAsianLayout
 - effect
 - em
 - emboss
 - fitText
 - highlight
 - i
 - iCs
 - imprint
 - kern
 - [done] lang
 - noProof
 - oMath
 - outline
 - position
 - rFonts
 - rPrChange
 - rStyle
 - rtl
 - shadow
 - shd
 - smallCaps
 - snapToGrid
 - spacing
 - specVanish
 - strike
 - sz
 - szCs
 - u
 - vanish
 - vertAlign
 - w
 - webHidden

 CASE #850 -> CASE #858 -> CASE #861
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlStylesReader::read_rPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
// CASE #850 -> CASE #858 -> CASE #861 -> CASE #1100

            TRY_READ_IF(lang)
            ELSE_TRY_READ_IF(rFonts)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL lang
//! w:lang handler (Languages for Run Content)
/*! ECMA-376, 17.3.2.20, p.314.
 Parent elements:
 - [done] rPr (§17.3.1.29) (within p)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - rPr (§17.7.9.1)
 - [done] rPr (§17.7.5.4) (within style)
 - rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)
 No child elements.

 CASE #850 -> CASE #858 -> CASE #861 -> CASE #1100
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_lang()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
// CASE #1100    
    TRY_READ_ATTR(bidi)
    QString language, country;
    if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(bidi, language, country)) {
        m_defaultParagraphStyle.addProperty("style:language-complex", language, KoGenStyle::TextType);
        m_defaultParagraphStyle.addProperty("style:country-complex", country, KoGenStyle::TextType);
    }
    else {
        kWarning() << "invalid value of \"bidi\" attribute:" << bidi << " - skipping";
    }
    TRY_READ_ATTR(val)
    if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(val, language, country)) {
        m_defaultParagraphStyle.addProperty("fo:language", language, KoGenStyle::TextType);
        m_defaultParagraphStyle.addProperty("fo:country", country, KoGenStyle::TextType);
    }
    else {
        kWarning() << "invalid value of \"val\" attribute:" << val << " - skipping";
    }

    TRY_READ_ATTR(eastAsia)
    if (MSOOXML::Utils::ST_Lang_to_languageAndCountry(eastAsia, language, country)) {
        m_defaultParagraphStyle.addProperty("style:language-asian", language, KoGenStyle::TextType);
        m_defaultParagraphStyle.addProperty("style:country-asian", country, KoGenStyle::TextType);
    }
    else {
        kWarning() << "invalid value of \"eastAsia\" attribute:" << eastAsia << " - skipping";
    }
    //kDebug() << "bidi:" << bidi << "val:" << val << "eastAsia:" << eastAsia;

    readNext();
    READ_EPILOGUE
}

//! @todo use  themeFontName:
#if 0
// CASE #1200
static QString themeFontName(const QString& asciiTheme)
{
    if (asciiTheme.contains(QLatin1String("minor"))) {
    }
    else if (asciiTheme.contains(QLatin1String("major"))) {
    }
    return QString();
}
#endif

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL rFonts
//! w:lang handler (Run Fonts)
/*! ECMA-376, 17.3.2.26, p.323.
 Parent elements:
 - rPr (§17.3.1.29) (within p)
 - rPr (§17.3.1.30)
 - rPr (§17.5.2.28)
 - rPr (§17.9.25)
 - rPr (§17.7.9.1)
 - rPr (§17.7.5.4) (within style)
 - rPr (§17.3.2.28)
 - rPr (§17.5.2.27)
 - rPr (§17.7.6.2)
 - rPr (§17.3.2.27)
 No child elements.

 CASE #850 -> CASE #858 -> CASE #861 -> CASE #1150
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_rFonts()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs( attributes() );
// CASE #1153    
    TRY_READ_ATTR(ascii)
    m_defaultParagraphStyle.addProperty("style:font-name", ascii, KoGenStyle::TextType);
// CASE #1155
    TRY_READ_ATTR(cs)
    m_defaultParagraphStyle.addProperty("style:font-name-complex", cs, KoGenStyle::TextType);
// CASE #1157
    TRY_READ_ATTR(eastAsia)
    m_defaultParagraphStyle.addProperty("style:font-name-asian", eastAsia, KoGenStyle::TextType);

    readNext();
    READ_EPILOGUE
}
