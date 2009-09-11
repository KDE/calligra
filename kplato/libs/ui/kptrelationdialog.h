/* This file is part of the KDE project
   Copyright (C) 2002 The koffice team <koffice@kde.org>
   Copyright (C) 2003, 2004 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTRELATIONDIALOG_H
#define KPTRELATIONDIALOG_H

#include "kplatoui_export.h"

#include "ui_relationpanel.h"
#include <QWidget>
#include <kdialog.h>

namespace KPlato
{

class RelationPanel;
class DurationWidget;

class Relation;
class Project;
class MacroCommand;

class RelationPanel : public QWidget, public Ui_RelationPanel
{
    Q_OBJECT
public:
    RelationPanel(QWidget *parent=0);
    
};

class KPLATOUI_EXPORT AddRelationDialog : public KDialog
{
    Q_OBJECT
public:
    AddRelationDialog(Project &project, Relation *rel, QWidget *p, const QString& caption=QString(), ButtonCodes buttons=Ok|Cancel);

    virtual MacroCommand *buildCommand();
    int selectedRelationType() const;
    
protected slots:
    void slotOk();
    void lagChanged();
    void typeClicked(int);
    void slotFinishStartToggled(bool);
    void slotFinishFinishToggled(bool);
    void slotStartStartToggled(bool);

protected:
    Project &m_project;
    RelationPanel *m_panel;
    Relation *m_relation;
};


class KPLATOUI_EXPORT ModifyRelationDialog : public AddRelationDialog
{
    Q_OBJECT
public:
    explicit ModifyRelationDialog(Project &project, Relation *rel, QWidget *p=0);

    virtual MacroCommand *buildCommand();
    bool relationIsDeleted() { return m_deleted; }

protected slots:
    void slotUser1();

private:
    bool m_deleted;
};

}  //KPlato namespace

#endif // RELATIONDIALOG_H
