/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PptxXmlCommentAuthorsReader.h"

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxAuthorsReader

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

class PptxXmlCommentAuthorsReader::Private
{
public:
    PptxXmlCommentAuthorsReaderContext *context;
};

PptxXmlCommentAuthorsReader::PptxXmlCommentAuthorsReader(KoOdfWriters *writers)
    : MsooXmlCommonReader(writers)
    , d(new Private)
{
}

PptxXmlCommentAuthorsReader::~PptxXmlCommentAuthorsReader()
{
    delete d;
}

KoFilter::ConversionStatus PptxXmlCommentAuthorsReader::read(MSOOXML::MsooXmlReaderContext *context)
{
    d->context = dynamic_cast<PptxXmlCommentAuthorsReaderContext *>(context);
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
        BREAK_IF_END_OF(CURRENT_EL)
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

    QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(id)
    READ_ATTR_WITHOUT_NS(name)

    d->context->authors.insert(id.toInt(), name);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
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

PptxXmlCommentAuthorsReaderContext::~PptxXmlCommentAuthorsReaderContext() = default;
