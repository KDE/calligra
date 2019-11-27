/*
 * This file is part of Office 2007 Filters for Calligra
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

#ifndef MSOOXMLDOCPROPERTIESREADER_H
#define MSOOXMLDOCPROPERTIESREADER_H

#include "MsooXmlReader.h"

#include <QMap>
#include <QString>

namespace MSOOXML
{

class KOMSOOXML_EXPORT MsooXmlDocPropertiesReader : public MsooXmlReader
{
public:
    explicit MsooXmlDocPropertiesReader(KoOdfWriters* writers);

    //! Reads/parses the file. The output goes to KoGenStyles* KoOdfWriters::meta
    KoFilter::ConversionStatus read(MsooXmlReaderContext* context = 0) override;

    KoFilter::ConversionStatus read_coreProperties();
private:
    QMap<QString,QString> elemMap;
};

} // namespace MSOOXML

#endif // MSOOXMLDOCPROPERTIESREADER_H
