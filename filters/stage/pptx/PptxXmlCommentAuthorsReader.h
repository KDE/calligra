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

#ifndef PPTXXMLCOMMENTAUTHORSREADER_H
#define PPTXXMLCOMMENTAUTHORSREADER_H

#include <MsooXmlCommonReader.h>

class PptxXmlCommentAuthorsReader : public MSOOXML::MsooXmlCommonReader
{
public:
    PptxXmlCommentAuthorsReader(KoOdfWriters* writers);
    virtual ~PptxXmlCommentAuthorsReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

    KoFilter::ConversionStatus read_cmAuthorLst();
    KoFilter::ConversionStatus read_cmAuthor();
private:
    class Private;
    Private* d;
};

class PptxXmlCommentAuthorsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    PptxXmlCommentAuthorsReaderContext();
    virtual ~PptxXmlCommentAuthorsReaderContext();

    QMap<int, QString> authors;
};

#endif // PPTXXMLCOMMENTSAUTHORSREADER_H
