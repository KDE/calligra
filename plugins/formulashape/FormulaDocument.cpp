/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "FormulaDocument.h"

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

// KFormula
#include "FormulaDebug.h"
#include "FormulaPart.h"
#include "KoFormulaShape.h"

class FormulaDocument::Private
{
public:
    Private();
    ~Private();

    KoFormulaShape *parent;
};

FormulaDocument::Private::Private() = default;

FormulaDocument::Private::~Private() = default;

FormulaDocument::FormulaDocument(KoFormulaShape *parent)
    : KoDocument(new FormulaPart(nullptr))
    , d(new Private)
{
    d->parent = parent;
}

FormulaDocument::~FormulaDocument()
{
    delete d;
}

bool FormulaDocument::loadOdf(KoOdfReadStore &odfStore)
{
    KoXmlDocument doc = odfStore.contentDoc();
    KoXmlElement bodyElement = doc.documentElement();

    debugFormula << bodyElement.nodeName();

    if (bodyElement.localName() != "math" || bodyElement.namespaceURI() != KoXmlNS::math) {
        errorFormula << "No <math:math> element found.";
        return false;
    }

    // When the formula is stored in an embedded document, it seems to
    // always have a <math:semantics> element that surrounds the
    // actual formula.  I have to check with the MathML spec what this
    // actually means and if it is obligatory.  /iw
    KoXmlNode semanticsNode = bodyElement.namedItemNS(KoXmlNS::math, "semantics");
    if (!semanticsNode.isNull()) {
        bodyElement = semanticsNode.toElement();
    }

    KoOdfLoadingContext odfLoadingContext(odfStore.styles(), odfStore.store());
    KoShapeLoadingContext context(odfLoadingContext, d->parent->resourceManager());

    return d->parent->loadOdfEmbedded(bodyElement, context);
}

bool FormulaDocument::loadXML(const KoXmlDocument &doc, KoStore *)
{
    Q_UNUSED(doc);

    // We don't support the old XML format any more.
    return false;
}

bool FormulaDocument::saveOdf(SavingContext &context)
{
    // FIXME: This code is copied from ChartDocument, so it needs to
    // be adapted to the needs of the KoFormulaShape.

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
    bodyWriter->startElement("office:formula");

    d->parent->saveOdf(savingContext);

    bodyWriter->endElement(); // office:formula
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

void FormulaDocument::paintContent(QPainter &painter, const QRect &rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
}
