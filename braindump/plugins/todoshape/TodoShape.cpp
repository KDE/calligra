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

#include "TodoShape.h"

#include <QPainter>
#include <QSvgRenderer>

#include <kdebug.h>

#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include "StatesRegistry.h"

TodoShape::TodoShape() : m_categoryId("todo"), m_stateId("unchecked")
{
  setSize(QSizeF(10, 10));
}

TodoShape::~TodoShape()
{
}

void TodoShape::paint( QPainter &painter,
                const KoViewConverter &converter )
{
  QRectF target = converter.documentToView(QRectF(QPointF(0,0), size()));
  painter.drawRect(target);
  const State* state = StatesRegistry::instance()->state(m_categoryId, m_stateId);
  if(state)
  {
    state->renderer()->render(&painter, target);
  } else {
    kError() << "No state found for m_categoryId = " << m_categoryId << " m_stateId = " << m_stateId;
  }
}

void TodoShape::saveOdf(KoShapeSavingContext & context) const
{
  KoXmlWriter &writer = context.xmlWriter();

  writer.startElement( "braindump:todo" );
  writer.addAttribute( "category", m_categoryId);
  writer.addAttribute( "state", m_stateId);
  saveOdfAttributes( context, OdfAllAttributes );
  saveOdfCommonChildElements( context );
  writer.endElement(); // braindump:todo
}

bool TodoShape::loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context)
{
  loadOdfAttributes( element, context, OdfAllAttributes );
  m_categoryId = element.attribute("category");
  m_stateId = element.attribute("state");
  return true;
}

const QString& TodoShape::categoryId() const {
  return m_categoryId;
}

void TodoShape::setCategoryId(const QString& _categoryId) {
  m_categoryId = _categoryId;
  notifyChanged();
  update();
}

const QString& TodoShape::stateId() const {
  return m_stateId;
}

void TodoShape::setStateId(const QString& _stateId) {
  m_stateId = _stateId;
  notifyChanged();
  update();
}
