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

#ifndef DOCXXMLSETTINGSREADER_H
#define DOCXXMLSETTINGSREADER_H

#include <MsooXmlCommonReader.h>

//! A context structure for DocxXmlSettingsReader
class DocxXmlSettingsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    DocxXmlSettingsReaderContext(QMap<QString, QVariant>& _documentSettings);

    QMap<QString, QVariant>& documentSettings;
};

//! A class reading application settings
class DocxXmlSettingsReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit DocxXmlSettingsReader(KoOdfWriters *writers);
    ~DocxXmlSettingsReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

protected:
    KoFilter::ConversionStatus read_settings();
    KoFilter::ConversionStatus read_defaultTabStop();
    KoFilter::ConversionStatus read_displayBackgroundShape();
    KoFilter::ConversionStatus read_clrSchemeMapping();

    DocxXmlSettingsReaderContext* m_context;

private:
    void init();
    class Private;
    Private* const d;
};

#endif //DOCXXMLSETTINGSREADER_H
