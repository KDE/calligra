/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CSVIMPORT_H
#define CSVIMPORT_H

#include <KoFilter.h>
#include <QVariantList>

class CSVFilter : public KoFilter
{
    Q_OBJECT

public:
    CSVFilter(QObject *parent, const QVariantList &);
    ~CSVFilter() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};
#endif // CSVIMPORT_H
