/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PathToolOptionWidget.h"
#include "KoPathTool.h"
#include <QAction>

PathToolOptionWidget::PathToolOptionWidget(KoPathTool *tool, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.corner->setDefaultAction(tool->action("pathpoint-corner"));
    widget.smooth->setDefaultAction(tool->action("pathpoint-smooth"));
    widget.symmetric->setDefaultAction(tool->action("pathpoint-symmetric"));
    widget.lineSegment->setDefaultAction(tool->action("pathsegment-line"));
    widget.curveSegment->setDefaultAction(tool->action("pathsegment-curve"));
    widget.linePoint->setDefaultAction(tool->action("pathpoint-line"));
    widget.curvePoint->setDefaultAction(tool->action("pathpoint-curve"));
    widget.addPoint->setDefaultAction(tool->action("pathpoint-insert"));
    widget.removePoint->setDefaultAction(tool->action("pathpoint-remove"));
    widget.breakPoint->setDefaultAction(tool->action("path-break-point"));
    widget.breakSegment->setDefaultAction(tool->action("path-break-segment"));
    widget.joinSegment->setDefaultAction(tool->action("pathpoint-join"));
    widget.mergePoints->setDefaultAction(tool->action("pathpoint-merge"));

    connect(widget.convertToPath, &QAbstractButton::released, tool->action("convert-to-path"), &QAction::trigger);
}

PathToolOptionWidget::~PathToolOptionWidget() = default;

void PathToolOptionWidget::setSelectionType(int type)
{
    const bool plain = type & PlainPath;
    if (plain)
        widget.stackedWidget->setCurrentIndex(0);
    else
        widget.stackedWidget->setCurrentIndex(1);
}
