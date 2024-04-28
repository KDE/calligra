/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWFRAMECONNECTSELECTOR_H
#define KWFRAMECONNECTSELECTOR_H

#include <dialogs/KWShapeConfigFactory.h>
#include <ui_KWFrameConnectSelector.h>

#include <KoShapeConfigWidgetBase.h>

#include <QList>

class KWFrameSet;

/// A widget that is shown for 1 textframe to connect it to a frameset
class KWFrameConnectSelector : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    /// constructor
    explicit KWFrameConnectSelector(FrameConfigSharedState *state);

    /// load all info from the argument frame into this widget
    bool canOpen(KoShape *shape);
    /// reimplemented
    void open(KoShape *shape) override;
    /// reimplemented
    void save() override;

    /// reimplemented
    bool showOnShapeCreate() override
    {
        return true;
    }

private Q_SLOTS:
    void existingRadioClicked(bool on);
    void frameSetSelected();
    void nameChanged(const QString &text);

private:
    Ui::KWFrameConnectSelector widget;

    QList<KWFrameSet *> m_frameSets;
    QList<QTreeWidgetItem *> m_items;
    FrameConfigSharedState *m_state;
    KoShape *m_shape;
};

#endif
