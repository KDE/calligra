/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Yolla Indria <yolla.indria@gmail.com>
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POWERPOINTIMPORT_H
#define POWERPOINTIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class PowerPointImport : public KoFilter
{
    Q_OBJECT
public:
    PowerPointImport(QObject *parent, const QVariantList &)
        : KoFilter(parent)
    {
    }
    ~PowerPointImport() override = default;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

    void setProgress(const int percent);
};

#endif // POWERPOINTIMPORT_H
