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

#ifndef KWANCHORINGPROPERTIES_H
#define KWANCHORINGPROPERTIES_H

#include <ui_KWAnchoringProperties.h>
#include <dialogs/KWShapeConfigFactory.h>

#include <KoShapeConfigWidgetBase.h>

#include <QList>

class KWFrame;
class KoShape;

/// A widget that is shown to allow the user to select the anchoring properties
class KWAnchoringProperties : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    /// constructor
    explicit KWAnchoringProperties(FrameConfigSharedState *state);

    /// load all info from the argument frames into this widget
    void open(const QList<KWFrame*> &frames);
    /// reimplemented
    void open(KoShape *shape);
    /// reimplemented
    void save();

    /// reimplemented
    virtual bool showOnShapeCreate() {
        return true;
    }

private slots:

private:
    Ui::KWAnchoringProperties widget;
    FrameConfigSharedState *m_state;

    QButtonGroup *m_anchor;
    QList<KWFrame*> m_frames;
    KoShape *m_shape;
};

#endif
