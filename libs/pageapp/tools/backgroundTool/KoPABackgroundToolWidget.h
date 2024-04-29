/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPABACKGROUNDTOOLWIDGET_H
#define KOPABACKGROUNDTOOLWIDGET_H

#include <QWidget>

#include "ui_BackgroundToolWidget.h"

class KoPABackgroundTool;

class KoPABackgroundToolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KoPABackgroundToolWidget(KoPABackgroundTool *tool, QWidget *parent = nullptr);
    ~KoPABackgroundToolWidget() override;

public Q_SLOTS:
    void slotActivePageChanged();

private Q_SLOTS:
    void setBackgroundImage();
    void useMasterBackground(bool doUse);
    void displayMasterShapes(bool doDisplay);

private:
    Ui::BackgroundToolWidget widget;
    KoPABackgroundTool *m_tool;
};

#endif /* KOPABACKGROUNDTOOLWIDGET_H */
