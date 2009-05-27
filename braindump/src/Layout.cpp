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

#include "Layout.h"

#include <KoShape.h>

struct Layout::Private : public KoShape {
  Layout* self;
  QList<KoShape*> shapes;
  protected:
    virtual void notifyShapeChanged(KoShape * shape, ChangeType type);
  private:
    // Fake
    virtual void paint(QPainter &painter, const KoViewConverter &converter) { Q_UNUSED(painter); Q_UNUSED(converter); qFatal("Shouldn't be called"); }
    virtual bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context) { Q_UNUSED(element); Q_UNUSED(context); qFatal("Shouldn't be called"); return false; }
    virtual void saveOdf(KoShapeSavingContext & context) const { Q_UNUSED(context); qFatal("Shouldn't be called"); }

};

void Layout::Private::notifyShapeChanged(KoShape * shape, ChangeType type) {
  switch(type)
  {
    case PositionChanged:
    case RotationChanged:
    case ScaleChanged:
    case ShearChanged:
    case SizeChanged:
    case GenericMatrixChange:
      self->shapeGeometryChanged(shape);
      break;
    default:
      break;
  }
}


Layout::Layout() : d(new Private)
{
  d->self = this;
}

Layout::~Layout() {
  foreach(KoShape* shape, d->shapes) {
    shape->removeDependee(d);
  }
  delete d;
}

void Layout::addShape(KoShape* _shape) {
  Q_ASSERT(not d->shapes.contains(_shape));
  shapeAdded(_shape);
  _shape->addDependee(d);
  d->shapes.push_back(_shape);
}

void Layout::removeShape(KoShape* _shape) {
  _shape->removeDependee(d);
  shapeRemoved(_shape);
  d->shapes.removeAll(_shape);
}

#include "Layout.moc"
