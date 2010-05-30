/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
 * Copyright (C) 2008-2009 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrNotes.h"

#include <KDebug>

#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoShape.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoPASavingContext.h>

#include "KPrDocument.h"
#include "KPrPage.h"

#include <QPainter>
// a helper class to load attributes of the thumbnail shape
class ShapeLoaderHelper : public KoShape
{
public:
    ShapeLoaderHelper() { }

    virtual void paint( QPainter &, const KoViewConverter & ) { }

    virtual void paintDecorations( QPainter &, const KoViewConverter &, const KoCanvasBase * ) { }

    virtual bool loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context )
    {
        return loadOdfAttributes( element, context, OdfAllAttributes );
    }

    virtual void saveOdf( KoShapeSavingContext & ) const { }
};

KPrNotes::KPrNotes( KPrPage *page, KPrDocument * document )
: KoPAPageBase()
, m_page( page )
, m_doc( document )
, m_imageCollection( new KoImageCollection() )
{
    // add default layer
    KoShapeLayer* layer = new KoShapeLayer;
    addShape( layer );

    // All sizes and positions are hardcoded for now
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("TextShapeID");
    Q_ASSERT(factory);
    m_textShape = factory->createDefaultShape(m_doc->resourceManager());
    m_textShape->setGeometryProtected(true);
    m_textShape->setAdditionalAttribute( "presentation:class", "notes" );
    m_textShape->setPosition(QPointF(62.22, 374.46));
    m_textShape->setSize(QSizeF(489.57, 356.37));

    factory = KoShapeRegistry::instance()->value("PictureShape");
    Q_ASSERT(factory);
    m_thumbnailShape = factory->createDefaultShape(m_doc->resourceManager());
    m_thumbnailShape->setGeometryProtected(true);
    m_thumbnailShape->setAdditionalAttribute( "presentation:class", "page" );
    m_thumbnailShape->setPosition(QPointF(108.00, 60.18));
    m_thumbnailShape->setSize(QSizeF(396.28, 296.96));

    layer->addShape( m_textShape );
    layer->addShape( m_thumbnailShape );
}

KPrNotes::~KPrNotes()
{
    delete m_imageCollection;
}

KoShape *KPrNotes::textShape()
{
    return m_textShape;
}

void KPrNotes::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter & writer = context.xmlWriter();
    writer.startElement("presentation:notes");

    context.addOption( KoShapeSavingContext::PresentationShape );
    m_textShape->saveOdf(context);
    context.removeOption( KoShapeSavingContext::PresentationShape );
    writer.startElement("draw:page-thumbnail");
    m_thumbnailShape->saveOdfAttributes( context, OdfAllAttributes );
    writer.addAttribute("draw:page-number", static_cast<KoPASavingContext &>(context).page());
    writer.endElement(); // draw:page-thumbnail

    KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>( shapes().last() );
    foreach ( KoShape *shape, layer->shapes() ) {
        if ( shape != m_textShape && shape != m_thumbnailShape ) {
            shape->saveOdf( context );
        }
    }

    writer.endElement(); // presentation:notes
}

bool KPrNotes::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KoXmlElement child;
    KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>( shapes().last() );

    forEachElement( child, element ) {
        if ( child.namespaceURI() != KoXmlNS::draw )
            continue;

        if ( child.tagName() == "page-thumbnail") {
            ShapeLoaderHelper *helper = new ShapeLoaderHelper();
            helper->loadOdf( child, context );
            m_thumbnailShape->setSize( helper->size() );
            m_thumbnailShape->setTransformation( helper->transformation() );
            m_thumbnailShape->setPosition( helper->position() );
            m_thumbnailShape->setShapeId( helper->shapeId() );
            delete helper;
        }
        else /* if ( child.tagName() == "frame") */ {
            KoShape *shape = KoShapeRegistry::instance()->createShapeFromOdf( child, context );
            if ( shape ) {
                if ( shape->shapeId() == "TextShapeID" &&
                        child.hasAttributeNS( KoXmlNS::presentation, "class" ) ) {
                    layer->removeShape( m_textShape );
                    delete m_textShape;
                    m_textShape = shape;
                    m_textShape->setAdditionalAttribute( "presentation:class", "notes" );
                    layer->addShape( m_textShape );
                }
                else {
                    layer->addShape( shape );
                }
            }
        }
    }

    return true;
}

void KPrNotes::paintComponent(QPainter& painter, const KoViewConverter& converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

KoPageLayout & KPrNotes::pageLayout()
{
    return m_pageLayout;
}

const KoPageLayout & KPrNotes::pageLayout() const
{
    return m_pageLayout;
}

bool KPrNotes::displayMasterShapes()
{
    return false;
}

void KPrNotes::setDisplayMasterShapes( bool )
{
}

bool KPrNotes::displayShape(KoShape *) const
{
    return true;
}

bool KPrNotes::displayMasterBackground()
{
    return false;
}

void KPrNotes::setDisplayMasterBackground( bool )
{
}

QPixmap KPrNotes::generateThumbnail( const QSize& )
{
    Q_ASSERT( 0 );
    return QPixmap();
}

void KPrNotes::updatePageThumbnail()
{
    // set image at least to 150 dpi we might need more when printing
    KoImageData *imageData = m_imageCollection->createImageData(m_doc->pageThumbnail(m_page, (m_thumbnailShape->size() * 150 / 72.).toSize()).toImage());
    m_thumbnailShape->setUserData( imageData );
}

void KPrNotes::paintPage( QPainter & painter, KoZoomHandler & /*zoomHandler*/ )
{
    Q_UNUSED(painter);
    // TODO implement when printing page with notes
    Q_ASSERT( 0 );
}
