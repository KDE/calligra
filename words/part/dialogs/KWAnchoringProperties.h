/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2011 KoGmbh <cbo@kogmbh.com>
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
#include <KoShapeAnchor.h>

#include <QList>
#include <QPointF>

class KoShape;
class KWCanvas;

/// A widget that is shown to allow the user to select the anchoring properties
class KWAnchoringProperties : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    /// constructor
    explicit KWAnchoringProperties(FrameConfigSharedState *state);

    /// load all info from the argument shapes into this widget
    /// returns true if at least one shape was accepted
    bool open(const QList<KoShape *> &shapes);
    /// reimplemented
    void open(KoShape *shape) override;
    /// reimplemented
    void save() override;
    /// save but add to undo command if command != 0
    void save(KUndo2Command *command, KWCanvas *canvas);

    /// reimplemented
    bool showOnShapeCreate() override {
        return true;
    }

private Q_SLOTS:
    void anchorTypeChanged(int anchorType);
    void vertPosChanged(int verticalAlign, QPointF offset = QPointF());
    void vertRelChanged(int index);
    void horizPosChanged(int horizontalAlign, QPointF offset = QPointF());
    void horizRelChanged(int index);

private:
    static const int vertRels[4][20];

    static const int horizRels[4][20];


    Ui::KWAnchoringProperties widget;
    FrameConfigSharedState *m_state;

    QButtonGroup *m_anchorTypeGroup;
    QButtonGroup *m_vertPosGroup;
    QButtonGroup *m_horizPosGroup;
    QList<KoShape *> m_shapes;
    int m_anchorType;
    int m_vertPos;
    int m_horizPos;
    int m_vertRel;
    int m_horizRel;
};

#endif
