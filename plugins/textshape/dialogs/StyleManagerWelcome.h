/* This file is part of the KDE project
  SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pstirnweiss@googlemail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef STYLEMANAGERWELCOME_H
#define STYLEMANAGERWELCOME_H

#include <QWidget>

#include <ui_StyleManagerWelcome.h>

class StyleManagerWelcome : public QWidget
{
    Q_OBJECT
public:
    explicit StyleManagerWelcome(QWidget *parent = nullptr);

private:
    Ui::StyleManagerWelcome widget;
};

#endif // STYLEMANAGERWELCOME_H
