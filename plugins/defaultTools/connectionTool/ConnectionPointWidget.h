/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CONNECTIONPOINTWIDGET_H
#define CONNECTIONPOINTWIDGET_H

#include <QWidget>
#include <ui_ConnectionPointWidget.h>

class ConnectionTool;

class ConnectionPointWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionPointWidget(ConnectionTool *tool, QWidget *parent = nullptr);

private Q_SLOTS:
    void toggleEditModeCheckbox(bool checked);

private:
    Ui::ConnectionPointWidget widget;
    QButtonGroup *m_horzGroup;
    QButtonGroup *m_vertGroup;
};

#endif // CONNECTIONPOINTWIDGET_H
