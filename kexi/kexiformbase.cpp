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
#include <qcolor.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <kurlrequester.h>

#include <qobjectlist.h>

#include <qlayout.h>

#include "kexiformbase.h"
#include "keximainwindow.h"
#include "formeditor/widgetcontainer.h"
#include "formeditor/container_frame.h"

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

		        m_urlreq = new KAction(i18n("URL Request"), "button",
		                Key_F7, actionCollection(), "widget_url_requester");

		        m_frame = new KAction(i18n("Frame"), "lineedit",
		                Key_F8, actionCollection(), "widget_frame");
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
		}
		void deactivate(QObject* o)
		{
			m_lineedit->disconnect(o);
			m_button->disconnect(o);
			m_urlreq->disconnect(o);
			m_frame->disconnect(o);
		}
	private:
	KToggleAction *m_formMode;

	KAction *m_lineedit;
	KAction *m_button;
	KAction *m_urlreq;
	KAction *m_frame;
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


KexiFormBase::KexiFormBase(QWidget *parent, const char *name, QString identifier)
	: KexiDialogBase(parent,name)
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
	topLevelEditor->addInteractive(new QPushButton("push button",topLevelEditor));
}

void KexiFormBase::slotWidgetFrame()
{
	topLevelEditor->addInteractive(new KFormEditor::container_Frame(topLevelEditor,"frame"));
}

void KexiFormBase::slotWidgetURLRequester()
{
	topLevelEditor->addInteractive(new KURLRequester("urlrequest",topLevelEditor));
}

KexiFormBase::~KexiFormBase(){
}

#include "kexiformbase.moc"
