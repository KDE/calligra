/* This file is part of the KDE project
 * Copyright (C) 2008 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (C) 2010-2014 Yue Liu <yue.liu@mail.com>
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

#include "StencilShapeFactory.h"

#include "StencilBoxDebug.h"

#include <KoShape.h>
#include <KoDrag.h>
#include <KoShapeOdfSaveHelper.h>
#include <KoOdf.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoOdfLoadingContext.h>
#include <KoStore.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoShapeRegistry.h>
#include <SvgParser.h>
#include <KoProperties.h>
#include <KoShapeGroup.h>
#include <KoShapeGroupCommand.h>

#include <QMimeData>
#include <QIODevice>
#include <KArchive/kcompressiondevice.h>

StencilShapeFactory::
StencilShapeFactory(const QString& id,
                    const QString& name,
                    const KoProperties* props)
    : KoShapeFactoryBase(id, name)
    , m_properties(props)
{
    // ensures ShapeCollectionDocker ignores the stencil shapes for now,
    // exclusively available by StencilBox
    setFamily("stencil");
}

StencilShapeFactory::
~StencilShapeFactory()
{
    delete m_properties;
}

KoShape* StencilShapeFactory::
createFromOdf(KoStore* store, KoDocumentResourceManager* documentRes) const
{
    KoOdfReadStore odfStore(store);
    QString errorMessage;
    if (! odfStore.loadAndParse(errorMessage)) {
        errorStencilBox << "loading and parsing failed:" << errorMessage << endl;
        return 0;
    }

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));
    if (realBody.isNull()) {
        errorStencilBox << "No body tag found!" << endl;
        return 0;
    }

    KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "drawing");
    if (body.isNull()) {
        errorStencilBox << "No office:drawing tag found!" << endl;
        return 0;
    }

    KoXmlElement page = KoXml::namedItemNS(body, KoXmlNS::draw, "page");
    if (page.isNull()) {
        errorStencilBox << "No page found!" << endl;
        return 0;
    }

    KoXmlElement shapeElement = KoXml::namedItemNS(page, KoXmlNS::draw, "g");
    if (shapeElement.isNull()) {
        shapeElement = KoXml::namedItemNS(page, KoXmlNS::draw, "custom-shape");
        if (shapeElement.isNull()) {
            errorStencilBox << "draw:g or draw:custom-shape element not found!" << endl;
            return 0;
        }
    }

    KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
    KoShapeLoadingContext context(loadingContext, documentRes);

    KoShapeRegistry* registry = KoShapeRegistry::instance();
    foreach (const QString & id, registry->keys()) {
        KoShapeFactoryBase* shapeFactory = registry->value(id);
        shapeFactory->newDocumentResourceManager(documentRes);
    }

    return KoShapeRegistry::instance()->createShapeFromOdf(shapeElement, context);
}

KoShape* StencilShapeFactory::
createFromSvg(QIODevice* in, KoDocumentResourceManager* documentRes) const
{
    if (!in->open(QIODevice::ReadOnly)) {
        debugStencilBox << "svg file open error";
        return 0;
    }

    int line, col;
    QString errormessage;
    KoXmlDocument inputDoc;
    const bool parsed = inputDoc.setContent(in, &errormessage, &line, &col);
    in->close();

    if (!parsed) {
        debugStencilBox << "Error while parsing file: "
        << "at line " << line << " column: " << col
        << " message: " << errormessage << endl;
        return 0;
    }

    SvgParser parser(documentRes);
    parser.setXmlBaseDir(id());
    QList<KoShape*> shapes = parser.parseSvg(inputDoc.documentElement());
    if (shapes.isEmpty())
        return 0;
    if (shapes.count() == 1)
        return shapes.first();

    KoShapeGroup *svgGroup = new KoShapeGroup;
    KoShapeGroupCommand cmd(svgGroup, shapes);
    cmd.redo();

    return svgGroup;
}

KoShape* StencilShapeFactory::
createDefaultShape(KoDocumentResourceManager* documentResources) const
{
    KoShape* shape = 0;
    KoStore* store = 0;
    QIODevice* in = 0;
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
bool StencilShapeFactory::
supports(const KoXmlElement& e, KoShapeLoadingContext& context) const
{
    Q_UNUSED(e);
    Q_UNUSED(context);
    return false;
}
