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

#ifndef DOCXIMPORT_H
#define DOCXIMPORT_H

#include <MsooXmlImport.h>
#include <QVariantList>

//! DOCX to ODT import filter
class DocxImport : public MSOOXML::MsooXmlImport
{
    Q_OBJECT
public:
    DocxImport(QObject *parent, const QVariantList &);
    ~DocxImport() override;

    /// Access the content defined within the words/settings.xml configuration-file.
    QMap<QString, QVariant> documentSettings() const;
    QVariant documentSetting(const QString &name) const;

protected:
    bool acceptsSourceMimeType(const QByteArray &mime) const override;

    bool acceptsDestinationMimeType(const QByteArray &mime) const override;

    KoFilter::ConversionStatus parseParts(KoOdfWriters *writers, MSOOXML::MsooXmlRelationships *relationships, QString &errorMessage) override;

    void writeConfigurationSettings(KoXmlWriter *settings) const override;

    class Private;
    Private *const d;
};

#endif
