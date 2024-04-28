/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Luke De Mouy <lukewolf101010devel@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SimpleSpellCheckingWidget.h"

#include "../ReviewTool.h"
#include "TextTool.h"

SimpleSpellCheckingWidget::SimpleSpellCheckingWidget(ReviewTool *tool, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SimpleSpellCheckingWidget)
{
    ui->setupUi(this);
    ui->toolAutoSpellCheck->setDefaultAction((QAction *)tool->action("tool_auto_spellcheck"));
}

SimpleSpellCheckingWidget::~SimpleSpellCheckingWidget()
{
    delete ui;
}
