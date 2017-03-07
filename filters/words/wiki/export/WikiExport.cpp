/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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
   Boston, MA 02110-1301, USA.
 */

#include "WikiExport.h"

#include <QFile>

// KF5
#include <kpluginfactory.h>

// Calligra
#include <KoStore.h>
#include <KoFilterChain.h>

// Filter libraries
#include "OdtReader.h"
#include "OdtReaderBackend.h"
#include "OdfTextReader.h"

// This filter
#include "OdtReaderWikiBackend.h"
#include "OdfReaderWikiContext.h"
#include "WikiExportDebug.h"

K_PLUGIN_FACTORY_WITH_JSON(WikiExportFactory, "calligra_filter_odt2wiki.json",
			   registerPlugin<WikiExport>();)

// Needed to instantiate the plugin factory.
#include "WikiExport.moc"


WikiExport::WikiExport(QObject *parent, const QVariantList &)
: KoFilter(parent)
{
}

WikiExport::~WikiExport()
{
}

KoFilter::ConversionStatus WikiExport::convert(const QByteArray& from, const QByteArray& to)
{
    // Check for types
    if (from != "application/vnd.oasis.opendocument.text" || to != "text/wiki") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read,
                                             "", KoStore::Auto);
    if (!odfStore->open("mimetype")) {
        errorWiki << "Unable to open input file!" << endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Create output file.
    QFile outfile(m_chain->outputFile());
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text )) {
        errorWiki << "Unable to open output file!" << endl;
        outfile.close();
        return KoFilter::FileNotFound;
    }

    // The actual conversion
    OdfReaderWikiContext  wikiContext(odfStore, outfile);

    OdtReaderBackend      odtBackend;
    OdtReaderWikiBackend  wikiTextBackend;

    OdtReader             odtReader;
    OdfTextReader         odfTextReader;
    odfTextReader.setBackend(&wikiTextBackend);
    odtReader.setTextReader(&odfTextReader);

    if (!odtReader.analyzeContent(&wikiContext)) {
        return KoFilter::ParsingError;
    }

    odtReader.readContent(&odtBackend, &wikiContext);

    outfile.close();

    return KoFilter::OK;
}
