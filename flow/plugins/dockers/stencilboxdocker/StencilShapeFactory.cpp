/* This file is part of the KDE project
 * Copyright (C) 2008 Peter Simonsson <peter.simonsson@gmail.com>
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

#include "StencilShapeFactory.h"

#include <KoShape.h>
#include <KoDrag.h>
#include <KoShapeOdfSaveHelper.h>
#include <KoOdf.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoOdfLoadingContext.h>
#include <KoStore.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoShapeRegistry.h>

#include <kdebug.h>

#include <QMimeData>
#include <QBuffer>

StencilShapeFactory::StencilShapeFactory(const QString &id, const QString &name, const QString &source, KoProperties* params)
    : KoShapeFactoryBase(id, name), m_shape(0)
{
    m_params = params;
    m_path = QString(source);
}

StencilShapeFactory::~StencilShapeFactory()
{
    delete m_shape;
}

KoShape *StencilShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    /*KoDrag drag;
    KoShapeOdfSaveHelper saveHelper(shapes);
    drag.setOdf(KoOdf::mimeType(KoOdf::Graphics), saveHelper);
    QMimeData* data = drag.mimeData();

    QByteArray arr = data->data(KoOdf::mimeType(KoOdf::Graphics));*/
    KoShape* shape = 0;

    //if ( !arr.isEmpty() ) {
        //QBuffer buffer( &arr );
        KoStore * store = KoStore::createStore( m_path, KoStore::Read );
        if(store->bad())
        {
            //emit loadingFailed(i18n("Not a valid Calligra file: %1", m_path));
            delete store;
            return shape;
        }

        KoOdfReadStore odfStore( store ); // Note: KoDfReadstore will not delete the KoStore *store;
        QString errorMessage;
        if ( ! odfStore.loadAndParse( errorMessage ) ) {
            kError() << "loading and parsing failed:" << errorMessage << endl;
            delete store;
            return shape;
        }

        KoXmlElement content = odfStore.contentDoc().documentElement();
        KoXmlElement realBody( KoXml::namedItemNS( content, KoXmlNS::office, "body" ) );
        if ( realBody.isNull() ) {
            kError() << "No body tag found!" << endl;
            delete store;
            return shape;
        }

        KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "drawing");
        if (body.isNull()) {
            kError() << "No office:drawing tag found!" << endl;
            //emit loadingFailed(i18n("No office:drawing tag found in file: %1", m_path));
            delete store;
            return shape;
        }

        KoXmlElement page = KoXml::namedItemNS(body, KoXmlNS::draw, "page");
        if (page.isNull()) {
            kError() << "No page found!" << endl;
            //emit loadingFailed(i18n("No page found in file: %1", m_path));
            delete store;
            return shape;
        }

        KoXmlNode group = KoXml::namedItemNS(page, KoXmlNS::draw, "g");
        if (group.isNull()) {
            kError() << "No group found!" << endl;
            //emit loadingFailed(i18n("No group found in file: %1", m_path));
            delete store;
            return shape;
        }

        KoXmlElement n_shape;
        while (!group.isNull()) {
            n_shape = group.toElement();
            if (!n_shape.isNull()) {
                break;
            }
            group = group.nextSibling();
        }
        if (n_shape.isNull()) {
            kError() << "No shapes found!" << endl;
            //emit loadingFailed(i18n("No shapes found in file: %1", m_path));
            delete store;
            return shape;
        }

        KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
        KoShapeLoadingContext context(loadingContext, documentResources);

        KoShapeRegistry *registry = KoShapeRegistry::instance();
        foreach (const QString &id, registry->keys()) {
            KoShapeFactoryBase *shapeFactory = registry->value(id);
            shapeFactory->newDocumentResourceManager(documentResources);
        }

        shape = KoShapeRegistry::instance()->createShapeFromOdf( n_shape, context );
            if ( shape ) {
                delete store;
                if(m_params->intProperty("keepAspectRatio")==1)
                    shape->setKeepAspectRatio(true);
                return shape;
            }

    delete store;
    return shape;
}

bool StencilShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(e);
    Q_UNUSED(context);
    return false;
}
