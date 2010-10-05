/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef VMLDRAWINGREADER_H
#define VMLDRAWINGREADER_H

#include <MsooXmlCommonReader.h>
#include <MsooXmlImport.h>

class VmlDrawingReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    //! Creates the context object.
    VmlDrawingReaderContext(MSOOXML::MsooXmlImport& _import,
        const QString& _path, const QString& _file,
        MSOOXML::MsooXmlRelationships& _relationships);
    MSOOXML::MsooXmlImport* import;
    const QString path;
    const QString file;
};

//! A class reading headers
class VmlDrawingReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit VmlDrawingReader(KoOdfWriters *writers);
    virtual ~VmlDrawingReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

    QVector<QString> content();

#include <MsooXmlVmlReaderMethods.h>

protected:
    KoFilter::ConversionStatus read_xml();

    VmlDrawingReaderContext* m_context;

private:
    void init();
    class Private;
    Private* const d;
    // List of draw:images meant to be used as object replacements
    QVector<QString> m_content;
};

#endif //DOCXXMLHEADERREADER_H
