/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#include "keximainwindow.h"
#include "kexidialogbase.h"
#include "kexicontexthelp_p.h"
#include "kexipart.h"
#include "kexipartinfo.h"

#include <kdebug.h>
#include <kapplication.h>
#include <kiconloader.h>

KexiDialogBase::KexiDialogBase(KexiMainWindow *parent, const QString &caption)
 : KMdiChildView(caption, parent, "KexiDialogBase"),m_isRegistered(false)
 , KexiActionProxy(parent, this)
{
	m_contextHelpInfo=new KexiContextHelpInfo();
	m_parentWindow=parent;
	m_instance=parent->instance();
	m_docID = -1;
}


KInstance *KexiDialogBase::instance() {
	return m_instance;
}

KexiDialogBase::~KexiDialogBase()
{
}

void KexiDialogBase::registerDialog() {
	m_parentWindow->registerChild(this);
	m_isRegistered=true;
	m_parentWindow->addWindow((KMdiChildView *)this);
	show();
	m_parentWindow->activeWindowChanged(this);
}

bool KexiDialogBase::isRegistered(){
	return m_isRegistered;
}

void KexiDialogBase::attachToGUIClient() {
	if (!guiClient())
		return;

}

void KexiDialogBase::detachFromGUIClient() {
	if (!guiClient())
		return;
	//TODO
}


void
KexiDialogBase::setDocID(int id)
{
	kdDebug() << "KexiDialogBase::setDocID(): id = " << id << endl;
	m_docID = id;
//	m_parentWindow->registerChild(this);
}

void KexiDialogBase::setContextHelp(const QString& caption, const QString& text, const QString& iconName) {
	m_contextHelpInfo->caption=caption;
	m_contextHelpInfo->text=text;
	m_contextHelpInfo->text=iconName;
	updateContextHelp();

}

/*
void KexiDialogBase::detach()
{
	KMdiChildView::detach();
	// update icon
	if (m_part) {
//		setIcon( DesktopIcon(m_part->info()->itemIcon()) );
	}
}*/

void KexiDialogBase::closeEvent( QCloseEvent * e )
{
	emit closing();
	KMdiChildView::closeEvent(e);
}

#include "kexidialogbase.moc"

