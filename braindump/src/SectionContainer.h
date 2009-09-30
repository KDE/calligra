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

#ifndef _SECTION_CONTAINER_H_
#define _SECTION_CONTAINER_H_

#include <KoShapeContainer.h>

class KoShapeLayer;
class Section;
class SectionShapeContainerModel;
class KoUndoStack;

class SectionContainer {
  public:
    SectionContainer(Section* );
    SectionContainer(const SectionContainer& _rhs, Section* );
  private:
    void initContainer(Section* );
  public:
    void setUndoStack(KoUndoStack* );
    Section* section();
    KoShapeLayer* layer();
    bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context, QList<KoShape*>& shapes);
    void saveOdf(KoShapeSavingContext & context) const;
    QMap<QString, KoDataCenter *> dataCenterMap() const;
    QRectF containerBound() const;
  private:
    SectionContainer(const SectionContainer& _rhs);
  private:
    Section* m_section;
    QMap<QString, KoDataCenter *> m_dataCenterMap;
    KoShapeLayer* m_layer;
    SectionShapeContainerModel* m_sectionModel;
};

#endif
