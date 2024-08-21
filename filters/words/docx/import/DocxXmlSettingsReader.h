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

#ifndef DOCXXMLSETTINGSREADER_H
#define DOCXXMLSETTINGSREADER_H

#include <MsooXmlCommonReader.h>

//! A context structure for DocxXmlSettingsReader
class DocxXmlSettingsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    DocxXmlSettingsReaderContext(QMap<QString, QVariant> &_documentSettings);

    QMap<QString, QVariant> &documentSettings;
};

//! A class reading application settings
class DocxXmlSettingsReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit DocxXmlSettingsReader(KoOdfWriters *writers);
    ~DocxXmlSettingsReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus read_settings();
    KoFilter::ConversionStatus read_defaultTabStop();
    KoFilter::ConversionStatus read_displayBackgroundShape();
    KoFilter::ConversionStatus read_clrSchemeMapping();

    DocxXmlSettingsReaderContext *m_context;

private:
    void init();
    class Private;
    Private *const d;
};

#endif // DOCXXMLSETTINGSREADER_H
