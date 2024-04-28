/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ELLIPSESHAPECONFIGWIDGET_H
#define ELLIPSESHAPECONFIGWIDGET_H

#include "EllipseShape.h"
#include <ui_EllipseShapeConfigWidget.h>

#include <KoShapeConfigWidgetBase.h>

class EllipseShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    EllipseShapeConfigWidget();
    /// reimplemented
    void open(KoShape *shape) override;
    /// reimplemented
    void save() override;
    /// reimplemented
    bool showOnShapeCreate() override
    {
        return false;
    }
    /// reimplemented
    KUndo2Command *createCommand() override;
private Q_SLOTS:
    void closeEllipse();

private:
    Ui::EllipseShapeConfigWidget widget;
    EllipseShape *m_ellipse;
};

#endif // ELLIPSESHAPECONFIGWIDGET_H
