/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWRUNAROUNDPROPERTIES_H
#define KWRUNAROUNDPROPERTIES_H

#include <dialogs/KWShapeConfigFactory.h>
#include <ui_KWRunAroundProperties.h>

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
    bool showOnShapeCreate() override
    {
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
