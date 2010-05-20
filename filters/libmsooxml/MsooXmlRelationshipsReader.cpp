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

#include "MsooXmlRelationshipsReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

//#define MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS MsooXmlRelationshipsReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

using namespace MSOOXML;

MsooXmlRelationshipsReaderContext::MsooXmlRelationshipsReaderContext(
    const QString& _path, const QString& _file, QMap<QString, QString>& _rels,
    QMap<QString, QString>& _targetsForTypes)
        : path(_path), file(_file)
        , rels(&_rels), targetsForTypes(&_targetsForTypes)
{
}

class MsooXmlRelationshipsReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
    QString pathAndFile;
};

MsooXmlRelationshipsReader::MsooXmlRelationshipsReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

MsooXmlRelationshipsReader::~MsooXmlRelationshipsReader()
{
    delete d;
}

void MsooXmlRelationshipsReader::init()
{
}

KoFilter::ConversionStatus MsooXmlRelationshipsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<MsooXmlRelationshipsReaderContext*>(context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus MsooXmlRelationshipsReader::readInternal()
{
    kDebug() << "=============================";

    d->pathAndFile = MsooXmlRelationshipsReader::relKey(m_context->path, m_context->file, QString());

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // Relationships
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("Relationships")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::relationships)) {
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

    TRY_READ(Relationships)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL Relationships
//! Relationships handler
/*!
 No parent elements.

 Child elements:
    - [done] Relationship
*/
KoFilter::ConversionStatus MsooXmlRelationshipsReader::read_Relationships()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(Relationship)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL Relationship
//! Relationship handler
/*!
 Parent elements:
    - [done] Relationships

No child elements.
*/
KoFilter::ConversionStatus MsooXmlRelationshipsReader::read_Relationship()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(Id)
    READ_ATTR_WITHOUT_NS(Type)
    READ_ATTR_WITHOUT_NS(Target)
    QString fixedPath(m_context->path);
    while (Target.startsWith("../")) {
//        kDebug() << "- Target:" << Target << "fixedPath:" << fixedPath;
        Target = Target.mid(3);
        fixedPath.truncate(fixedPath.lastIndexOf('/'));
//        kDebug() << "= Target:" << Target << "fixedPath:" << fixedPath;
    }
    /*    kDebug() << "adding rel:";
        kDebug() << d->pathAndFile + Id;
        kDebug() << fixedPath + '/' + Target;*/

    m_context->rels->insert(d->pathAndFile + Id, fixedPath + '/' + Target);
//    kDebug() << "added target" << Target << "for type" << Type << "path=" << fixedPath << "key="
//             << targetKey(fixedPath + '/' + m_context->file, Type);
    m_context->targetsForTypes->insert(targetKey(fixedPath + '/' + m_context->file, Type), fixedPath + '/' + Target);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
