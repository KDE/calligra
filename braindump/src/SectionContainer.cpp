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

#include <QMimeData>


#include <KoDrag.h>
#include <KoOdf.h>
#include <KoOdfPaste.h>
#include <KoShapeOdfSaveHelper.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoShapeLoadingContext.h>
#include <KoUndoStack.h>

#include "SectionShapeContainerModel.h"
#include "Utils.h"
#include "Layout.h"

SectionContainer::SectionContainer(Section* section, KoUndoStack* _stack) : m_section(0), m_layer(0)
{
  initContainer(section, _stack);
}

SectionContainer::SectionContainer(const SectionContainer& _rhs)
{
  Q_UNUSED(_rhs);
  qFatal("Can't copy");
}

class SectionContainerShapePaste : public KoOdfPaste
{
  public:
    SectionContainerShapePaste(SectionContainer* _container, KoShapeLayer* _layer, Layout* _layout) : m_container(_container), m_layer(_layer), m_layout(_layout) {}
    virtual ~SectionContainerShapePaste() {}
    virtual bool process(const KoXmlElement & body, KoOdfReadStore & odfStore)
    {
      KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
      KoShapeLoadingContext context(loadingContext, m_container->dataCenterMap());
      QList<KoShape*> shapes;
      m_container->loadOdf(body, context, shapes);
      m_layout->addShapes(shapes);
      return true;
    }
  private:
    SectionContainer* m_container;
    KoShapeLayer* m_layer;
    Layout* m_layout;
};

SectionContainer::SectionContainer(const SectionContainer& _rhs, Section* _section ) : m_section(0), m_layer(0) {
  initContainer(_section, dynamic_cast<KoUndoStack*>(_rhs.m_dataCenterMap["UndoStack"]));
  KoShapeOdfSaveHelper saveHelper(_rhs.m_layer->childShapes());
  KoDrag drag;
  drag.setOdf(KoOdf::mimeType(KoOdf::Text), saveHelper);
  QMimeData* mimeData = drag.mimeData();
  
  Q_ASSERT(mimeData->hasFormat(KoOdf::mimeType(KoOdf::Text)));
  
  SectionContainerShapePaste paste(this, m_layer, _section->layout());
  bool success = paste.paste(KoOdf::Text, mimeData);
  Q_ASSERT(success);
  
  delete mimeData;
}

void SectionContainer::initContainer(Section* _section, KoUndoStack* _stack) {
  m_section = _section;
  m_sectionModel = new SectionShapeContainerModel(m_section);
  m_layer = new KoShapeLayer(m_sectionModel);
  m_dataCenterMap["UndoStack"] = _stack;
  foreach (QString id, KoShapeRegistry::instance()->keys()) {
    KoShapeFactory *shapeFactory = KoShapeRegistry::instance()->value(id);
    shapeFactory->populateDataCenterMap(m_dataCenterMap);
  }
}

Section* SectionContainer::section()
{
  return m_section;
}

KoShapeLayer* SectionContainer::layer() {
  return m_layer;
}

bool SectionContainer::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context, QList<KoShape*>& shapes)
{
  m_sectionModel->setUpdateLayout(false);
  KoXmlElement child;
  forEachElement(child, element) {
    KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf(child, context);
    if (shape) {
      m_layer->addChild(shape);
      shapes.push_back(shape);
    }
  }
  m_sectionModel->setUpdateLayout(true);
  return true;
}
void SectionContainer::saveOdf(KoShapeSavingContext & context) const
{
  context.xmlWriter().startElement("braindump:section");

  QList<KoShape*> shapes = m_layer->childShapes();
  qSort(shapes.begin(), shapes.end(), KoShape::compareShapeZIndex);

  foreach(KoShape* shape, shapes) {
      shape->saveOdf(context);
  }

  context.xmlWriter().endElement();
}

QMap<QString, KoDataCenter* > SectionContainer::dataCenterMap() const
{
  return m_dataCenterMap;
}

QRectF SectionContainer::containerBound() const
{
  QRectF b;
  Utils::containerBoundRec(m_layer->childShapes(), b);
  return b;
}
