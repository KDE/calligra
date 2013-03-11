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

#ifndef ODFPARSER_H
#define ODFPARSER_H

// Qt
#include <QString>
#include <QVariantList>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QHash>
#include <QList>

// Calligra
#include <KoFilter.h>

class KoStore;


class OdfParser
{
public:
    enum VectorType {
        VectorTypeOther,        // Uninitialized
        VectorTypeWmf,          // Windows MetaFile
        VectorTypeEmf,          // Extended MetaFile
        VectorTypeSvm           // StarView Metafile
        // ... more here later
    };

    OdfParser();
    virtual ~OdfParser();

    KoFilter::ConversionStatus parseMetadata(KoStore *odfStore,
                                             QHash<QString, QString> &metadata);
    // Parse manifest
    //
    // Format is QHash<path, type>
    // where
    //   path  is the full path of the file stored in the manifest
    //   type  is the mimetype of the file.
    //
    KoFilter::ConversionStatus parseManifest(KoStore *odfStore,
                                             QHash<QString, QString> &manifest);

private:
};

#endif // ODFPARSER_H
