/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 KoGmbh <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWANCHORINGPROPERTIES_H
#define KWANCHORINGPROPERTIES_H

#include <dialogs/KWShapeConfigFactory.h>
#include <ui_KWAnchoringProperties.h>

#include <KoShapeAnchor.h>
#include <KoShapeConfigWidgetBase.h>

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
    bool showOnShapeCreate() override
    {
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
