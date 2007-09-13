/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "PictureShape.h"

#include <KoImageData.h>
#include <KoViewConverter.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoShapeLoadingContext.h>
#include <KoOasisLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoStoreDevice.h>
#include <KoUnit.h>

#include <QPainter>
#include <kdebug.h>

PictureShape::PictureShape()
    : m_imageData(0)
{
    setKeepAspectRatio(true);
}

PictureShape::~PictureShape() {
}

void PictureShape::paint( QPainter& painter, const KoViewConverter& converter ) {
    if(m_imageData != userData())
        m_imageData = dynamic_cast<KoImageData*> (userData());
    if(m_imageData == 0)
        return;

    QPixmap pm = m_imageData->pixmap();
    QRectF target = converter.documentToView(QRectF(QPointF(0,0), size()));
    painter.drawPixmap(target.toRect(), pm, QRect(0, 0, pm.width(), pm.height()));
}

void PictureShape::saveOdf( KoShapeSavingContext & context ) const
{
    // make sure we have a valid image data pointer before saving
    KoImageData * data = m_imageData;
    if( data != userData() )
        data = dynamic_cast<KoImageData*> (userData());
    if(data == 0)
        return;

    KoXmlWriter &writer = context.xmlWriter();
    writer.startElement("draw:image");
    saveOdfAttributes(context, OdfMandatories | OdfSize | OdfPosition | OdfTransformation);
    // In the spec, only the xlink:href attribute is marked as mandatory, cool :)
    QString name = context.addImageForSaving( data->pixmap() );
    writer.addAttribute("xlink:href", name);
    writer.endElement();
}

bool PictureShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context )
{
    Q_UNUSED(context);

    //QPointF pos = position();
    //double x = KoUnit::parseValue( element.attribute("x") ) + pos.x();
    double x = KoUnit::parseValue( element.attribute("x") );
    double y = KoUnit::parseValue( element.attribute("y") );
    double width = KoUnit::parseValue( element.attribute("width") );
    double height = KoUnit::parseValue( element.attribute("height") );
    int zindex = qMax(0, element.attribute("z-index").toInt() ) + 1;

    setPosition( QPointF(x,y) );
    setSize( QSizeF(width,height) );
    setZIndex(zindex);

    if( context.imageCollection() )
    {
        const QString href = element.attribute("href");

        KoImageData * data = new KoImageData( context.imageCollection() );
        data->setStoreHref( href );
        setUserData( data );
    }
/*
    QDomNamedNodeMap attrs = element.attributes();
    for (int iAttr = 0 ; iAttr < attrs.count() ; iAttr++)
        kDebug(32500) <<"PictureShape::loadOdf Attribute" << iAttr <<" :" << attrs.item(iAttr).nodeName() <<"\t" << attrs.item(iAttr).nodeValue();
    //kDebug(32500) <<"PictureShape::loadOdf xlink:href=" << element.attribute("href");
    KoStore* store = context.koLoadingContext().store();
    Q_ASSERT(store);
    //TODO use e.g. KWDocument::imageCollection() here rather then creating a new one
    KoImageCollection* imagecollection = new KoImageCollection();
    //imagecollection->loadFromStore(store)
    KoImageData* imagedata = new KoImageData(imagecollection);
    bool ok = imagedata->loadFromStore(new KoStoreDevice(store));
    setUserData( imagedata );
    return ok;
*/

    kDebug()<<"PictureShape::loadOdf x="<<x<<" y="<<y<<" width="<<width<<" height="<<height<<" zindex="<<zindex;
    return true;
}
