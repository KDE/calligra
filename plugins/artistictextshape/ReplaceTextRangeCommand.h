/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REPLACETEXTRANGECOMMAND_H
#define REPLACETEXTRANGECOMMAND_H

#include "ArtisticTextRange.h"
#include "ArtisticTextTool.h"
#include <QPointer>
#include <kundo2command.h>

class ArtisticTextShape;

/// Undo command to replace a range of text on an artistic text shape
class ReplaceTextRangeCommand : public KUndo2Command
{
public:
    ReplaceTextRangeCommand(ArtisticTextShape *shape, const QString &text, int from, int count, ArtisticTextTool *tool, KUndo2Command *parent = nullptr);
    ReplaceTextRangeCommand(ArtisticTextShape *shape,
                            const ArtisticTextRange &text,
                            int from,
                            int count,
                            ArtisticTextTool *tool,
                            KUndo2Command *parent = nullptr);
    ReplaceTextRangeCommand(ArtisticTextShape *shape,
                            const QList<ArtisticTextRange> &text,
                            int from,
                            int count,
                            ArtisticTextTool *tool,
                            KUndo2Command *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<ArtisticTextTool> m_tool;
    ArtisticTextShape *m_shape;
    QList<ArtisticTextRange> m_newFormattedText;
    QList<ArtisticTextRange> m_oldFormattedText;
    int m_from;
    int m_count;
};

#endif // REPLACETEXTRANGECOMMAND_H
