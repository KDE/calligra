/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Ariya Hidayat <ariya@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WPGIMPORT_H
#define WPGIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class WPGImport : public KoFilter
{
    Q_OBJECT

public:
    WPGImport(QObject *parent, const QVariantList &);
    ~WPGImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // WPGIMPORT_H
