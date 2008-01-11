/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "KWImageFrame.h"

#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>

#include <KDebug>

class DummyShape : public KoShape {
public:
    // TODO paint a 'placeholder' kind of picture to indicate that a plugin is missing.
    void paint(QPainter &, const KoViewConverter &) {}
    void saveOdf(KoShapeSavingContext &) const { }
    bool loadOdf( const KoXmlElement &, KoShapeLoadingContext &) { return false; }
    virtual KoShape * cloneShape() const { return 0; }
};

static KoShape *createShape(const KoImageData &image) {
    KoShapeFactory *factory = KoShapeRegistry::instance()->value("PictureShape");
    if(factory) {
        KoShape *shape = factory->createDefaultShape( 0 );
        shape->setUserData(new KoImageData(image));
        return shape;
    }
    return new DummyShape();
}

KWImageFrame::KWImageFrame(const KoImageData &image, KWFrameSet *parent)
    : KWFrame(createShape(image), parent),
    m_imageData(image),
    m_fullKritaShape(false)
{
    m_quality = static_cast<KWImageFrame::ImageQuality> (m_imageData.imageQuality());
}

KWImageFrame::~KWImageFrame() {
}

void KWImageFrame::setImageQuality(KWImageFrame::ImageQuality quality) {
    if(m_quality == quality)
        return;
    if(quality == EditableQuality) {
        // create and initialize a krita shape.
        KoShapeFactory *factory = KoShapeRegistry::instance()->value("KritaShape");
        if(factory) {
            KoShape *shape = factory->createDefaultShape( 0 );
            shape->setUserData(new KoImageData(m_imageData));
            setShape(shape);
            m_imageData.setImageQuality(KoImageData::NoPreviewImage); // flush pixmap from imageData object.
        }
        else {
            kWarning() << "Krita not installed; keeping preview resolution\n";
            return;
        }
    }
    else {
        if(m_quality == EditableQuality)
            setShape(createShape(m_imageData));
        m_imageData.setImageQuality( static_cast<KoImageData::ImageQuality>(quality));
    }
    m_quality = quality;
}

KWImageFrame::ImageQuality KWImageFrame::imageQuality() const {
    if(m_quality == EditableQuality)
        return EditableQuality;
    return static_cast<KWImageFrame::ImageQuality> (m_imageData.imageQuality());
}

