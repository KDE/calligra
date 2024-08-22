// SPDX-FileCopyrightText: 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "SimpleSetupWidget.h"

#include "KWView.h"

SimpleSetupWidget::SimpleSetupWidget(KWView *view, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);

    widget.setup->setDefaultAction(view->action("format_page"));
}

SimpleSetupWidget::~SimpleSetupWidget() = default;
