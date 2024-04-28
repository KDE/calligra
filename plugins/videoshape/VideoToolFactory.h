/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VIDEOTOOLFACTORY_H
#define VIDEOTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class VideoToolFactory : public KoToolFactoryBase
{
public:
    VideoToolFactory();
    ~VideoToolFactory();

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // VIDEOTOOLFACTORY_H
