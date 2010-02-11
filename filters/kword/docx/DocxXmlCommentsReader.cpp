/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "DocxXmlCommentsReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

#define MSOOXML_CURRENT_NS "w"
#define MSOOXML_CURRENT_CLASS DocxXmlCommentsReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

DocxXmlCommentsReaderContext::DocxXmlCommentsReaderContext(QMap<int, DocxComment>& _comments)
        : comments(&_comments)
{
}

class DocxXmlCommentsReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
    QString pathAndFile;
};

DocxXmlCommentsReader::DocxXmlCommentsReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

DocxXmlCommentsReader::~DocxXmlCommentsReader()
{
    delete d;
}

void DocxXmlCommentsReader::init()
{
}

KoFilter::ConversionStatus DocxXmlCommentsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<DocxXmlCommentsReaderContext*>(context);
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }
    //w:comments
    readNext();
    kDebug() << *this << namespaceUri();
    if (!expectEl("w:comments")) {
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

    TRY_READ(comments)

    if (!expectElEnd("w:comments")) {
        return KoFilter::WrongFormat;
    }
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL comments
//! 17.13.4.6 comments (Comments Collection)
KoFilter::ConversionStatus DocxXmlCommentsReader::read_comments()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(comment)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL comment
//! 17.13.4.2 comment (Comment Content)
KoFilter::ConversionStatus DocxXmlCommentsReader::read_comment()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    DocxComment comment;
    READ_ATTR(id) // ECMA: If this attribute is omitted, then the document is non-conformant.
    int idNumber;
    STRING_TO_INT(id, idNumber, "comment@w:id")
    TRY_READ_ATTR(author)
    comment.setAuthor(author);
    TRY_READ_ATTR(date)
    if (date.endsWith("Z")) {
        date.remove(date.length()-1, 1);
    }
    comment.setDateTime(QDateTime::fromString(date, Qt::ISODate));
    if (!comment.dateTime().isValid()) {
        raiseError(i18n("Invalid comment date \"%1\"", date));
        return KoFilter::WrongFormat;
    }

    //! @todo This could be done better! Text can have formatting and this extracts only pure text.
    //!       Use DocxXmlDocumentReader
    while (!atEnd()) {
        readNext();
        kDebug() << name();
        if (QUALIFIED_NAME_IS(t) && isStartElement()) {
            readNext();
            comment.setText(comment.text() + text().toString());
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    m_context->comments->insert(idNumber, comment);
//    kDebug() << "id:" <<  idNumber << "date:" << comment.dateTime() <<  " author: " << comment.author()
//        << "text:" << comment.text();
    READ_EPILOGUE
}
