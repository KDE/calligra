/***************************************************************************
                          kexiview.h  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch
			   (C) 2002 by Joseph Wenninger <jowenn@kde.org>
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXI_H
#define KEXI_H

#include <kdockwidget.h>
#include <kurl.h>
#include <koView.h>

/**
  *@author lucijan busch
  */

class QWidget;

class KAction;
class KToggleAction;
class KRecentFilesAction;

class KoContextHelpDocker;

class KexiWorkspace;
class KexiTabBrowser;
class KexiDoc;
class KexiFormBase;
class KexiProject;
class DCOPObject;

class KexiView : public KoView  {
   Q_OBJECT

public:
	enum KexiWindowMode {MultipleWindowMode=0,SingleWindowMode=1,MDIWindowMode=2};

	KexiView(KexiWindowMode winmode, KexiProject* part,QWidget *parent=0, const char *name=0);
	~KexiView();
    virtual DCOPObject* dcopObject();

	void			solveDeps();

	KexiWorkspace* workspace() { return static_cast<KexiWorkspace*>(m_workspace->qt_cast("KexiWorkspace")); };
	QWidget* workspaceWidget()const { return m_workspace; };
	KexiTabBrowser* browser()const { return m_browser; };
	KexiWindowMode windowMode()const { return m_windowMode;};

	KoContextHelpDocker	*help() { return m_help; };

	KexiProject *project()const ;

    virtual void updateReadWrite( bool readwrite );

    //printing
    virtual void setupPrinter(KPrinter &printer);
    virtual void print(KPrinter &printer);

    void addQDockWindow(QDockWindow *w);
    void removeQDockWindow(QDockWindow * w);
//public slots:
//    void slotShowRelations();
protected:
        virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

	//various init-functions
	void			initActions();
	void			initMainDock();
	void			initDocBrowser();
	void			initHelper();

	QWidget			*m_mainContent;
	KDockWidget		*m_mainDock;
	QWidget			*m_workspace;
	KexiTabBrowser	  	*m_browser;
	KoContextHelpDocker	*m_help;
	KexiWindowMode		m_windowMode;

	KexiDoc			*m_doc;


	KAction			*m_actionRelations;

	QPtrList<KAction>	*m_formActionList;
	KexiFormBase		*m_lastForm;

	bool			m_docLoaded;

protected slots:
	void			slotActiveWindowChanged(QWidget *w);

	void			finalizeInit();

	void			slotShowProjectProps();

	void			slotSettings();

	void			slotProjectModified();
	void			slotDBAvaible();

private:
	KexiProject *m_project;
	DCOPObject *dcop;
	QPtrList<QDockWindow> m_dockWins;
};

#endif
