/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2000 Michael Johnson <mikej@xnet.com>
   SPDX-FileCopyrightText: 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2010-2011 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Christoph Cullmann <cullmann@kde.org>
   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DocxExport.h"

#include <QFile>

// KF5
#include <KPluginFactory>

// Calligra
#include <KoFilterChain.h>
#include <KoStore.h>
#include <KoXmlWriter.h>

// Filter libraries
#include "OdtReader.h"

// This filter
#include "DocxExportDebug.h"
#include "DocxFile.h"
#include "DocxStyleWriter.h"
#include "OdfReaderDocxContext.h"
#include "OdfTextReaderDocxBackend.h"
#include "OdtReaderDocxBackend.h"

K_PLUGIN_FACTORY_WITH_JSON(DocxExportFactory, "calligra_filter_odt2docx.json", registerPlugin<DocxExport>();)

// Needed to instantiate the plugin factory.
#include "DocxExport.moc"

DocxExport::DocxExport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

DocxExport::~DocxExport() = default;

KoFilter::ConversionStatus DocxExport::convert(const QByteArray &from, const QByteArray &to)
{
    // Check for types
    if (from != "application/vnd.oasis.opendocument.text" || to != "application/vnd.openxmlformats-officedocument.wordprocessingml.document") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read, "", KoStore::Auto);
    if (!odfStore->open("mimetype")) {
        errorDocx << "Unable to open input file!" << Qt::endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Start the conversion

    // Collects all the parts of the docx file and writes the result at the end.
    DocxFile docxFile;

    OdfReaderDocxContext docxBackendContext(odfStore, &docxFile);

    // The backends
    OdtReaderDocxBackend docxBackend;
    OdfTextReaderDocxBackend docxTextBackend;

    // The readers
    OdtReader odtReader;
    OdfTextReader odfTextReader;
    odfTextReader.setBackend(&docxTextBackend);
    odtReader.setTextReader(&odfTextReader);

    if (!odtReader.analyzeContent(&docxBackendContext)) {
        return KoFilter::ParsingError;
    }

    DocxStyleWriter styleWriter(&docxBackendContext);
    styleWriter.read();

    // Add the styles to the docx file.
    docxFile.addContentFile("", "/word/styles.xml", "application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml", styleWriter.documentContent());

    if (!odtReader.readContent(&docxBackend, &docxBackendContext)) {
        return KoFilter::ParsingError;
    }

    bool commentsExist = !docxBackendContext.commentsContent().isEmpty();

    if (commentsExist) {
        // Add comments file
        QByteArray tempArray;
        QBuffer tempBuffer(&tempArray);
        KoXmlWriter commentWriter(&tempBuffer);
        commentWriter.startDocument(nullptr);
        commentWriter.startElement("w:comments");
        commentWriter.addAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");
        commentWriter.addCompleteElement(docxBackendContext.commentsContent());
        commentWriter.endElement(); // w:comments
        commentWriter.endDocument();
        docxFile.addContentFile("", "/word/comments.xml", "application/vnd.openxmlformats-officedocument.wordprocessingml.comments+xml", tempArray);
    }

    // Add the newly converted document contents to the docx file.
    docxFile.addContentFile("",
                            "/word/document.xml",
                            "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml",
                            docxBackendContext.documentContent());

    // Write the output file.
    return docxFile.writeDocx(m_chain->outputFile(), to, docxBackendContext, commentsExist);
}
