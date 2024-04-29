/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ROUNDCORNERSCOMMAND_H
#define ROUNDCORNERSCOMMAND_H

#include <QPointF>
#include <kundo2command.h>

class KoPathShape;
class KoPathSegment;
class KoPathPoint;

/// command for rounding corners on a path shape
class RoundCornersCommand : public KUndo2Command
{
public:
    RoundCornersCommand(KoPathShape *path, qreal radius, KUndo2Command *parent = nullptr);
    ~RoundCornersCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    void roundPath();
    KoPathPoint *addSegment(KoPathShape *p, KoPathSegment &s);
    void copyPath(KoPathShape *dst, KoPathShape *src);
    QPointF tangentAtStart(const KoPathSegment &s);
    QPointF tangentAtEnd(const KoPathSegment &s);

    qreal m_radius;
    KoPathShape *m_path;
    KoPathShape *m_copy;
};

#endif // ROUNDCORNERSCOMMAND_H
