/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef RECTANGLESHAPECONFIGWIDGET_H
#define RECTANGLESHAPECONFIGWIDGET_H

#include <ui_RectangleShapeConfigWidget.h>

#include <KoShapeConfigWidgetBase.h>

class RectangleShape;

class RectangleShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    RectangleShapeConfigWidget();
    /// reimplemented
    void open(KoShape *shape) override;
    /// reimplemented
    void save() override;
    /// reimplemented
    void setUnit(const KoUnit &unit) override;
    /// reimplemented
    bool showOnShapeCreate() override
    {
        return false;
    }
    /// reimplemented
    KUndo2Command *createCommand() override;

private:
    Ui::RectangleShapeConfigWidget widget;
    RectangleShape *m_rectangle;
};

#endif // RECTANGLESHAPECONFIGWIDGET_H
