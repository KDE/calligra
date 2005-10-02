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

#include <kdialogbase.h>

class QButtonGroup;
class KCommand;

namespace KPlato
{

class RelationPanel;
class KPTDurationWidget;

class KPTNode;
class KPTRelation;
class KPTPart;
class KPTModifyRelationTypeCmd;

class KPTAddRelationDialog : public KDialogBase 
{
    Q_OBJECT
public:
    KPTAddRelationDialog(KPTRelation *rel, QWidget *p, QString caption=QString::null, int buttons=Ok|Cancel, const char *n=0);
    
    virtual KCommand *buildCommand(KPTPart *part);

protected slots:
    void slotOk();
    void lagChanged();
    void typeClicked(int);

protected:
    RelationPanel *m_panel;
    QButtonGroup *relationType;
    KPTRelation *m_relation;
    KPTDurationWidget *m_lag;
};


class KPTModifyRelationDialog : public KPTAddRelationDialog 
{
    Q_OBJECT
public:
    KPTModifyRelationDialog(KPTRelation *rel, QWidget *p=0, const char *n=0);

    virtual KCommand *buildCommand(KPTPart *part);
    bool relationIsDeleted() { return m_deleted; }
    
protected slots:
    void slotUser1();

private:
    bool m_deleted;
};

}  //KPlato namespace

#endif // KPTRELATIONDIALOG_H
