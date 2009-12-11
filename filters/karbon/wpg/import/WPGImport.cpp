/* This file is part of the KDE project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>

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

#include <WPGImport.h>
#include <WPGImport.moc>

#include <QBuffer>
#include <QByteArray>
#include <QString>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>

#include <libwpg/libwpg.h>
#include <libwpg/WPGStreamImplementation.h>

#include "FileOutputHandler.hxx"
#include "OdgExporter.hxx"

#include <iostream>

typedef KGenericFactory<WPGImport> WPGImportFactory;
K_EXPORT_COMPONENT_FACTORY(libwpgimport, WPGImportFactory("kofficefilters"))

WPGImport::WPGImport(QObject* parent, const QStringList&)
        : KoFilter(parent)
{
}

WPGImport::~WPGImport()
{
}

static QByteArray createManifest()
{
    KoXmlWriter* manifestWriter;
    QByteArray manifestData;
    QBuffer manifestBuffer(&manifestData);

    manifestBuffer.open(QIODevice::WriteOnly);
    manifestWriter = new KoXmlWriter(&manifestBuffer);

    manifestWriter->startDocument("manifest:manifest");
    manifestWriter->startElement("manifest:manifest");
    manifestWriter->addAttribute("xmlns:manifest", "urn:oasis:names:tc:openoffice:xmlns:manifest:1.0");
    manifestWriter->addManifestEntry("/", "application/vnd.oasis.opendocument.graphics");
    //manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    manifestWriter->addManifestEntry("content.xml", "text/xml");
    manifestWriter->endElement();
    manifestWriter->endDocument();
    delete manifestWriter;

    return manifestData;
}


KoFilter::ConversionStatus WPGImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/x-wpg")
        return KoFilter::NotImplemented;

    if (to != "application/vnd.oasis.opendocument.graphics")
        return KoFilter::NotImplemented;


    WPXInputStream* input = new libwpg::WPGFileStream(m_chain->inputFile().toLocal8Bit());
    if (input->isOLEStream()) {
        WPXInputStream* olestream = input->getDocumentOLEStream();
        if (olestream) {
            delete input;
            input = olestream;
        }
    }

    if (!libwpg::WPGraphics::isSupported(input)) {
        std::cerr << "ERROR: Unsupported file format (unsupported version) or file is encrypted!" << std::endl;
        delete input;
        return KoFilter::NotImplemented;
    }

    // do the conversion
    std::ostringstream tmpStringStream;
    FileOutputHandler tmpHandler(tmpStringStream);
    OdgExporter exporter(&tmpHandler);
    libwpg::WPGraphics::parse(input, &exporter);
    delete input;


    // create output store
    KoStore* storeout;
    storeout = KoStore::createStore(m_chain->outputFile(), KoStore::Write,
                                    "application/vnd.oasis.opendocument.graphics", KoStore::Zip);

    if (!storeout) {
        kWarning() << "Couldn't open the requested file.";
        return KoFilter::FileNotFound;
    }

#if 0
    if (!storeout->open("styles.xml")) {
        kWarning() << "Couldn't open the file 'styles.xml'.";
        return KoFilter::CreationError;
    }
    //storeout->write( createStyles() );
    storeout->close();
#endif

    if (!storeout->open("content.xml")) {
        kWarning() << "Couldn't open the file 'content.xml'.";
        return KoFilter::CreationError;
    }
    storeout->write(tmpStringStream.str().c_str());
    storeout->close();

    // store document manifest
    storeout->enterDirectory("META-INF");
    if (!storeout->open("manifest.xml")) {
        kWarning() << "Couldn't open the file 'META-INF/manifest.xml'.";
        return KoFilter::CreationError;
    }
    storeout->write(createManifest());
    storeout->close();

    // we are done!
    delete storeout;

    return KoFilter::OK;
}
