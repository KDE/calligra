/* This file is part of the KDE project
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include "kexicontexthelp.h"
#include <kocontexthelp.h>
#include <kxmlguiclient.h>
#include <klocale.h>
#include <qlayout.h>

KexiContextHelp::KexiContextHelp(KexiView *view,QWidget *parent, const char *name)
	: KexiDialogBase(view,parent,name) {

	setCaption(i18n("Context Help"));
	( new QVBoxLayout(this))->setAutoAdd(true);
	m_widget=new KoContextHelpWidget(this);
	m_guiClient= new KXMLGUIClient();
        registerAs(KexiDialogBase::ToolWindow);
}

void KexiContextHelp::setContextHelp( const QString& title, const QString& text, const QPixmap* icon = 0 )
{
	m_widget->setContextHelp(title,text,icon);
}


KXMLGUIClient *KexiContextHelp::guiClient() {
	return m_guiClient;

}


KexiContextHelp::~KexiContextHelp() {
}

#include "kexicontexthelp.moc"
