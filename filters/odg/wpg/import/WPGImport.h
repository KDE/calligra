/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2013 Yue Liu <yue.liu@mail.com>

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
    explicit WPGImport(QObject *parent, const QVariantList &);
    ~WPGImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // WPGIMPORT_H
