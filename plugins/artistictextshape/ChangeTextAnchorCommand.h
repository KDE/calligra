/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGETEXTANCHORCOMMAND_H
#define CHANGETEXTANCHORCOMMAND_H

#include "ArtisticTextShape.h"
#include <kundo2command.h>

class ChangeTextAnchorCommand : public KUndo2Command
{
public:
    ChangeTextAnchorCommand(ArtisticTextShape *shape, ArtisticTextShape::TextAnchor anchor);
    void undo() override;
    void redo() override;

private:
    ArtisticTextShape *m_shape;
    ArtisticTextShape::TextAnchor m_anchor;
    ArtisticTextShape::TextAnchor m_oldAnchor;
};

#endif // CHANGETEXTANCHORCOMMAND_H
