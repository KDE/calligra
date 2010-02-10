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

#ifndef MSOOXMLNOTESREADER_H
#define MSOOXMLNOTESREADER_H

#include "MsooXmlReader.h"
#include <QMap>

namespace MSOOXML
{

struct Note
{
    Note() : number(0) {}
    QString text;
    int number; //!< Note number, initialized to 0
};

//! A context structure for MsooXmlRelationshipsReader
class MsooXmlNotesReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    MsooXmlNotesReaderContext(QMap<QString, Note>& _notes);
    QMap<QString, Note> *notes;
};

//! A class reading MSOOXML rels markup - *.xml.rels part.
class MsooXmlNotesReader : public MSOOXML::MsooXmlReader
{
public:
    explicit MsooXmlNotesReader(KoOdfWriters *writers);
    virtual ~MsooXmlNotesReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus readInternal();
    typedef KoFilter::ConversionStatus(MsooXmlNotesReader::*ReadMethod)();
    MsooXmlNotesReaderContext* m_context;

private:
    void init();
    class Private;
    Private* const d;
};

}

#endif //MSOOXMLNOTESREADER_H
