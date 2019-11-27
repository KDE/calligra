/* This file is part of the KDE project

   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef EXPORTMOBI_H
#define EXPORTMOBI_H

#include <QObject>

#include <QVariantList>
#include <KoFilter.h>
#include <KoStore.h>
#include <QHash>
#include <QList>

class MobiFile;

class ExportMobi : public KoFilter
{
    Q_OBJECT
public:
    ExportMobi(QObject *parent, const QVariantList &);
    ~ExportMobi() override;
    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to) override;
    
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
