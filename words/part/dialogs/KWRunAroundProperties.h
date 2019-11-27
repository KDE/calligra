/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#ifndef KWRUNAROUNDPROPERTIES_H
#define KWRUNAROUNDPROPERTIES_H

#include <ui_KWRunAroundProperties.h>
#include <dialogs/KWShapeConfigFactory.h>

#include <KoShapeConfigWidgetBase.h>

#include <QList>

class KoShape;

/// A widget that is shown to allow the user to select the run around properties
class KWRunAroundProperties : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    /// constructor
    explicit KWRunAroundProperties(FrameConfigSharedState *state);

    /// load all info from the argument frames into this widget
    /// returns true if at least one frame was accepted
    bool open(const QList<KoShape *> &shapes);
    /// reimplemented
    void open(KoShape *shape) override;
    /// reimplemented
    void save() override;
    /// save but add to undo command if command != 0
    void save(KUndo2Command *command);

    /// reimplemented
    bool showOnShapeCreate() override {
        return true;
    }

private Q_SLOTS:
    void enoughRunAroundToggled(bool checked);

private:
    Ui::KWRunAroundProperties widget;
    FrameConfigSharedState *m_state;

    QButtonGroup *m_runAroundSide;
    QButtonGroup *m_runAround;
    QButtonGroup *m_runAroundContour;
    QList<KoShape *> m_shapes;
};

#endif // KWRUNAROUNDPROPERTIES_H
