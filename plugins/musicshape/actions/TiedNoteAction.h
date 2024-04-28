/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TIEDNOTEACTION_H
#define TIEDNOTEACTION_H

#include "AbstractNoteMusicAction.h"

class TiedNoteAction : public AbstractNoteMusicAction
{
public:
    explicit TiedNoteAction(SimpleEntryTool *tool);

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

#endif // ERASERACTION_H
