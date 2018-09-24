/*
 * This file is part of Office 2007 Filters for Calligra
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

#include <KoXmlWriter.h>

//#define MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlCommentsReader
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

XlsxXmlCommentsReaderContext::XlsxXmlCommentsReaderContext(XlsxComments& _comments, MSOOXML::DrawingMLTheme* _themes,
    QVector<QString>& _colorIndices)
    : comments(&_comments)
    , themes(_themes)
    , colorIndices(_colorIndices)
{
}

XlsxXmlCommentsReaderContext::~XlsxXmlCommentsReaderContext()
{
}

XlsxXmlCommentsReader::XlsxXmlCommentsReader(KoOdfWriters *writers)
    : XlsxXmlCommonReader(writers)
{
}

XlsxXmlCommentsReader::~XlsxXmlCommentsReader()
{
}

KoFilter::ConversionStatus XlsxXmlCommentsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlCommentsReaderContext*>(context);
    Q_ASSERT(m_context);
    m_colorIndices = m_context->colorIndices;
    m_themes = m_context->themes;
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result != KoFilter::OK) {
        qCWarning(lcXlsxImport) << "Failure reading the comments";
    }
    // We're not going to fail reading the whole file because the comments cannot be read
    return KoFilter::OK;
}

KoFilter::ConversionStatus XlsxXmlCommentsReader::readInternal()
{
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // comments
    readNext();
    qCDebug(lcXlsxImport) << *this << namespaceUri();

    if (!expectEl("comments")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        qCDebug(lcXlsxImport) << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(QString(), MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::spreadsheetml)));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(comments)

    qCDebug(lcXlsxImport) << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL comments
/*
 Parent elements:
 - root element

 Child elements:
 - [done] authors (Authors) §18.7.2
 - [done] commentList (List of Comments) §18.7.4
 - extLst (Future Feature Data Storage Area) §18.2.10

*/
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_comments()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(authors)
            ELSE_TRY_READ_IF(commentList)
//            ELSE_TRY_READ_IF(extLst)
        }
    }
    READ_EPILOGUE
}

//! 18.7.2 authors (Authors)
/*
 Parent elements:
 - [done] comments (§18.7.6)

 Child elements:
 - [done] author (Author) §18.7.1

*/
#undef CURRENT_EL
#define CURRENT_EL authors
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_authors()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(author)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

//! 18.7.1 author (Author)
/*
 Parent elements:
 - [done] authors (§18.7.2)

 Child elements:
 - none
*/
#undef CURRENT_EL
#define CURRENT_EL author
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_author()
{
    READ_PROLOGUE
    readNext();
    const QString author(text().toString().trimmed());
    qCDebug(lcXlsxImport) << "Added author #" << (m_context->comments->count() + 1) << author;
    m_context->comments->m_authors.append(author);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL commentPr
/*
 Parent elements:
 - [done] comment (§18.7.3)

 Child elements:
 - anchor (Object Cell Anchor) §18.3.1.1

*/
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_commentPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            //TRY_READ_IF(anchor)
            //ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL commentList
/*
 Parent elements:
 - [done] comments (§18.7.6)

 Child elements:
 - [done] comment (Comment) §18.7.3

*/
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_commentList()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(comment)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL comment
/*
 Parent elements:
 - [done] commentList (§18.7.4)

 Child elements:
 - [done] commentPr (Comment Properties) §18.7.5
 - [done] text (Comment Text) §18.7.7

*/
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
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(text)
            ELSE_TRY_READ_IF(commentPr)
            ELSE_WRONG_FORMAT
        }
    }
    if (comment.get()) {
        comment.get()->texts = m_currentCommentText;
        m_context->comments->insert(ref, comment.release());
        qCDebug(lcXlsxImport) << "Added comment for" << ref;
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL text
/*
 Parent elements:
 - [done] comment (§18.7.3)

 Child elements:
 - phoneticPr (Phonetic Properties) §18.4.3
 - [done] r (Rich Text Run) §18.4.4
 - rPh (Phonetic Run) §18.4.6
 - [done] t (Text) §18.4.12

*/
KoFilter::ConversionStatus XlsxXmlCommentsReader::read_text()
{
    READ_PROLOGUE

    QByteArray commentData;
    QBuffer commentBuffer(&commentData);
    commentBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter commentWriter(&commentBuffer, 0/*indentation*/);
    MSOOXML::Utils::XmlWriteBuffer buf;
    body = buf.setWriter(&commentWriter);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(r)
            ELSE_TRY_READ_IF(t)
            //! @todo ELSE_TRY_READ_IF(rPh)
            //! @todo ELSE_TRY_READ_IF(phoneticPr)
            //! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = buf.releaseWriter();
    commentBuffer.close();
    m_currentCommentText = commentData;

    READ_EPILOGUE
}
