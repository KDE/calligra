/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2000 S.R.Haque <shaheedhaque@hotmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later

DESCRIPTION
*/

#ifndef WMFIMPORT_H
#define WMFIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class WMFImport : public KoFilter
{
    Q_OBJECT

public:
    WMFImport(QObject *parent, const QVariantList &);
    ~WMFImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // WMFIMPORT_H
