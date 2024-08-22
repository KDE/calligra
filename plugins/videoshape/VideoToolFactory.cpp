/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoToolFactory.h"

#include "VideoShape.h"
#include "VideoTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

VideoToolFactory::VideoToolFactory()
    : KoToolFactoryBase("VideoToolFactoryId")
{
    setToolTip(i18n("Video handling"));
    setIconName(koIconName("video-x-generic"));
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(VIDEOSHAPEID);
}

VideoToolFactory::~VideoToolFactory() = default;

KoToolBase *VideoToolFactory::createTool(KoCanvasBase *canvas)
{
    return new VideoTool(canvas);
}
