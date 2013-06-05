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

#include "DocxExport.h"

#include <QFile>

// KDE
#include <kdebug.h>
#include <kpluginfactory.h>

// Calligra
#include <KoStore.h>
#include <KoFilterChain.h>

// Filter libraries
#include "OdtReader.h"

// This filter
#include "DocxFile.h"
#include "OdfReaderDocxContext.h"
#include "OdtReaderDocxBackend.h"


K_PLUGIN_FACTORY(DocxExportFactory, registerPlugin<DocxExport>();)
K_EXPORT_PLUGIN(DocxExportFactory("wordsdocxexportng", "calligrafilters"))


DocxExport::DocxExport(QObject *parent, const QVariantList &)
: KoFilter(parent)
{
}

DocxExport::~DocxExport()
{
}

KoFilter::ConversionStatus DocxExport::convert(const QByteArray& from, const QByteArray& to)
{
    // Check for types
    if (from != "application/vnd.oasis.opendocument.text"
        || to != "application/vnd.openxmlformats-officedocument.wordprocessingml.document") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read,
                                             "", KoStore::Auto);
    // If we don't call disallowNameExpansion(), then filenames that
    // begin with numbers will not be opened. Embedded images often
    // have names like this.
    odfStore->disallowNameExpansion();
    if (!odfStore->open("mimetype")) {
        kError(30503) << "Unable to open input file!" << endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Start the conversion

    // Collects all the parts of the docx file and writes the result at the end.
    DocxFile docxFile;

    OdfReaderDocxContext  docxBackendContext(odfStore, &docxFile);
    OdtReaderDocxBackend  docxBackend(&docxBackendContext);
    OdtReader             odtReader;
    odtReader.readContent(&docxBackend, &docxBackendContext);

    // Add the newly converted document contents to the docx file.
    docxFile.addContentFile("", "word/document.xml",
                            "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml",
                            docxBackendContext.m_documentContent);

    // Write the output file.
    return docxFile.writeDocx(m_chain->outputFile(), to, docxBackendContext);
}
