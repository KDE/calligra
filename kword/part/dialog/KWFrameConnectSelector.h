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

#ifndef KWFRAMECONNECTSELECTOR_H
#define KWFRAMECONNECTSELECTOR_H

#include "ui_KWFrameConnectSelector.h"
#include <dialog/KWShapeConfigFactory.h>

#include <KoShapeConfigWidgetBase.h>

#include <QWidget>
#include <QList>

class KWFrame;
class KWTextFrame;
class KWFrameSet;
class KWDocument;

/// A widget that is shown for 1 textframe to connect it to a frameset
class KWFrameConnectSelector : public KoShapeConfigWidgetBase {
    Q_OBJECT
public:
    /// constructor
    KWFrameConnectSelector(FrameConfigSharedState *state);
    ~KWFrameConnectSelector();

    /// load all info from the argument frame into this widget
    bool open(KWFrame *frame);
    /// reimplemented
    void open(KoShape *shape);
    /// reimplemented
    void save();
    /// reimplemented
    KAction *createAction();

private slots:
    void frameSetSelected();
    void nameChanged(const QString &text);

private:
    Ui::KWFrameConnectSelector widget;

    QList<KWFrameSet*> m_frameSets;
    QList<QTreeWidgetItem *> m_items;
    FrameConfigSharedState *m_state;
    KWFrame *m_frame;
};

#endif
