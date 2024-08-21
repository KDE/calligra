/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef XLSXXMLCOMMENTSREADER_H
#define XLSXXMLCOMMENTSREADER_H

#include "XlsxUtils.h"
#include "XlsxXmlCommonReader.h"
#include <MsooXmlTheme.h>

class XlsxComments;
class XlsxComment
{
public:
    explicit XlsxComment(uint authorId);
    QString texts;
    //    QString ref;
    inline QString author(const XlsxComments *comments) const;

private:
    uint m_authorId;
};

typedef QHash<QString, XlsxComment *> XlsxCommentsBase;

//! Comments mapped by cell references. Owns each comment.
/*! Used by worksheet readers for putting comments into the cells.
 @todo many-cell references
*/
class XlsxComments : public XlsxCommentsBase
{
public:
    XlsxComments();
    QString author(uint id) const
    {
        const QString result(id < (uint)m_authors.count() ? m_authors.at(id) : QString());
        if (result.isEmpty()) {
            qCWarning(lcXlsxImport) << "No author for ID" << id;
        }
        return result;
    }

private:
    friend class XlsxXmlCommentsReader;
    QList<QString> m_authors;
};

QString XlsxComment::author(const XlsxComments *comments) const
{
    return comments->author(m_authorId);
}

class XlsxXmlCommentsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    explicit XlsxXmlCommentsReaderContext(XlsxComments &_comments, MSOOXML::DrawingMLTheme *_themes, QVector<QString> &_colorIndices);
    ~XlsxXmlCommentsReaderContext() override;

    XlsxComments *comments;
    MSOOXML::DrawingMLTheme *themes;
    QVector<QString> &colorIndices;
};

class XlsxXmlCommentsReader : public XlsxXmlCommonReader
{
public:
    explicit XlsxXmlCommentsReader(KoOdfWriters *writers);
    ~XlsxXmlCommentsReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus read_comments();
    KoFilter::ConversionStatus read_authors();
    KoFilter::ConversionStatus read_author();
    KoFilter::ConversionStatus read_commentList();
    KoFilter::ConversionStatus read_comment();
    KoFilter::ConversionStatus read_commentPr();
    KoFilter::ConversionStatus read_text();

private:
    KoFilter::ConversionStatus readInternal();

    XlsxXmlCommentsReaderContext *m_context;
    QString m_currentCommentText;
    QString m_currentAuthor;
};

#endif
