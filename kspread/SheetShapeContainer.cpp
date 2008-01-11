/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "SheetShapeContainer.h"

#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "Doc.h"
#include "Sheet.h"

using namespace KSpread;

class SheetShapeContainer::Private
{
public:
    Sheet* sheet;
};

SheetShapeContainer::SheetShapeContainer( Sheet* sheet )
    : d( new Private )
{
    d->sheet = sheet;
}

SheetShapeContainer::SheetShapeContainer(const SheetShapeContainer& other, Sheet* sheet)
    : d( new Private )
{
    d->sheet = sheet;
    KoShape* shape;
    QList<KoShape*> shapes = other.iterator();
    for (int i = 0; i < shapes.count(); ++i)
    {
        shape = KoShapeRegistry::instance()->value(shapes[i]->shapeId())->createDefaultShape( 0 );
        shape->copySettings(shapes[i]);
        addChild(shape);
    }
}

SheetShapeContainer::~SheetShapeContainer()
{
    delete d;
}

bool SheetShapeContainer::loadOdf( const KoXmlElement& elements, KoShapeLoadingContext& context )
{
    KoXmlElement element;
    forEachElement(element, elements)
    {
        if (element.namespaceURI() == KoXmlNS::draw)
        {
            KoShape* shape = KoShapeRegistry::instance()->createShapeFromOdf(element, context);
            if (shape)
            {
                shape->setParent(this);
                d->sheet->doc()->addShape(shape);
            }
        }
    }
    return true;
}

void SheetShapeContainer::saveOdf( KoShapeSavingContext& context ) const
{
    const QList<KoShape*> children = iterator();
    if (children.isEmpty())
        return;
    context.xmlWriter().startElement("table:shapes");
    foreach (KoShape* shape, children)
        shape->saveOdf(context);
    context.xmlWriter().endElement();
}
