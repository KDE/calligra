/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef WIKIEXPORT_H
#define WIKIEXPORT_H

#include <KoFilter.h>

#include <QVariantList>

class QByteArray;

class WikiExport : public KoFilter
{
    Q_OBJECT

public:
    WikiExport(QObject *parent, const QVariantList &);
    ~WikiExport() override;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
};

#endif /* WIKIEXPORT_H */
