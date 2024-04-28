/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ConnectionPointWidget.h"
#include "ConnectionTool.h"
#include <QAction>

ConnectionPointWidget::ConnectionPointWidget(ConnectionTool *tool, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.alignLeft->setDefaultAction(tool->action("align-left"));
    widget.alignCenterH->setDefaultAction(tool->action("align-centerh"));
    widget.alignRight->setDefaultAction(tool->action("align-right"));
    widget.alignTop->setDefaultAction(tool->action("align-top"));
    widget.alignCenterV->setDefaultAction(tool->action("align-centerv"));
    widget.alignBottom->setDefaultAction(tool->action("align-bottom"));
    widget.alignPercent->setDefaultAction(tool->action("align-relative"));

    widget.escapeAll->setDefaultAction(tool->action("escape-all"));
    widget.escapeHorz->setDefaultAction(tool->action("escape-horizontal"));
    widget.escapeVert->setDefaultAction(tool->action("escape-vertical"));
    widget.escapeLeft->setDefaultAction(tool->action("escape-left"));
    widget.escapeRight->setDefaultAction(tool->action("escape-right"));
    widget.escapeUp->setDefaultAction(tool->action("escape-up"));
    widget.escapeDown->setDefaultAction(tool->action("escape-down"));

    connect(widget.toggleEditMode, &QCheckBox::stateChanged, tool, &ConnectionTool::toggleConnectionPointEditMode);
    connect(tool, &ConnectionTool::sendConnectionPointEditState, this, &ConnectionPointWidget::toggleEditModeCheckbox);
}

void ConnectionPointWidget::toggleEditModeCheckbox(bool checked)
{
    widget.toggleEditMode->blockSignals(true);
    if (checked)
        widget.toggleEditMode->setCheckState(Qt::Checked);
    else
        widget.toggleEditMode->setCheckState(Qt::Unchecked);
    widget.toggleEditMode->blockSignals(false);
}
