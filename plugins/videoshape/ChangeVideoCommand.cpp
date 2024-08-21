/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeVideoCommand.h"

#include "VideoCollection.h"
#include "VideoData.h"
#include "VideoShape.h"

#include <KLocalizedString>

ChangeVideoCommand::ChangeVideoCommand(VideoShape *videoShape, VideoData *newVideoData, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_newVideoData(newVideoData)
    , m_shape(videoShape)
{
    setText(kundo2_i18n("Change video"));

    m_oldVideoData = m_shape->videoData() ? new VideoData(*(m_shape->videoData())) : nullptr;
}

ChangeVideoCommand::~ChangeVideoCommand()
{
    delete m_oldVideoData;
    delete m_newVideoData;
}

void ChangeVideoCommand::redo()
{
    // we need new here as setUserData deletes the old data
    m_shape->setUserData(m_newVideoData ? new VideoData(*m_newVideoData) : nullptr);
}

void ChangeVideoCommand::undo()
{
    // we need new here as setUserData deletes the old data
    m_shape->setUserData(m_oldVideoData ? new VideoData(*m_oldVideoData) : nullptr);
}
