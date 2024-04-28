/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PDFIMPORT_H
#define PDFIMPORT_H

#include <KoFilter.h>
#include <QObject>
#include <QVariantList>

class PdfImport : public KoFilter
{
    Q_OBJECT

public:
    PdfImport(QObject *parent, const QVariantList &);
    ~PdfImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif
