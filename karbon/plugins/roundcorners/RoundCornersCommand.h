/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ROUNDCORNERSCOMMAND_H
#define ROUNDCORNERSCOMMAND_H

#include <kundo2command.h>
#include <QPointF>

class KoPathShape;
class KoPathSegment;
class KoPathPoint;

/// command for rounding corners on a path shape
class RoundCornersCommand : public KUndo2Command
{
public:
    RoundCornersCommand(KoPathShape * path, qreal radius, KUndo2Command * parent = 0);
    ~RoundCornersCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    void roundPath();
    KoPathPoint * addSegment(KoPathShape * p, KoPathSegment & s);
    void copyPath(KoPathShape * dst, KoPathShape * src);
    QPointF tangentAtStart(const KoPathSegment &s);
    QPointF tangentAtEnd(const KoPathSegment &s);

    qreal m_radius;
    KoPathShape * m_path;
    KoPathShape * m_copy;
};

#endif // ROUNDCORNERSCOMMAND_H
