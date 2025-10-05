/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PdfImport.h"

#include "PdfImportDebug.h"
#include "SvgOutputDev.h"

#include <QFile>
#include <QString>

#include <KoFilter.h>
#include <KoFilterChain.h>

#include <KPluginFactory>

// Don't show this warning: it's an issue in poppler
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// poppler includes
#include <GlobalParams.h>
#include <PDFDoc.h>

#include <memory>

K_PLUGIN_FACTORY_WITH_JSON(PdfImportFactory, "calligra_filter_pdf2svg.json", registerPlugin<PdfImport>();)

PdfImport::PdfImport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
    debugPdf << "PDF Import Filter";
}

PdfImport::~PdfImport() = default;

KoFilter::ConversionStatus PdfImport::convert(const QByteArray &from, const QByteArray &to)
{
    debugPdf << "to:" << to << " from:" << from;

    if (from != "application/pdf" || to != "image/svg+xml") {
        return KoFilter::NotImplemented;
    }

    // read config file
    globalParams = std::make_unique<GlobalParams>();
    if (!globalParams) {
        return KoFilter::NotImplemented;
    }

    auto fname = std::make_unique<GooString>(QFile::encodeName(m_chain->inputFile()).data());
    auto pdfDoc = new PDFDoc(std::move(fname));
    if (!pdfDoc) {
        globalParams.reset();
        return KoFilter::StupidError;
    }

    if (!pdfDoc->isOk()) {
        globalParams.reset();
        delete pdfDoc;
        return KoFilter::StupidError;
    }

    double hDPI = 72.0;
    double vDPI = 72.0;

    int firstPage = 1;
    int lastPage = pdfDoc->getNumPages();

    debugPdf << "converting pages" << firstPage << "-" << lastPage;

    SvgOutputDev dev(m_chain->outputFile());
    if (dev.isOk()) {
        int rotate = 0;
        bool useMediaBox = true;
        bool crop = false;
        bool printing = false;
        pdfDoc->displayPages(&dev, firstPage, lastPage, hDPI, vDPI, rotate, useMediaBox, crop, printing);
        dev.dumpContent();
    }

    debugPdf << "wrote file to" << m_chain->outputFile();

    delete pdfDoc;
    globalParams.reset();

    return KoFilter::OK;
}

#include "PdfImport.moc"
