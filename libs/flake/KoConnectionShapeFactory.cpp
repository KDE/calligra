/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoConnectionShapeFactory.h"

#include "KoConnectionShape.h"
#include "KoConnectionShapeConfigWidget.h"

#include <KLocalizedString>
#include <KoIcon.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeStroke.h>
#include <KoXmlNS.h>

KoConnectionShapeFactory::KoConnectionShapeFactory()
    : KoShapeFactoryBase(KOCONNECTIONSHAPEID, i18n("Tie"))
{
    setToolTip(i18n("A connection between two other shapes"));
    setIconName(koIconName("x-shape-connection"));
    setXmlElementNames(KoXmlNS::draw, QStringList("connector"));
    setLoadingPriority(1);
    setHidden(true); // Don't show this shape in collections. Only ConnectionTool should create
}

KoShape *KoConnectionShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    KoConnectionShape *shape = new KoConnectionShape();
    shape->setStroke(new KoShapeStroke());
    shape->setShapeId(KoPathShapeId);
    return shape;
}

bool KoConnectionShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return (e.localName() == "connector" && e.namespaceURI() == KoXmlNS::draw);
}

QList<KoShapeConfigWidgetBase *> KoConnectionShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> panels;
    panels.append(new KoConnectionShapeConfigWidget());
    return panels;
}
