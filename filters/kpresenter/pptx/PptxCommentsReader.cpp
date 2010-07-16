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

#include "PptxCommentsReader.h"

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxCommentsReader

#include <KoXmlWriter.h>
#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

PptxCommentsReader::PptxCommentsReader(KoOdfWriters* writers)
: MsooXmlCommonReader(writers)
{
}

PptxCommentsReader::~PptxCommentsReader()
{
}

KoFilter::ConversionStatus PptxCommentsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    TRY_READ(cmLst)

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL cmLst

KoFilter::ConversionStatus PptxCommentsReader::read_cmLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(cm)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cm

KoFilter::ConversionStatus PptxCommentsReader::read_cm()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(pos)
            ELSE_TRY_READ_IF(text)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL extLst

KoFilter::ConversionStatus PptxCommentsReader::read_extLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
//             TRY_READ_IF(ext)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pos

KoFilter::ConversionStatus PptxCommentsReader::read_pos()
{
    READ_PROLOGUE

    QXmlStreamAttributes attrs( attributes() );

    READ_ATTR(x);
    READ_ATTR(y);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL text

KoFilter::ConversionStatus PptxCommentsReader::read_text()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        if (isCharacters()) {
            body->addTextSpan(text().toString());
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}


PptxCommentsReaderContext::PptxCommentsReaderContext()
: MsooXmlReaderContext()
{
}

PptxCommentsReaderContext::~PptxCommentsReaderContext()
{
}

