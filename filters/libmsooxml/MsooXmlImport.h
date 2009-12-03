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

class KZip;
class KoOdfWriteStore;
class KoStore;

namespace MSOOXML {

class MsooXmlReader;
class MsooXmlReaderContext;
class MsooXmlRelationships;

//! A base class for MSOOXML-to-ODF import filters
class MSOOXML_EXPORT MsooXmlImport : public KoOdfExporter
{
    Q_OBJECT
public:
    MsooXmlImport( const QString& bodyContentElement, QObject * parent );

    virtual ~MsooXmlImport();

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
                                        const QString& destinationName);

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

private:
    //! Opens file for converting and performs convertions.
    //! @return status of convertion.
    KoFilter::ConversionStatus openFile(KoOdfWriters *writers, QString& errorMessage);

    KoFilter::ConversionStatus loadAndParse(const QString& filename,
                                            KoXmlDocument& doc, QString& errorMessage);

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
