
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

#ifndef _REMEMBER_POSITION_COMMAND_H_
#define _REMEMBER_POSITION_COMMAND_H_

#include <QList>
#include <QMap>
#include <QRectF>

#include <kundo2command.h>

class KoShape;

/**
 * This command should be used before an operation that can trigger a relayouting
 * that will destroy useful position information. For instance, if you split a
 * group in a 'column' layout, then the layout will change all position of the shapes
 * when undoing, you will want to see the shapes to come back to their original position.
 */
class RememberPositionCommand : public KUndo2Command
{
public:
    explicit RememberPositionCommand(const QList<KoShape*>& _shapes, KUndo2Command *cmd = 0);
    virtual ~RememberPositionCommand();
public:
    virtual void undo();
    /// Will do nothing
    virtual void redo();
private:
    QMap<KoShape*, QRectF> m_shapesToGeom;
};

#endif
