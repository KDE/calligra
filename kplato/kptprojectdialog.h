/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@suse.de

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTPROJECTDIALOG_H
#define KPTPROJECTDIALOG_H

#include "kptresource.h"

#include <kdialogbase.h>
#include <klineedit.h>

#include <qlistbox.h>
#include <qstring.h>

class KPTProject;
class QTextEdit;
class QDateTimeEdit;
class QTimeEdit;
class QButtonGroup;
class QListBox;

class KPTGroupItem : public QListBoxText
{
public:
    KPTGroupItem(KPTResourceGroup *item)
        : QListBoxText(item->name()) { m_group = item; }
    
    KPTResourceGroup *m_group;
};

class KPTResourceItem : public QListBoxText
{
public:
    KPTResourceItem(KPTResource *item)
        : QListBoxText(item->name()) { m_resource = item; }

    KPTResource *m_resource;
};


class KPTProjectDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTProjectDialog(KPTProject &project, QWidget *parent=0,
		     const char *name=0);

protected slots:
    void slotOk();
    void slotAddGroup();
    void slotEditGroup();
    void slotDeleteGroup();
    void slotAddResource();
    void slotEditResource();
    void slotDeleteResource();

    void slotGroupChanged(QListBoxItem * item);
    void slotResourceChanged(QListBoxItem * item);

private:
    KPTProject &project;
    KLineEdit *namefield;
    KLineEdit *leaderfield;
    QTextEdit *descriptionfield;
    
    QDateTimeEdit *sneTime, *fnlTime, *msoTime;
    
    QButtonGroup *constraints;
    QListBox *group, *resource;
    
    KPTGroupItem *m_groupItem;
    KPTResourceItem *m_resourceItem;

};

class KPTGroupDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTGroupDialog(QWidget *parent=0, const char *name=0);
    QString name();
    
protected slots:
    void slotOk();
    
private:
    KLineEdit *m_name;
};

#endif // KPTPROJECTDIALOG_H
