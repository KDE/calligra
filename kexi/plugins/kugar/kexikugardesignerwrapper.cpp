/***************************************************************************
                          kexikugardesignerwrapper.cpp  -  description
                             -------------------
    begin                : Mon Nov 22 2002
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@jowenn.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "kexikugardesignerwrapper.h"
#include "kexikugarhandleritem.h"
#include "kexikugarhandler.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kparts/event.h>
#include <kdebug.h>
#include <koDocument.h>
#include <koView.h>
#include <qfile.h>
#include <qlayout.h>
#include <qapplication.h>
#include <kexiview.h>

KexiKugarDesignerWrapper::KexiKugarDesignerWrapper(KexiView *view, QWidget *parent, const char *name, KexiKugarHandlerItem *item,bool newrep)
	: KexiDialogBase(view,parent,name),m_view(0),m_item(item),m_initFailed(false)
{
	setCaption(i18n("Edit Report %1").arg(m_item->identifier()));

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));

	(new QVBoxLayout(this))->setAutoAdd(true);
//	m_part->openURL("/usr/src/kde3/koffice/kugar/samples/sample1.kud");
//	part->widget()->show();
	setMinimumWidth(50);
	setMinimumHeight(50);
	registerAs(DocumentWindow);
}

KexiKugarDesignerWrapper::~KexiKugarDesignerWrapper(){}


bool KexiKugarDesignerWrapper::initFailed() {
	return m_initFailed;
}

void KexiKugarDesignerWrapper::getPath(QString &path) {
	KexiKugarHandler *kkh=dynamic_cast<KexiKugarHandler*>(m_item->projectPart());
	QString tempPath=kkh->tempPath();
	if (!tempPath.isEmpty()) tempPath=tempPath+m_item->shortIdentifier()+"/";
	path=tempPath;
}

KXMLGUIClient *KexiKugarDesignerWrapper::guiClient()
{
	kdDebug()<<"KexiKugarDesignerWrapper::guiClient()"<<endl;
	if (kexiView()->shell()) kdDebug()<<"Shell window exists"<<kexiView()->shell()<<endl;
	else
	kdDebug()<<"Shell window does NOT exist"<<endl;
	if (!m_view) {
		 kdDebug()<<"Creating KuDesigner VIEW *******"<<endl;
		 m_view=m_item->designer()->createView(this,"");
		 m_view->show();
		 KParts::GUIActivateEvent ev(true);
		 QApplication::sendEvent(m_item->designer(),&ev);
		 QApplication::sendEvent(m_view,&ev);
	}
	//if (m_view==0) kdDebug()<<"That shouldn't happen"<<endl;
	return m_view;
//	return new KXMLGUIClient();
//	return 0;
}

void KexiKugarDesignerWrapper::activateActions()
{
}

void KexiKugarDesignerWrapper::deactivateActions()
{
}

#include "kexikugardesignerwrapper.moc"
