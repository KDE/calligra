/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "ChartDocument.h"

// Qt
#include <QIODevice>
#include <QPainter>
#include <QWidget>

// Calligra
#include <KoDocument.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoView.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

// KoChart
#include "ChartDebug.h"
#include "ChartPart.h"
#include "ChartShape.h"

namespace KoChart
{

class ChartDocument::Private
{
public:
    Private();
    ~Private();

    ChartShape *parent;
};

ChartDocument::Private::Private() = default;

ChartDocument::Private::~Private() = default;

ChartDocument::ChartDocument(ChartShape *parent)
    : KoDocument(new ChartPart(nullptr))
    , d(new Private)
{
    d->parent = parent;
    // Needed by KoDocument::nativeOasisMimeType().
    // KoEmbeddedDocumentSaver uses that method to
    // get the mimetype of the embedded document.
}

ChartDocument::~ChartDocument()
{
    delete d;
}

bool ChartDocument::loadOdf(KoOdfReadStore &odfStore)
{
    KoXmlDocument doc = odfStore.contentDoc();
    KoXmlNode bodyNode = doc.documentElement().namedItemNS(KoXmlNS::office, "body");
    if (bodyNode.isNull()) {
        errorChart << "No <office:body> element found.";
        return false;
    }
    KoXmlNode chartElementParentNode = bodyNode.namedItemNS(KoXmlNS::office, "chart");
    if (chartElementParentNode.isNull()) {
        errorChart << "No <office:chart> element found.";
        return false;
    }
    KoXmlElement chartElement = chartElementParentNode.namedItemNS(KoXmlNS::chart, "chart").toElement();
    if (chartElement.isNull()) {
        errorChart << "No <chart:chart> element found.";
        return false;
    }
    KoOdfLoadingContext odfLoadingContext(odfStore.styles(), odfStore.store());
    KoShapeLoadingContext context(odfLoadingContext, d->parent->resourceManager());

    return d->parent->loadOdfChartElement(chartElement, context);
}

bool ChartDocument::loadXML(const KoXmlDocument &doc, KoStore *)
{
    Q_UNUSED(doc);

    // We don't support the old XML format any more.
    return false;
}

bool ChartDocument::saveOdf(SavingContext &context)
{
    KoOdfWriteStore &odfStore = context.odfStore;
    KoStore *store = odfStore.store();
    KoXmlWriter *manifestWriter = odfStore.manifestWriter();
    KoXmlWriter *contentWriter = odfStore.contentWriter();
    if (!contentWriter)
        return false;

    KoGenStyles mainStyles;
    KoXmlWriter *bodyWriter = odfStore.bodyWriter();
    if (!bodyWriter)
        return false;

    KoEmbeddedDocumentSaver &embeddedSaver = context.embeddedSaver;

    KoShapeSavingContext savingContext(*bodyWriter, mainStyles, embeddedSaver);

    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:chart");

    d->parent->saveOdf(savingContext);

    bodyWriter->endElement(); // office:chart
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);
    odfStore.closeContentWriter();

    // Add manifest line for content.xml and styles.xml
    manifestWriter->addManifestEntry(url().path() + "/content.xml", "text/xml");
    manifestWriter->addManifestEntry(url().path() + "/styles.xml", "text/xml");

    // save the styles.xml
    if (!mainStyles.saveOdfStylesDotXml(store, manifestWriter))
        return false;

    if (!savingContext.saveDataCenter(store, manifestWriter)) {
        return false;
    }

    return true;
}

void ChartDocument::paintContent(QPainter &painter, const QRect &rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
}

} // namespace KoChart
