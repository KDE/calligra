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

class QListViewItem;
class QPopupMenu;
class QHBoxLayout;
class QTabWidget;
class KListView;

class KPTGanttView;
class KPTPertView;
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

    KPTPart *getPart();
    
    QPopupMenu *popupMenu( const QString& name );
	void updateViews();

public slots:
    
protected slots:
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewGantt();
    void slotViewPert();
    void slotViewResources();
    void slotAddSubProject();
    void slotAddTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotProjectCalculate();
    void slotConfigure();
    
    void slotOpenNode();     
    void slotOpen(QListViewItem *item);
    
    void slotConnectNode();
	void slotChanged(QWidget *);
	void slotChanged();
	void slotUpdate(bool calculate);
    
#ifndef NDEBUG
    void slotPrintDebug();
#endif

protected:
    virtual void updateReadWrite(bool readwrite);

private:
    KPTGanttView *m_ganttview;
    QHBoxLayout *m_ganttlayout;
    KPTPertView *m_pertview;
    QHBoxLayout *m_pertlayout;
	QTabWidget *m_tab;
    
    int m_viewGrp;
    int m_defaultFontSize;
    
    KAction *actionEditCut;
    KAction *actionEditCopy;
    KAction *actionEditPaste;

};

#endif
