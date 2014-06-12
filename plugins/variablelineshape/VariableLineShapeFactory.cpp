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

#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>
#include "KoShapeBasedDocumentBase.h"

#include <KoIcon.h>
#include <klocale.h>
#include <kdebug.h>

VariableLineShapeFactory::VariableLineShapeFactory()
    : KoShapeFactoryBase(VariableLineShapeId, i18n("Variable Line"))
{
    setToolTip(i18n("Variable width line"));
    setIconName(koIconNameCStr("variable-width-line"));
    setXmlElementNames(KoXmlNS::draw, QStringList("lineshape"));
    setFamily("geometric");
    setLoadingPriority(1);
}

KoShape *VariableLineShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    VariableLineShape *defaultLine = new VariableLineShape();
    defaultLine->setShapeId(VariableLineShapeId);
    defaultLine->setStroke(new KoShapeStroke(1.0));

    QLinearGradient *gradient = new QLinearGradient(QPointF(0,0), QPointF(1,1));
    gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient->setColorAt(0.0, Qt::white);
    gradient->setColorAt(1.0, Qt::black);
    
    defaultLine->setBackground(QSharedPointer<KoGradientBackground>(new KoGradientBackground(gradient)));

    return defaultLine;
}

bool VariableLineShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return e.localName() == "lineshape" && e.namespaceURI() == KoXmlNS::draw;
}
