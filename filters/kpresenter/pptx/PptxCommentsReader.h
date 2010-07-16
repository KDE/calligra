/* This file is part of the KDE project
 * Copyright (C) 2010 Carlos Licea <carlos@kdab.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PPTXCOMMENTSREADER_H
#define PPTXCOMMENTSREADER_H

#include <MsooXmlCommonReader.h>

#include <QString>
#include <QDate>

class PptxCommentsReader : public MSOOXML::MsooXmlCommonReader
{
public:
    PptxCommentsReader(KoOdfWriters *writers);
    virtual ~PptxCommentsReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

    KoFilter::ConversionStatus read_cmLst();
    KoFilter::ConversionStatus read_cm();
    KoFilter::ConversionStatus read_extLst();
    KoFilter::ConversionStatus read_pos();
    KoFilter::ConversionStatus read_text();

private:
    void saveOdfComments();

    class Private;
    Private* d;
};

class PptxCommentsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    PptxCommentsReaderContext();
    virtual ~PptxCommentsReaderContext();

    QMap<int, QString> authors;
};

#endif // PPTXCOMMENTSREADER_H
