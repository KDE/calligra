/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010-2014 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StencilShapeFactory.h"

#include "StencilBoxDebug.h"

#include <KoDrag.h>
#include <KoOdf.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoProperties.h>
#include <KoShape.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeGroup.h>
#include <KoShapeGroupCommand.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeOdfSaveHelper.h>
#include <KoShapeRegistry.h>
#include <KoStore.h>
#include <KoXmlNS.h>
#include <SvgParser.h>

#include <KCompressionDevice>
#include <QIODevice>
#include <QMimeData>

StencilShapeFactory::StencilShapeFactory(const QString &id, const QString &name, const KoProperties *props)
    : KoShapeFactoryBase(id, name)
    , m_properties(props)
{
    // ensures ShapeCollectionDocker ignores the stencil shapes for now,
    // exclusively available by StencilBox
    setFamily("stencil");
}

StencilShapeFactory::~StencilShapeFactory()
{
    delete m_properties;
}

KoShape *StencilShapeFactory::createFromOdf(KoStore *store, KoDocumentResourceManager *documentRes) const
{
    KoOdfReadStore odfStore(store);
    QString errorMessage;
    if (!odfStore.loadAndParse(errorMessage)) {
        errorStencilBox << "loading and parsing failed:" << errorMessage << Qt::endl;
        return nullptr;
    }

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        errorStencilBox << "No body tag found!" << Qt::endl;
        return nullptr;
    }

    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "drawing");
    if (body.isNull()) {
        errorStencilBox << "No office:drawing tag found!" << Qt::endl;
        return nullptr;
    }

    KoXmlElement page = KoXml::namedItemNS(body, KoXmlNS::draw, "page");
    if (page.isNull()) {
        errorStencilBox << "No page found!" << Qt::endl;
        return nullptr;
    }

    KoXmlElement shapeElement = KoXml::namedItemNS(page, KoXmlNS::draw, "g");
    if (shapeElement.isNull()) {
        shapeElement = KoXml::namedItemNS(page, KoXmlNS::draw, "custom-shape");
        if (shapeElement.isNull()) {
            errorStencilBox << "draw:g or draw:custom-shape element not found!" << Qt::endl;
            return nullptr;
        }
    }

    KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
    KoShapeLoadingContext context(loadingContext, documentRes);

    KoShapeRegistry *registry = KoShapeRegistry::instance();
    foreach (const QString &id, registry->keys()) {
        KoShapeFactoryBase *shapeFactory = registry->value(id);
        shapeFactory->newDocumentResourceManager(documentRes);
    }

    return KoShapeRegistry::instance()->createShapeFromOdf(shapeElement, context);
}

KoShape *StencilShapeFactory::createFromSvg(QIODevice *in, KoDocumentResourceManager *documentRes) const
{
    if (!in->open(QIODevice::ReadOnly)) {
        debugStencilBox << "svg file open error";
        return nullptr;
    }

    int line, col;
    QString errormessage;
    KoXmlDocument inputDoc;
    const bool parsed = inputDoc.setContent(in, &errormessage, &line, &col);
    in->close();

    if (!parsed) {
        debugStencilBox << "Error while parsing file: "
                        << "at line " << line << " column: " << col << " message: " << errormessage << Qt::endl;
        return nullptr;
    }

    SvgParser parser(documentRes);
    parser.setXmlBaseDir(id());
    QList<KoShape *> shapes = parser.parseSvg(inputDoc.documentElement());
    if (shapes.isEmpty())
        return nullptr;
    if (shapes.count() == 1)
        return shapes.first();

    KoShapeGroup *svgGroup = new KoShapeGroup;
    KoShapeGroupCommand cmd(svgGroup, shapes);
    cmd.redo();

    return svgGroup;
}

KoShape *StencilShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    KoShape *shape = nullptr;
    KoStore *store = nullptr;
    QIODevice *in = nullptr;
    QString ext = id().mid(id().lastIndexOf('.')).toLower();
    if (ext == ".odg") {
        store = KoStore::createStore(id(), KoStore::Read);
        if (!store->bad()) {
            shape = createFromOdf(store, documentResources);
        }
        delete store;
    } else if (ext == ".svg") {
        in = new KCompressionDevice(id(), KCompressionDevice::None);
        shape = createFromSvg(in, documentResources);
        delete in;
    } else if (ext == ".svgz") {
        in = new KCompressionDevice(id(), KCompressionDevice::GZip);
        shape = createFromSvg(in, documentResources);
        delete in;
    } else {
        debugStencilBox << "stencil format" << ext << "unsupported";
    }

    if (shape) {
        if (m_properties->intProperty("keepAspectRatio") == 1)
            shape->setKeepAspectRatio(true);
    }

    return shape;
}

// StencilShapeFactory shouldn't participate element support detection
bool StencilShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(e);
    Q_UNUSED(context);
    return false;
}
