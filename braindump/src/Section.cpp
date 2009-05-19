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
#include "KoShapeGroup.h"

class FakeContainer : public KoShapeContainer {
  public:
    virtual bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context) { Q_UNUSED(element); Q_UNUSED(context); return false; }
    virtual void saveOdf(KoShapeSavingContext & context) const { Q_UNUSED(context); }
    virtual void paintComponent(QPainter &painter, const KoViewConverter &converter) { Q_UNUSED(painter); Q_UNUSED(converter); }
  private:
};


Section::Section() : SectionGroup(0)
{
  m_layer = new KoShapeLayer;
  m_container = new FakeContainer;
  m_container->addChild(m_layer);
}

KoShapeLayer* Section::layer()
{
  return m_layer;
}

KoShapeContainer* Section::container()
{
  return m_container;
}

const QString& Section::name() const
{
  return m_name;
}

void Section::setName(const QString& _name)
{
  m_name = _name;
}
