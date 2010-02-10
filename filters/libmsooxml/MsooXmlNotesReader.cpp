/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MsooXmlNotesReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

#define MSOOXML_CURRENT_CLASS MsooXmlNotesReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

using namespace MSOOXML;

MsooXmlNotesReaderContext::MsooXmlNotesReaderContext(QMap<QString, Note>& _notes)
        : notes(&_notes)
{
}

class MsooXmlNotesReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
    QString pathAndFile;
};

MsooXmlNotesReader::MsooXmlNotesReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

MsooXmlNotesReader::~MsooXmlNotesReader()
{
    delete d;
}

void MsooXmlNotesReader::init()
{
}

KoFilter::ConversionStatus MsooXmlNotesReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<MsooXmlNotesReaderContext*>(context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    return result;
}

KoFilter::ConversionStatus MsooXmlNotesReader::readInternal()
{
    readNext();

    int counter = 0;
    //! @todo use READ* macros
    while (!atEnd()) {
        QString id;
        Note note;
        if (name() == "footnote" || name() == "endnote") {
            const QXmlStreamAttributes attr(attributes());
            id = attr.value("w:id").toString();
        }
        else if (name() == "t" && isStartElement()) {
            // this could be done better! Text can have formatting and this extracts only pure text
            readNext();
            note.text += text().toString();
        }

        if ((name() == "footnote" || name() == "endnote") && isEndElement()) {
            if (!id.isEmpty()) {
                counter++;
                note.number = counter;
                m_context->notes->insert(id, note);
                //kDebug() << "id: " <<  id << " note text: " << note.text;
            }
            id.clear();
        }
        readNext();
        if (hasError()) {
            break;
        }
    }

    return KoFilter::OK;
}
