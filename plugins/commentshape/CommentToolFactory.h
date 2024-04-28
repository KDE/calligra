/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef COMMENTTOOLFACTORY_H
#define COMMENTTOOLFACTORY_H

#include <KoToolFactoryBase.h>

class CommentToolFactory : public KoToolFactoryBase
{
public:
    CommentToolFactory();
    virtual ~CommentToolFactory();

    virtual KoToolBase *createTool(KoCanvasBase *canvas);
    virtual bool canCreateTool(KoCanvasBase *canvas) const;
};

#endif // COMMENTTOOLFACTORY_H
