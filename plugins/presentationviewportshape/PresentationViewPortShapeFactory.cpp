/* This file is part of the KDE project
 * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
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

#include "PresentationViewPortShapeFactory.h"
#include "PresentationViewPortShape.h"

#include <KoShapeFactoryBase.h>
//#include "KoLineBorder.h"
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoGradientBackground.h>
#include <KoShapeLoadingContext.h>
#include <KoDocumentResourceManager.h>

#include <KoIcon.h>
#include <klocale.h>

PresentationViewPortShapeFactory::PresentationViewPortShapeFactory()
    : KoShapeFactoryBase("PresentationViewPortShape", i18n("PresentationViewPort"))
{	
    setToolTip(i18n("A Presentation View Port"));
    setIconName(koIconNameCStr("rectangle-shape"));//TODO New icon needed shaped like [ ]
    setLoadingPriority(2);
    
    QList<QPair<QString, QStringList> > elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::svg), QStringList("rect")));
    elementNamesList.append(qMakePair(QString(KoXmlNS::svg), QStringList("sozi:frame")));
    setXmlElements(elementNamesList);

}

KoShape *PresentationViewPortShapeFactory::createDefaultShape( KoDocumentResourceManager* documentResources ) const
{
    PresentationViewPortShape *viewport = new PresentationViewPortShape();
    viewport->setShapeId(PresentationViewPortShapeId);
    
    return viewport;
}

bool PresentationViewPortShapeFactory::supports(const KoXmlElement & e, KoShapeLoadingContext &/*context*/) const
{
    Q_UNUSED(e);
    return (e.localName() == "rect" && e.namespaceURI() == KoXmlNS::svg) || (e.localName() == "sozi:frame" && e.namespaceURI() == KoXmlNS::svg);
}

