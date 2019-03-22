/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MSOOXMLCOMMONREADER_IMPL_H
#define MSOOXMLCOMMONREADER_IMPL_H

#include <KoXmlWriter.h>

void MSOOXML_CURRENT_CLASS::initInternal()
{
    m_insideHdr = false;
    m_insideFtr = false;
    m_hasPosOffsetH = false;
    m_hasPosOffsetV = false;
    m_posOffsetH = 0;
    m_posOffsetV = 0;
    m_currentTextStyleProperties = 0;
    m_read_t_args = false;
}

void MSOOXML_CURRENT_CLASS::doneInternal()
{
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = nullptr;
}

#undef CURRENT_EL
#define CURRENT_EL t
//! t (Text)
//! ECMA-376, 17.3.3.31, p.379. (WordprocessingML)
//! ECMA-376, 21.1.2.3.11, p.3632 (DrawingML)
//! ECMA-376, 22.1.2.116, p.4226 (MathML)
/*! This element specifies the actual text for this text run. This is
  the text that is formatted using all specified body, paragraph and
  run properties. This element shall be present within a run of text.

 Parent elements:
 ----------------
 WordprocessingML:
 - r (§22.1.2.87)
 - [done] r (§17.3.2.25)

 DrawingML:
 - [done] fld (§21.1.2.2.4)
 - [done] r (§21.1.2.3.8)

 No child elements.
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_t()
{
    if (m_read_t_args) {
        READ_PROLOGUE_IF_NS(a)
    } else {
        READ_PROLOGUE
    }

    while (!atEnd()) {
        readNext();
        //debugMsooXml << *this;
        if (isCharacters()) {
            body->addTextSpan(text().toString());
#ifdef PPTXXMLSLIDEREADER_CPP
            d->textBoxHasContent = true;
#endif
        }
        if (m_read_t_args) {
            BREAK_IF_END_OF_WITH_NS(a, CURRENT_EL)
        } else {
            BREAK_IF_END_OF(CURRENT_EL)
        }
    }
//debugMsooXml << "{1}";

    if (m_read_t_args) {
        m_read_t_args = false;
        READ_EPILOGUE_IF_NS(a)
    } else {
        READ_EPILOGUE
    }
}



// ================================================================
//                     Namespace: mc
// ================================================================
// ARRRRRGH!

// The way that READ_PROLOGUE is defined via QUALIFIED_NAME makes it
// impossible to use it in files that handle tags both with and
// without namespaces.  This means that we cannot use READ_PROLOGUE in
// the functions below, and most likely also not the READ_IF variants.
// The above is only true when called from XmlWorksheetReader.  For Docx,
// there are always namespaces, so it doesn't apply.
// Same is true for READ_EPILOGUE.


#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "mc"

#undef CURRENT_EL
#define CURRENT_EL AlternateContent
//! Alternate content handler
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_AlternateContent()
{
    m_choiceAccepted = false;

    while (!atEnd()) {
        readNext();
        if (isEndElement() && name() == "AlternateContent") {
            break;
        }

        if (isStartElement()) {
            if (name() == "Choice") {
                TRY_READ(Choice)
            }
            else if (!m_choiceAccepted && qualifiedName() == "mc:Fallback") {
                TRY_READ(Fallback)
            }
            else {
                skipCurrentElement();
            }
        }
    }

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL Choice
//! Choice handler
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Choice()
{

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(Requires)

    // 'Requires="v"' means that the contents of the Choice part
    // is VML, which we support (or something else we do support,
    // Lassi is not sure).  For all other alternatives we
    // don't dare try to interpret it, but instead we use the
    // AlternateContent which is what MSO 2007 would have given us.
    if (Requires != "v") {
        skipCurrentElement();
        return KoFilter::OK;
    }

    m_choiceAccepted = true;
    while (!atEnd()) {
        readNext();
        if (isEndElement() && name() == "Choice") {
            break;
        }
        if (isStartElement()) {
#ifdef PPTXXMLSLIDEREADER_CPP
            TRY_READ_IF_NS(p, oleObj)
#endif
        }
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL Fallback
//! Fallback handler
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Fallback()
{

    while (!atEnd()) {
        readNext();
        if (isEndElement() && name() == "Fallback") {
            break;
        }

        if (isStartElement()) {
#ifdef PPTXXMLSLIDEREADER_CPP
            TRY_READ_IF_NS(p, sp)
#endif
#ifdef DOCXXMLDOCUMENTREADER_H
            TRY_READ_IF_NS(w, pict)
#endif
#ifdef XLSXXMLWORKSHEETREADER_CPP
            // FIXME: This Choice/Content/Fallback structure needs a more general treatment.
            if (name() == "oleObject") {
                TRY_READ(oleObject)
            }
#endif
        }
    }
    return KoFilter::OK;
}

#endif
