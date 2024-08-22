/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFormulaShapeFactory.h"

#include <KLocalizedString>

#include <KoIcon.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

#include "FormulaDebug.h"
#include "KoFormulaShape.h"

KoFormulaShapeFactory::KoFormulaShapeFactory()
    : KoShapeFactoryBase(KoFormulaShapeId, i18n("Formula"))
{
    setToolTip(i18n("A formula"));
    setIconName(koIconName("x-shape-formula"));

    // The following lines let the formula shape load both embedded and
    // inline formulas.
    //
    // The line below tells the shape registry which XML elements that
    // the shape supports.
    //
    // FIXME: Find out if inline formulas are supported by ODF.

    QList<QPair<QString, QStringList>> elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::draw), QStringList("object")));
    elementNamesList.append(qMakePair(QString(KoXmlNS::math), QStringList("math")));
    setXmlElements(elementNamesList);

    setLoadingPriority(1);
    /*    KoShapeTemplate t;
        t.id = KoFormulaShapeId;
        t.name = i18n("Formula");
        t.toolTip = i18n("A formula");
        t.icon = ""; //TODO add it
        props = new KoProperties();
        t.properties = props;
        addTemplate( t );*/
}

KoFormulaShapeFactory::~KoFormulaShapeFactory() = default;

KoShape *KoFormulaShapeFactory::createDefaultShape(KoDocumentResourceManager *resourceManager) const
{
    KoFormulaShape *formula = new KoFormulaShape(resourceManager);
    formula->setShapeId(KoFormulaShapeId);
    return formula;
}

bool KoFormulaShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    if ((e.localName() == "math" && e.namespaceURI() == KoXmlNS::math)) {
        return true;
    }

    if (e.localName() == "object" && e.namespaceURI() == KoXmlNS::draw) {
        QString href = e.attribute("href");
        if (!href.isEmpty()) {
            // check the mimetype
            if (href.startsWith(QLatin1String("./"))) {
                href.remove(0, 2);
            }

            const QString mimetype = context.odfLoadingContext().mimeTypeForPath(href);
            return mimetype.isEmpty() || mimetype == "application/vnd.oasis.opendocument.formula";
        }
    }

    return false;
}
