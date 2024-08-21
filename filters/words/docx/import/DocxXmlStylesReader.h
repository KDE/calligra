/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef DOCXXMLSTYLESREADER_H
#define DOCXXMLSTYLESREADER_H

#include <QMap>

#include "DocxXmlDocumentReader.h"
#include <MsooXmlDrawingTableStyle.h>

#include <KoCharacterStyle.h>
#include <KoGenStyle.h>

//! A class reading MSOOXML DOCX markup - styles.xml part.
/*! @todo generalize for other MSOOXML subformats.
 */
class DocxXmlStylesReader : public DocxXmlDocumentReader
{
public:
    explicit DocxXmlStylesReader(KoOdfWriters *writers);

    ~DocxXmlStylesReader() override;

    //! Reads/parses the file. The output goes mainly to KoGenStyles* KoOdfWriters::mainStyles
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus read_docDefaults();
    KoFilter::ConversionStatus read_pPrDefault();
    KoFilter::ConversionStatus read_rPrDefault();
    KoFilter::ConversionStatus read_style();
    KoFilter::ConversionStatus read_name();
    KoFilter::ConversionStatus read_tblStylePr();

    QMap<QByteArray, KoGenStyle *> m_defaultStyles;
    void createDefaultStyle(KoGenStyle::Type type, const char *family);

    QString m_name; //!< set by read_name()

    MSOOXML::DrawingTableStyle *m_currentStyle;

private:
    void init();
    // #include <MsooXmlCommonReaderMethods.h>
};

#endif
