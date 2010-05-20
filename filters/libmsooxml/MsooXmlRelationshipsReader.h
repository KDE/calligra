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

#ifndef MSOOXMLRELATIONSHIPSREADER_H
#define MSOOXMLRELATIONSHIPSREADER_H

#include "MsooXmlReader.h"
#include <QMap>

namespace MSOOXML
{

//! A context structure for MsooXmlRelationshipsReader
class MsooXmlRelationshipsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    MsooXmlRelationshipsReaderContext(const QString& _path, const QString& _file, QMap<QString, QString>& _rels,
                                      QMap<QString, QString>& _targetsForTypes);
    const QString path;
    const QString file;
    QMap<QString, QString> *rels;
    QMap<QString, QString> *targetsForTypes;
};

//! A class reading MSOOXML rels markup - *.xml.rels part.
class MsooXmlRelationshipsReader : public MSOOXML::MsooXmlReader
{
public:
    explicit MsooXmlRelationshipsReader(KoOdfWriters *writers);

    virtual ~MsooXmlRelationshipsReader();

    //! Reads/parses the file of format *.xml.rels.
    //! The output goes to MsooXmlRelationships structure.
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

    //! @return key for use in MsooXmlRelationships::target()
    static inline QString relKey(const QString& path, const QString& file, const QString& id) {
        return path + '\n' + file + '\n' + id;
    }

//! @return key for use in MsooXmlRelationships::targetForType()
        static inline QString targetKey(const QString& pathAndFile, const QString& relType) {
        return pathAndFile + '\n' + relType;
    }

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_Relationships();
    KoFilter::ConversionStatus read_Relationship();

    MsooXmlRelationshipsReaderContext* m_context;
private:
    void init();

    class Private;
    Private* const d;
};

}

#endif //MSOOXMLRELATIONSHIPSREADER_H
