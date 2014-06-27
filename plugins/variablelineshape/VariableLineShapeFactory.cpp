/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "VariableLineShapeFactory.h"

#include "VariableLineShape.h"
#include "KoShapeBasedDocumentBase.h"
#include "KoShapeStroke.h"
#include <KoXmlNS.h>
#include <KoGradientBackground.h>
#include <KoShapeLoadingContext.h>

#include <KoIcon.h>
#include <klocale.h>
#include <kdebug.h>

VariableLineShapeFactory::VariableLineShapeFactory()
    : KoShapeFactoryBase(VariableLineShapeId, i18n("VariableLineShape"))
{
    setToolTip(i18n("Variable width line"));
    setIconName(koIconNameCStr("variableline-shape"));
    setXmlElementNames(KoXmlNS::draw, QStringList("lineshape"));
    setFamily("geometric");
    setLoadingPriority(1);
}

KoShape *VariableLineShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(documentResources);
    VariableLineShape *defaultLine = new VariableLineShape();
    
    defaultLine->setStroke(new KoShapeStroke(1.0));
    defaultLine->setShapeId(VariableLineShapeId);
    defaultLine->moveTo(QPointF(0, 0));
    defaultLine->lineTo(QPointF(50, 50));
    defaultLine->lineTo(QPointF(0, 100));
    defaultLine->lineTo(QPointF(50, 150));
    defaultLine->normalize();
    defaultLine->initDefaultPointWidth();

    return defaultLine;
}

bool VariableLineShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return e.localName() == "lineshape" && e.namespaceURI() == KoXmlNS::draw;
}
