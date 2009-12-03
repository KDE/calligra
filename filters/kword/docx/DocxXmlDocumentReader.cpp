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

#include "DocxXmlDocumentReader.h"
#include "DocxImport.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#define MSOOXML_CURRENT_NS "w"
#define MSOOXML_CURRENT_CLASS DocxXmlDocumentReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

DocxXmlDocumentReaderContext::DocxXmlDocumentReaderContext(
    DocxImport& _import,
    const QString& _path, const QString& _file,
    MSOOXML::MsooXmlRelationships& _relationships)
    : MSOOXML::MsooXmlReaderContext(&_relationships),
      import(&_import), path(_path), file(_file)
{
}

class DocxXmlDocumentReader::Private {
public:
    Private()
    {
    }
    ~Private()
    {
    }
};

DocxXmlDocumentReader::DocxXmlDocumentReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , d(new Private)
{
    init();
}

DocxXmlDocumentReader::~DocxXmlDocumentReader()
{
    delete d;
}

void DocxXmlDocumentReader::init()
{
    initInternal();
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
}

KoFilter::ConversionStatus DocxXmlDocumentReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<DocxXmlDocumentReaderContext*>(context);
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    //w:document
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("w:document")) {
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

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(body)
//! @todo TRY_READ_IF(background)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(document)
    }

    if (!expectElEnd("w:document")) {
        return KoFilter::WrongFormat;
    }
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL body
//! w:body handler (Document Body)
/*! ECMA-376, 17.2.2, p. 204.
*/
//! @todo support all child elements
KoFilter::ConversionStatus DocxXmlDocumentReader::read_body()
{
    READ_PROLOGUE

    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i=0; i<namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(p)
//! @todo add ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#define blipFill_NS "pic"

#include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "pic" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds pic:pic, etc.
