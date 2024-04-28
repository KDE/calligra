/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ArtisticTextShapeFactory.h"
#include "ArtisticTextShape.h"

#include <KoColorBackground.h>
#include <KoXmlNS.h>

#include <KLocalizedString>
#include <KoIcon.h>

ArtisticTextShapeFactory::ArtisticTextShapeFactory()
    : KoShapeFactoryBase(ArtisticTextShapeID, i18n("ArtisticTextShape"))
{
    setToolTip(i18n("A shape which shows a single text line"));
    setIconName(koIconNameNeeded("currently falls back to x-shape-text", "x-shape-text-artistic"));
    setLoadingPriority(5);
    setXmlElementNames(KoXmlNS::svg, QStringList("text"));
}

KoShape *ArtisticTextShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    ArtisticTextShape *text = new ArtisticTextShape();
    text->setBackground(QSharedPointer<KoShapeBackground>(new KoColorBackground(QColor(Qt::black))));
    text->setPlainText(i18n("Artistic Text"));
    return text;
}

bool ArtisticTextShapeFactory::supports(const KoXmlElement & /*element*/, KoShapeLoadingContext & /*context*/) const
{
    // the artistic text shape is embedded as svg into an odf file
    // so we tell the caller we do not support any element
    return false;
}
