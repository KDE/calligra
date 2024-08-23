/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EXPORTMOBI_H
#define EXPORTMOBI_H

#include <QObject>

#include <KoFilter.h>
#include <KoStore.h>
#include <QHash>
#include <QList>
#include <QSizeF>
#include <QVariantList>

class MobiFile;

class ExportMobi : public KoFilter
{
    Q_OBJECT
public:
    ExportMobi(QObject *parent, const QVariantList &);
    ~ExportMobi() override;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    KoFilter::ConversionStatus extractImages(KoStore *odfStore, MobiFile *mobi);

private:
    QByteArray m_palmDoc;
    QHash<QString, QString> m_metaData;
    QHash<QString, QString> m_manifest;
    QHash<QString, QSizeF> m_imagesSrcList;
    QHash<int, QByteArray> m_imagesList;
    QList<int> m_imagesSize;
};

#endif // EXPORTMOBI_H
