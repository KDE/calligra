/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "PdfImport.h"
#include "SvgOutputDev.h"

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtSvg/QSvgGenerator>

#include <KoFilter.h>
#include <KoFilterChain.h>

#include <KGenericFactory>
#include <KRun>
#include <KProcess>
#include <KShell>
#include <KDebug>

#include <poppler/PDFDoc.h>
#include <poppler/GlobalParams.h>

typedef KGenericFactory<PdfImport> PdfImportFactory;
K_EXPORT_COMPONENT_FACTORY(libkarbonpdfimport, PdfImportFactory("kofficefilters"))

PdfImport::PdfImport(QObject*parent, const QStringList&)
        : KoFilter(parent)
{
    kDebug(30516) << "PDF Import Filter";
}

PdfImport::~PdfImport()
{
}

KoFilter::ConversionStatus PdfImport::convert(const QByteArray& from, const QByteArray& to)
{
    kDebug(30516) << "to:" << to << " from:" << from;

    if (from != "application/pdf" || to != "image/svg+xml") {
        return KoFilter::NotImplemented;
    }

    // read config file
    globalParams = new GlobalParams();
    if (! globalParams)
        return KoFilter::NotImplemented;

    GooString * fname = new GooString(QFile::encodeName(m_chain->inputFile()).data());
    PDFDoc * pdfDoc = new PDFDoc(fname, 0, 0, 0);
    if (! pdfDoc) {
        delete globalParams;
        return KoFilter::StupidError;
    }

    if (! pdfDoc->isOk()) {
        delete globalParams;
        delete pdfDoc;
        return KoFilter::StupidError;
    }

    double hDPI = 72.0;
    double vDPI = 72.0;

    int firstPage = 1;
    int lastPage = pdfDoc->getNumPages();

    kDebug(30516) << "converting pages" << firstPage << "-" << lastPage;

    SvgOutputDev * dev = new SvgOutputDev(m_chain->outputFile());
    if (dev->isOk()) {
        int rotate = 0;
        GBool useMediaBox = gTrue;
        GBool crop = gFalse;
        GBool printing = gFalse;
        pdfDoc->displayPages(dev, firstPage, lastPage, hDPI, vDPI, rotate, useMediaBox, crop, printing);
        dev->dumpContent();
    }

    kDebug(30516) << "wrote file to" << m_chain->outputFile();

    delete dev;
    delete pdfDoc;
    delete globalParams;
    globalParams = 0;

    // check for memory leaks
    Object::memCheck(stderr);

    return KoFilter::OK;
}

#include "PdfImport.moc"
