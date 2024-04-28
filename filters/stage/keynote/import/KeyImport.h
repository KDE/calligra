/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2013 Yue Liu <yue.liu@mail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KEYIMPORT_H
#define KEYIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class KeyImport : public KoFilter
{
    Q_OBJECT

public:
    KeyImport(QObject *parent, const QVariantList &);
    ~KeyImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // KEYIMPORT_H
