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

#ifndef KWFRAMERUNAROUNDPROPERTIES_H
#define KWFRAMERUNAROUNDPROPERTIES_H

#include "ui_KWFrameRunaroundProperties.h"
#include <dialog/KWShapeConfigFactory.h>

#include <KoShapeConfigWidgetBase.h>

#include <QWidget>
#include <QList>

class KWFrame;
class KoShape;

class KWFrameRunaroundProperties : public KoShapeConfigWidgetBase {
    Q_OBJECT
public:
    KWFrameRunaroundProperties(FrameConfigSharedState *state);
    ~KWFrameRunaroundProperties();

    void open(const QList<KWFrame*> &frames);
    void open(KoShape *shape);
    void save();
    KAction *createAction();

private:
    Ui::KWFrameRunaroundProperties widget;
    FrameConfigSharedState *m_state;

    QButtonGroup *m_runAroundSide, *m_runAround;
    QList<KWFrame*> m_frames;
    KoShape *m_shape;
};

#endif
