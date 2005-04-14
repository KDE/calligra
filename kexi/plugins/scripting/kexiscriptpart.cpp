/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
#include "kexiscriptpart.h"

#include <kdebug.h>
#include <kgenericfactory.h>

#include "kexiviewbase.h"
#include "keximainwindow.h"
#include "kexiproject.h"
#include <kexipartitem.h>
#include <kexidialogbase.h>

#include "kexiscripteditor.h"

KexiScriptPart::KexiScriptPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	m_names["instance"] = i18n("Script");
	m_supportedViewModes = Kexi::DesignViewMode;
}

KexiScriptPart::~KexiScriptPart()
{
}

void
KexiScriptPart::initPartActions()
{
}

void
KexiScriptPart::initInstanceActions()
{
}

KexiViewBase*
KexiScriptPart::createView(QWidget *parent, KexiDialogBase* dialog,
	KexiPart::Item &item, int)
{
	KexiMainWindow *win = dialog->mainWin();
	if (!win || !win->project() || !win->project()->dbConnection())
		return 0;

	KexiScriptEditor *view = new KexiScriptEditor(win, parent, item.name().latin1());

	return view;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_script, KGenericFactory<KexiScriptPart>("kexihandler_script") )

#include "kexiscriptpart.moc"

