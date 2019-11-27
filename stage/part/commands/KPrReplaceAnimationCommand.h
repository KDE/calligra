/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRREPLACEANIMATIONCOMMAND_H
#define KPRREPLACEANIMATIONCOMMAND_H

#include <kundo2command.h>

#include "stage_export.h"
#include "animations/KPrShapeAnimation.h"

class KPrDocument;

/// Command to replace an animation
class STAGE_EXPORT KPrReplaceAnimationCommand : public KUndo2Command
{
public:
    KPrReplaceAnimationCommand(KPrDocument *doc, KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation, KUndo2Command *parent=0);

    ~KPrReplaceAnimationCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    enum AnimationDelete  {
        DeleteOld,
        DeleteNew
    };
    KPrShapeAnimation *m_oldAnimation;
    KPrShapeAnimation *m_newAnimation;
    KPrDocument *m_document;
    AnimationDelete m_deleteAnimation;
};

#endif // KPRREPLACEANIMATIONCOMMAND_H
