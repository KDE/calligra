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

#include "PptxXmlDocumentReader.h"
#include "PptxXmlSlideReader.h"
#include "PptxImport.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxXmlDocumentReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

PptxXmlDocumentReaderContext::PptxXmlDocumentReaderContext(
    PptxImport& _import, const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
    PptxSlideProperties& _masterSlideProperties, MSOOXML::MsooXmlRelationships& _relationships)
        : import(&_import), themes(&_themes), masterSlideProperties(&_masterSlideProperties),
        relationships(&_relationships)
{
}

class PptxXmlDocumentReader::Private
{
public:
    Private()
            : slideNumber(0)
            , slideReader(0) {
    }
    ~Private() {
        delete slideReader;
    }
    uint slideNumber; //!< temp., see todo in PptxXmlDocumentReader::read_sldId()
    PptxXmlSlideReader *slideReader;
private:
};

PptxXmlDocumentReader::PptxXmlDocumentReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

PptxXmlDocumentReader::~PptxXmlDocumentReader()
{
    delete d;
}

void PptxXmlDocumentReader::init()
{
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
}

KoFilter::ConversionStatus PptxXmlDocumentReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<PptxXmlDocumentReaderContext*>(context);
    Q_ASSERT(m_context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus PptxXmlDocumentReader::readInternal()
{
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // p:presentation
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("p:presentation")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::presentationml)) {
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
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("p", MSOOXML::Schemas::presentationml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::presentationml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(presentation)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL sldId
//! p:sldId handler (Slide ID)
/*! This element specifies a presentation slide that is available within the corresponding presentation.
 ECMA-376, 19.2.1.33, p. 2797.
 Parent elements:
    - [done] sldIdLst (§19.2.1.34)
 Child elements:
    - extLst (Extension List)
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldId()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITH_NS(r, id)
    kDebug() << "id:" << id << "r:id:" << r_id;
//! @todo use MSOOXML::MsooXmlRelationships
//!       (for now we hardcode relationships, e.g. we use slide1, slide2...)
    d->slideNumber++; // counted from 1
    QString path("ppt/slides");
    QString file = QString("slide%1.xml").arg(d->slideNumber);
    kDebug() << "path:" << path + "/" + file;
    if (!d->slideReader) {
        d->slideReader = new PptxXmlSlideReader(this);
    }
    PptxXmlSlideReaderContext context(
        *m_context->import,
        path, file,
        d->slideNumber, *m_context->themes,
        PptxXmlSlideReader::Slide, *m_context->masterSlideProperties,
        *m_context->relationships);

    const KoFilter::ConversionStatus result = m_context->import->loadAndParseDocument(
                d->slideReader, path + "/" + file, &context);

    if (result != KoFilter::OK) {
        raiseError(d->slideReader->errorString());
        return result;
    }
    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldIdLst
//! p:sldIdLst handler (List of Slide IDs)
/*! ECMA-376, 19.2.1.34, p. 2798.
 Parent elements:
    - [done] presentation (§19.2.1.26)
 Child elements:
    - [done] sldId (Slide ID)
*/
KoFilter::ConversionStatus PptxXmlDocumentReader::read_sldIdLst()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sldId)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL presentation
//! p:presentation handler (Presentation)
/*! ECMA-376, 19.2.1.26, p. 2790.
 Root element.
 Child elements:
    - ustDataLst (Customer Data List) §19.3.1.18
    - custShowLst (List of Custom Shows) §19.2.1.7
    - defaultTextStyle (Presentation Default Text Style) §19.2.1.8
    - embeddedFontLst (Embedded Font List) §19.2.1.10
    - extLst (Extension List) §19.2.1.12
    - handoutMasterIdLst (List of Handout Master IDs) §19.2.1.15
    - kinsoku (Kinsoku Settings) §19.2.1.17
    - modifyVerifier (Modification Verifier) §19.2.1.19
    - notesMasterIdLst (List of Notes Master IDs) §19.2.1.21
    - notesSz (Notes Slide Size) §19.2.1.22
    - photoAlbum (Photo Album Information) §19.2.1.24
    - sldIdLst (List of Slide IDs) §19.2.1.34
    - sldMasterIdLst (List of Slide Master IDs) §19.2.1.37
    - sldSz (Presentation Slide Size) §19.2.1.39
    - smartTags (Smart Tags) §19.2.1.40
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlDocumentReader::read_presentation()
{
    READ_PROLOGUE

    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    while (!atEnd()) {
//        kDebug() << "!!" << qualifiedName() << JOIN(MSOOXML_CURRENT_NS ":", CURRENT_EL);
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(sldIdLst)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}
