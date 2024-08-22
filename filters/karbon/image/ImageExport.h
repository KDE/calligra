/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef IMAGEEXPORT_H
#define IMAGEEXPORT_H

#include <KoFilter.h>
#include <QVariantList>

class ImageExport : public KoFilter
{
    Q_OBJECT

public:
    ImageExport(QObject *parent, const QVariantList &);
    ~ImageExport() override = default;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif // IMAGEEXPORT_H
