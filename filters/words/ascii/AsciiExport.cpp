/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2000 Michael Johnson <mikej@xnet.com>
   SPDX-FileCopyrightText: 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2010-2011 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Christoph Cullmann <cullmann@kde.org>
   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AsciiExport.h"

#include <QFile>

// KF5
#include <KPluginFactory>

// Calligra
#include <KoFilterChain.h>
#include <KoStore.h>

// Filter libraries
#include "OdfTextReader.h"
#include "OdtReader.h"
#include "OdtReaderBackend.h"

// This filter
#include "AsciiExportDebug.h"
#include "OdfReaderAsciiContext.h"
#include "OdtReaderAsciiBackend.h"

K_PLUGIN_FACTORY_WITH_JSON(AsciiExportFactory, "calligra_filter_odt2ascii.json", registerPlugin<AsciiExport>();)

// Needed to instantiate the plugin factory.
#include "AsciiExport.moc"

AsciiExport::AsciiExport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

AsciiExport::~AsciiExport() = default;

KoFilter::ConversionStatus AsciiExport::convert(const QByteArray &from, const QByteArray &to)
{
    // Check for types
    if (from != "application/vnd.oasis.opendocument.text" || to != "text/plain") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read, "", KoStore::Auto);

    if (!odfStore->open("mimetype")) {
        errorAsciiExport << "Unable to open input file!" << Qt::endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Create output file.
    QFile outfile(m_chain->outputFile());
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errorAsciiExport << "Unable to open output file!" << Qt::endl;
        outfile.close();
        return KoFilter::FileNotFound;
    }

    // The actual conversion
    OdfReaderAsciiContext asciiContext(odfStore, outfile);

    OdtReaderBackend odtBackend; // Use generic backend for document level
    OdtReaderAsciiBackend asciiTextBackend; // Special backend for this filter for text level

    // Create all necessary readers
    OdtReader odtReader;
    OdfTextReader odfTextReader;
    odfTextReader.setBackend(&asciiTextBackend);
    odtReader.setTextReader(&odfTextReader);

    odtReader.readContent(&odtBackend, &asciiContext);

    outfile.close();

    return KoFilter::OK;
}
