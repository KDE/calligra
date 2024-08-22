/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgShapeFactory.h"
#include "KoShapeGroup.h"
#include "KoShapeGroupCommand.h"
#include "KoShapeLoadingContext.h"
#include "KoShapeRegistry.h"
#include "SvgParser.h"
#include <KLocalizedString>
#include <KoOdfLoadingContext.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>

#define SVGSHAPEFACTORYID "SvgShapeFactory"

SvgShapeFactory::SvgShapeFactory()
    : KoShapeFactoryBase(SVGSHAPEFACTORYID, i18n("Embedded svg shape"))
{
    setLoadingPriority(4);
    setXmlElementNames(QString(KoXmlNS::draw), QStringList("image"));
    // hide from add shapes docker as the shape is not able to be dragged onto
    // the canvas as createDefaultShape returns 0.
    setHidden(true);
}

SvgShapeFactory::~SvgShapeFactory() = default;

void SvgShapeFactory::addToRegistry()
{
    KoShapeRegistry *registry = KoShapeRegistry::instance();
    if (!registry->contains(QString(SVGSHAPEFACTORYID))) {
        registry->addFactory(new SvgShapeFactory);
    }
}

bool SvgShapeFactory::supports(const KoXmlElement &element, KoShapeLoadingContext &context) const
{
    if (element.localName() == "image" && element.namespaceURI() == KoXmlNS::draw) {
        QString href = element.attribute("href");
        if (href.isEmpty())
            return false;

        // check the mimetype
        if (href.startsWith(QLatin1String("./"))) {
            href.remove(0, 2);
        }

        QString mimetype = context.odfLoadingContext().mimeTypeForPath(href, true);
        return (mimetype == "image/svg+xml");
    }

    return false;
}

KoShape *SvgShapeFactory::createShapeFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    const KoXmlElement &imageElement(KoXml::namedItemNS(element, KoXmlNS::draw, "image"));
    if (imageElement.isNull()) {
        errorFlake << "svg image element not found";
        return nullptr;
    }

    if (imageElement.tagName() == "image") {
        debugFlake << "trying to create shapes from svg image";
        QString href = imageElement.attribute("href");
        if (href.isEmpty())
            return nullptr;

        // check the mimetype
        if (href.startsWith(QLatin1String("./"))) {
            href.remove(0, 2);
        }
        QString mimetype = context.odfLoadingContext().mimeTypeForPath(href);
        debugFlake << mimetype;
        if (mimetype != "image/svg+xml")
            return nullptr;

        if (!context.odfLoadingContext().store()->open(href))
            return nullptr;

        KoStoreDevice dev(context.odfLoadingContext().store());
        KoXmlDocument xmlDoc;

        int line, col;
        QString errormessage;

        const bool parsed = xmlDoc.setContent(&dev, &errormessage, &line, &col);

        context.odfLoadingContext().store()->close();

        if (!parsed) {
            errorFlake << "Error while parsing file: "
                       << "at line " << line << " column: " << col << " message: " << errormessage << Qt::endl;
            return nullptr;
        }

        SvgParser parser(context.documentResourceManager());

        QList<KoShape *> shapes = parser.parseSvg(xmlDoc.documentElement());
        if (shapes.isEmpty())
            return nullptr;

        int zIndex = 0;
        if (element.hasAttributeNS(KoXmlNS::draw, "z-index")) {
            zIndex = element.attributeNS(KoXmlNS::draw, "z-index").toInt();
        } else {
            zIndex = context.zIndex();
        }

        if (shapes.count() == 1) {
            KoShape *shape = shapes.first();
            shape->setZIndex(zIndex);

            // TODO: This needs to be reviewed, in case more than one shape
            context.odfLoadingContext().styleStack().save();
            bool loaded = shape->loadOdf(element, context);
            context.odfLoadingContext().styleStack().restore();
            if (!loaded) {
                errorFlake << "Failed to load svg shape: " << shape->shapeId();
                delete shape;
                return nullptr;
            }
            return shape;
        }
        // FIXME: should this not be created form the draw:g element?
        KoShapeGroup *svgGroup = new KoShapeGroup;
        KoShapeGroupCommand cmd(svgGroup, shapes);
        cmd.redo();
        svgGroup->setZIndex(zIndex);

        return svgGroup;
    }

    return nullptr;
}
