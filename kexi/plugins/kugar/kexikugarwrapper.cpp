/***************************************************************************
                          kexikugarwrapper.cpp  -  description
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


#include "kexikugarwrapper.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kdebug.h>

#include <qfile.h>
#include <qlayout.h>

KexiKugarWrapper::KexiKugarWrapper(KexiView *view, QWidget *parent, const char *name, QString identifier)
	: KexiDialogBase(view,parent,name),m_part(0)
{
	setCaption(i18n("Report %1").arg(identifier));

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));

	(new QVBoxLayout(this))->setAutoAdd(true);
	m_part=KParts::ComponentFactory::createPartInstanceFromLibrary<KParts::ReadOnlyPart>(QFile::encodeName("libkugarpart"),
			this,0,this,0,QStringList("template=/usr/src/kde3/koffice/kugar/samples/sample2.kut"));
	
	m_part->openURL("/usr/src/kde3/koffice/kugar/samples/sample1.kud");
//	part->widget()->show();
	setMinimumWidth(50);
	setMinimumHeight(50);
	registerAs(DocumentWindow);
}

KexiKugarWrapper::~KexiKugarWrapper(){}


KXMLGUIClient *KexiKugarWrapper::guiClient()
{
	if (m_part==0) kdDebug()<<"That shouldn't happen"<<endl;
	return m_part;
//	return new KXMLGUIClient();
//	return 0;
}

void KexiKugarWrapper::activateActions()
{
}

void KexiKugarWrapper::deactivateActions()
{
}


#include "kexikugarwrapper.moc"
