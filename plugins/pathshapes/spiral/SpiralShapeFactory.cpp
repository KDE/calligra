/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpiralShapeFactory.h"
#include "SpiralShape.h"
#include "SpiralShapeConfigWidget.h"
#include <KoShapeLoadingContext.h>
#include <KoShapeStroke.h>

#include <KLocalizedString>
#include <KoIcon.h>

SpiralShapeFactory::SpiralShapeFactory()
    : KoShapeFactoryBase(SpiralShapeId, i18n("Spiral"))
{
    setToolTip(i18n("A spiral shape"));
    setIconName(koIconName("spiral-shape"));
    setFamily("geometric");
    setLoadingPriority(1);
}

KoShape *SpiralShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    SpiralShape *spiral = new SpiralShape();

    spiral->setStroke(new KoShapeStroke(1.0));
    spiral->setShapeId(KoPathShapeId);

    return spiral;
}

bool SpiralShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(e);
    Q_UNUSED(context);
    return false;
}

QList<KoShapeConfigWidgetBase *> SpiralShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> panels;
    panels.append(new SpiralShapeConfigWidget());
    return panels;
}
