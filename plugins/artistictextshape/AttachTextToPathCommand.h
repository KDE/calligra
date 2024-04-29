/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ATTACHTEXTTOPATHCOMMAND_H
#define ATTACHTEXTTOPATHCOMMAND_H

#include <QTransform>
#include <kundo2command.h>

class ArtisticTextShape;
class KoPathShape;

class AttachTextToPathCommand : public KUndo2Command
{
public:
    AttachTextToPathCommand(ArtisticTextShape *textShape, KoPathShape *pathShape, KUndo2Command *parent = nullptr);
    /// reimplemented from KUndo2Command
    void redo() override;
    /// reimplemented from KUndo2Command
    void undo() override;

private:
    ArtisticTextShape *m_textShape;
    KoPathShape *m_pathShape;
    QTransform m_oldMatrix;
};

#endif // ATTACHTEXTTOPATHCOMMAND_H
