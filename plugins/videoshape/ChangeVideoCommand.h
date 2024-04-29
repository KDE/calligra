/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGEVIDEOCOMMAND_H
#define CHANGEVIDEOCOMMAND_H

#include <kundo2command.h>

class VideoData;
class VideoShape;

class ChangeVideoCommand : public KUndo2Command
{
public:
    ChangeVideoCommand(VideoShape *videoShape, VideoData *newVideoData, KUndo2Command *parent = nullptr);
    ~ChangeVideoCommand();

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    VideoData *m_oldVideoData;
    VideoData *m_newVideoData;
    VideoShape *m_shape;
};

#endif // CHANGEVIDEOCOMMAND_H
