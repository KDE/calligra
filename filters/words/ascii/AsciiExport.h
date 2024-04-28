/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ASCIIEXPORT_H
#define ASCIIEXPORT_H

#include <KoFilter.h>

#include <QVariantList>

class QByteArray;

class AsciiExport : public KoFilter
{
    Q_OBJECT

public:
    AsciiExport(QObject *parent, const QVariantList &);
    ~AsciiExport() override;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
};

#endif /* ASCIIEXPORT_H */
