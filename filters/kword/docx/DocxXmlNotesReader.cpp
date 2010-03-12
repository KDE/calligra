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

#include "DocxXmlNotesReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
#include <limits.h>

#define MSOOXML_CURRENT_NS "w"
#define MSOOXML_CURRENT_CLASS MsooXmlNotesReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

DocxXmlNotesReaderContext::DocxXmlNotesReaderContext(QMap<int, DocxNote>& _notes)
    : notes(&_notes)
{
}

class DocxXmlNotesReader::Private
{
public:
    Private() : counter(0) {
    }
    ~Private() {
    }
    QString pathAndFile;
    int counter;
};

DocxXmlNotesReader::DocxXmlNotesReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlReader(writers)
    , m_context(0)
    , d(new Private)
{
    init();
}

DocxXmlNotesReader::~DocxXmlNotesReader()
{
    delete d;
}

void DocxXmlNotesReader::init()
{
    d->counter = 0;
}

KoFilter::ConversionStatus DocxXmlNotesReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<DocxXmlNotesReaderContext*>(context);
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }
    //w:footnotes/w:endnotes
    readNext();
    kDebug() << *this << namespaceUri();
    if (!expectEl(QList<QByteArray>() << "w:footnotes" << "w:endnotes")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }
    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    /*    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }*/
    //! @todo find out whether the namespace returned by namespaceUri()
    //!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::wordprocessingml));
        return KoFilter::WrongFormat;
    }

    const QString qn(qualifiedName().toString());

    RETURN_IF_ERROR(read_notes())

            if (!expectElEnd(qn)) {
        return KoFilter::WrongFormat;
    }
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

KoFilter::ConversionStatus DocxXmlNotesReader::read_notes()
{
    kDebug() << qualifiedName();
    const QString notesMainName(qualifiedName().toString());
    const QByteArray itemName(qualifiedName() == "w:footnotes" ? "w:footnote" : "w:endnote");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (qualifiedName() == itemName.constData()) {
                RETURN_IF_ERROR(read_note(itemName))
                    }
            ELSE_WRONG_FORMAT
                }
        if (qualifiedName() == notesMainName && isEndElement())
            break;
    }
    return KoFilter::OK;
}

KoFilter::ConversionStatus DocxXmlNotesReader::read_note(const char *noteType)
{
    const QXmlStreamAttributes attrs(attributes());

    DocxNote note;

    TRY_READ_ATTR(id)
            int idNumber = INT_MAX; // good enough value
    if (!id.isEmpty()) {
        bool ok;
        idNumber = id.toInt(&ok);
        if (!ok)
            idNumber = INT_MAX;
    }

    while (!atEnd()) {
        readNext();
        if (name() == "t" && isStartElement()) {
            // this could be done better! Text can have formatting and this extracts only pure text
            readNext();
            note.text += text().toString();
        }
        if (qualifiedName() == noteType && isEndElement())
            break;
    }


    TRY_READ_ATTR(type);


    if (type.isNull()) {
        // not a separator or continuationSeparator, but a real note.
        // docx saves the separator and continuations separator as footnote elements.
        d->counter++;
        note.number = d->counter;
        if (m_context->notes->contains(idNumber)) {
            // ECMA: If more than one footnote shares the same ID, then this document shall
            // be considered non-conformant. If more than one endnote shares the same ID,
            // then this document shall be considered non-conformant.
            raiseError(i18n("Note \"%1\" already exists", idNumber));
            return KoFilter::WrongFormat;
        }
        m_context->notes->insert(idNumber, note);
    }
    return KoFilter::OK;
}
