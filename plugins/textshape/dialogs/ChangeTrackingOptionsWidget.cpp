/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeTrackingOptionsWidget.h"
#include <QDebug>

ChangeTrackingOptionsWidget::ChangeTrackingOptionsWidget(TextTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_tool(tool)
{
    widget.setupUi(this);
    connect(widget.recordChangesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(recordChangesChanged(int)));
    connect(widget.showChangesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showChangesChanged(int)));
    connect(widget.configureChangeTrackingButton, SIGNAL(clicked(bool)), this, SLOT(configureSettingsPressed()));
}

void ChangeTrackingOptionsWidget::toggleShowChanges(bool isChecked)
{
    widget.showChangesCheckBox->setChecked(isChecked);
}

void ChangeTrackingOptionsWidget::toggleRecordChanges(bool isChecked)
{
    widget.recordChangesCheckBox->setChecked(isChecked);
}

void ChangeTrackingOptionsWidget::recordChangesChanged(int isChecked)
{
    m_tool->toggleRecordChanges(isChecked);
    Q_EMIT doneWithFocus();
}

void ChangeTrackingOptionsWidget::showChangesChanged(int isChecked)
{
    m_tool->toggleShowChanges(isChecked);
    Q_EMIT doneWithFocus();
}

void ChangeTrackingOptionsWidget::configureSettingsPressed()
{
    m_tool->configureChangeTracking();
    Q_EMIT doneWithFocus();
}
