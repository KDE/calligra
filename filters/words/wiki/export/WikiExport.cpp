/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "WikiExport.h"

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
#include "OdfReaderWikiContext.h"
#include "OdtReaderWikiBackend.h"
#include "WikiExportDebug.h"

K_PLUGIN_FACTORY_WITH_JSON(WikiExportFactory, "calligra_filter_odt2wiki.json", registerPlugin<WikiExport>();)

// Needed to instantiate the plugin factory.
#include "WikiExport.moc"

WikiExport::WikiExport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

WikiExport::~WikiExport() = default;

KoFilter::ConversionStatus WikiExport::convert(const QByteArray &from, const QByteArray &to)
{
    // Check for types
    if (from != "application/vnd.oasis.opendocument.text" || to != "text/wiki") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read, "", KoStore::Auto);
    if (!odfStore->open("mimetype")) {
        errorWiki << "Unable to open input file!" << Qt::endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Create output file.
    QFile outfile(m_chain->outputFile());
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errorWiki << "Unable to open output file!" << Qt::endl;
        outfile.close();
        return KoFilter::FileNotFound;
    }

    // The actual conversion
    OdfReaderWikiContext wikiContext(odfStore, outfile);

    OdtReaderBackend odtBackend;
    OdtReaderWikiBackend wikiTextBackend;

    OdtReader odtReader;
    OdfTextReader odfTextReader;
    odfTextReader.setBackend(&wikiTextBackend);
    odtReader.setTextReader(&odfTextReader);

    if (!odtReader.analyzeContent(&wikiContext)) {
        return KoFilter::ParsingError;
    }

    odtReader.readContent(&odtBackend, &wikiContext);

    outfile.close();

    return KoFilter::OK;
}
