/* This file is part of the KDE project

   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>
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


// Own
#include "EpubFile.h"

// Qt
#include <QString>
#include <QByteArray>
#include <QList>

// Calligra
#include <KoXmlWriter.h>
#include <KoStore.h>
#include <KoStoreDevice.h>

#include "EpubExportDebug.h"


// ================================================================
//                         class EpubFile

EpubFile::EpubFile()
{
}

EpubFile::~EpubFile()
{
}


KoFilter::ConversionStatus EpubFile::writeEpub(const QString &fileName,
                                               const QByteArray &appIdentification,
                                               QHash<QString, QString> metadata)
{
    // Create the store and check if everything went well.
    KoStore *epubStore = KoStore::createStore(fileName, KoStore::Write,
                                              appIdentification, KoStore::Auto);
    if (!epubStore || epubStore->bad()) {
        warnEpub << "Unable to create output file!";
        delete epubStore;
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus  status;

    // Write META-INF/container.xml
    status = writeMetaInf(epubStore);
    if (status != KoFilter::OK) {
        delete epubStore;
        return status;
    }

    // Write content.opf
    status = writeOpf(epubStore, metadata);
    if (status != KoFilter::OK) {
        delete epubStore;
        return status;
    }

    // Write toc.ncx
    status = writeNcx(epubStore, metadata);
    if (status != KoFilter::OK) {
        delete epubStore;
        return status;
    }

    // Write contents of added files.
    status = FileCollector::writeFiles(epubStore);

    delete epubStore;
    return status;
}


// ----------------------------------------------------------------
//                         Private functions


KoFilter::ConversionStatus EpubFile::writeMetaInf(KoStore *epubStore)
{
    // We can hardcode this one.
    if (!epubStore->open("META-INF/container.xml")) {
        debugEpub << "Can not to open META-INF/container.xml.";
        return KoFilter::CreationError;
    }

    KoStoreDevice metaDevice(epubStore);
    KoXmlWriter writer(&metaDevice);

    writer.startElement("container");
    writer.addAttribute("version", "1.0");
    writer.addAttribute("xmlns", "urn:oasis:names:tc:opendocument:xmlns:container");

    writer.startElement("rootfiles");

    writer.startElement("rootfile");
    writer.addAttribute("full-path", pathPrefix() + "content.opf");
    writer.addAttribute("media-type", "application/oebps-package+xml");
    writer.endElement(); // rootfile

    writer.endElement(); // rootfiles
    writer.endElement(); // container

    epubStore->close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus EpubFile::writeOpf(KoStore *epubStore,
                                              QHash<QString, QString> &metadata)
{
    if (!epubStore->open(pathPrefix() + "content.opf")) {
        debugEpub << "Can not create content.opf .";
        return KoFilter::CreationError;
    }

    KoStoreDevice opfDevice(epubStore);
    KoXmlWriter writer(&opfDevice);

    //FIXME: Write   <?xml version="1.0"?> -> FIXED
    writer.startDocument(nullptr, nullptr, nullptr);

    // FIXME: Get the unique identifier
    writer.startElement("package");
    writer.addAttribute("version", "2.0");
    writer.addAttribute("xmlns", "http://www.idpf.org/2007/opf");
    writer.addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    writer.addAttribute("xmlns:opf", "http://www.idpf.org/2007/opf");
    writer.addAttribute("unique-identifier", "BookId"); // FIXME: Where to get this?

    // ==== metadata ====
    writer.startElement("metadata");

    // Required elements are: title, language and identifier
    writer.startElement("dc:title");
    writer.addTextNode(metadata.value("title"));
    writer.endElement(); // dc:title

    writer.startElement("meta");
    writer.addAttribute("name", "cover");
    writer.addAttribute("content", "cover-image");
    writer.endElement();

    writer.startElement("dc:language");
    if (!metadata.value("language").isEmpty())
        writer.addTextNode(metadata.value("language"));
    else
        writer.addTextNode("en");
    writer.endElement(); // dc:language

    writer.startElement("dc:identifier");
    writer.addAttribute("id", "BookId");
    writer.addAttribute("opf:scheme", "ISBN");
    writer.addTextNode("123456789X");  // FIXME: Where to get this?
    writer.endElement(); // dc:identifier

    writer.startElement("dc:creator");
    writer.addTextNode(metadata.value("creator"));  // It's the "Author" profile name
    writer.endElement(); // dc:creator

    writer.startElement("dc:subject");
    writer.addTextNode("");  // FIXME: Here should come subject info with form : Fiction &amp; Fantasy &amp; ...
    writer.endElement(); // dc:subject

    // FIXME: many more (optional)

    writer.endElement(); // metadata

    // ==== manifest ====
    writer.startElement("manifest");

    // Write manifest entry for each embedded file.
    foreach(FileInfo *file, files()) {
        // Since paths are relative from where this file is, remove
        // the common prefix from the reference.
        QString relativeFilename = file->m_fileName;
        if (relativeFilename.startsWith(pathPrefix()))
            relativeFilename = relativeFilename.right(relativeFilename.size()
                                                      - pathPrefix().size());

        writer.startElement("item");
        writer.addAttribute("id", file->m_id);
        writer.addAttribute("href", relativeFilename);
        writer.addAttribute("media-type", file->m_mimetype);
        writer.endElement(); // item
    }

    // toc.ncx 
    writer.startElement("item");
    writer.addAttribute("id", "ncx");
    writer.addAttribute("href", "toc.ncx");
    writer.addAttribute("media-type", "application/x-dtbncx+xml");
    writer.endElement(); // item

    writer.endElement(); // manifest

    // ==== spine ====
    writer.startElement("spine");
    writer.addAttribute("toc", "ncx");

    // If there is a cover image, it should be the first page for now
    // maybe we can have an option to lets user choose the cover page
    // number in future.
    foreach (FileInfo *file, files()) {
        if ( file->m_id == "cover") {
            writer.startElement("itemref");
            writer.addAttribute("idref", file->m_id);
            writer.endElement(); // itemref
            break;
        }
    }
    foreach(FileInfo *file, files()) {
        // Since paths are relative from where this file is, remove
        // the common prefix from the reference.
        if (file->m_id == "cover") {
            continue;
        }
        QString relativeFilename = file->m_fileName;
        if (relativeFilename.contains(filePrefix())) {
            if (relativeFilename.startsWith(pathPrefix())) {
                relativeFilename = relativeFilename.right(relativeFilename.size() 
                                                          - pathPrefix().size());
            }
            writer.startElement("itemref");
            writer.addAttribute("idref", file->m_id);
            writer.endElement(); // itemref
        }
    }

    writer.endElement(); // spine

    /*
    This part is only efficient in EPub2.X
    In fact the <guide> element is ignores by EPub3 readers
    For more info. see official specifications
    */
    bool atLeastOneInfo = false;

    foreach (FileInfo *file, files()) {
        // Here we coul expend the filter with cover | toc | index, etc..
        // For now, be simple
        if ( file->m_id == "cover") {
            atLeastOneInfo = true;
            break;
        }
    }

    // If we have something interesting to point out, write <guide> element
    if (atLeastOneInfo) {
        // ==== Guide ====
        writer.startElement("guide");

        //if there is a cover
        foreach (FileInfo *file, files()) {
            if ( file->m_id == "cover") {
                writer.startElement("reference");
                writer.addAttribute("href", "cover.xhtml");
                writer.addAttribute("type", "cover");
                writer.endElement(); // reference
                break;
            }
        }

        writer.endElement(); // guide
    }

    writer.endElement(); // package

    epubStore->close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus EpubFile::writeNcx(KoStore *epubStore,
                                              QHash<QString, QString> &metadata)
{
    if (!epubStore->open(pathPrefix() + "toc.ncx")) {
        debugEpub << "Can not create toc.ncx.";
        return KoFilter::CreationError;
    }

    KoStoreDevice ncxDevice(epubStore);
    KoXmlWriter writer(&ncxDevice);

//    QByteArray arr =" <!DOCTYPE ncx PUBLIC "-//NISO//DTD ncx 2005-1//EN" "http://www.daisy.org/z3986/2005/ncx-2005-1.dtd">";
//    ncxDevice.write(arr);

    writer.startElement("ncx");
    writer.addAttribute("version", "2005-1");
    writer.addAttribute("xml:lang", "en");
    writer.addAttribute("xmlns", "http://www.daisy.org/z3986/2005/ncx/");

    writer.startElement("head");

//    arr = "<!-- The following four metadata items are required for all NCX documents,"
//            "including those conforming to the relaxed constraints of OPS 2.0 -->";
//    ncxDevice.write(arr);

    // ##### write meta tags ######
    writer.startElement("meta");
    writer.addAttribute("name", "dtb:uid");
    writer.addAttribute("content", "123456789X");
    writer.endElement();

    writer.startElement("meta");
    writer.addAttribute("name", "dtb:depth");
    writer.addAttribute("content", "1"); // FIXME: For now it is 1
    writer.endElement();

    writer.startElement("meta");
    writer.addAttribute("name", "dtb:totalPageCount");
    writer.addAttribute("content", "0"); // must be 0
    writer.endElement();

    writer.startElement("meta");
    writer.addAttribute("name", "dtb:maxPageNumber");
    writer.addAttribute("content", "0"); // must be 0
    writer.endElement();

    writer.endElement(); //head


    // ###### title ########
    writer.startElement("docTitle");
    writer.startElement("text");
    writer.addTextNode(metadata.value("title"));
    writer.endElement();
    writer.endElement(); // docTitle

    // ##### docAuthor ######
    writer.startElement("docAuthor");

    writer.startElement("text");
    writer.addTextNode(metadata.value("creator"));
    writer.endElement();

    writer.endElement(); // docAuthor

    // #### navMap #####
    writer.startElement("navMap");

    int playOrder = 1;
    foreach (FileInfo *file, files()) {
        if ( file->m_id == "cover") {
            writer.startElement("navPoint");
            writer.addAttribute("id", "navpoint-" + QString::number(playOrder));
            writer.addAttribute("playOrder", QString::number(playOrder));

            writer.startElement("navLabel");

            writer.startElement("text");
            writer.addTextNode(file->m_label);
            writer.endElement();

            writer.endElement(); // navLabel

            writer.startElement("content");
            QString src = file->m_fileName;
            src = src.right(src.size() - pathPrefix().size());
            writer.addAttribute("src", src);
            writer.endElement(); // content

            writer.endElement(); // navePoint
            playOrder ++;
            break;
        }
    }

    foreach(FileInfo *file, files()) {

        if (file->m_label.isEmpty()) {
            continue;
        }
        if (file->m_id == "cover-html") {
            continue;
        }

        // Since paths are relative from where this file is, remove
        // the common prefix from the reference.
        QString relativeFilename = file->m_fileName;
        if (relativeFilename.contains(filePrefix())) {
            if (relativeFilename.startsWith(pathPrefix()))
                relativeFilename = relativeFilename.right(relativeFilename.size()
                                                          - pathPrefix().size());

            writer.startElement("navPoint");
            writer.addAttribute("id", "navpoint-" + QString::number(playOrder));
            writer.addAttribute("playOrder", QString::number(playOrder));

            writer.startElement("navLabel");

            writer.startElement("text");
            writer.addTextNode(file->m_label);
            writer.endElement();

            writer.endElement(); // navLabel

            writer.startElement("content");
            writer.addAttribute("src", relativeFilename);
            writer.endElement(); // content

            writer.endElement(); // navePoint

            playOrder++;
        }
    }

    writer.endElement(); // navMap
    writer.endElement(); // ncx

    epubStore->close();
    return KoFilter::OK;
}
