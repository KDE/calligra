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

#include "FreeLayout.h"

#include <klocale.h>
#include <KoShapeContainer.h>

FreeLayout::FreeLayout() : Layout("freelayout")
{
}

FreeLayout::~FreeLayout() {
}

void FreeLayout::shapeAdded(KoShape* ) {
  updateSize();
}

void FreeLayout::shapeRemoved(KoShape* ) {
  updateSize();
}

void FreeLayout::shapeGeometryChanged(KoShape* ) {
  updateSize();
}

void containerBoundRec( QList<KoShape*> shapes, QRectF& b)
{
  foreach(KoShape* shape, shapes)
  {
    b |= shape->boundingRect();
    KoShapeContainer* cont = dynamic_cast<KoShapeContainer*>(shape);
    if(cont)
    {
      containerBoundRec(cont->iterator(), b);
    }
  }
}

void FreeLayout::updateSize() {
  QRectF b;
  containerBoundRec(shapes(), b);
}

FreeLayoutFactory::FreeLayoutFactory() : LayoutFactory("freelayout", i18n("Free")) {
}

FreeLayoutFactory::~FreeLayoutFactory() {
}

Layout* FreeLayoutFactory::createLayout() const {
  return new FreeLayout;
}
