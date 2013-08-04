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

#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <KoXmlWriter.h>

#include <kpluginfactory.h>
#include <kdebug.h>


#include <QString>
#include <QFile>

#include <libwpg/libwpg.h>
#if LIBWPG_VERSION_MINOR<2
#include <libwpg/WPGStreamImplementation.h>
#else
#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>
#endif

#include <iostream>

K_PLUGIN_FACTORY_WITH_JSON(WPGImportFactory, "calligra_filter_wpg2svg.json", registerPlugin<WPGImport>();)
//K_EXPORT_PLUGIN(WPGImportFactory("calligrafilters"))

WPGImport::WPGImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

WPGImport::~WPGImport()
{
}


KoFilter::ConversionStatus WPGImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/x-wpg")
        return KoFilter::NotImplemented;

    if (to != "image/svg+xml")
        return KoFilter::NotImplemented;

#if LIBWPG_VERSION_MINOR<2
    WPXInputStream* input = new libwpg::WPGFileStream(m_chain->inputFile().toLocal8Bit());
    if (input->isOLEStream()) {
        WPXInputStream* olestream = input->getDocumentOLEStream();
        if (olestream) {
            delete input;
            input = olestream;
        }
    }
    libwpg::WPGString output;
#else
    WPXInputStream* input = new WPXFileStream(m_chain->inputFile().toLocal8Bit());
    if (input->isOLEStream()) {
        WPXInputStream* olestream = input->getDocumentOLEStream("Anything");
        if (olestream) {
            delete input;
            input = olestream;
        }
     }
     ::WPXString output;
#endif

    if (!libwpg::WPGraphics::isSupported(input)) {
        kWarning() << "ERROR: Unsupported file format (unsupported version) or file is encrypted!";
        delete input;
        return KoFilter::NotImplemented;
    }

    if (!libwpg::WPGraphics::generateSVG(input, output)) {
        kWarning() << "ERROR: SVG Generation failed!";
        delete input;
        return KoFilter::ParsingError;
    }

    delete input;

    QFile outputFile(m_chain->outputFile());
    if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        kWarning() << "ERROR: Could not open output file" << m_chain->outputFile();
        return KoFilter::InternalError;
    }
    outputFile.write(output.cstr());
    outputFile.close();

    return KoFilter::OK;
}
