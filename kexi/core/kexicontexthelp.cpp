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
#include <kdebug.h>
#include <kiconloader.h>

KexiContextHelp::KexiContextHelp(KexiMainWindow *view, QWidget *parent)
	:KoContextHelpWidget(parent,"kexi_contexthelp")
{
	kdDebug()<<"KexiContextHelp::KexiContextHelp()"<<endl;
	setCaption(i18n("Context Help"));
	setIcon(SmallIcon("help"));
	connect(this,SIGNAL(linkClicked( const QString& )),
		this,SLOT(linkClickedInternal( const QString& )));
}

void KexiContextHelp::linkClickedInternal(const QString& link) {
	kdDebug()<<"KexiContextHelp: Link: "<<link<<endl;
	unhandledLink(link);
}

KexiContextHelp::~KexiContextHelp()
{
}

#include "kexicontexthelp.moc"
