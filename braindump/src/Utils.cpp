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

#include "Utils.h"

#include <QRectF>

#include <KoShape.h>

#include <KoShapeContainer.h>

#include "SectionGroup.h"
#include "Section.h"
#include "SectionContainer.h"

void Utils::containerBoundRec(QList<KoShape*> shapes, QRectF& b)
{
    foreach(KoShape * shape, shapes) {
        containerBoundRec(shape, b);
    }
}

void Utils::containerBoundRec(KoShape* shape, QRectF& b)
{
    b |= shape->boundingRect();
    KoShapeContainer* cont = dynamic_cast<KoShapeContainer*>(shape);
    if(cont) {
        containerBoundRec(cont->shapes(), b);
    }
}

Section* Utils::sectionForLayer(KoShapeLayer* _layer, SectionGroup* _sectionGroup)
{
    foreach(Section * section, _sectionGroup->sections()) {
        if(section->sectionContainer()->layer() == _layer) {
            return section;
        }
        if(Section* ls = sectionForLayer(_layer, section)) {
            return ls;
        }
    }
    return 0;
}
