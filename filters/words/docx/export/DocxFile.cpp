/* This file is part of the KDE project

   Copyright (C) 2012-2014 Inge Wallin            <inge@lysator.liu.se>
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

// Calligra
#include <KoXmlWriter.h>
#include <KoStore.h>
#include <KoStoreDevice.h>

// This filter
#include "OpcContentTypes.h"
#include "OdfReaderDocxContext.h"
#include "DocxExportDebug.h"


// ================================================================
//                         class DocxFile

DocxFile::DocxFile()
{
}

DocxFile::~DocxFile()
{
}

// todo: commentsexist should be probably replaced with qlist qpair later
KoFilter::ConversionStatus DocxFile::writeDocx(const QString &fileName,
                                               const QByteArray &appIdentification,
                                               const OdfReaderDocxContext &context,
                                               bool  commentsExist)
{
    Q_UNUSED(context);

    m_commentsExist = commentsExist;
    // Create the store and check if everything went well.
    // FIXME: Should docxStore be deleted from a finalizer?
    KoStore *docxStore = KoStore::createStore(fileName, KoStore::Write,
                                              appIdentification, KoStore::Auto, false);
    if (!docxStore || docxStore->bad()) {
        warnDocx << "Unable to create output file!";
        delete docxStore;
        return KoFilter::FileNotFound;
    }
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

    // Write contents of added files.
    status = FileCollector::writeFiles(docxStore);
    if (status != KoFilter::OK) {
        delete docxStore;
        return status;
    }

    // Finally, write the [Content_Types}.xml file.
    OpcContentTypes  contentTypes;
    contentTypes.addDefault("rels", "application/vnd.openxmlformats-package.relationships+xml");
    contentTypes.addDefault("xml", "application/xml");
    foreach (const FileInfo *file, files()) {
        contentTypes.addFile(file->fileName, file->mimetype);
    }
    contentTypes.writeToStore(docxStore);

    delete docxStore;
    return status;
}




// ----------------------------------------------------------------
//                         Private functions


KoFilter::ConversionStatus DocxFile::writeTopLevelRels(KoStore *docxStore)
{
    // We can hardcode this one.
    if (!docxStore->open("_rels/.rels")) {
        debugDocx << "Can not to open _rels/.rels.";
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
    writer.endElement();  // Relationship

    // Doc props core
    //writer.startElement("Relationship");
    //writer.addAttribute("Id", "rId2");
    //writer.addAttribute("Type", "http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties");
    //writer.addAttribute("Target", "docProps/core.xml");
    //writer.endElement();  // Relationship

    // Doc props app
    //writer.startElement("Relationship");
    //writer.addAttribute("Id", "rId3");
    //writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties");
    //writer.addAttribute("Target", "docProps/app.xml");
    //writer.endElement();  // Relationship

    writer.endElement();  // Relationships
    writer.endDocument();

    docxStore->close();
    return KoFilter::OK;
}

KoFilter::ConversionStatus DocxFile::writeDocumentRels(KoStore *docxStore)
{
    // We can hardcode this for now but should not be int he future.
    if (!docxStore->open("word/_rels/document.xml.rels")) {
        debugDocx << "Can not to open word/_rels/document.xml.rels.";
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
    // or rather go through a list of these and add all from the list
    if (m_commentsExist) {
        writer.startElement("Relationship");
        writer.addAttribute("Id", "rId2");
        writer.addAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/comments");
        writer.addAttribute("Target", "comments.xml");
        writer.endElement();
    }
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

