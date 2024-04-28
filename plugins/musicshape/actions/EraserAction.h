/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ERASERACTION_H
#define ERASERACTION_H

#include "AbstractNoteMusicAction.h"

class EraserAction : public AbstractNoteMusicAction
{
public:
    explicit EraserAction(SimpleEntryTool *tool);

    void mousePress(MusicCore::Chord *chord, MusicCore::Note *note, qreal distance, const QPointF &pos) override;
    void mousePress(MusicCore::StaffElement *note, qreal distance, const QPointF &pos) override;

    //! To avoid 'mousePress() was hidden' warning
    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override
    {
        AbstractNoteMusicAction::mousePress(staff, bar, pos);
    }
};

#endif // ERASERACTION_H
