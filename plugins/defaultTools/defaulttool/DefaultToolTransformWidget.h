/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef DEFAULTTOOLTRANSFORMWIDGET_H
#define DEFAULTTOOLTRANSFORMWIDGET_H

#include <KoFlake.h>
#include <ui_DefaultToolTransformWidget.h>

#include <QMenu>

class KoInteractionTool;

class DefaultToolTransformWidget : public QMenu, Ui::DefaultToolTransformWidget
{
    Q_OBJECT
public:
    explicit DefaultToolTransformWidget(KoInteractionTool *tool, QWidget *parent = nullptr);

    /// Sets the unit used by the unit aware child widgets
    void setUnit(const KoUnit &unit);

private Q_SLOTS:
    void resourceChanged(int key, const QVariant &res);
    void rotationChanged();
    void shearXChanged();
    void shearYChanged();
    void scaleXChanged();
    void scaleYChanged();
    void resetTransformations();

private:
    KoInteractionTool *m_tool;
};

#endif
