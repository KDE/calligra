/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EPUBFILE_H
#define EPUBFILE_H

#include <QHash>

#include <KoFilter.h>

#include "FileCollector.h"

class QString;
class QByteArray;

class KoStore;

class EpubFile : public FileCollector
{
public:
    EpubFile();
    ~EpubFile() override;

    // When you have created all the content and added it using
    // addContentFile(), call this function once and it will write the
    // epub to the disk.
    KoFilter::ConversionStatus writeEpub(const QString &fileName, const QByteArray &appIdentification, QHash<QString, QString> metadata);

private:
    KoFilter::ConversionStatus writeMetaInf(KoStore *epubStore);
    KoFilter::ConversionStatus writeOpf(KoStore *epubStore, QHash<QString, QString> &metadata);
    KoFilter::ConversionStatus writeNcx(KoStore *epubStore, QHash<QString, QString> &metadata);

private:
};

#endif // EPUBFILE_H
