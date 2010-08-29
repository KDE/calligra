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

#include "PptxXmlCommentAuthorsReader.h"

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxAuthorsReader

#include <KoXmlWriter.h>
#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

class PptxXmlCommentAuthorsReader::Private
{
public:
    PptxXmlCommentAuthorsReaderContext* context;
};

PptxXmlCommentAuthorsReader::PptxXmlCommentAuthorsReader(KoOdfWriters* writers)
: MsooXmlCommonReader(writers), d(new Private)
{
}

PptxXmlCommentAuthorsReader::~PptxXmlCommentAuthorsReader()
{
    delete d;
}

KoFilter::ConversionStatus PptxXmlCommentAuthorsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    d->context = dynamic_cast<PptxXmlCommentAuthorsReaderContext*>(context);
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

KoFilter::ConversionStatus PptxXmlCommentAuthorsReader::read_cmAuthorLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(cmAuthor)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cmAuthor

KoFilter::ConversionStatus PptxXmlCommentAuthorsReader::read_cmAuthor()
{
    READ_PROLOGUE

    QXmlStreamAttributes attrs( attributes() );

    READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITHOUT_NS(name)

    d->context->authors.insert(id.toInt(), name);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
//         if (isStartElement()) {
//             TRY_READ_IF(extLst)
//             ELSE_WRONG_FORMAT
//         }
    }

    READ_EPILOGUE
}

PptxXmlCommentAuthorsReaderContext::PptxXmlCommentAuthorsReaderContext()
: MsooXmlReaderContext()
{

}

PptxXmlCommentAuthorsReaderContext::~PptxXmlCommentAuthorsReaderContext()
{

}
