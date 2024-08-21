/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PPTXXMLCOMMENTSREADER_H
#define PPTXXMLCOMMENTSREADER_H

#include <MsooXmlCommonReader.h>

#include <QDate>
#include <QString>

class PptxXmlCommentsReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit PptxXmlCommentsReader(KoOdfWriters *writers);
    ~PptxXmlCommentsReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

    KoFilter::ConversionStatus read_cmLst();
    KoFilter::ConversionStatus read_cm();
    KoFilter::ConversionStatus read_extLst();
    KoFilter::ConversionStatus read_pos();
    KoFilter::ConversionStatus read_text();

private:
    void saveOdfComments();

    class Private;
    Private *d;
};

class PptxXmlCommentsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    PptxXmlCommentsReaderContext();
    ~PptxXmlCommentsReaderContext() override;

    QMap<int, QString> authors;
};

#endif // PPTXXMLCOMMENTSREADER_H
