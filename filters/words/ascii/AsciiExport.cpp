/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Michael Johnson <mikej@xnet.com>
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2010-2011 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2010 Christoph Cullmann <cullmann@kde.org>
   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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

#include "AsciiExport.h"

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
#include "OdtReaderAsciiBackend.h"
#include "OdfReaderAsciiContext.h"
#include "AsciiExportDebug.h"


K_PLUGIN_FACTORY_WITH_JSON(AsciiExportFactory, "calligra_filter_odt2ascii.json",
			   registerPlugin<AsciiExport>();)

// Needed to instantiate the plugin factory.
#include "AsciiExport.moc"


AsciiExport::AsciiExport(QObject *parent, const QVariantList &)
: KoFilter(parent)
{
}

AsciiExport::~AsciiExport()
{
}

KoFilter::ConversionStatus AsciiExport::convert(const QByteArray& from, const QByteArray& to)
{
    // Check for types
    if (from != "application/vnd.oasis.opendocument.text" || to != "text/plain") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read,
                                             "", KoStore::Auto);

    if (!odfStore->open("mimetype")) {
        errorAsciiExport << "Unable to open input file!" << endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Create output file.
    QFile outfile(m_chain->outputFile());
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text )) {
        errorAsciiExport << "Unable to open output file!" << endl;
        outfile.close();
        return KoFilter::FileNotFound;
    }

    // The actual conversion
    OdfReaderAsciiContext  asciiContext(odfStore, outfile);

    OdtReaderBackend       odtBackend;        // Use generic backend for document level
    OdtReaderAsciiBackend  asciiTextBackend;  // Special backend for this filter for text level

    // Create all necessary readers
    OdtReader              odtReader;
    OdfTextReader          odfTextReader;
    odfTextReader.setBackend(&asciiTextBackend);
    odtReader.setTextReader(&odfTextReader);

    odtReader.readContent(&odtBackend, &asciiContext);

    outfile.close();

    return KoFilter::OK;
}
