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

#include "StateShape.h"

#include <QPainter>
#include <QSvgRenderer>

#include <kdebug.h>

#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include "StatesRegistry.h"

StateShape::StateShape() : m_categoryId("todo"), m_stateId("unchecked"), m_shape(0)
{
  setSize(QSizeF(10, 10));
}

StateShape::~StateShape()
{
}

void StateShape::paint( QPainter &painter,
                const KoViewConverter &converter )
{
  QRectF target = converter.documentToView(QRectF(QPointF(0,0), size()));
  const State* state = StatesRegistry::instance()->state(m_categoryId, m_stateId);
  if(state)
  {
    state->renderer()->render(&painter, target);
  } else {
    kError() << "No state found for m_categoryId = " << m_categoryId << " m_stateId = " << m_stateId;
  }
}

void StateShape::saveOdf(KoShapeSavingContext & context) const
{
  KoXmlWriter &writer = context.xmlWriter();

  writer.startElement( "braindump:state" );
  writer.addAttribute( "category", m_categoryId);
  writer.addAttribute( "state", m_stateId);
  if(m_shape)
  {
    writer.addAttribute( "attached", context.drawId(m_shape));
  }
  saveOdfAttributes( context, OdfAllAttributes );
  saveOdfCommonChildElements( context );
  writer.endElement(); // braindump:shape
}

bool StateShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
  m_categoryId = element.attribute("category");
  m_stateId = element.attribute("state");
  if(element.hasAttribute("attached"))
  {
    QString shapeAttached = element.attribute("attached");
    KoShape *shape = context.shapeById( shapeAttached );
    Q_ASSERT(shape);
    attachTo(shape);
  }
  loadOdfAttributes( element, context, OdfAllAttributes );
  return true;
}

const QString& StateShape::categoryId() const {
  return m_categoryId;
}

void StateShape::setCategoryId(const QString& _categoryId) {
  m_categoryId = _categoryId;
  notifyChanged();
  update();
}

const QString& StateShape::stateId() const {
  return m_stateId;
}

void StateShape::setStateId(const QString& _stateId) {
  m_stateId = _stateId;
  notifyChanged();
  update();
}

void StateShape::attachTo(KoShape* _shape) {
  if(_shape == m_shape) return;
  update();
  if(m_shape) {
    m_shape->removeDependee(this);
  }
  m_shape = _shape;
  if(m_shape)
  {
    m_shape->addDependee(this);
    QRectF r = m_shape->boundingRect();
    QPointF pt(r.left() - 0.5 * size().width(), 0.5 * (r.top() + r.bottom()));
    setAbsolutePosition(pt, KoFlake::CenteredPosition);
    m_lastOrigin = r.topLeft();
  }
  update();
}

KoShape* StateShape::attachedShape() {
  return m_shape;
}

void StateShape::notifyShapeChanged( KoShape * shape, ChangeType type ) {
  if( shape == m_shape )
  {
    if( type == KoShape::Deleted )
    {
      m_shape = 0;
    }
    else
    {
      update();
      QPointF offset = m_lastOrigin - m_shape->boundingRect().topLeft();
      QMatrix m;
      m.translate( -offset.x(), -offset.y() );
      applyAbsoluteTransformation( m );
      m_lastOrigin = m_shape->boundingRect().topLeft();
      update();
    }
  }
}
