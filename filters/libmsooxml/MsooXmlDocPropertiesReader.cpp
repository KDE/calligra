/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "MsooXmlDocPropertiesReader.h"
#include "MsooXmlSchemas.h"
#include <KoXmlWriter.h>
#include <MsooXmlUtils.h>

#define MSOOXML_CURRENT_NS "cp"
#define MSOOXML_CURRENT_CLASS MsooXmlDocPropertiesReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include "MsooXmlReader_p.h"

using namespace MSOOXML;

MsooXmlDocPropertiesReader::MsooXmlDocPropertiesReader(KoOdfWriters *writers)
    : MsooXmlReader(writers)
{
    elemMap.insert(QLatin1String("dc:creator"), QLatin1String("meta:initial-creator"));
    elemMap.insert(QLatin1String("cp:lastModifiedBy"), QLatin1String("dc:creator"));
    elemMap.insert(QLatin1String("dc:description"), QLatin1String("dc:description"));
    elemMap.insert(QLatin1String("dc:subject"), QLatin1String("dc:subject"));
    elemMap.insert(QLatin1String("dc:title"), QLatin1String("dc:title"));
    elemMap.insert(QLatin1String("cp:keywords"), QLatin1String("meta:keyword"));
    elemMap.insert(QLatin1String("dcterms:created"), QLatin1String("meta:creation-date"));
    elemMap.insert(QLatin1String("dcterms:modified"), QLatin1String("dc:date"));
    elemMap.insert(QLatin1String("cp:revision"), QLatin1String("meta:editing-cycles"));
}

KoFilter::ConversionStatus MsooXmlDocPropertiesReader::read(MsooXmlReaderContext *)
{
    debugMsooXml << "=============================";

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();

    if (!expectEl("cp:coreProperties"))
        return KoFilter::WrongFormat;

    if (!expectNS(MSOOXML::Schemas::core_properties))
        return KoFilter::WrongFormat;

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        debugMsooXml << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
    //! @todo find out whether the namespace returned by namespaceUri()
    //!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("cp", MSOOXML::Schemas::core_properties))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::core_properties)));
        return KoFilter::WrongFormat;
    }
    //! @todo expect other namespaces too...

    debugMsooXml << qualifiedName();
    TRY_READ(coreProperties)

    debugMsooXml << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL coreProperties
KoFilter::ConversionStatus MsooXmlDocPropertiesReader::read_coreProperties()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            const QString qn = qualifiedName().toString();
            while (!isEndElement() && !isCharacters())
                readNext();

            const QMap<QString, QString>::ConstIterator it = elemMap.constFind(qn);
            if (it == elemMap.constEnd()) {
                debugMsooXml << "Unknown metadata ignored:" << qn;
                while (!isEndElement())
                    readNext();
                continue;
            }
            debugMsooXml << "Found:" << it.key() << "Mapped to:" << it.value();
            const QString t = text().toString();
            // can't use qPrintable() the string has to remain valid until endElement is called
            // which we can't do if we call qPrintable, the QByteArray falls out of scope after
            // the statement in which it is used
            QByteArray elementArray = it.value().toLocal8Bit();
            meta->startElement(elementArray.constData());
            meta->addTextNode(t.toUtf8());
            meta->endElement();
            while (!isEndElement())
                readNext();
        }
    }

    READ_EPILOGUE
}
