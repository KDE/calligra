/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003-2006 Ariya Hidayat <ariya@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EXCELIMPORT_H
#define EXCELIMPORT_H

#include <KoFilter.h>
#include <KoStore.h>
#include <QVariantList>

class ExcelImport : public KoFilter
{
    Q_OBJECT

public:
    ExcelImport(QObject *parent, const QVariantList &);
    ~ExcelImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private Q_SLOTS:
    void slotSigProgress(int progress);

private:
    class Private;
    Private *d;
};

#endif // EXCELIMPORT_H
