/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jim Courtiau <jeremy.courtiau@gmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTATIONTOOLWIDGET_H
#define KPRPRESENTATIONTOOLWIDGET_H

#include "ui_KPrPresentationTool.h"

class KPrPresentationToolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrPresentationToolWidget(QWidget *parent = nullptr);
    Ui::KPrPresentationTool presentationToolUi();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::KPrPresentationTool m_uiWidget;
};

#endif // KPRPRESENTATIONTOOLWIDGET_H
