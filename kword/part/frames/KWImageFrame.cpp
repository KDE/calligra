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

static KoShape *createShape() {
    KoShapeFactory *factory = KoShapeRegistry::instance()->value("PictureShape");
    if(factory) {
        KoShape *shape = factory->createDefaultShape( 0 );
        return shape;
    }
    return new DummyShape();
}

KWImageFrame::KWImageFrame(KWFrameSet *parent, KoShape *imageshape)
    : KWFrame(imageshape ? imageshape : createShape(), parent)
{
}

KWImageFrame::~KWImageFrame() {
}
