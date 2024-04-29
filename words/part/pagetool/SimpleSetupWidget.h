// SPDX-FileCopyrightText: 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef SIMPLESETUPWIDGET_H
#define SIMPLESETUPWIDGET_H

#include <QWidget>

#include "ui_SimpleSetupWidget.h"

class KWView;

class SimpleSetupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleSetupWidget(KWView *view, QWidget *parent = nullptr);
    ~SimpleSetupWidget() override;

private:
    Ui::SimpleSetupWidget widget;
};

#endif // SIMPLESETUPWIDGET_H
