/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "HtmlFile.h"

// Qt
#include <QByteArray>
#include <QList>
#include <QString>

// Calligra
#include <KoStore.h>

#include "HtmlExportDebug.h"

// ================================================================
//                         class HtmlFile

HtmlFile::HtmlFile() = default;

HtmlFile::~HtmlFile() = default;

KoFilter::ConversionStatus HtmlFile::writeHtml(const QString &fileName)
{
    // Create the store and check if everything went well.
    KoStore *htmlStore = KoStore::createStore(fileName, KoStore::Write, "", KoStore::Directory);
    if (!htmlStore || htmlStore->bad()) {
        warnHtml << "Unable to create output file!";
        delete htmlStore;
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus status;

    // Write contents of added files.
    status = FileCollector::writeFiles(htmlStore);

    delete htmlStore;
    return status;
}

// ----------------------------------------------------------------
//                         Private functions
