/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CollectionShapeFactory.h"

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

#include <QDebug>
#include <QMimeData>
#include <QBuffer>

CollectionShapeFactory::CollectionShapeFactory(const QString &id, KoShape* shape)
    : KoShapeFactoryBase(id, shape->name()), m_shape(shape)
{
}

CollectionShapeFactory::~CollectionShapeFactory()
{
    delete m_shape;
}

KoShape *CollectionShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    QList<KoShape*> shapes;

    shapes << m_shape;

    KoDrag drag;
    KoShapeOdfSaveHelper saveHelper(shapes);
    drag.setOdf(KoOdf::mimeType(KoOdf::Graphics), saveHelper);
    QMimeData* data = drag.mimeData();

    QByteArray arr = data->data(KoOdf::mimeType(KoOdf::Graphics));
    KoShape* shape = 0;

    if ( !arr.isEmpty() ) {
        QBuffer buffer( &arr );
        KoStore * store = KoStore::createStore( &buffer, KoStore::Read );
        KoOdfReadStore odfStore( store ); // Note: KoDfReadstore will not delete the KoStore *store;

        QString errorMessage;
        if ( ! odfStore.loadAndParse( errorMessage ) ) {
            qCritical() << "loading and parsing failed:" << errorMessage << endl;
            delete store;
            return 0;
        }

        KoXmlElement content = odfStore.contentDoc().documentElement();
        KoXmlElement realBody( KoXml::namedItemNS( content, KoXmlNS::office, "body" ) );

        if ( realBody.isNull() ) {
            qCritical() << "No body tag found!" << endl;
            delete store;
            return 0;
        }

        KoXmlElement body = KoXml::namedItemNS( realBody, KoXmlNS::office, KoOdf::bodyContentElement( KoOdf::Text, false ) );

        if ( body.isNull() ) {
            qCritical() << "No" << KoOdf::bodyContentElement(KoOdf::Text, true ) << "tag found!" << endl;
            delete store;
            return 0;
        }

        KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
        KoShapeLoadingContext context(loadingContext, documentResources);

        KoXmlElement element;

        forEachElement(element, body)
        {
            KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf( element, context );
            if ( shape ) {
                delete data;
                delete store;
                return shape;
            }
        }
        delete store;
    }

    delete data;
    return shape;
}

bool CollectionShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(e);
    Q_UNUSED(context);
    return false;
}
