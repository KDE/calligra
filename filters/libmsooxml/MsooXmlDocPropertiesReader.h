/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
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
    explicit MsooXmlDocPropertiesReader(KoOdfWriters *writers);

    //! Reads/parses the file. The output goes to KoGenStyles* KoOdfWriters::meta
    KoFilter::ConversionStatus read(MsooXmlReaderContext *context = nullptr) override;

    KoFilter::ConversionStatus read_coreProperties();

private:
    QMap<QString, QString> elemMap;
};

} // namespace MSOOXML

#endif // MSOOXMLDOCPROPERTIESREADER_H
