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

#include "WebShapeFactory.h"

#include <QWebPage>
#include <QWebFrame>

#include <KLocalizedString>

#include <KoIcon.h>
#include <KoProperties.h>

#include "WebShape.h"

WebShapeFactory::WebShapeFactory()
    : KoShapeFactoryBase(WEBSHAPEID,
                         i18n("Web Shape"))
{
    setToolTip(i18n("A web shape"));
    setIconName(koIconName("applications-internet"));
    setXmlElementNames("http://kde.org/braindump", QStringList("web"));
}

KoShape *WebShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(documentResources);
    WebShape* fooShape = new WebShape();
    fooShape->setShapeId(WEBSHAPEID);
    // set defaults
    return fooShape;
}

KoShape *WebShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(documentResources);
    WebShape* fooShape = new WebShape();
    if(params->contains("url")) {
        fooShape->setUrl(params->property("url").toUrl());
    }
    fooShape->setShapeId(WEBSHAPEID);
    // use the params
    return fooShape;
}

bool WebShapeFactory::supports(const KoXmlElement& e, KoShapeLoadingContext& /*context*/) const
{
    return (e.localName() == "web" && e.namespaceURI() == "http://kde.org/braindump");
}

QList<KoShapeConfigWidgetBase*> WebShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase*> answer;
    return answer;
}
