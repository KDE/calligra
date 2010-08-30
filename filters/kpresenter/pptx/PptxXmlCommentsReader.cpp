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

#include "PptxXmlCommentsReader.h"

#include <QPoint>

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxCommentsReader

#include <KoXmlWriter.h>
#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>
#include <MsooXmlUnits.h>

class PptxXmlCommentsReader::Private
{
public:
    int currentComment;

    QMap<int, QString> authors;
    QMap<int, QString> texts;
    QMap<int, QPoint> positions;
    QMap<int, QString> dates;
    PptxXmlCommentsReaderContext* context;
};

PptxXmlCommentsReader::PptxXmlCommentsReader(KoOdfWriters* writers)
: MsooXmlCommonReader(writers)
, d( new Private() )
{
    d->currentComment = 0;
}

PptxXmlCommentsReader::~PptxXmlCommentsReader()
{
    delete d;
}

KoFilter::ConversionStatus PptxXmlCommentsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    d->context = dynamic_cast<PptxXmlCommentsReaderContext*>(context);
    Q_ASSERT(d->context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    KoFilter::ConversionStatus result = read_cmLst();
    Q_ASSERT(result == KoFilter::OK);
    if( result == KoFilter::OK ) {
        saveOdfComments();
    }

    return KoFilter::OK;
}

void PptxXmlCommentsReader::saveOdfComments()
{
    for(int i = 0; i < d->currentComment; ++i) {
        body->startElement("officeooo:annotation"); //TODO replace with standarized element name

        QPoint position = d->positions.value(i);
        //FIXME according to the documentation these measurements are EMUs
        //but I still get wrong values and that's why I multiply by 1500
        const int fixmeFactor = 1500;
        body->addAttribute("svg:x", EMU_TO_CM_STRING(position.x()*fixmeFactor));
        body->addAttribute("svg:y", EMU_TO_CM_STRING(position.y()*fixmeFactor));

        body->startElement("dc:creator");
        body->addTextSpan(d->authors.value(i));
        body->endElement();//dc:creator

        body->startElement("dc:date");
        body->addTextSpan(d->dates.value(i));
        body->endElement();//dc:date

        body->startElement("text:p");
        body->addTextSpan(d->texts.value(i));
        body->endElement();//text:p

        body->endElement();//officeooo:annotation
    }
}

#undef CURRENT_EL
#define CURRENT_EL cmLst

KoFilter::ConversionStatus PptxXmlCommentsReader::read_cmLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(cm)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cm

KoFilter::ConversionStatus PptxXmlCommentsReader::read_cm()
{
    READ_PROLOGUE

    QXmlStreamAttributes attrs( attributes() );

    TRY_READ_ATTR_WITHOUT_NS(authorId)
    const QString author = d->context->authors.value(authorId.toInt());
    d->authors.insert(d->currentComment, author);

    TRY_READ_ATTR_WITHOUT_NS(dt)
    d->dates.insert(d->currentComment, dt);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(pos)
            ELSE_TRY_READ_IF(text)
            ELSE_WRONG_FORMAT
        }
    }

    d->currentComment++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL extLst

KoFilter::ConversionStatus PptxXmlCommentsReader::read_extLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
//             TRY_READ_IF(ext)
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pos

KoFilter::ConversionStatus PptxXmlCommentsReader::read_pos()
{
    READ_PROLOGUE

    QXmlStreamAttributes attrs( attributes() );

    READ_ATTR_WITHOUT_NS(x);
    READ_ATTR_WITHOUT_NS(y);

    d->positions.insert(d->currentComment, QPoint(x.toInt(),y.toInt()));

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL text

KoFilter::ConversionStatus PptxXmlCommentsReader::read_text()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isCharacters()) {
            d->texts.insert( d->currentComment, text().toString());
        }
    }

    READ_EPILOGUE
}


PptxXmlCommentsReaderContext::PptxXmlCommentsReaderContext()
: MsooXmlReaderContext()
{
}

PptxXmlCommentsReaderContext::~PptxXmlCommentsReaderContext()
{
}

