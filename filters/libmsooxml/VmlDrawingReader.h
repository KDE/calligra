/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef VMLDRAWINGREADER_H
#define VMLDRAWINGREADER_H

#include <MsooXmlCommonReader.h>
#include <MsooXmlImport.h>

class KOMSOOXML_EXPORT VmlDrawingReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    //! Creates the context object.
    VmlDrawingReaderContext(MSOOXML::MsooXmlImport &_import, const QString &_path, const QString &_file, MSOOXML::MsooXmlRelationships &_relationships);
    MSOOXML::MsooXmlImport *import;
    const QString path;
    const QString file;
};

//! A class reading headers
class KOMSOOXML_EXPORT VmlDrawingReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit VmlDrawingReader(KoOdfWriters *writers);
    ~VmlDrawingReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

    QMap<QString, QString> content();
    QMap<QString, QString> frames();

// Note: Do not move this, it just defines parts of this class
// It is separated out, as it is not part of the OOXML spec
#include <MsooXmlVmlReaderMethods.h>

protected:
    KoFilter::ConversionStatus read_xml();

    VmlDrawingReaderContext *m_context;

private:
    void init();
    class Private;
    Private *const d;
    // List of draw:images meant to be used as object replacements
    QMap<QString, QString> m_content;

    // List of beginning draw:frames meant to be used if the position is unknown otherwise
    // Note that the ending draw:frame still needs to be added by the calling program
    QMap<QString, QString> m_frames;
};

#endif // DOCXXMLHEADERREADER_H
