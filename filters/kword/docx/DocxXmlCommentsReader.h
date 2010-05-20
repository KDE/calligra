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

#ifndef DOCXXMLCOMMENTSREADER_H
#define DOCXXMLCOMMENTSREADER_H

#include <MsooXmlReader.h>
#include <QMap>
#include <QDateTime>

//! Data structure for a single comment
class DocxComment
{
public:
    DocxComment() : m_null(true) {}
    bool isNull() const { return m_null; }
    QString author() const { return m_author; }
    void setAuthor(const QString& author) { m_author = author; m_null = false; }
    QDateTime dateTime() const { return m_dateTime; }
    void setDateTime(const QDateTime& dateTime) { m_dateTime = dateTime; m_null = false; }
    QString text() const { return m_text; }
    void setText(const QString& text) { m_text = text; m_null = false; }
private:
    QString m_author;
    QDateTime m_dateTime;
    QString m_text;
    bool m_null;
};

//! A context structure for DocxXmlCommentsReader
class DocxXmlCommentsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    DocxXmlCommentsReaderContext(QMap<int, DocxComment>& _comments);
    QMap<int, DocxComment> *comments;
};

//! A class reading MSOOXML comments - comments.xml part.
class DocxXmlCommentsReader : public MSOOXML::MsooXmlReader
{
public:
    explicit DocxXmlCommentsReader(KoOdfWriters *writers);
    virtual ~DocxXmlCommentsReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_comments();
    KoFilter::ConversionStatus read_comment();
    DocxXmlCommentsReaderContext* m_context;

private:
    void init();
    class Private;
    Private* const d;
};

#endif //DOCXXMLCOMMENTSREADER_H
