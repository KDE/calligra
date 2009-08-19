/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <koffice-devel@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KPTTASKDESCRIPTIONPANEL_H
#define KPTTASKDESCRIPTIONPANEL_H

#include "kplatoui_export.h"

#include "ui_kpttaskdescriptionpanelbase.h"

#include <QWidget>

#include <KDialog>

namespace KPlato
{

class TaskDescriptionPanel;
class Task;
class Node;
class MacroCommand;
        
class TaskDescriptionPanelImpl : public QWidget, public Ui_TaskDescriptionPanelBase
{
    Q_OBJECT
public:
    TaskDescriptionPanelImpl( Node &node, QWidget *parent );
        
public slots:
    virtual void slotChanged();

signals:
    void textChanged( bool );

protected:
    Node &m_node;
};

class TaskDescriptionPanel : public TaskDescriptionPanelImpl
{
    Q_OBJECT
public:
    explicit TaskDescriptionPanel( Node &node, QWidget *parent = 0, bool readOnly = false );

    MacroCommand *buildCommand();

    bool ok();

    void setStartValues( Node &node );

protected:
    void initDescription( bool readOnly );
};

class KPLATOUI_EXPORT TaskDescriptionDialog : public KDialog
{
    Q_OBJECT
public:
    /**
     * The constructor for the task description dialog.
     * @param node the node to show
     * @param parent parent widget
     */
    explicit TaskDescriptionDialog( Task &task, QWidget *parent = 0, bool readOnly = false  );

    MacroCommand *buildCommand();

protected slots:
    void slotButtonClicked( int button );

protected:
    TaskDescriptionPanel *m_descriptionTab;
};

} //KPlato namespace

#endif // KPTTASKDESCRIPTIONPANEL_H
