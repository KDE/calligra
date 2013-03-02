/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat <gopalakbhat@gmail.com>
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

// Own
#include "FormulaShapeFactory.h"

// Qt
#include <QList>

// KDE
#include <klocale.h>
#include <kdebug.h>

// Calligra
#include <KoXmlNS.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoDocumentResourceManager.h>
#include <KoProperties.h>
#include <KoIcon.h>

// This shape
#include "FormulaShape.h"

FormulaShapeFactory::FormulaShapeFactory()
    : KoShapeFactoryBase(FORMULASHAPEID, i18n("Formula editing"))
{
    setToolTip(i18n("Formula editing"));
    setIconName(koIconNameCStr("edittext"));
    setLoadingPriority(1);

    // Tell the shape loader which tag we can store
    // Template: You must change this.
    QList<QPair<QString, QStringList> > elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::calligra), QStringList("template")));
    setXmlElements(elementNamesList);
}

bool FormulaShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    // Template: Change this to your own supported element and namespace.
    if (e.localName() == "template" && e.namespaceURI() == KoXmlNS::calligra) {
        return true;
    }

    return false;
}

KoShape *FormulaShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    FormulaShape *formulaShape = new FormulaShape();
    formulaShape->setShapeId(FORMULASHAPEID);

    return formulaShape;
}

KoShape *FormulaShapeFactory::createShape(const KoProperties *params,
                                           KoDocumentResourceManager *documentResources) const
{
    FormulaShape *shape = static_cast<FormulaShape*>(createDefaultShape(documentResources));

    return shape;
}

QList<KoShapeConfigWidgetBase*> FormulaShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase*> result;

    return result;
}
