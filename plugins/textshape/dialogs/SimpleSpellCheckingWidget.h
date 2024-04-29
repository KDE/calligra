/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Luke De Mouy <lukewolf101010devel@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLESPELLCHECKINGWIDGET_H
#define SIMPLESPELLCHECKINGWIDGET_H

#include <QWidget>

#include "ui_SimpleSpellCheckingWidget.h"
namespace Ui
{
class SimpleSpellCheckingWidget;
}
class ReviewTool;

class SimpleSpellCheckingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleSpellCheckingWidget(ReviewTool *tool, QWidget *parent = nullptr);
    ~SimpleSpellCheckingWidget() override;

private:
    Ui::SimpleSpellCheckingWidget *ui;
};

#endif // SIMPLESPELLCHECKINGWIDGET_H
