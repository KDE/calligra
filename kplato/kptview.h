/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#ifndef KPLATO_VIEW
#define KPLATO_VIEW

#include <koView.h>


class KListView;

class KPTPart;
class KPTNode;
class KPTNodeItem;


class KPTView : public KoView {
    Q_OBJECT

public:
    KPTView(KPTPart* part, QWidget* parent=0, const char* name=0);

    /**
     * Support zooming.
     */
    virtual void setZoom(double zoom);

protected slots:
    void slotEditProject();
    void slotAddSubProject();
    void slotAddTask();
    void slotAddMilestone();
    void slotSelectionChanged();

protected:
    virtual void updateReadWrite(bool readwrite);

private:
    void displayProject();
    void displayChildren(const KPTNode &node, KPTNodeItem *item);

    KListView *m_listview;

    int m_defaultFontSize;
};

#endif
