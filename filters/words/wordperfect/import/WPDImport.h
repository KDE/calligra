/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Yue Liu <yue.liu@mail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WPDIMPORT_H
#define WPDIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class WPDImport : public KoFilter
{
    Q_OBJECT

public:
    WPDImport(QObject *parent, const QVariantList &);
    ~WPDImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // WPDIMPORT_H
