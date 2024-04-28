/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ACCIDENTALACTION_H
#define ACCIDENTALACTION_H

#include "AbstractNoteMusicAction.h"

class AccidentalAction : public AbstractNoteMusicAction
{
public:
    AccidentalAction(int accidentals, SimpleEntryTool *tool);

    void renderPreview(QPainter &painter, const QPointF &point) override;
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

private:
    int m_accidentals;
};

#endif // ACCIDENTALACTION_H
