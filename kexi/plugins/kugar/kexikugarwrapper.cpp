/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "kexikugarwrapper.h"
#include "kexikugarhandleritem.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kdebug.h>

#include <qfile.h>
#include <qlayout.h>
#include <qobjectlist.h>

KexiKugarWrapper::KexiKugarWrapper(KexiView *view, KexiKugarHandlerItem* item,const QString& path,
                                QWidget *parent, const char *name)
	: KexiDialogBase(view,parent,name),m_part(0)
{
	setCaption(i18n("Report %1").arg(item->name()));

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));

	(new QVBoxLayout(this))->setAutoAdd(true);
	m_part=KParts::ComponentFactory::createPartInstanceFromLibrary<KParts::ReadOnlyPart>(QFile::encodeName("libkugarpart"),
			this,"embeddedKugarView",this,0,QStringList("template=/usr/src/kde3/koffice/kugar/samples/sample2.kut"));
	
	m_part->openURL(path);
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

void KexiKugarWrapper::setupPrinter(KPrinter &printer) {

	QObject *c=0;
	QObjectList *l=queryList("KoView");
	QObjectListIt it(*l);
	if (it.current()!=0) {
		c=it.current();
	}
	delete l;
	if (!c) return;
	(static_cast<KoView*>(c->qt_cast("KoView")))->setupPrinter(printer);
}

void KexiKugarWrapper::print(KPrinter &printer) {
	QObject *c=0;
	QObjectList *l=queryList("KoView");
	QObjectListIt it(*l);
	if (it.current()!=0) {
		c=it.current();
	}
	delete l;
	if (!c) return;
	(static_cast<KoView*>(c->qt_cast("KoView")))->print(printer);

} // if (m_part!=0) m_part->print(printer); }


#include "kexikugarwrapper.moc"
