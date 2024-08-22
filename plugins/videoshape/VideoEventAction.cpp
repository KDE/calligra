/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009-2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoEventAction.h"

#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>

#include <QUrl>

#include "VideoData.h"
#include "VideoDebug.h"
#include "VideoShape.h"

#include "FullScreenPlayer.h"

VideoEventAction::VideoEventAction(VideoShape *parent)
    : KoEventAction()
    , m_shape(parent)
    , m_player(nullptr)
{
    setId(QString("videoeventaction"));
}

VideoEventAction::~VideoEventAction() = default;

bool VideoEventAction::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    return true;
}

void VideoEventAction::saveOdf(KoShapeSavingContext &context) const
{
    Q_UNUSED(context);
}

void VideoEventAction::start()
{
    VideoData *videoData = qobject_cast<VideoData *>(m_shape->userData());
    Q_ASSERT(videoData);
    m_player = new FullScreenPlayer(videoData->playableUrl());
}

void VideoEventAction::finish()
{
}
