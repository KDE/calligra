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

#include "Section.h"

#include "KoShapeLayer.h"
#include "KoShapeRegistry.h"
#include "KoShapeSavingContext.h"
#include "KoXmlWriter.h"

Section::Section() : SectionGroup(0)
{
  KoShapeLayer* layer = new KoShapeLayer;
  addChild(layer);
  foreach (QString id, KoShapeRegistry::instance()->keys()) {
    KoShapeFactory *shapeFactory = KoShapeRegistry::instance()->value(id);
    shapeFactory->populateDataCenterMap(m_dataCenterMap);
  }
}

bool Section::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
  loadOdfAttributes(element, context, OdfMandatories | OdfAdditionalAttributes | OdfCommonChildElements);

  KoXmlElement child;
  forEachElement(child, element) {
    KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf(child, context);
    if (shape) {
      addChild(shape);
    }
  }
  return true;
}
void Section::saveOdf(KoShapeSavingContext & context) const
{
  context.xmlWriter().startElement("braindump:section");
  saveOdfAttributes(context, (OdfMandatories ^ OdfLayer) | OdfAdditionalAttributes);

  QList<KoShape*> shapes = iterator();
  qSort(shapes.begin(), shapes.end(), KoShape::compareShapeZIndex);

  foreach(KoShape* shape, shapes) {
      shape->saveOdf(context);
  }

  saveOdfCommonChildElements(context);
  context.xmlWriter().endElement();
}
void Section::paintComponent(QPainter &painter, const KoViewConverter &converter)
{
  Q_UNUSED(painter);
  Q_UNUSED(converter);
}

QMap<QString, KoDataCenter* > Section::dataCenterMap() const
{
  return m_dataCenterMap;
}
