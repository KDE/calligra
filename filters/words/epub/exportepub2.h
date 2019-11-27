/* This file is part of the KDE project
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>

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

#ifndef EXPORTEPUB2_H
#define EXPORTEPUB2_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QFile>
#include <QTextStream>
#include <QHash>
#include <QList>

#include <KoFilter.h>
#include <KoStore.h>


class EpubFile;

class ExportEpub2 : public KoFilter
{
    Q_OBJECT
public:
    enum VectorType {
        VectorTypeOther,        // Uninitialized
        VectorTypeWmf,          // Windows MetaFile
        VectorTypeEmf,          // Extended MetaFile
        VectorTypeSvm           // StarView Metafile
        // ... more here later
    };

    ExportEpub2(QObject *parent, const QVariantList &);
    ~ExportEpub2() override;
    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to) override;

private:
    KoFilter::ConversionStatus extractImages(KoStore *odfStore, EpubFile *epubFile);
    KoFilter::ConversionStatus extractMediaFiles(EpubFile *epubFile);

    ExportEpub2::VectorType vectorType(QByteArray &content);
    bool convertSvm(QByteArray &input, QByteArray &output, QSize size);
    bool convertEmf(QByteArray &input, QByteArray &output, QSize size);
    bool convertWmf(QByteArray &input, QByteArray &output, QSizeF size);

    bool isSvm(QByteArray &content);
    bool isEmf(QByteArray &content);
    bool isWmf(QByteArray &content);

    KoFilter::ConversionStatus extractCoverImage(KoStore *odfStore, EpubFile *epubFile);
    void writeCoverImage(EpubFile *epubFile, const QString coverPath);

public Q_SLOTS:


private:
    QHash<QString, QString> m_metadata;
    QHash<QString, QString> m_manifest;
    QHash<QString, QSizeF>  m_imagesSrcList;
    QHash<QString, QString> m_mediaFilesList;
};

#endif // EXPORTEPUB2_H
