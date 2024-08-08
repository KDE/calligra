/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
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

#ifndef _COLUMN_LAYOUT_H_
#define _COLUMN_LAYOUT_H_

#include <Layout.h>
#include <LayoutFactory.h>

class ColumnLayout : public Layout
{
public:
    ColumnLayout();
    virtual ~ColumnLayout();
    QRectF boundingBox() const override;

protected:
    void shapesAdded(QList<KoShape *> _shape) override;
    void shapeAdded(KoShape *_shape) override;
    void shapeRemoved(KoShape *_shape) override;
    void shapeGeometryChanged(KoShape *_shape) override;
    void relayout() override;

private:
    QList<KoShape *> m_shapes;
    bool m_isUpdating;
};

class ColumnLayoutFactory : public LayoutFactory
{
public:
    ColumnLayoutFactory();
    virtual ~ColumnLayoutFactory();
    Layout *createLayout() const override;
};

#endif
