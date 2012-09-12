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

#ifndef _STATESHAPE_H_
#define _STATESHAPE_H_

#include <KoShape.h>

#define STATESHAPEID "StateShape"

class StateShape : public KoShape
{
public:
    StateShape();
    ~StateShape();

    // absolutly necessary:
    void paint(QPainter &painter,
               const KoViewConverter &converter, KoShapePaintingContext &paintcontext);
    virtual void saveOdf(KoShapeSavingContext & context) const;
    virtual bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context);
    const QString& categoryId() const;
    void setCategoryId(const QString& _categoryId);
    const QString& stateId() const;
    void setStateId(const QString& _stateId);
private:
    QString m_categoryId, m_stateId;
};


#endif
