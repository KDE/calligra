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
class QWidgetStack;

class KListView;
class KPrinter;
class KSelectAction;

class KPTGanttView;
class KPTPertView;
class KPTResourceView;
class KPTReportView;
class KPTPart;
class KPTNode;
class KPTProject;

class DCOPObject;

class KPTView : public KoView {
    Q_OBJECT

public:
    KPTView(KPTPart* part, QWidget* parent=0, const char* name=0);
    ~KPTView();
    /**
     * Support zooming.
     */
    virtual void setZoom(double zoom);

    KPTPart *getPart()const;

	KPTProject& getProject() const;

    virtual void setupPrinter(KPrinter &printer);
    virtual void print(KPrinter &printer);

    QPopupMenu *popupMenu(const QString& name);
    void setReportGenerateMenu();

    void projectCalculate();

    virtual DCOPObject* dcopObject();
    
public slots:
    void slotUpdate(bool calculate);
    void slotEditResource();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewGantt();
    void slotViewPert();
    void slotViewResources();
    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotConfigure();

protected slots:
    void slotProjectCalendar();
    void slotProjectCalculate();
    void slotReportDesign();
    void slotReportGenerate(int);

    void slotOpenNode();
	void slotDeleteTask();
	void slotIndentTask();
	void slotUnindentTask();
	void slotMoveTaskUp();
	void slotMoveTaskDown();

    void slotConnectNode();
	void slotChanged(QWidget *);
	void slotChanged();

    void slotAboutToShow(QWidget *widget);

#ifndef NDEBUG
    void slotPrintDebug();
    void slotPrintCalendarDebug();
#endif

protected:
    virtual void updateReadWrite(bool readwrite);
	KPTNode *currentTask();

private:
    KPTGanttView *m_ganttview;
    QHBoxLayout *m_ganttlayout;
    KPTPertView *m_pertview;
    QHBoxLayout *m_pertlayout;
	QWidgetStack *m_tab;
	KPTResourceView *m_resourceview;
	KPTReportView *m_reportview;
    QPtrList<QString> m_reportTemplateFiles;

    int m_viewGrp;
    int m_defaultFontSize;

    KAction *actionEditCut;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KSelectAction *actionReportGenerate;
    DCOPObject* m_dcop;

};

#endif
