/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 The Karbon Developers

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __EPSIMPORT_H__
#define __EPSIMPORT_H__

#include <QObject>

#include <KoFilter.h>
#include <QVariantList>

class EpsImport : public KoFilter
{
    Q_OBJECT

public:
    EpsImport(QObject *parent, const QVariantList &);
    ~EpsImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif
