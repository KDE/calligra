/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EXPORTHTML_H
#define EXPORTHTML_H

#include <QFile>
#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantList>

#include <KoFilter.h>
#include <KoStore.h>

class HtmlFile;

class ExportHtml : public KoFilter
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

    ExportHtml(QObject *parent, const QVariantList &);
    ~ExportHtml() override;
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    KoFilter::ConversionStatus extractImages(KoStore *odfStore, HtmlFile *epubFile);

#if 0
    ExportHtml::VectorType vectorType(QByteArray &content);
    bool convertSvm(QByteArray &input, QByteArray &output, QSize size);
    bool convertEmf(QByteArray &input, QByteArray &output, QSize size);
    bool convertWmf(QByteArray &input, QByteArray &output, QSizeF size);

    bool isSvm(QByteArray &content);
    bool isEmf(QByteArray &content);
    bool isWmf(QByteArray &content);
#endif
public Q_SLOTS:

private:
    QHash<QString, QString> m_metadata;
    QHash<QString, QString> m_manifest;
    QHash<QString, QSizeF> m_imagesSrcList;
};

#endif // EXPORTHTML_H
