/*
 * This file is part of Office 2007 Filters for Calligra
 * SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef PPTXIMPORT_H
#define PPTXIMPORT_H

#include <MsooXmlImport.h>
#include <QVariantList>

//! PPTX to ODP import filter
class PptxImport : public MSOOXML::MsooXmlImport
{
    Q_OBJECT
public:
    PptxImport(QObject *parent, const QVariantList &);
    ~PptxImport() override;

protected:
    bool acceptsSourceMimeType(const QByteArray &mime) const override;

    bool acceptsDestinationMimeType(const QByteArray &mime) const override;

    KoFilter::ConversionStatus parseParts(KoOdfWriters *writers, MSOOXML::MsooXmlRelationships *relationships, QString &errorMessage) override;
    class Private;
    Private *const d;
};

#endif
