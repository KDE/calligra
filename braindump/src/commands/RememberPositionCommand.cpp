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

#include "RememberPositionCommand.h"
#include <KoShape.h>

RememberPositionCommand::RememberPositionCommand(const QList<KoShape*>& _shapes, KUndo2Command *cmd) : KUndo2Command(cmd)
{
    foreach(KoShape * shape, _shapes) {
        m_shapesToGeom[shape] = QRectF(shape->absolutePosition(), shape->size());
    }
}

RememberPositionCommand::~RememberPositionCommand()
{
}

void RememberPositionCommand::undo()
{
    for(QMap<KoShape*, QRectF>::iterator it = m_shapesToGeom.begin();
            it != m_shapesToGeom.end(); ++it) {
        it.key()->setAbsolutePosition(it.value().topLeft());
        it.key()->setSize(it.value().size());
    }
}

void RememberPositionCommand::redo()
{
}
