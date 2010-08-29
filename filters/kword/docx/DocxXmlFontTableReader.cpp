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

#include "DocxXmlFontTableReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#define MSOOXML_CURRENT_NS "w"
#define BIND_READ_CLASS MSOOXML::MsooXmlStylesReader
#define MSOOXML_CURRENT_CLASS DocxXmlFontTableReader

#include <MsooXmlReader_p.h>

DocxXmlFontTableReaderContext::DocxXmlFontTableReaderContext(KoGenStyles& _styles)
        : styles(&_styles)
{
}

DocxXmlFontTableReader::DocxXmlFontTableReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
{
}

DocxXmlFontTableReader::DocxXmlFontTableReader(QIODevice* io, KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(io, writers)
        , m_context(0)
{
}

DocxXmlFontTableReader::~DocxXmlFontTableReader()
{
}

KoFilter::ConversionStatus DocxXmlFontTableReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<DocxXmlFontTableReaderContext*>(context);
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    //w:document
    readNext();
    kDebug() << namespaceUri();

    if (!expectEl("w:fonts")) {
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

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::wordprocessingml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(fonts)

    if (!expectElEnd("w:fonts")) {
        return KoFilter::WrongFormat;
    }

    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL fonts
//! fonts handler (Font Table Root Element)
/*! ECMA-376, 17.8.3.11, p.773.
 This element specifies the root element for a font table part within a WordprocessingML document,
 and specifies information about the fonts used in this document, each contained within a child
 font element.

 Root element.

 Child elements:
 - [done] font (Properties for a Single Font) §17.8.3.10
*/
//! CASE #001
//! CASE #701
KoFilter::ConversionStatus DocxXmlFontTableReader::read_fonts()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(font)) {
                TRY_READ(font)
                if (m_currentFontFace.isNull())
                    return KoFilter::WrongFormat;
                kDebug() << "added font face:" << m_currentFontFace.name();
                m_context->styles->insertFontFace(m_currentFontFace);
                m_currentFontFace = KoFontFace();
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL font
//! font handler (Properties for a Single Font)
/*! ECMA-376, 17.8.3.10, p.771.
 This element specifies the properties for one of the fonts used in this document.

 Parent elements:
 - [done] fonts (§17.8.3.11)

 Child elements:
 - altName (Alternate Names for Font) §17.8.3.1
 - charset (Character Set Supported By Font) §17.8.3.2
 - embedBold (Bold Style Font Style Embedding) §17.8.3.3
 - embedBoldItalic (Bold Italic Font Style Embedding) §17.8.3.4
 - embedItalic (Italic Font Style Embedding) §17.8.3.5
 - embedRegular (Regular Font Style Embedding) §17.8.3.6
 - [done] family (Font Family) §17.8.3.9
 - notTrueType (Raster or Vector Font) §17.8.3.12
 - panose1 (Panose-1 Typeface Classification Number) §17.8.3.13
 - [done] pitch (Font Pitch) §17.8.3.14
 - sig (Supported Unicode Subranges and Code Pages) §17.8.3.16
*/
//! CASE #850
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlFontTableReader::read_font()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
// CASE #1100
    /*! Specifies the primary name of the current font.
        This name shall be used to link the information stored
        in this element with uses of this value in the rFonts element
        (§17.3.2.26) in document content. */
    QString currentFontFaceName;
    READ_ATTR_WITH_NS_INTO(w, name, currentFontFaceName)
    m_currentFontFace.setName(currentFontFaceName);
    m_currentFontFace.setFamily(currentFontFaceName);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(family)
            ELSE_TRY_READ_IF(pitch)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE_WITHOUT_RETURN
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL family
//! family handler (Font Family)
/*! ECMA-376, 17.8.3.9, p.771.
 This element specifies the font family of the current font. This information
 can be used as defined in font substitution logic to locate an appropriate
 substitute font when this font is not available.

 Parent elements:
 - [done] font (§17.8.3.10)

 No child elements.
*/
KoFilter::ConversionStatus DocxXmlFontTableReader::read_family()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    QString familyGeneric;
    READ_ATTR_WITH_NS_INTO(w, val, familyGeneric)
    m_currentFontFace.setFamilyGeneric(familyGeneric);

    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pitch
//! pitch handler (Font Pitch)
/*! ECMA-376, 17.8.3.14, p.775.
 This element specifies the font pitch of the current font. This information
 can be used as defined in font substitution logic to locate an appropriate
 substitute font when this font is not available.

 Parent elements:
 - [done] font (§17.8.3.10)

 No child elements.
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlFontTableReader::read_pitch()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITH_NS(w, val)
    m_currentFontFace.setPitch(w_val == "fixed" ? KoFontFace::FixedPitch : KoFontFace::VariablePitch);

    SKIP_EVERYTHING
    READ_EPILOGUE
}
