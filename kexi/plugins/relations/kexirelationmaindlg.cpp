/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <qlayout.h>

#include <keximainwindow.h>
#include <kexiproject.h>
#include <kexidb/connection.h>
#include "kexirelationview.h"
#include "kexirelationmaindlg.h"

KexiRelationMainDlg::KexiRelationMainDlg(KexiMainWindow *win)
 : KexiDialogBase(win, i18n("Relations"))
{
	kdDebug() << "KexiRelationMainDlg()" << endl;
	m_view = new KexiRelationView(this);
	m_view->addTable(win->project()->dbConnection()->tableSchema("cars"));
	m_view->addTable(win->project()->dbConnection()->tableSchema("persons"));

	QVBoxLayout *g = new QVBoxLayout(this);
	g->addWidget(m_view);

	setIcon(SmallIcon("relation")); 
	setDocID(0xdeadbeef);
	registerDialog();
}

QWidget*
KexiRelationMainDlg::mainWidget()
{
	return m_view;
}

KexiRelationMainDlg::~KexiRelationMainDlg()
{
}

#include "kexirelationmaindlg.moc"

