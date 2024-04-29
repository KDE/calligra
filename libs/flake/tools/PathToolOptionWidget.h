/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef PATHTOOLOPTIONWIDGET_H
#define PATHTOOLOPTIONWIDGET_H

#include <QFlags>
#include <QWidget>

#include <ui_PathToolOptionWidgetBase.h>

class KoPathTool;

class PathToolOptionWidget : public QWidget
{
    Q_OBJECT
public:
    enum Type { PlainPath = 1, ParametricShape = 2 };
    Q_DECLARE_FLAGS(Types, Type)

    explicit PathToolOptionWidget(KoPathTool *tool, QWidget *parent = nullptr);
    ~PathToolOptionWidget() override;

public Q_SLOTS:
    void setSelectionType(int type);

private:
    Ui::PathToolOptionWidgetBase widget;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PathToolOptionWidget::Types)

#endif
