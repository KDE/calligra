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

#include "ui_relationpanel.h"
#include <QWidget>
#include <kdialog.h>

class Q3ButtonGroup;
class QAbstractButton;
class KCommand;

namespace KPlato
{

class RelationPanel;
class DurationWidget;

class Node;
class Relation;
class Part;
class ModifyRelationTypeCmd;

class RelationPanel : public QWidget, public Ui_RelationPanel
{
    Q_OBJECT
public:
    RelationPanel(QWidget *parent=0);
    
    DurationWidget *lag;
};

class AddRelationDialog : public KDialog
{
    Q_OBJECT
public:
    AddRelationDialog(Relation *rel, QWidget *p, const QString& caption=QString(), ButtonCodes buttons=Ok|Cancel, const char *n=0);

    virtual KCommand *buildCommand(Part *part);
    int selectedRelationType() const;
    
protected slots:
    void slotOk();
    void lagChanged();
    void typeClicked(int);
    void slotFinishStartToggled(bool);
    void slotFinishFinishToggled(bool);
    void slotStartStartToggled(bool);

protected:
    RelationPanel *m_panel;
    Q3ButtonGroup *relationType;
    Relation *m_relation;
    DurationWidget *m_lag;
};


class ModifyRelationDialog : public AddRelationDialog
{
    Q_OBJECT
public:
    explicit ModifyRelationDialog(Relation *rel, QWidget *p=0, const char *n=0);

    virtual KCommand *buildCommand(Part *part);
    bool relationIsDeleted() { return m_deleted; }

protected slots:
    void slotUser1();

private:
    bool m_deleted;
};

}  //KPlato namespace

#endif // RELATIONDIALOG_H
