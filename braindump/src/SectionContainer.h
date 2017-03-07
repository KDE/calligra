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

#include <KoShapeBasedDocumentBase.h>
#include <KoShapeContainer.h>

class KoShapeLayer;
class Section;
class SectionShapeContainerModel;
class RootSection;

class SectionContainer : public KoShapeBasedDocumentBase
{
public:
    SectionContainer(Section* , RootSection* _rootSection);
    SectionContainer(const SectionContainer& _rhs, Section*);
private:
    void initContainer(Section* , RootSection* _rootSection);
public:
    virtual void addShape(KoShape* shape);
    virtual void removeShape(KoShape* shape);
public:
    Section* section();
    KoShapeLayer* layer();
    bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context, QList<KoShape*>& shapes);
    void saveOdf(KoShapeSavingContext & context) const;
    QRectF containerBound() const;
private:
    SectionContainer(const SectionContainer& _rhs);
private:
    Section* m_section;
    KoShapeLayer* m_layer;
    RootSection* m_rootSection;
    SectionShapeContainerModel* m_sectionModel;
};

#endif
