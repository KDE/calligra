/* This file is part of the KDE project
   Copyright (C) 2002 The Koffice Team <koffice@kde.org>

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
 
#ifndef KPTPROJECTLIST_H
#define KPTPROJECTLIST_H

#include "kptnode.h"

#include <klistview.h>

class KPTView;
class KPTNode;
class KPTNodeItem;
class QWidget;

 class KPTProjectList : public KListView
{
    Q_OBJECT
    
 public:
     KPTProjectList( KPTView *view, QWidget *parent );

    ~KPTProjectList();
 
    int canvasHeight() const;
 	
 public slots:
    void slotSetContentsPos(int x, int y);
    
 protected:
    void displayProject();
    void displayChildren(const KPTNode &node, KPTNodeItem *item);

private:
    int m_defaultFontSize;
    KPTView *m_mainview;

};
 #endif
