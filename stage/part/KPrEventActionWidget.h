/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPREVENTACTIONWIDGET_H
#define KPREVENTACTIONWIDGET_H

#include <QWidget>

#include "stage_export.h"

class KPrEventActionData;
class KUndo2Command;

/**
 * Widget used for configuring an event action
 *
 * Each event action needs to implement this widget to make it configurable
 */
class STAGE_EXPORT KPrEventActionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrEventActionWidget(QWidget *parent = nullptr);
    ~KPrEventActionWidget() override;

    /**
     * Set the data for the widget
     */
    virtual void setData(KPrEventActionData *eventActionData) = 0;

Q_SIGNALS:
    /**
     * Send signal to add the created command to the command history
     */
    void addCommand(KUndo2Command *command);
};

#endif // KPREVENTACTIONWIDGET_H
