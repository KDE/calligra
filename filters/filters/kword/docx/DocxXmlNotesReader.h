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

#ifndef DOCXXMLNOTESREADER_H
#define DOCXXMLNOTESREADER_H

#include <MsooXmlReader.h>
#include <QMap>

//! Data structure for a single note
class DocxNote
{
public:
    DocxNote() : number(-1) {}
    bool isNull() const { return number < 0; }
    QString text;
    int number; //!< Note number, initialized to -1
};

//! A context structure for DocxXmlNotesReader
class DocxXmlNotesReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    DocxXmlNotesReaderContext(QMap<int, DocxNote>& _notes);
    QMap<int, DocxNote> *notes;
};

//! A class reading MSOOXML notes markup - endnotes.xml or footnotes.xml part.
class DocxXmlNotesReader : public MSOOXML::MsooXmlReader
{
public:
    explicit DocxXmlNotesReader(KoOdfWriters *writers);
    virtual ~DocxXmlNotesReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_notes();
    KoFilter::ConversionStatus read_note(const char *noteType);

    typedef KoFilter::ConversionStatus(DocxXmlNotesReader::*ReadMethod)();
    DocxXmlNotesReaderContext* m_context;

private:
    void init();
    class Private;
    Private* const d;
};

#endif //DOCXXMLNOTESREADER_H
