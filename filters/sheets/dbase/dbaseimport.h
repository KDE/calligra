/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __DBASEIMPORT_H
#define __DBASEIMPORT_H

#include <KoFilter.h>
#include <KoStore.h>
#include <QByteArray>
#include <QVariantList>

class DBaseImport : public KoFilter
{
    Q_OBJECT

public:
    DBaseImport(QObject *parent, const QVariantList &);
    ~DBaseImport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // __DBASEIMPORT_H
