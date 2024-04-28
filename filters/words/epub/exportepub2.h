/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EXPORTEPUB2_H
#define EXPORTEPUB2_H

#include <QFile>
#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantList>

#include <KoFilter.h>
#include <KoStore.h>

class EpubFile;

class ExportEpub2 : public KoFilter
{
    Q_OBJECT
public:
    enum VectorType {
        VectorTypeOther, // Uninitialized
        VectorTypeWmf, // Windows MetaFile
        VectorTypeEmf, // Extended MetaFile
        VectorTypeSvm // StarView Metafile
        // ... more here later
    };

    ExportEpub2(QObject *parent, const QVariantList &);
    ~ExportEpub2() override;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

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
    QHash<QString, QSizeF> m_imagesSrcList;
    QHash<QString, QString> m_mediaFilesList;
};

#endif // EXPORTEPUB2_H
