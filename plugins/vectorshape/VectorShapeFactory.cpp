/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "VectorShapeFactory.h"

// VectorShape
#include "VectorDebug.h"
#include "VectorShape.h"
#include "VectorShapeConfigWidget.h"

// Calligra
#include <KoIcon.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

// KF5
#include <KLocalizedString>

VectorShapeFactory::VectorShapeFactory()
    : KoShapeFactoryBase(VectorShape_SHAPEID, i18n("Vector image"))
{
    setToolTip(i18n("A shape that shows a vector image (EMF/WMF/SVM)"));
    setIconName(koIconNameNeededWithSubs("a generic vector image icon", "x-shape-vectorimage", "application-x-wmf"));
    setXmlElementNames(KoXmlNS::draw, QStringList("image"));
    setLoadingPriority(5);
}

KoShape *VectorShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(documentResources)
    VectorShape *shape = new VectorShape();
    shape->setShapeId(VectorShape_SHAPEID);

    return shape;
}

bool VectorShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    if (e.localName() == "image" && e.namespaceURI() == KoXmlNS::draw) {
        QString href = e.attribute("href");
        if (!href.isEmpty()) {
            // check the mimetype
            if (href.startsWith(QLatin1String("./"))) {
                href.remove(0, 2);
            }
            // LO 3.5 does not write a mimetype for embedded wmf files, so guess also from content
            const QString mimetype = context.odfLoadingContext().mimeTypeForPath(href, true);

            return mimetype == QLatin1String("image/x-svm") || mimetype == QLatin1String("image/x-emf") || mimetype == QLatin1String("image/x-wmf") ||
                // Note: the Vector Shape supports SVG, but _NOT_ in this method, otherwise it will stomp all over loading the artistic text shape's svg
                // mimetype == QLatin1String("image/svg+xml") ||
                // next three for backward compatibility with Calligra
                mimetype == QLatin1String("application/x-svm") || mimetype == QLatin1String("application/x-emf")
                || mimetype == QLatin1String("application/x-wmf") ||
                // seems like MSO does not always write a mimetype
                // see jeffcoweb.jeffco.k12.co.us%2Fhigh%2Fchatfield%2Fdepartments%2Fbusiness%2Fbanking_finance%2Funit_Plan_Budget.odp
                mimetype.isEmpty() ||
                // next for compatibility with OO/LO and our filters
                // see drwho.virtadpt.net%2Ffiles%2FNOVALUG-Tor.odp
                mimetype.startsWith(QLatin1String("application/x-openoffice"));
        }
        return true;
    }

    return false;
}

QList<KoShapeConfigWidgetBase *> VectorShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> result;
    result.append(new VectorShapeConfigWidget());
    return result;
}
