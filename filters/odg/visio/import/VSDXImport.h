/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011-2013 Yue Liu <yue.liu@mail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef VSDXIMPORT_H
#define VSDXIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class VSDXImport : public KoFilter
{
    Q_OBJECT
public:
    explicit VSDXImport(QObject *parent, const QVariantList &);
    ~VSDXImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // VSDXIMPORT_H
