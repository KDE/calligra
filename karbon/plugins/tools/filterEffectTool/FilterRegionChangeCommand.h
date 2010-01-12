/* This file is part of the KDE project
 * Copyright (c) 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef FILTERREGIONCHANGECOMMAND_H
#define FILTERREGIONCHANGECOMMAND_H

#include <QtGui/QUndoCommand>
#include <QtCore/QRectF>

class KoShape;
class KoFilterEffect;

/// A command to change the region of a filter effect
class FilterRegionChangeCommand : public QUndoCommand
{
public:
    explicit FilterRegionChangeCommand(KoFilterEffect *effect, const QRectF &filterRegion, KoShape *shape = 0, QUndoCommand *parent = 0);

    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    KoFilterEffect * m_effect;
    QRectF m_oldRegion;
    QRectF m_newRegion;
    KoShape * m_shape;
};

#endif // FILTERREGIONCHANGECOMMAND_H
