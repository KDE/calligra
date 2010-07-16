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

#include "PptxCommentAuthorsReader.h"

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxAuthorsReader

#include <KoXmlWriter.h>
#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

class PptxCommentAuthorsReader::Private
{
public:
    PptxCommentAuthorsReaderContext* context;
};

PptxCommentAuthorsReader::PptxCommentAuthorsReader(KoOdfWriters* writers)
: MsooXmlCommonReader(writers)
{
}

PptxCommentAuthorsReader::~PptxCommentAuthorsReader()
{
}

KoFilter::ConversionStatus PptxCommentAuthorsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    d->context = dynamic_cast<PptxCommentAuthorsReaderContext*>(context);
    Q_ASSERT(d->context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    TRY_READ(cmAuthorLst)

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL cmAuthorLst

KoFilter::ConversionStatus PptxCommentAuthorsReader::read_cmAuthorLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(cmAuthor)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cmAuthor

KoFilter::ConversionStatus PptxCommentAuthorsReader::read_cmAuthor()
{
    READ_PROLOGUE

    QXmlStreamAttributes attrs( attributes() );

    READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITHOUT_NS(name)

    d->context->authors.insert(id.toInt(), name);

    while (!atEnd()) {
        readNext();
//         if (isStartElement()) {
//             TRY_READ_IF(extLst)
//             ELSE_WRONG_FORMAT
//         }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

PptxCommentAuthorsReaderContext::PptxCommentAuthorsReaderContext()
: MsooXmlReaderContext()
{

}

PptxCommentAuthorsReaderContext::~PptxCommentAuthorsReaderContext()
{

}

