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

#include "XlsxXmlCommentsReader.h"
#include "XlsxXmlWorksheetReader.h"
#include "XlsxImport.h"
#include <MsooXmlSchemas.h>

//#define MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlCommentsReaderContext
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>

#include <memory>

XlsxComment::XlsxComment(uint authorId)
    : m_authorId(authorId)
{
}

XlsxComments::XlsxComments()
{
}

XlsxXmlCommentsReaderContext::XlsxXmlCommentsReaderContext(XlsxComments& _comments)
    : MSOOXML::MsooXmlReaderContext()
    , comments(&_comments)
{
}

XlsxXmlCommentsReaderContext::~XlsxXmlCommentsReaderContext()
{
}

XlsxXmlCommentsReader::XlsxXmlCommentsReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
{
}

XlsxXmlCommentsReader::~XlsxXmlCommentsReader()
{
}

KoFilter::ConversionStatus XlsxXmlCommentsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlCommentsReaderContext*>(context);
    Q_ASSERT(m_context);

    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus XlsxXmlCommentsReader::readInternal()
{
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // comments
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("comments")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(QString(), MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::spreadsheetml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(comments)

    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL comments
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_comments()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(authors)
            ELSE_TRY_READ_IF(commentList)
//            ELSE_TRY_READ_IF(extLst)
        }
    }
    READ_EPILOGUE
}

//! 18.7.2 authors (Authors)
#undef CURRENT_EL
#define CURRENT_EL authors
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_authors()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(author)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

//! 18.7.1 author (Author)
#undef CURRENT_EL
#define CURRENT_EL author
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_author()
{
    READ_PROLOGUE
    readNext();
    const QString author(text().toString().trimmed());
    kDebug() << "Added author #" << (m_context->comments->count() + 1) << author;
    m_context->comments->m_authors.append(author);
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL commentList
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_commentList()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(comment)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL comment
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_comment()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(ref)
    READ_ATTR_WITHOUT_NS(authorId)
    int authorIdInt = -1;
    STRING_TO_INT(authorId, authorIdInt, "comment@authorId")
    std::auto_ptr<XlsxComment> comment(authorIdInt < 0 ? 0 : new XlsxComment(authorIdInt));
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(text)
            //! @todo ELSE_TRY_READ_IF(commentPr)
            //! @todo add ELSE_WRONG_FORMAT
        }
    }
    if (comment.get()) {
        comment.get()->texts = m_currentCommentText;
        m_context->comments->insert(ref, comment.release());
        kDebug() << "Added comment for" << ref;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL text
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_text()
{
    READ_PROLOGUE
    m_currentCommentText.clear();
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(r)
            ELSE_TRY_READ_IF(t)
            //! @todo ELSE_TRY_READ_IF(rPh)
            //! @todo ELSE_TRY_READ_IF(phoneticPr)
            //! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL r
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_r()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(t)
            //! @todo ELSE_TRY_READ_IF(rPr)
            //! @todo add ELSE_WRONG_FORMAT
        }
    }
    kDebug() << m_currentCommentText;
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL t
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_t()
{
    READ_PROLOGUE
    readNext();
    //! @todo is trimming ok here?
    m_currentCommentText += text().toString().trimmed();
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
