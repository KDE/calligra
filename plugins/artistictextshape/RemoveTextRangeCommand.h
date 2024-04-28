/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REMOVETEXTRANGECOMMAND_H
#define REMOVETEXTRANGECOMMAND_H

#include "ArtisticTextTool.h"
#include <QPointer>
#include <kundo2command.h>

class ArtisticTextShape;

/// Undo command to remove a range of text from an artistic text shape
class RemoveTextRangeCommand : public KUndo2Command
{
public:
    RemoveTextRangeCommand(ArtisticTextTool *tool, ArtisticTextShape *shape, int from, unsigned int count);

    void redo() override;
    void undo() override;

private:
    QPointer<ArtisticTextTool> m_tool;
    ArtisticTextShape *m_shape;
    int m_from;
    int m_count;
    QList<ArtisticTextRange> m_text;
    int m_cursor;
};

#endif // REMOVETEXTRANGECOMMAND_H
