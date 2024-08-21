/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PPTXXMLCOMMENTAUTHORSREADER_H
#define PPTXXMLCOMMENTAUTHORSREADER_H

#include <MsooXmlCommonReader.h>

class PptxXmlCommentAuthorsReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit PptxXmlCommentAuthorsReader(KoOdfWriters *writers);
    ~PptxXmlCommentAuthorsReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

    KoFilter::ConversionStatus read_cmAuthorLst();
    KoFilter::ConversionStatus read_cmAuthor();

private:
    class Private;
    Private *d;
};

class PptxXmlCommentAuthorsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    PptxXmlCommentAuthorsReaderContext();
    ~PptxXmlCommentAuthorsReaderContext() override;

    QMap<int, QString> authors;
};

#endif // PPTXXMLCOMMENTSAUTHORSREADER_H
