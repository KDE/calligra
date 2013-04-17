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
#include "DocxFile.h"

// Qt
#include <QString>
#include <QByteArray>
#include <QList>

// KDE
#include <kdebug.h>

// Calligra
#include <KoXmlWriter.h>
#include <KoStore.h>
#include <KoStoreDevice.h>


// ================================================================
//                         class DocxFile

DocxFile::DocxFile()
{
}

DocxFile::~DocxFile()
{
}


KoFilter::ConversionStatus DocxFile::writeDocx(const QString &fileName,
                                               const QByteArray &appIdentification,
                                               const OdtTraverserDocxContext &context)
{
    // Create the store and check if everything went well.
    KoStore *docxStore = KoStore::createStore(fileName, KoStore::Write,
                                              appIdentification, KoStore::Auto);
    if (!docxStore || docxStore->bad()) {
        kWarning(30003) << "Unable to create output file!";
        delete docxStore;
        return KoFilter::FileNotFound;
    }
    docxStore->disallowNameExpansion();

    KoFilter::ConversionStatus  status;

    // Write top-level rels
    status = writeTopLevelRels(docxStore);
    if (status != KoFilter::OK) {
        delete docxStore;
        return status;
    }

    // Write rels for word/document.xml
    status = writeDocumentRels(docxStore);
    if (status != KoFilter::OK) {
        delete docxStore;
        return status;
    }

#if 0
    // Write META-INF/container.xml
    status = writeMetaInf(docxStore);
    if (status != KoFilter::OK) {
        delete docxStore;
        return status;
    }

    // Write content.opf
    status = writeOpf(docxStore, metadata);
    if (status != KoFilter::OK) {
        delete docxStore;
        return status;
    }

    // Write toc.ncx
    status = writeNcx(docxStore, metadata);
    if (status != KoFilter::OK) {
        delete docxStore;
        return status;
    }
#endif
    // Write contents of added files.
    status = FileCollector::writeFiles(docxStore);

    delete docxStore;
    return status;
}


// ----------------------------------------------------------------
//                         Private functions


KoFilter::ConversionStatus DocxFile::writeTopLevelRels(KoStore *docxStore)
{
    // We can hardcode this one.
    if (!docxStore->open("_rels/.rels")) {
        kDebug(30503) << "Can not to open _rels/.rels.";
        return KoFilter::CreationError;
    }

    KoStoreDevice metaDevice(docxStore);
    KoXmlWriter writer(&metaDevice);

    writer.startDocument(0, 0, 0);
    writer.startElement("Relationships");
    writer.addAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/relationships");

    // The document itself
    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId1");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument");
    writer.addAttribute("Target", "word/document.xml");
    writer.endElement();        // Relationship

    // Doc props core
    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId2");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties");
    writer.addAttribute("Target", "docProps/core.xml");
    writer.endElement();        // Relationship

    // Doc props app
    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId3");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties");
    writer.addAttribute("Target", "docProps/app.xml");
    writer.endElement();        // Relationship

    writer.endElement();        // Relationships
    writer.endDocument();

    docxStore->close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus DocxFile::writeDocumentRels(KoStore *docxStore)
{
    // We can hardcode this for now but should not be int he future.
    if (!docxStore->open("word/_rels/document.xml.rels")) {
        kDebug(30503) << "Can not to open word/_rels/document.xml.rels.";
        return KoFilter::CreationError;
    }

    KoStoreDevice metaDevice(docxStore);
    KoXmlWriter writer(&metaDevice);

    writer.startDocument(0, 0, 0);
    writer.startElement("Relationships");
    writer.addAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/relationships");

    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId1");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles");
    writer.addAttribute("Target", "styles.xml");
    writer.endElement();

    // FIXME: Enable these when we find that we need them
#if 0
    // Settings.xml
    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId2");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/settings");
    writer.addAttribute("Target", "settings.xml");
    writer.endElement();
#endif
#if 0
    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId3");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/webSettings");
    writer.addAttribute("Target", "webSettings.xml");
    writer.endElement();
#endif
#if 0
    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId4");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/fontTable");
    writer.addAttribute("Target", "fontTable.xml");
    writer.endElement();
#endif
#if 0
    writer.startElement("Relationship");
    writer.addAttribute("Id", "rId5");
    writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme");
    writer.addAttribute("Target", "theme/theme1.xml");
    writer.endElement();
#endif

    writer.endElement();        // Relationships
    writer.endDocument();

    docxStore->close();
    return KoFilter::OK;
}

#if 0
KoFilter::ConversionStatus DocxFile::writeMetaInf(KoStore *docxStore)
{
    // We can hardcode this one.
    if (!docxStore->open("META-INF/container.xml")) {
        kDebug(30503) << "Can not to open META-INF/container.xml.";
        return KoFilter::CreationError;
    }

    KoStoreDevice metaDevice(docxStore);
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

    docxStore->close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus DocxFile::writeOpf(KoStore *docxStore,
                                              QHash<QString, QString> &metadata)
{
    if (!docxStore->open(pathPrefix() + "content.opf")) {
        kDebug(30503) << "Can not create content.opf .";
        return KoFilter::CreationError;
    }

    KoStoreDevice opfDevice(docxStore);
    KoXmlWriter writer(&opfDevice);

    //FIXME: Write   <?xml version="1.0"?> -> FIXED
    writer.startDocument(NULL,NULL,NULL);

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

    // FIXME: dc:creator and many more (optional)

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

    writer.endElement(); // package

    docxStore->close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus DocxFile::writeNcx(KoStore *docxStore,
                                              QHash<QString, QString> &metadata)
{
    if (!docxStore->open(pathPrefix() + "toc.ncx")) {
        kDebug(30503) << "Can not create toc.ncx.";
        return KoFilter::CreationError;
    }

    KoStoreDevice ncxDevice(docxStore);
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

    docxStore->close();
    return KoFilter::OK;
}
#endif
