/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#include "kexidialogbase.h"
#include "kexiview.h"
#include "kexiworkspaceSDI.h"

KexiWorkspaceSDI::KexiWorkspaceSDI(QWidget *parent, const char *name, KexiView *v)
 : QWidgetStack(parent, name), KexiWorkspace()
{
}

void
KexiWorkspaceSDI::addItem(KexiDialogBase *newItem)
{
	addWidget(newItem);
}

KexiDialogBase*
KexiWorkspaceSDI::activeDocumentView()
{
	return static_cast<KexiDialogBase *>(visibleWidget());
}

void
KexiWorkspaceSDI::activateView(KexiDialogBase *kdb)
{
	raiseWidget(kdb);
}

void
KexiWorkspaceSDI::takeItem(KexiDialogBase *delItem)
{
	removeWidget(delItem);
}

void
KexiWorkspaceSDI::slotWindowActivated(QWidget*)
{
}


KexiWorkspaceSDI::~KexiWorkspaceSDI()
{
}

#include "kexiworkspaceSDI.moc"
