/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Pdf2OdgImport.h"

#include "PdfImportDebug.h"
#include "SvgOutputDev.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include <KoFilterChain.h>
#include <KoPAMasterPage.h>
#include <KoPAPage.h>
#include <KoPageLayout.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeLayer.h>
#include <KoXmlReader.h>
#include <SvgParser.h>
#include <commands/KoShapeUngroupCommand.h>

#include <KarbonDocument.h>

#include <KPluginFactory>

#include <poppler-version.h>

// Don't show this warning: it's an issue in poppler
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// poppler includes
#include <GlobalParams.h>
#include <PDFDoc.h>

#define POPPLER_VERSION_MACRO ((POPPLER_VERSION_MAJOR << 16) | (POPPLER_VERSION_MINOR << 8) | (POPPLER_VERSION_MICRO))

K_PLUGIN_FACTORY_WITH_JSON(Pdf2OdgImportFactory, "calligra_filter_pdf2odg.json", registerPlugin<Pdf2OdgImport>();)

Pdf2OdgImport::Pdf2OdgImport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
    debugPdf << "PDF Import Filter";
}

Pdf2OdgImport::~Pdf2OdgImport() = default;

KoFilter::ConversionStatus Pdf2OdgImport::convert(const QByteArray &from, const QByteArray &to)
{
    debugPdf << "to:" << to << " from:" << from;

    if (from != "application/pdf" || to != "application/vnd.oasis.opendocument.graphics") {
        return KoFilter::NotImplemented;
    }
    debugPdf << m_chain->inputFile();
    m_document = dynamic_cast<KarbonDocument *>(m_chain->outputDocument());
    if (!m_document) {
        errorPdf << "No KarbonDocument" << m_chain->outputDocument();
        return KoFilter::CreationError;
    }
    Q_ASSERT(m_document->pages().isEmpty());

    // read config file
    globalParams = std::unique_ptr<GlobalParams>(new GlobalParams);
    if (!globalParams)
        return KoFilter::NotImplemented;

#if POPPLER_VERSION_MACRO < QT_VERSION_CHECK(22, 03, 0)
    GooString *fname = new GooString(QFile::encodeName(m_chain->inputFile()).data());
    PDFDoc *pdfDoc = new PDFDoc(fname, 0, 0, 0);
#else
    std::unique_ptr<GooString> fname = std::make_unique<GooString>(QFile::encodeName(m_chain->inputFile()).data());
    PDFDoc *pdfDoc = new PDFDoc(std::move(fname));
#endif
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

    QString tmpPath = QDir::tempPath().append("/karbon");
    debugPdf << "converting pages" << firstPage << "-" << lastPage;
    KoFilter::ConversionStatus status = KoFilter::OK;
    for (int page = firstPage; status == KoFilter::OK && page <= lastPage; ++page) {
        QTemporaryFile tmpFile(tmpPath);
        if (!tmpFile.open()) {
            status = KoFilter::CreationError;
            break;
        }
        tmpFile.close();
        debugPdf << "tmpFile:" << tmpFile.fileName();
        SvgOutputDev *dev = new SvgOutputDev(tmpFile.fileName());
        if (dev->isOk()) {
            int rotate = 0;
            bool useMediaBox = true;
            bool crop = false;
            bool printing = false;
            pdfDoc->displayPage(dev, page, hDPI, vDPI, rotate, useMediaBox, crop, printing);
            dev->dumpContent();
            status = convert(page, tmpFile);
        }
        delete dev;
    }
    delete pdfDoc;
    globalParams.reset();
    return status;
}

KoFilter::ConversionStatus Pdf2OdgImport::convert(int pageNumber, QFile &in)
{
    debugPdf << "converting page:" << pageNumber << in.fileName();
    int line, col;
    QString errormessage;

    KoXmlDocument inputDoc;

    const bool parsed = inputDoc.setContent(&in, &errormessage, &line, &col);

    in.close();

    if (!parsed) {
        errorPdf << "Error while parsing file: "
                 << "at line " << line << " column: " << col << " message: " << errormessage << Qt::endl;
        // ### TODO: feedback to the user
        return KoFilter::ParsingError;
    }

    if (m_document->pages().count() < pageNumber) {
        debugPdf << "add page" << pageNumber;
        KoPAMasterPage *mp = dynamic_cast<KoPAMasterPage *>(m_document->pages(true).value(pageNumber - 1));
        if (!mp) {
            mp = new KoPAMasterPage();
            m_document->insertPage(mp, pageNumber - 1);
        }
        m_document->insertPage(new KoPAPage(mp), pageNumber - 1);
    }
    // Do the conversion!
    convert(inputDoc.documentElement(), pageNumber);

    return KoFilter::OK;
}

void Pdf2OdgImport::convert(const KoXmlElement &rootElement, int pageNumber)
{
    debugPdf << rootElement.tagName();
    if (!m_document) {
        return;
    }
    // set default page size to A4
    QSizeF pageSize(550.0, 841.0);

    SvgParser parser(m_document->resourceManager());
    parser.setXmlBaseDir(QFileInfo(m_chain->inputFile()).filePath());

    QList<KoShape *> toplevelShapes = parser.parseSvg(rootElement, &pageSize);
    // parse the root svg element
    buildDocument(toplevelShapes, parser.shapes(), pageNumber);

    // set the page size
    KoPageLayout &layout = m_document->pages().at(pageNumber - 1)->pageLayout();
    layout.width = pageSize.width();
    layout.height = pageSize.height();
}

void Pdf2OdgImport::buildDocument(const QList<KoShape *> &toplevelShapes, const QList<KoShape *> &shapes, int pageNumber)
{
    Q_UNUSED(shapes);
    KoPAPageBase *page = m_document->pages().at(pageNumber - 1);
    // if we have only top level groups, make them layers
    bool onlyTopLevelGroups = true;
    foreach (KoShape *shape, toplevelShapes) {
        if (!dynamic_cast<KoShapeGroup *>(shape) || shape->filterEffectStack()) {
            onlyTopLevelGroups = false;
            break;
        }
    }
    KoShapeLayer *oldLayer = nullptr;
    if (page->shapeCount()) {
        oldLayer = dynamic_cast<KoShapeLayer *>(page->shapes().first());
    }
    if (onlyTopLevelGroups) {
        foreach (KoShape *shape, toplevelShapes) {
            // ungroup toplevel groups
            KoShapeGroup *group = dynamic_cast<KoShapeGroup *>(shape);
            // NOTE:
            // Only groups on first page has visible == true
            // Maybe an issue with poppler?
            group->setVisible(true);
            QList<KoShape *> children = group->shapes();
            KoShapeUngroupCommand cmd(group, children, QList<KoShape *>() << group);
            cmd.redo();

            KoShapeLayer *layer = new KoShapeLayer();
            foreach (KoShape *child, children) {
                layer->addShape(child);
            }
            if (!group->name().isEmpty()) {
                layer->setName(group->name());
            }
            layer->setVisible(group->isVisible());
            layer->setZIndex(group->zIndex());
            page->addShape(layer);
            delete group;
        }
    } else {
        KoShapeLayer *layer = new KoShapeLayer();
        foreach (KoShape *shape, toplevelShapes) {
            shape->setVisible(true);
            layer->addShape(shape);
        }
        layer->setVisible(true);
        page->addShape(layer);
    }
    if (oldLayer) {
        page->removeShape(oldLayer);
        delete oldLayer;
    }
}

#include "Pdf2OdgImport.moc"
