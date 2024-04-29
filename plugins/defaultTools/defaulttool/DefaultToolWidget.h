/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef DEFAULTTOOLWIDGET_H
#define DEFAULTTOOLWIDGET_H

#include <KoFlake.h>
#include <ui_DefaultToolWidget.h>

#include <QWidget>

class KoInteractionTool;

class DefaultToolWidget : public QWidget, Ui::DefaultToolWidget
{
    Q_OBJECT
public:
    explicit DefaultToolWidget(KoInteractionTool *tool, QWidget *parent = nullptr);

    /// Sets the unit used by the unit aware child widgets
    void setUnit(const KoUnit &unit);

private Q_SLOTS:
    void positionSelected(KoFlake::Position position);
    void updatePosition();
    void positionHasChanged();
    void updateSize();
    void sizeHasChanged();
    void resourceChanged(int key, const QVariant &res);
    void aspectButtonToggled(bool keepAspect);

private:
    KoInteractionTool *m_tool;
    bool m_blockSignals;
};

#endif
