/* This file is part of the KDE project
   Copyright (C) 2004   Lucijan Busch <lucijan@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kexiproject.h>
#include <kexidb/connection.h>
#include <keximainwindow.h>

#include "kexiquerydocument.h"
#include "kexiqueryview.h"

KexiQueryView::KexiQueryView(KexiMainWindow *win, QWidget *parent, KexiQueryDocument *doc, const char *name)
 : KexiDataTable(win, parent, name)
{
	m_doc = doc;
	afterSwitchFrom(0);
}

bool
KexiQueryView::afterSwitchFrom(int)
{
	KexiDB::Cursor *rec = mainWin()->project()->dbConnection()->executeQuery(*m_doc->schema());
	setData(rec);
	return true;
}

KexiQueryView::~KexiQueryView()
{
}

#include "kexiqueryview.moc"

