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
    void saveOdf(KoShapeSavingContext *) {}
};

static KoShape *createShape(const KoImageData &image) {
    KoShapeFactory *factory = KoShapeRegistry::instance()->value("PictureShape");
    if(factory)
        return factory->createDefaultShape();
    return new DummyShape();
}

KWImageFrame::KWImageFrame(const KoImageData &image, KWFrameSet *parent)
    : KWFrame(createShape(image), parent),
    m_imageData(image),
    m_fullKritaShape(false)
{
    shape()->setUserData(new KoImageData(m_imageData));
}

KWImageFrame::~KWImageFrame() {
}

void KWImageFrame::setImageQuality(KWImageFrame::ImageQuality quality) {
}

KWImageFrame::ImageQuality KWImageFrame::imageQuality() const {
}

