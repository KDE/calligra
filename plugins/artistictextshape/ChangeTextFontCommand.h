/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGETEXTFONTCOMMAND_H
#define CHANGETEXTFONTCOMMAND_H

#include "ArtisticTextRange.h"
#include <QFont>
#include <kundo2command.h>

class ArtisticTextShape;

class ChangeTextFontCommand : public KUndo2Command
{
public:
    ChangeTextFontCommand(ArtisticTextShape *shape, const QFont &font, KUndo2Command *parent = nullptr);
    ChangeTextFontCommand(ArtisticTextShape *shape, int from, int count, const QFont &font, KUndo2Command *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    ArtisticTextShape *m_shape;
    QFont m_newFont;
    QList<ArtisticTextRange> m_oldText;
    QList<ArtisticTextRange> m_newText;
    int m_rangeStart;
    int m_rangeCount;
};

#endif // CHANGETEXTFONTCOMMAND_H
