/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef DOTSACTION_H
#define DOTSACTION_H

#include "AbstractNoteMusicAction.h"

class DotsAction : public AbstractNoteMusicAction
{
public:
    explicit DotsAction(SimpleEntryTool *tool);

    void mousePress(MusicCore::Chord *chord, MusicCore::Note *note, qreal distance, const QPointF &pos) override;

    //! To avoid 'mousePress() was hidden' warning
    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override
    {
        AbstractNoteMusicAction::mousePress(staff, bar, pos);
    }
    //! To avoid 'mousePress() was hidden' warning
    void mousePress(MusicCore::StaffElement *se, qreal distance, const QPointF &pos) override
    {
        AbstractNoteMusicAction::mousePress(se, distance, pos);
    }
};

#endif // DOTSACTION_H
