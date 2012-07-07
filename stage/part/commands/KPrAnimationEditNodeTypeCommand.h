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

#ifndef KPRANIMATIONEDITNODETYPECOMMAND_H
#define KPRANIMATIONEDITNODETYPECOMMAND_H

#include <kundo2command.h>

#include "stage_export.h"
#include "animations/KPrShapeAnimation.h"

class KPrAnimationStep;
class KPrAnimationSubStep;
class KPrPage;

/// Command to edit node type
class STAGE_EXPORT KPrAnimationEditNodeTypeCommand : public KUndo2Command
{
public:
    KPrAnimationEditNodeTypeCommand(KPrShapeAnimation *animation, KPrAnimationStep *newStep, KPrAnimationSubStep *newSubStep,
                                    KPrShapeAnimation::Node_Type type, KUndo2Command *parent = 0);
    //Reparent children (Node_Type is not changed in children
    KPrAnimationEditNodeTypeCommand(KPrShapeAnimation *animation, KPrAnimationStep *newStep, KPrAnimationSubStep *newSubStep,
                                    KPrShapeAnimation::Node_Type type, QList<KPrShapeAnimation *> children, KUndo2Command *parent = 0);
    KPrAnimationEditNodeTypeCommand(KPrShapeAnimation *animation, KPrAnimationStep *newStep, KPrAnimationSubStep *newSubStep,
                                    KPrShapeAnimation::Node_Type newType, QList<KPrShapeAnimation *> children,
                                    QList<KPrAnimationSubStep *> movedSubSteps, KPrPage *activePage = 0, KUndo2Command *parent=0);


    virtual ~KPrAnimationEditNodeTypeCommand();

    /// redo the command
    void redo ();
    /// revert the actions done in redo
    void undo ();

private:
    KPrShapeAnimation * m_animation;
    KPrAnimationStep *m_newStep;
    KPrAnimationStep *m_oldStep;
    KPrAnimationSubStep *m_newSubStep;
    KPrAnimationSubStep *m_oldSubStep;
    KPrShapeAnimation::Node_Type m_newType;
    KPrShapeAnimation::Node_Type m_oldType;
    QList<KPrShapeAnimation *> m_children;
    QList<KPrAnimationSubStep *> m_substeps;
    KPrPage *m_activePage;
};

#endif // KPRANIMATIONEDITNODETYPECOMMAND_H
