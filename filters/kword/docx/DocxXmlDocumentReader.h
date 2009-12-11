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

#ifndef DOCXXMLDOCREADER_H
#define DOCXXMLDOCREADER_H

#include <MsooXmlCommonReader.h>

#include <KoGenStyle.h>
#include <styles/KoCharacterStyle.h>

//#define NO_DRAWINGML_PICTURE // disables pic:pic, etc. in MsooXmlCommonReader

class DocxImport;
class DocxXmlDocumentReaderContext;
namespace MSOOXML
{
class MsooXmlRelationships;
}

//! A class reading MSOOXML DOCX markup - document.xml part.
class DocxXmlDocumentReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit DocxXmlDocumentReader(KoOdfWriters *writers);

    virtual ~DocxXmlDocumentReader();

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_body();

    typedef KoFilter::ConversionStatus(DocxXmlDocumentReader::*ReadMethod)();
    QStack<ReadMethod> m_calls;

    DocxXmlDocumentReaderContext* m_context;
private:
    void init();

#include <MsooXmlCommonReaderMethods.h>
#include <MsooXmlCommonReaderDrawingMLMethods.h>

    class Private;
    Private* const d;
};

//! Context for DocxXmlDocumentReader
class DocxXmlDocumentReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    //! Creates the context object.
    DocxXmlDocumentReaderContext(
        DocxImport& _import,
        const QString& _path, const QString& _file,
        MSOOXML::MsooXmlRelationships& _relationships
        /*        uint _slideNumber,
                const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
                PptxXmlSlideReader::Type _type, PptxSlideProperties& _slideProperties,
                MSOOXML::MsooXmlRelationships& _relationships*/
    );
    DocxImport* import;
    const QString path;
    const QString file;
    /*    const uint slideNumber;
        const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;
        PptxXmlSlideReader::Type type;
        PptxSlideProperties* slideProperties;
        MSOOXML::MsooXmlRelationships* relationships;*/
};

#endif //DOCXXMLDOCREADER_H
