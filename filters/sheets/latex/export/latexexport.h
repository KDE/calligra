/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Robert JACOLIN <rjacolin@ifrance.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __LATEXEXPORT_H__
#define __LATEXEXPORT_H__

#include <QByteArray>
#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>

#include <KoFilter.h>
#include <KoStore.h>
#include <QVariantList>

class LATEXExport : public KoFilter
{
    Q_OBJECT

public:
    LATEXExport(QObject *parent, const QVariantList &);
    ~LATEXExport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif /* __LATEXExport_H__ */
