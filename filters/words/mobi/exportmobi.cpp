/* This file is part of the KDE project
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

#include "exportmobi.h"

#include <KoFilterChain.h>
#include <kpluginfactory.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

#include "MobiExportDebug.h"
#include "OdtMobiHtmlConverter.h"
#include "FileCollector.h"
#include "MobiFile.h"
#include "OdfParser.h"
#include "PalmDocCompression.h"
#include "MobiHeaderGenerator.h"



K_PLUGIN_FACTORY_WITH_JSON(ExportMobiFactory, "calligra_filter_odt2mobi.json",
			   registerPlugin<ExportMobi>();)

// Needed to instantiate the plugin factory.
#include "exportmobi.moc"


ExportMobi::ExportMobi(QObject *parent, const QVariantList &) :
    KoFilter(parent)
{
}

ExportMobi::~ExportMobi()
{
}

KoFilter::ConversionStatus ExportMobi::convert(const QByteArray &from, const QByteArray &to)
{
    // Check mimetypes
    if (to != "application/x-mobipocket-ebook" || from != "application/vnd.oasis.opendocument.text") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read,
                                             "", KoStore::Auto);
    if (!odfStore->open("mimetype")) {
        errorMobi << "Unable to open input file!" << endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    //************ Start the conversion *************
    KoFilter::ConversionStatus  status;

    // Parse input files.
    OdfParser  odfParser;
    status = odfParser.parseMetadata(odfStore, m_metaData);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // Parse manifest
    status = odfParser.parseManifest(odfStore, m_manifest);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }


    // Create content files.
    MobiFile mobi;

    OdtMobiHtmlConverter converter;
    OdtMobiHtmlConverter::ConversionOptions options = {
        false,                   // don't put styles in css file
        false,                    // don't break into chapters
        true                     // It is mobi.
    };
    status = converter.convertContent(odfStore, m_metaData, &options, &mobi,
                                      m_imagesSrcList);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    // Extract images
    status = extractImages(odfStore, &mobi);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }


    // I need the text content in FileCollector
    QByteArray textContent;
    foreach (FileCollector::FileInfo *file, mobi.files()) {
        if (file->m_mimetype == "application/xhtml+xml") {
            textContent = file->m_fileContents;
            break;
        }
    }

    // Start compression data.
    PalmDocCompression palmCompression;
    QByteArray compressContent;
    QList<qint32> recordOffset;

    palmCompression.compressContent(textContent, compressContent, recordOffset);

    // Generate mobi headers.
    MobiHeaderGenerator headerGenerator;
    headerGenerator.generateMobiHeaders(m_metaData, (compressContent.size()/* - recordOffset.size()*/),
                                        textContent.size(), m_imagesSize, recordOffset);


    // After each text block i should insert a zero byte, i use record offset to
    // set them after each insert record offset move one byte to forward.
    int move = 0;
    for (int i = 1; i < recordOffset.size(); i++) {
        compressContent.insert((recordOffset.at(i) + qint32(move)), '\0');
        move++;
    }

    mobi.addContentRawText(compressContent);


    // Write Mobi file
    status = mobi.writeMobiFile(m_chain->outputFile(), headerGenerator);
    if (status != KoFilter::OK) {
        delete odfStore;
        return status;
    }

    delete odfStore;
    return KoFilter::OK;
}

KoFilter::ConversionStatus ExportMobi::extractImages(KoStore *odfStore, MobiFile *mobi)
{

    // Extract images and add them to epubFile one by one
    QByteArray imgContent;
    int imgId = 1;
    foreach (const QString &imgSrc, m_imagesSrcList.keys()) {
        if (!odfStore->hasFile(imgSrc)) {
            warnMobi << "Can not to extract this image, image "<< imgSrc<< "is an external image";
            // Ignore the external image.
            continue;
        }
        if (!odfStore->extractFile(imgSrc, imgContent)) {
            debugMobi << "Can not to extract file";
            return KoFilter::FileNotFound;
        }
        m_imagesSize << imgContent.size();
        m_imagesList.insert(imgId, imgContent);
        mobi->addContentImage(imgId, imgContent);
        imgId++;
    }
    return KoFilter::OK;
}
