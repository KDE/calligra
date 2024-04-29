/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLEENTRYWIDGET_H
#define SIMPLEENTRYWIDGET_H

#include <ui_SimpleEntryWidget.h>

#include <QWidget>

class SimpleEntryTool;

class SimpleEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleEntryWidget(SimpleEntryTool *tool, QWidget *parent = nullptr);
    void setVoiceListEnabled(bool enabled);
Q_SIGNALS:
    void voiceChanged(int voice);
private Q_SLOTS:
private:
    Ui::SimpleEntryWidget widget;
    SimpleEntryTool *m_tool;
};

#endif // SIMPLEENTRYWIDGET_H
