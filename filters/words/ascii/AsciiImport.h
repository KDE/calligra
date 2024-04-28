/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ASCIIIMPORT_H
#define ASCIIIMPORT_H

#include <KoFilter.h>

#include <QVariantList>

class QByteArray;

class AsciiImport : public KoFilter
{
    Q_OBJECT

public:
    AsciiImport(QObject *parent, const QVariantList &);
    ~AsciiImport() override;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif /* ASCIIIMPORT_H */
