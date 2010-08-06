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

#ifndef MSOOXMLIMPORT_H
#define MSOOXMLIMPORT_H

#include "msooxml_export.h"

#include <QByteArray>
#include <QHash>

#include <KoOdfExporter.h>
#include <KoXmlReader.h>

class QSize;
class KZip;
class KoOdfWriteStore;
class KoStore;

namespace MSOOXML
{

class MsooXmlReader;
class MsooXmlReaderContext;
class MsooXmlRelationships;
class DrawingMLTheme;

//! A base class for MSOOXML-to-ODF import filters
class MSOOXML_EXPORT MsooXmlImport : public KoOdfExporter
{
    Q_OBJECT
public:
    MsooXmlImport(const QString& bodyContentElement, QObject * parent);

    virtual ~MsooXmlImport();

    KoStore* outputStore() const { return m_outputStore; }

    //! KoFilter::UsageError is returned if this method is called outside
    //! of the importing process, i.e. not from within parseParts().
    KoFilter::ConversionStatus loadAndParseDocument(MsooXmlReader *reader, const QString& path,
            MsooXmlReaderContext* context = 0);

    //! KoFilter::UsageError is returned if this method is called outside
    //! of the importing process, i.e. not from within parseParts().
    KoFilter::ConversionStatus loadAndParseDocument(MsooXmlReader *reader, const QString& path,
            QString& errorMessage,
            MsooXmlReaderContext* context = 0);

    /*! Copies file @a sourceName from the input archive to the output document
     under @a destinationName name. @return KoFilter::OK on success.
     On failure @a errorMessage is set.
     KoFilter::UsageError is returned if this method is called outside
     of the importing process, i.e. not from within parseParts(). */
    KoFilter::ConversionStatus copyFile(const QString& sourceName,
                                        const QString& destinationName,
                                        bool oleFile);

    /*! @return size of image file @a sourceName read from zip archive @a zip.
    Size of the image is returned in @a size.
    @return KoFilter::OK on success.
    On failure @a errorMessage is set. */
    KoFilter::ConversionStatus imageSize(const QString& sourceName, QSize* size);

protected:
    virtual KoFilter::ConversionStatus createDocument(KoStore *outputStore,
            KoOdfWriters *writers);

    virtual KoFilter::ConversionStatus parseParts(KoOdfWriters *writers,
            MsooXmlRelationships *relationships, QString& errorMessage) = 0;

    //! KoFilter::UsageError is returned if this method is called outside
    //! of the importing process, i.e. not from within parseParts().
    KoFilter::ConversionStatus loadAndParseDocument(
        const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
        QString& errorMessage, MsooXmlReaderContext* context = 0);

    //! Like @ref loadAndParseDocument(const QByteArray&, MsooXmlReader*, KoOdfWriters*, QString&, MsooXmlReaderContext*)
    //! but file name is provided directly instead of content type
    KoFilter::ConversionStatus loadAndParseDocumentFromFile(
        const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
        QString& errorMessage, MsooXmlReaderContext* context);

    //! Like @ref loadAndParseDocument(const QByteArray&, MsooXmlReader*, KoOdfWriters*, QString&, MsooXmlReaderContext*)
    //! but return KoFilter::OK if the document for the content type is not found.
    KoFilter::ConversionStatus loadAndParseDocumentIfExists(
        const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
        QString& errorMessage, MsooXmlReaderContext* context = 0);

    //! Like @ref loadAndParseDocumentIfExists(const QByteArray&, MsooXmlReader*, KoOdfWriters*, QString&, MsooXmlReaderContext*)
    //! but file name is provided directly instead of content type
    KoFilter::ConversionStatus loadAndParseDocumentFromFileIfExists(
        const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
        QString& errorMessage, MsooXmlReaderContext* context = 0);

    KoFilter::ConversionStatus parseThemes(QMap<QString, MSOOXML::DrawingMLTheme*>& themes,
        KoOdfWriters *writers, QString& errorMessage);

    //! @return part names associated with @a contentType
    QList<QByteArray> partNames(const QByteArray& contentType) const { return m_contentTypes.values(contentType); }
private:
    //! Opens file for converting and performs convertions.
    //! @return status of convertion.
    KoFilter::ConversionStatus openFile(KoOdfWriters *writers, QString& errorMessage);

    KoFilter::ConversionStatus loadAndParse(const QString& filename,
                                            KoXmlDocument& doc, QString& errorMessage);

    KoFilter::ConversionStatus loadAndParseDocumentInternal(
        const QByteArray& contentType, MsooXmlReader *reader, KoOdfWriters *writers,
        QString& errorMessage, MsooXmlReaderContext* context, bool *pathFound);

    KoFilter::ConversionStatus loadAndParseDocumentFromFileInternal(
        const QString& fileName, MsooXmlReader *reader, KoOdfWriters *writers,
        QString& errorMessage, MsooXmlReaderContext* context, bool *pathFound);

    KZip* m_zip; //!< Input zip file

    KoStore* m_outputStore; //!< output store used for copying files

    //! XML from "[Content_Types].xml" file.
    KoXmlDocument m_contentTypesXML;
    //! Content types from m_contentTypesXML: ContentType -> PartName mapping
    QMultiHash<QByteArray, QByteArray> m_contentTypes;

    //! XML with document contents, typically /word/document.xml
    KoXmlDocument m_documentXML;
};

} // namespace MSOOXML

#endif
