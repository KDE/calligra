/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXI_H
#define KEXI_H

#include <kdockwidget.h>
#include <kurl.h>
#include <koView.h>
#include <qmap.h>

/**
  *@author lucijan busch
  */

class QWidget;

class KAction;
class KToggleAction;
class KRecentFilesAction;

class KexiContextHelp;
class KexiDialogBase;

class KexiWorkspace;
class KexiTabBrowser;
class KexiDoc;
class KexiFormBase;
class KexiProject;
class DCOPObject;

typedef QMap<QString, KexiDialogBase *> Windows;

class KEXICORE_EXPORT KexiView : public KoView
{
   Q_OBJECT

public:
	enum KexiWindowMode {MultipleWindowMode=0,SingleWindowMode=1,MDIWindowMode=2,EmbeddedMode=3};

	KexiView(KexiWindowMode winmode, KexiProject* part,QWidget *parent=0, const char *name=0);
	~KexiView();
    virtual DCOPObject* dcopObject();

	void			solveDeps();

	KexiWorkspace* workspace() { return static_cast<KexiWorkspace*>(m_workspace->qt_cast("KexiWorkspace")); };
	QWidget* workspaceWidget()const { return m_workspace; };
	KexiTabBrowser* browser()const { return m_browser; };
	KexiWindowMode windowMode()const { return m_windowMode;};

	KexiContextHelp	*help()const { return m_help; }

	KexiProject *project()const ;

    virtual void updateReadWrite( bool readwrite );

#ifndef KEXI_NO_PRINT
    //printing
    virtual void setupPrinter(KPrinter &printer);
    virtual void print(KPrinter &printer);
#endif

    void addQDockWindow(QDockWindow *w);
    void removeQDockWindow(QDockWindow * w);

    bool activateWindow(const QString &id);
    void registerDialog(KexiDialogBase *dlg, const QString &identifier);
    void removeDialog(const QString &identifier);
//public slots:
//    void slotShowRelations();
protected:
        virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

	//various init-functions
	void			initActions();
	void			initMainDock();
	void			initDocBrowser();

	QWidget			*m_mainContent;
	KDockWidget		*m_mainDock;
	QWidget			*m_workspace;
	KexiTabBrowser	  	*m_browser;
	KexiContextHelp		*m_help;
	KexiWindowMode		m_windowMode;

	KexiDoc			*m_doc;


	KAction			*m_actionRelations;
	KToggleAction		*m_actionHelper;

	QPtrList<KAction>	*m_formActionList;
	KexiFormBase		*m_lastForm;

	bool			m_docLoaded;

protected slots:
	void			slotActiveWindowChanged(QWidget *w);

	void			finalizeInit();
	void			initHelper(bool h=true);

	void			slotShowProjectProps();
	void			slotShowSettings();


	void			slotSettings();

	void			slotProjectModified();
	void			slotDBAvaible();

private:
	KexiProject *m_project;
	DCOPObject *dcop;
	QPtrList<QDockWindow> m_dockWins;
	Windows		m_wins;
};

#endif
