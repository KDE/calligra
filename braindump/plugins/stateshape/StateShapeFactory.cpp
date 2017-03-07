/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "StateShapeFactory.h"

#include <KLocalizedString>

#include <KoProperties.h>
#include <KoIcon.h>

#include "StateShape.h"

StateShapeFactory::StateShapeFactory()
    : KoShapeFactoryBase(STATESHAPEID,
                         i18n("State Shape"))
{
    setToolTip(i18n("A state shape"));
    setIconName(koIconName("stateshape"));
    setXmlElementNames("http://kde.org/braindump", QStringList("state"));
}

KoShape *StateShapeFactory::createDefaultShape(KoDocumentResourceManager */*documentResources*/) const
{
    StateShape* fooShape = new StateShape();
    fooShape->setShapeId(STATESHAPEID);
    // set defaults
    return fooShape;
}

KoShape *StateShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager */*documentResources*/) const
{
    Q_UNUSED(params);
    StateShape* fooShape = new StateShape();
    fooShape->setShapeId(STATESHAPEID);
    if(params->contains("state")) {
        fooShape->setStateId(params->stringProperty("state"));
    }
    if(params->contains("category")) {
        fooShape->setCategoryId(params->stringProperty("category"));
    }
    // use the params
    return fooShape;
}

bool StateShapeFactory::supports(const KoXmlElement& e, KoShapeLoadingContext& /*context*/) const
{
    return (e.localName() == "state" && e.namespaceURI() == "http://kde.org/braindump");
}

QList<KoShapeConfigWidgetBase*> StateShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase*> answer;
    return answer;
}
