/*
 * This file is part of Office 2007 Filters for Calligra
 * SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef XLSXIMPORT_H
#define XLSXIMPORT_H

#include <MsooXmlImport.h>
#include <QVariantList>

//! XLSX to ODS import filter
class XlsxImport : public MSOOXML::MsooXmlImport
{
    Q_OBJECT
public:
    XlsxImport(QObject *parent, const QVariantList &);
    ~XlsxImport() override;

protected:
    bool acceptsSourceMimeType(const QByteArray &mime) const override;

    bool acceptsDestinationMimeType(const QByteArray &mime) const override;

    KoFilter::ConversionStatus parseParts(KoOdfWriters *writers, MSOOXML::MsooXmlRelationships *relationships, QString &errorMessage) override;

    class Private;
    Private *const d;
};

#endif
