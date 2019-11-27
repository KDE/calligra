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


#ifndef KPRREORDERANIMATIONCOMMAND_H
#define KPRREORDERANIMATIONCOMMAND_H

#include <kundo2command.h>

#include "stage_export.h"

class KPrShapeAnimations;
class KPrShapeAnimation;

class STAGE_EXPORT KPrReorderAnimationCommand : public KUndo2Command
{
public:
    KPrReorderAnimationCommand(KPrShapeAnimations *shapeAnimationsModel, KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation, KUndo2Command *parent = 0);

    ~KPrReorderAnimationCommand() override;

    /// redo the command
    void redo () override;
    /// revert the actions done in redo
    void undo () override;

private:
    KPrShapeAnimations *m_shapeAnimationsModel;
    KPrShapeAnimation *m_oldAnimation;
    KPrShapeAnimation *m_newAnimation;
};

#endif // KPRREORDERANIMATIONCOMMAND_H
