/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#include "kexicontexthelp.h"
#include <kocontexthelp.h>
#include <kxmlguiclient.h>
#include <kapplication.h>
#include <klocale.h>
#include <kaction.h>
#include <qlayout.h>

KexiContextHelp::KexiContextHelp(KexiView *view, QWidget *parent, const char *name)
: KexiDialogBase(view, "KexiContextHelp", parent, name)
{
	setCustomCaption(i18n("Context Help"));
//js	( new QVBoxLayout(this))->setAutoAdd(true);
	m_widget=new KoContextHelpWidget(this);
	gridLayout()->addWidget( m_widget, 0, 0 );
	connect(m_widget,SIGNAL(linkClicked( const QString& )),
		this,SLOT(linkClickedInternal( const QString& )));
	m_guiClient= new KXMLGUIClient();
//js        registerAs(KexiDialogBase::ToolWindow);
//	connect(dock(),SIGNAL(visibilityChanged(bool)), this, SLOT(slotVisibilityChanged(bool)));
}

void KexiContextHelp::setContextHelp( const QString& title, const QString& text, const QPixmap* icon )
{
	m_widget->setContextHelp(title,text,icon);
}

void KexiContextHelp::linkClickedInternal(const QString& link) {

}

KXMLGUIClient *KexiContextHelp::guiClient() {
	return m_guiClient;
}


KexiContextHelp::~KexiContextHelp()
{
}

#include "kexicontexthelp.moc"
