/* This file is part of the KDE libraries
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


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
#include <qmap.h>
#include <qstatusbar.h>
#include <qtabwidget.h>

#include <qobjectlist.h>

#include <qlayout.h>

#include "kexiformhandleritem.h"
#include "kexiformbase.h"
#include "kexiview.h"
#include "kexiproject.h"
#include "kexitablepart.h"
#include "kexidataprovider.h"
#include "kexirecordnavigator.h"
#include "kexieventhandler.h"
#include "formeditor/widgetcontainer.h"
#include "formeditor/container_frame.h"
#include "formeditor/container_tabwidget.h"
#include "formeditor/propertyeditor.h"
#include "formeditor/eventeditor.h"
#include "formeditor/widgetwatcher.h"

#include "kexidbwidgetcontainer.h"
#include "kexidbwidgets.h"

class KexiFormBase::EditGUIClient: public KXMLGUIClient
{
	public:
		EditGUIClient():KXMLGUIClient()
		{
			m_formMode = new KToggleAction(i18n("Edit Form"),"form_edit",
				0,actionCollection(),"form_edit");

			m_label = new KAction(i18n("Text Label"), "label",
				Key_F4, actionCollection(), "widget_label");
			
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
			connect(m_label,SIGNAL(activated()),o,SLOT(slotWidgetLabel()));
			connect(m_lineedit,SIGNAL(activated()),o,SLOT(slotWidgetLineEdit()));
			connect(m_button,SIGNAL(activated()),o,SLOT(slotWidgetPushButton()));
			connect(m_urlreq,SIGNAL(activated()),o,SLOT(slotWidgetURLRequester()));
			connect(m_frame,SIGNAL(activated()),o,SLOT(slotWidgetFrame()));
			connect(m_tabWidget,SIGNAL(activated()),o,SLOT(slotWidgetTabWidget()));
			connect(m_formMode, SIGNAL(toggled(bool)), o, SLOT(slotToggleFormMode(bool)));
		}
		void deactivate(QObject* o)
		{
			m_label->disconnect(o);
			m_lineedit->disconnect(o);
			m_button->disconnect(o);
			m_urlreq->disconnect(o);
			m_frame->disconnect(o);
			m_tabWidget->disconnect(o);
		}
	private:
	KToggleAction *m_formMode;

	KAction *m_label;
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


KexiFormBase::KexiFormBase(KexiView *view, KexiFormHandlerItem *item, QWidget *parent, const QString &s, const char *name, QString identifier)
	: KexiDialogBase(view,parent,identifier.latin1())
{
	setMinimumWidth(50);
	setMinimumHeight(50);

//	initActions();

	m_source = s;
	m_project = view->project();
	m_item = item;

	setCaption(i18n("%1 [Edit Mode]").arg(identifier));

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));

	resize( 250, 250 );

	QVBoxLayout *l=new QVBoxLayout(this);
	l->setAutoAdd(true);
	topLevelEditor=new KexiDBWidgetContainer(this,"foo","bar");
//	topLevelEditor->setWidgetList(item->widgetList());
	topLevelEditor->setPropertyBuffer(item->propertyBuffer());
	topLevelEditor->setDataSource(s);

	QDockWindow *editorWindow = new QDockWindow(view->mainWindow(), "edoc");
	editorWindow->setCaption(i18n("Properties"));
	editorWindow->setResizeEnabled(true);
	view->mainWindow()->moveDockWindow(editorWindow, DockRight);

	QTabWidget *formProperties = new QTabWidget(editorWindow);
	editorWindow->setWidget(formProperties);
	formProperties->show();

	PropertyEditor *peditor = new PropertyEditor(formProperties);
	connect(topLevelEditor, SIGNAL(activated(QObject *)), peditor, SLOT(setObject(QObject *)));
	connect(topLevelEditor, SIGNAL(widgetInserted(QObject *)), this, SLOT(slotWidgetInserted(QObject *)));

	EventEditor *eeditor = new EventEditor(formProperties);
	connect(topLevelEditor, SIGNAL(activated(QObject *)), eeditor, SLOT(setObject(QObject *)));

	formProperties->insertTab(peditor, i18n("Properties"));
	formProperties->insertTab(eeditor, i18n("Events"));


	KexiProjectHandler *sh = m_project->handlerForMime("kexi/script");
	if(sh)
	{
		KexiEventHandler *ev = sh->eventHandler();
		if(ev)
		{
			eeditor->appendFake(ev->name(), ev->formHandler());
			ev->provideObject(this);
		}
	}
//	peditor->show();
//	eeditor->show();

//	QStatusBar *status = new QStatusBar(this);
//	KexiRecordNavigator *nv = new KexiRecordNavigator(0, this);
//	status->addWidget(nv, 2, true);
//	status->setFixedWidth(20);

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


void KexiFormBase::slotWidgetLabel()
{
	QWidget *w = new KexiDBLabel(topLevelEditor, "label");
	w->setName(m_item->widgetWatcher()->genName("label").latin1());
	topLevelEditor->addInteractive(w);
}

void KexiFormBase::slotWidgetLineEdit()
{
	kdDebug() << "add line edit widget at " << this << endl;
	KexiDBLineEdit *w = new KexiDBLineEdit(topLevelEditor);
	w->setName(m_item->widgetWatcher()->genName("lineedit").latin1());
	topLevelEditor->addInteractive(w);
}

void KexiFormBase::slotWidgetPushButton()
{
	QPushButton *w = new QPushButton(i18n("Push button"), topLevelEditor);
	w->setName(m_item->widgetWatcher()->genName("button").latin1());
	topLevelEditor->addInteractive(w);
}

void KexiFormBase::slotWidgetFrame()
{
	QWidget *w = new KFormEditor::container_Frame(topLevelEditor,"frame");
	w->setName(m_item->widgetWatcher()->genName("frame").latin1());
	topLevelEditor->addInteractive(w);
}

void KexiFormBase::slotWidgetTabWidget()
{
	QWidget *w = new KFormEditor::container_TabWidget(topLevelEditor,"tabwidget");
	w->setName(m_item->widgetWatcher()->genName("tabwidget").latin1());
	topLevelEditor->addInteractive(w);
}

void KexiFormBase::slotWidgetURLRequester()
{
	QWidget *w = new KURLRequester("urlrequest",topLevelEditor);
	w->setName(m_item->widgetWatcher()->genName("urlrequest").latin1());
	topLevelEditor->addInteractive(w);
}

void KexiFormBase::slotToggleFormMode(bool state)
{
	kdDebug() << "KexiFormBase::slotToggleFormMode()" << endl;
	topLevelEditor->setEditMode(state);
	if(!state)
	{
		kdDebug() << "KexiFormBase::slotToggleFormMode() source: " << m_source << endl;
//		KexiTablePart *p = static_cast<KexiTablePart*>(m_project->handlerForMime("kexi/table"));
//		KexiDBRecord *rec = p->records(m_source, QMap<QString,QString>());
	}
}

void KexiFormBase::slotWidgetInserted(QObject *o)
{
	char *n = o->name();
	m_item->widgetWatcher()->insert(n, o);
	KexiProjectHandler *sh = m_project->handlerForMime("kexi/script");
	if(sh)
	{
		KexiEventHandler *ev = sh->eventHandler();
		if(ev)
		{
			ev->provideObject(o);
		}
	}
}

KexiFormBase::~KexiFormBase(){
}

#include "kexiformbase.moc"
