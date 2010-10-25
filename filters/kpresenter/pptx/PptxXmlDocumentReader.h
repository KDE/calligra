/*
 * This file is part of Office 2007 Filters for KOffice
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

#ifndef PPTXXMLDOCREADER_H
#define PPTXXMLDOCREADER_H

#include <MsooXmlThemesReader.h>
#include "PptxXmlSlideReader.h"

class PptxImport;
class PptxSlideProperties;
class PptxSlideLayoutProperties;

namespace MSOOXML
{
class MsooXmlRelationships;
}

class PptxXmlDocumentReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    PptxXmlDocumentReaderContext(PptxImport& _import, const QString& _path, const QString& _file,
                                 MSOOXML::MsooXmlRelationships& _relationships);
    PptxImport *import;
    const QString path;
    const QString file;
    MSOOXML::MsooXmlRelationships* relationships;
    bool firstReadRound;
    MSOOXML::DrawingMLTheme* themes;
};

//! A class reading MSOOXML PPTX markup - presentation.xml part.
class PptxXmlDocumentReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit PptxXmlDocumentReader(KoOdfWriters *writers);

    virtual ~PptxXmlDocumentReader();

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_presentation();
    KoFilter::ConversionStatus read_sldMasterIdLst();
    KoFilter::ConversionStatus read_sldMasterId();
    KoFilter::ConversionStatus read_sldIdLst();
    KoFilter::ConversionStatus read_sldId();
    KoFilter::ConversionStatus read_sldSz();
    KoFilter::ConversionStatus read_defaultTextStyle();

    // Locates slide layout informaitons for given slide. Caches the result.
    PptxSlideLayoutProperties* slideLayoutProperties(const QString& slidePath, const QString& slideFile);

    KoOdfWriters *m_writers;
    PptxXmlDocumentReaderContext* m_context;

    void initializeContext(PptxXmlSlideReaderContext& context, const MSOOXML::DrawingMLTheme& theme);

    // Default pptx styles
    QVector<KoGenStyle> defaultParagraphStyles;
    QVector<KoGenStyle> defaultTextStyles;
    QVector<MSOOXML::Utils::ParagraphBulletProperties> defaultListStyles;
    QVector<QString> defaultBulletColors;
    QVector<QString> defaultTextColors;
    QVector<QString> defaultLatinFonts;

    enum ColorReadingState {rprState, buClrState};
    ColorReadingState m_colorState;

private:

    void init();

    class Private;
    Private* const d;
#include <MsooXmlCommonReaderMethods.h>
#include <MsooXmlCommonReaderDrawingMLMethods.h>
#include <MsooXmlDrawingReaderTableMethods.h>
};

#endif //PPTXXMLDOCREADER_H
