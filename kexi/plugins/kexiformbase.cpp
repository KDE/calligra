/***************************************************************************
                          kexiformbase.cpp  -  description
                             -------------------
    begin                : Mon Jun 17 2002
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


#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klineedit.h>

#include <qptrlist.h>
#include <qsize.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <kurlrequester.h>
#include <qdockwindow.h>

#include <qobjectlist.h>

#include <qlayout.h>

#include "kexiformhandleritem.h"
#include "kexiformbase.h"
#include "kexiview.h"
#include "formeditor/widgetcontainer.h"
#include "formeditor/container_frame.h"
#include "formeditor/container_tabwidget.h"
#include "formeditor/propertyeditor.h"

class KexiFormBase::EditGUIClient: public KXMLGUIClient
{
	public:
		EditGUIClient():KXMLGUIClient()
		{
			m_formMode = new KToggleAction(i18n("Edit Form"),"form_edit",
				0,actionCollection(),"form_edit");

		        m_lineedit = new KAction(i18n("Line Edit"), "lineedit",
                		Key_F5, actionCollection(), "widget_line_edit");

		        m_button = new KAction(i18n("Push Button"), "button",
		                Key_F6,  actionCollection(), "widget_push_button");

		        m_urlreq = new KAction(i18n("URL Request"), "urlrequest",
		                Key_F7, actionCollection(), "widget_url_requester");

		        m_frame = new KAction(i18n("Frame"), "frame",
		                Key_F8, actionCollection(), "widget_frame");

		        m_tabWidget = new KAction(i18n("Tab Widget"), "tabwidget",
		                Key_F9, actionCollection(), "widget_tabwidget");
			setXMLFile("kexiformeditorui.rc");
		}
		virtual ~EditGUIClient(){;}
		void activate(QObject* o)
		{
			m_formMode->setChecked(true);
			connect(m_lineedit,SIGNAL(activated()),o,SLOT(slotWidgetLineEdit()));
			connect(m_button,SIGNAL(activated()),o,SLOT(slotWidgetPushButton()));
			connect(m_urlreq,SIGNAL(activated()),o,SLOT(slotWidgetURLRequester()));
			connect(m_frame,SIGNAL(activated()),o,SLOT(slotWidgetFrame()));
			connect(m_tabWidget,SIGNAL(activated()),o,SLOT(slotWidgetTabWidget()));
		}
		void deactivate(QObject* o)
		{
			m_lineedit->disconnect(o);
			m_button->disconnect(o);
			m_urlreq->disconnect(o);
			m_frame->disconnect(o);
			m_tabWidget->disconnect(o);
		}
	private:
	KToggleAction *m_formMode;

	KAction *m_lineedit;
	KAction *m_button;
	KAction *m_urlreq;
	KAction *m_frame;
	KAction *m_tabWidget;
};

class KexiFormBase::ViewGUIClient: public KXMLGUIClient
{
	public:
		ViewGUIClient():KXMLGUIClient()
		{
			m_formMode = new KToggleAction(i18n("Edit Form"),"form_edit",
				0,actionCollection(),"form_edit");

			setXMLFile("kexiformviewui.rc");
		}
		virtual ~ViewGUIClient(){;}

		void activate(QObject* o)
		{
			m_formMode->setChecked(false);
		}

		void deactivate(QObject* o)
		{
		}
	private:
	KToggleAction *m_formMode;

};



KexiFormBase::EditGUIClient *KexiFormBase::m_editGUIClient=0;
KexiFormBase::ViewGUIClient *KexiFormBase::m_viewGUIClient=0;


KexiFormBase::KexiFormBase(KexiView *view, KexiFormHandlerItem *item, QWidget *parent, const char *name, QString identifier)
	: KexiDialogBase(view,parent,name)
{
	setMinimumWidth(50);
	setMinimumHeight(50);

//	initActions();

	setCaption(i18n("%1 [Edit Mode]").arg(identifier));

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));

	resize( 250, 250 );

	QVBoxLayout *l=new QVBoxLayout(this);
	l->setAutoAdd(true);
	topLevelEditor=new KFormEditor::WidgetContainer(this,"foo","bar");
	topLevelEditor->setWidgetList(item->widgetList());
	topLevelEditor->setPropertyBuffer(item->propertyBuffer());

	QDockWindow *editorWindow = new QDockWindow(view->mainWindow(), "edoc");
	editorWindow->setCaption(i18n("Properties"));
	editorWindow->setResizeEnabled(true);
	view->mainWindow()->moveDockWindow(editorWindow, DockRight);

	PropertyEditor *peditor = new PropertyEditor(editorWindow);
	editorWindow->setWidget(peditor);
	peditor->show();
	connect(topLevelEditor, SIGNAL(activated(QObject *)), peditor, SLOT(setObject(QObject *)));


//	mainWindow()->guiFactory()->addClient(guiClient());
//	activateActions();
	registerAs(DocumentWindow);
}


KXMLGUIClient *KexiFormBase::guiClient()
{
	if (!m_editGUIClient)
		m_editGUIClient=new EditGUIClient();
	return m_editGUIClient;
}

void KexiFormBase::activateActions()
{
	m_editGUIClient->activate(this);
}

void KexiFormBase::deactivateActions()
{
	m_editGUIClient->deactivate(this);
}


void KexiFormBase::slotWidgetLineEdit()
{
	kdDebug() << "add line edit widget at " << this << endl;
	topLevelEditor->addInteractive(new KLineEdit(topLevelEditor));
}

void KexiFormBase::slotWidgetPushButton()
{
	topLevelEditor->addInteractive(new QPushButton(i18n("Push button"),topLevelEditor));
}

void KexiFormBase::slotWidgetFrame()
{
	topLevelEditor->addInteractive(new KFormEditor::container_Frame(topLevelEditor,"frame"));
}

void KexiFormBase::slotWidgetTabWidget()
{
	topLevelEditor->addInteractive(new KFormEditor::container_TabWidget(topLevelEditor,"tabwidget"));
}



void KexiFormBase::slotWidgetURLRequester()
{
	topLevelEditor->addInteractive(new KURLRequester("urlrequest",topLevelEditor));
}

KexiFormBase::~KexiFormBase(){
}

#include "kexiformbase.moc"
