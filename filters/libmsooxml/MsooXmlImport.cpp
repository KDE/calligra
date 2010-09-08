/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
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

#include "MsooXmlImport.h"
#include "MsooXmlReader.h"
#include "MsooXmlUtils.h"
#include "MsooXmlSchemas.h"
#include "MsooXmlContentTypes.h"
#include "MsooXmlRelationships.h"
#include "MsooXmlThemesReader.h"

#include <QColor>
#include <QFile>
#include <QFont>
#include <QPen>
#include <QRegExp>
#include <QImage>

#include <kdeversion.h>
#include <KDebug>
#include <KZip>
#include <KGenericFactory>
#include <KMessageBox>

#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoUnit.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>

#include <memory>

using namespace MSOOXML;

MsooXmlImport::MsooXmlImport(const QString& bodyContentElement, QObject* parent)
        : KoOdfExporter(bodyContentElement, parent),
        m_zip(0),
        m_outputStore(0)
{
}

MsooXmlImport::~MsooXmlImport()
{
}

KoFilter::ConversionStatus MsooXmlImport::createDocument(KoStore *outputStore,
        KoOdfWriters *writers)
{
    kDebug() << "######################## start ####################";
    KoFilter::ConversionStatus status = OK;
//! @todo show this message in error details in the GUI:
    QString errorMessage;

    KZip zip(m_chain->inputFile());
    kDebug() << "Store created";

    if (!zip.open(QIODevice::ReadOnly)) {
        errorMessage = i18n("Could not open the requested file %1", m_chain->inputFile());
//! @todo transmit the error to the GUI...
        kDebug() << errorMessage;
        return KoFilter::FileNotFound;
    }

    if (!zip.directory()) {
        errorMessage = i18n("Could not read ZIP directory of the requested file %1", m_chain->inputFile());
//! @todo transmit the error to the GUI...
        kDebug() << errorMessage;
        return KoFilter::FileNotFound;
    }

    m_zip = &zip; // set context
    m_outputStore = outputStore; // set context

    status = openFile(writers, errorMessage);

    m_zip = 0; // clear context
    m_outputStore = 0; // clear context

    QImage thumbnail;
    if (status == KoFilter::OK) {
        // We do not care about the failure
        Utils::loadThumbnail(thumbnail, &zip);
    } else {
        kDebug() << "openFile() != OK";
//! @todo transmit the error to the GUI...
        kDebug() << errorMessage;
        return status;
    }

    if (!zip.close()) {
        return KoFilter::StorageCreationError;
    }

    if (status != KoFilter::OK) {
//! @todo transmit the error to the GUI...
        kDebug() << errorMessage;
    }
    kDebug() << "######################## done ####################";
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::copyFile(const QString& sourceName,
        const QString& destinationName, bool oleFile)
{
    if (!m_zip || !m_outputStore) {
        return KoFilter::UsageError;
    }
    QString errorMessage;
    const KoFilter::ConversionStatus status = Utils::copyFile(
                m_zip, errorMessage, sourceName, m_outputStore, destinationName, oleFile);
//! @todo transmit the error to the GUI...
    kDebug() << errorMessage;
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::imageSize(const QString& sourceName, QSize* size)
{
    if (!m_zip) {
        return KoFilter::UsageError;
    }
    QString errorMessage;
    const KoFilter::ConversionStatus status = Utils::imageSize(
        m_zip, errorMessage, sourceName, size);
//! @todo transmit the error to the GUI...
    kDebug() << errorMessage;
    return status;
}

// private
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentInternal(
    const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context, bool *pathFound)
{
    *pathFound = false;
    const QString fileName = m_contentTypes.value(contentType);
    kDebug() << contentType << "fileName=" << fileName;
    if (fileName.isEmpty()) {
        errorMessage = i18n("Could not find path for type %1", QString(contentType));
        kWarning() << errorMessage;
        return KoFilter::FileNotFound;
    }
    KoFilter::ConversionStatus status = loadAndParseDocumentFromFileInternal(
        fileName, reader, writers, errorMessage, context, pathFound);
    *pathFound = status != KoFilter::FileNotFound;
    return status;
}

// private
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentFromFileInternal(
    const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context, bool *pathFound)
{
    *pathFound = false;
    if (!m_zip) {
        return KoFilter::UsageError;
    }
    KoFilter::ConversionStatus status = Utils::loadAndParseDocument(
               reader, m_zip, writers, errorMessage, fileName, context);
    *pathFound = status != KoFilter::FileNotFound;
    return status;
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocument(
    const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    return loadAndParseDocumentInternal(contentType, reader, writers, errorMessage, context, &pathFound);
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentFromFile(
    const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    return loadAndParseDocumentFromFileInternal(fileName, reader, writers, errorMessage, context, &pathFound);
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentIfExists(
    const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    const KoFilter::ConversionStatus status( loadAndParseDocumentInternal(
        contentType, reader, writers, errorMessage, context, &pathFound) );
    if (!pathFound)
        return KoFilter::OK;
    return status;
}

// protected
KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocumentFromFileIfExists(
    const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    bool pathFound;
    const KoFilter::ConversionStatus status( loadAndParseDocumentFromFileInternal(
        fileName, reader, writers, errorMessage, context, &pathFound) );
    if (!pathFound)
        return KoFilter::OK;
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocument(
    MsooXmlReader *reader, const QString& path,
    MsooXmlReaderContext* context)
{
    if (!m_zip) {
        return KoFilter::UsageError;
    }
    QString errorMessage;
    KoFilter::ConversionStatus status = Utils::loadAndParseDocument(
                                            reader, m_zip, reader, errorMessage, path, context);
    if (status != KoFilter::OK)
        reader->raiseError(errorMessage);
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::loadAndParseDocument(
    MsooXmlReader *reader, const QString& path,
    QString& errorMessage, MsooXmlReaderContext* context)
{
    if (!m_zip) {
        return KoFilter::UsageError;
    }
    KoFilter::ConversionStatus status = Utils::loadAndParseDocument(
                                            reader, m_zip, reader, errorMessage, path, context);
    return status;
}

KoFilter::ConversionStatus MsooXmlImport::openFile(KoOdfWriters *writers, QString& errorMessage)
{
    static const char *Content_Types_xml =  "[Content_Types].xml";
    KoFilter::ConversionStatus status = loadAndParse(Content_Types_xml, m_contentTypesXML, errorMessage);
    if (status != KoFilter::OK) {
        kDebug() << Content_Types_xml << "could not be parsed correctly! Aborting!";
        return status;
    }

    RETURN_IF_ERROR( Utils::loadContentTypes(m_contentTypesXML, m_contentTypes) )
    MsooXmlRelationships relationships(*this, writers, errorMessage);
    RETURN_IF_ERROR( parseParts(writers, &relationships, errorMessage) )
//! @todo sigProgress()
    emit sigProgress(10);

    return KoFilter::OK;
}

KoFilter::ConversionStatus MsooXmlImport::loadAndParse(const QString& filename, KoXmlDocument& doc, QString& errorMessage)
{
    return Utils::loadAndParse(doc, m_zip, errorMessage, filename);
}

#include "MsooXmlImport.moc"
