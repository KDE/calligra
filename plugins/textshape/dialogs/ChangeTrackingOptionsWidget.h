/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef __CHANGE_TRACKING_OPTIONS_WIDGET_H__
#define __CHANGE_TRACKING_OPTIONS_WIDGET_H__

#include <TextTool.h>
#include <ui_ChangeTrackingOptionsWidget.h>

class ChangeTrackingOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChangeTrackingOptionsWidget(TextTool *tool, QWidget *parent = nullptr);

private Q_SLOTS:
    void recordChangesChanged(int isChecked);
    void showChangesChanged(int isChecked);
    void configureSettingsPressed();

public Q_SLOTS:
    void toggleShowChanges(bool on);
    void toggleRecordChanges(bool on);

Q_SIGNALS:
    void doneWithFocus();

private:
    Ui::ChangeTrackingOptions widget;
    TextTool *m_tool;
};

#endif
