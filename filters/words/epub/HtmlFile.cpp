/* This file is part of the KDE project

   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>
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


// Own
#include "HtmlFile.h"

// Qt
#include <QString>
#include <QByteArray>
#include <QList>

// Calligra
#include <KoStore.h>

#include "HtmlExportDebug.h"

// ================================================================
//                         class HtmlFile

HtmlFile::HtmlFile()
{
}

HtmlFile::~HtmlFile()
{
}


KoFilter::ConversionStatus HtmlFile::writeHtml(const QString &fileName)
{
    // Create the store and check if everything went well.
    KoStore *htmlStore = KoStore::createStore(fileName, KoStore::Write, "", KoStore::Directory);
    if (!htmlStore || htmlStore->bad()) {
        warnHtml << "Unable to create output file!";
        delete htmlStore;
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus  status;

    // Write contents of added files.
    status = FileCollector::writeFiles(htmlStore);

    delete htmlStore;
    return status;
}


// ----------------------------------------------------------------
//                         Private functions
