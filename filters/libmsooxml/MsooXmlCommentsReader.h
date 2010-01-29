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

#ifndef MSOOXMLCOMMENTSREADER_H
#define MSOOXMLCOMMENTSREADER_H

#include "MsooXmlReader.h"
#include <QMap>

namespace MSOOXML
{

//! A context structure for MsooXmlRelationshipsReader
class MsooXmlCommentsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    MsooXmlCommentsReaderContext(QMap<QString, QStringList>& _comments);
    QMap<QString, QStringList> *comments;
};

//! A class reading MSOOXML rels markup - *.xml.rels part.
class MsooXmlCommentsReader : public MSOOXML::MsooXmlReader
{
public:
    explicit MsooXmlCommentsReader(KoOdfWriters *writers);
    virtual ~MsooXmlCommentsReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus readInternal();
    typedef KoFilter::ConversionStatus(MsooXmlCommentsReader::*ReadMethod)();
    MsooXmlCommentsReaderContext* m_context;
    
private:
    void init();
    class Private;
    Private* const d;
};

}

#endif //MSOOXMLCOMMENTSREADER_H
