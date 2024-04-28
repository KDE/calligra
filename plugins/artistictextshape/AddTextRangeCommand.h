/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ADDTEXTRANGECOMMAND_H
#define ADDTEXTRANGECOMMAND_H

#include "ArtisticTextRange.h"
#include "ArtisticTextTool.h"
#include <QPointer>
#include <kundo2command.h>

class ArtisticTextShape;

/// Undo command to add a range of text to a artistic text shape
class AddTextRangeCommand : public KUndo2Command
{
public:
    AddTextRangeCommand(ArtisticTextTool *tool, ArtisticTextShape *shape, const QString &text, int from);
    AddTextRangeCommand(ArtisticTextTool *tool, ArtisticTextShape *shape, const ArtisticTextRange &text, int from);

    void redo() override;
    void undo() override;

private:
    QPointer<ArtisticTextTool> m_tool;
    ArtisticTextShape *m_shape;
    QString m_plainText;
    ArtisticTextRange m_formattedText;
    QList<ArtisticTextRange> m_oldFormattedText;
    int m_from;
};

#endif // ADDTEXTRANGECOMMAND_H
