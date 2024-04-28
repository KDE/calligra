/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DOCXEXPORT_H
#define DOCXEXPORT_H

#include <KoFilter.h>

#include <QVariantList>

class QByteArray;

class DocxExport : public KoFilter
{
    Q_OBJECT

public:
    DocxExport(QObject *parent, const QVariantList &);
    ~DocxExport() override;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
};

#endif /* DOCXEXPORT_H */
