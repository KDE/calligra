/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef DEFAULTTOOLARRANGEWIDGET_H
#define DEFAULTTOOLARRANGEWIDGET_H

#include <KoFlake.h>
#include <ui_DefaultToolArrangeWidget.h>

#include <QWidget>

class KoInteractionTool;

class DefaultToolArrangeWidget : public QWidget, Ui::DefaultToolArrangeWidget
{
    Q_OBJECT
public:
    explicit DefaultToolArrangeWidget(KoInteractionTool *tool, QWidget *parent = nullptr);

private:
    KoInteractionTool *m_tool;
};

#endif
