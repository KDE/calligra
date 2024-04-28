/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "RectangleShapeFactory.h"
#include "KoShapeStroke.h"
#include "RectangleShape.h"
#include "RectangleShapeConfigWidget.h"
#include <KoGradientBackground.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

#include <KLocalizedString>
#include <KoIcon.h>

RectangleShapeFactory::RectangleShapeFactory()
    : KoShapeFactoryBase(RectangleShapeId, i18n("Rectangle"))
{
    setToolTip(i18n("A rectangle"));
    setIconName(koIconName("rectangle-shape"));
    setFamily("geometric");
    setLoadingPriority(1);

    QList<QPair<QString, QStringList>> elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::draw), QStringList("rect")));
    elementNamesList.append(qMakePair(QString(KoXmlNS::svg), QStringList("rect")));
    setXmlElements(elementNamesList);
}

KoShape *RectangleShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    RectangleShape *rect = new RectangleShape();

    rect->setStroke(new KoShapeStroke(1.0));
    rect->setShapeId(KoPathShapeId);

    QLinearGradient *gradient = new QLinearGradient(QPointF(0, 0), QPointF(1, 1));
    gradient->setCoordinateMode(QGradient::ObjectBoundingMode);

    gradient->setColorAt(0.0, Qt::white);
    gradient->setColorAt(1.0, Qt::green);
    rect->setBackground(QSharedPointer<KoGradientBackground>(new KoGradientBackground(gradient)));

    return rect;
}

bool RectangleShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext & /*context*/) const
{
    Q_UNUSED(e);
    return (e.localName() == "rect" && e.namespaceURI() == KoXmlNS::draw);
}

QList<KoShapeConfigWidgetBase *> RectangleShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> panels;
    panels.append(new RectangleShapeConfigWidget());
    return panels;
}
