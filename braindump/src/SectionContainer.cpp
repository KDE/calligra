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

#include "SectionContainer.h"

#include "KoShapeLayer.h"
#include "KoShapeRegistry.h"
#include "KoShapeSavingContext.h"
#include "KoXmlWriter.h"

SectionContainer::SectionContainer(Section* section) : m_section(section), m_layer(new KoShapeLayer)
{
  addChild(m_layer);
  foreach (QString id, KoShapeRegistry::instance()->keys()) {
    KoShapeFactory *shapeFactory = KoShapeRegistry::instance()->value(id);
    shapeFactory->populateDataCenterMap(m_dataCenterMap);
  }
}

Section* SectionContainer::section()
{
  return m_section;
}

bool SectionContainer::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
  loadOdfAttributes(element, context, OdfMandatories | OdfAdditionalAttributes | OdfCommonChildElements);

  KoXmlElement child;
  forEachElement(child, element) {
    KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf(child, context);
    if (shape) {
      m_layer->addChild(shape);
    }
  }
  return true;
}
void SectionContainer::saveOdf(KoShapeSavingContext & context) const
{
  context.xmlWriter().startElement("braindump:section");
  saveOdfAttributes(context, (OdfMandatories ^ OdfLayer) | OdfAdditionalAttributes);

  QList<KoShape*> shapes = m_layer->iterator();
  qSort(shapes.begin(), shapes.end(), KoShape::compareShapeZIndex);

  foreach(KoShape* shape, shapes) {
      shape->saveOdf(context);
  }

  saveOdfCommonChildElements(context);
  context.xmlWriter().endElement();
}
void SectionContainer::paintComponent(QPainter &painter, const KoViewConverter &converter)
{
  Q_UNUSED(painter);
  Q_UNUSED(converter);
}

QMap<QString, KoDataCenter* > SectionContainer::dataCenterMap() const
{
  return m_dataCenterMap;
}

void containerBoundRec( const KoShapeContainer* parent, QRectF& b)
{
  foreach(KoShape* shape, parent->iterator())
  {
    b |= shape->boundingRect();
    KoShapeContainer* cont = dynamic_cast<KoShapeContainer*>(shape);
    if(cont)
    {
      containerBoundRec(cont, b);
    }
  }
}

QRectF SectionContainer::containerBound() const
{
  QRectF b;
  containerBoundRec(this, b);
  return b;
}
