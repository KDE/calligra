/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGETEXTOFFSETCOMMAND_H
#define CHANGETEXTOFFSETCOMMAND_H

#include <kundo2command.h>

class ArtisticTextShape;

class ChangeTextOffsetCommand : public KUndo2Command
{
public:
    ChangeTextOffsetCommand(ArtisticTextShape *textShape, qreal oldOffset, qreal newOffset, KUndo2Command *parent = nullptr);
    /// reimplemented from KUndo2Command
    void redo() override;
    /// reimplemented from KUndo2Command
    void undo() override;

private:
    ArtisticTextShape *m_textShape;
    qreal m_oldOffset;
    qreal m_newOffset;
};

#endif // CHANGETEXTOFFSETCOMMAND_H
