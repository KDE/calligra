/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DETACHTEXTFROMPATHCOMMAND_H
#define DETACHTEXTFROMPATHCOMMAND_H

#include <kundo2command.h>
#include <QPainterPath>

class ArtisticTextShape;
class KoPathShape;

class DetachTextFromPathCommand : public KUndo2Command
{
public:
    explicit DetachTextFromPathCommand( ArtisticTextShape * textShape, KUndo2Command * parent = 0 );
    /// reimplemented from KUndo2Command
    void redo() override;
    /// reimplemented from KUndo2Command
    void undo() override;
private:
    ArtisticTextShape * m_textShape;
    KoPathShape * m_pathShape;
    QPainterPath m_path;
};

#endif // DETACHTEXTFROMPATHCOMMAND_H
