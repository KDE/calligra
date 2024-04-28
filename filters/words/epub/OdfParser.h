/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFPARSER_H
#define ODFPARSER_H

// Qt
#include <QFile>
#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantList>

// Calligra
#include <KoFilter.h>

class KoStore;

class OdfParser
{
public:
    enum VectorType {
        VectorTypeOther, // Uninitialized
        VectorTypeWmf, // Windows MetaFile
        VectorTypeEmf, // Extended MetaFile
        VectorTypeSvm // StarView Metafile
        // ... more here later
    };

    OdfParser();
    virtual ~OdfParser();

    KoFilter::ConversionStatus parseMetadata(KoStore *odfStore, QHash<QString, QString> &metadata);
    // Parse manifest
    //
    // Format is QHash<path, type>
    // where
    //   path  is the full path of the file stored in the manifest
    //   type  is the mimetype of the file.
    //
    KoFilter::ConversionStatus parseManifest(KoStore *odfStore, QHash<QString, QString> &manifest);

private:
};

#endif // ODFPARSER_H
