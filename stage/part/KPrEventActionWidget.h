/* This file is part of the KDE project
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
    explicit KPrEventActionWidget(QWidget *parent = 0);
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
