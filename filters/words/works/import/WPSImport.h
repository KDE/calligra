/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011-2013 Yue Liu <yue.liu@mail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WPSIMPORT_H
#define WPSIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class WPSImport : public KoFilter
{
    Q_OBJECT

public:
    WPSImport(QObject *parent, const QVariantList &);
    ~WPSImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // WPSIMPORT_H
