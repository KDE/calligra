// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef SIMPLEHEADERFOOTERWIDGET_H
#define SIMPLEHEADERFOOTERWIDGET_H

#include <QWidget>

#include "ui_SimpleHeaderFooterWidget.h"

class KWView;

class SimpleHeaderFooterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleHeaderFooterWidget(KWView *view, QWidget *parent = nullptr);
    ~SimpleHeaderFooterWidget() override;

private:
    Ui::SimpleHeaderFooterWidget widget;
};

#endif // SIMPLEHEADERFOOTERWIDGET_H
