/* This file is part of the KDE project
   Copyright (C) 2002 The koffice team <koffice@kde.org>

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

#ifndef KPTRELATIONDIALOG_H
#define KPTRELATIONDIALOG_H

#include <kdialogbase.h>

class KPTNode;
class KPTRelation;
class QButtonGroup;


class KPTRelationDialog : public KDialogBase 
{
    Q_OBJECT
public:
    KPTRelationDialog(KPTRelation *rel, QWidget *p=0, const char *n=0);
    KPTRelationDialog(KPTNode *parentNode, KPTNode *childNode, QWidget *p=0, const char *n=0);

protected slots:
    void slotOk();
    void slotUser1();

protected:
    void init();
    
private:
    QButtonGroup *relationType;
//    QButtonGroup *timingType;
    KPTRelation *m_relation;
    KPTNode *m_parent, *m_child;
};


#endif // KPTRELATIONDIALOG_H
