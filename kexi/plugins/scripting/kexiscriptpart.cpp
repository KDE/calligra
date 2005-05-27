/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

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

#include "kexiscriptdesignview.h"
#include "kexiscripttextview.h"
#include "kexiscriptmanager.h"

KexiScriptPart::KexiScriptPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
    m_names["instance"] = i18n("Script");
    m_supportedViewModes = Kexi::DesignViewMode | Kexi::TextViewMode;

    m_manager = new KexiScriptManager(this);
}

KexiScriptPart::~KexiScriptPart()
{
}

void KexiScriptPart::initPartActions()
{
}

void KexiScriptPart::initInstanceActions()
{
#ifdef KEXI_KROSS_SUPPORT
    createSharedAction(Kexi::DesignViewMode, i18n("Execute script"), "exec", 0, "script_execute");
#endif
}

KexiViewBase* KexiScriptPart::createView(QWidget *parent, KexiDialogBase* dialog, KexiPart::Item &item, int viewMode)
{
    if(viewMode == Kexi::DesignViewMode) {
        KexiMainWindow *win = dialog->mainWin();
        if(!win || !win->project() || !win->project()->dbConnection())
            return 0;
        return new KexiScriptDesignView(m_manager, win, parent, item.name().latin1());
    }

    if(viewMode == Kexi::TextViewMode) {
        KexiMainWindow *win = dialog->mainWin();
        if(!win || !win->project() || !win->project()->dbConnection())
            return 0;
        return new KexiScriptTextView(m_manager, win, parent, item.name().latin1());
    }

    return 0;
}

QString KexiScriptPart::i18nMessage(const QCString& englishMessage) const
{
	if (englishMessage=="Design of object \"%1\" has been modified.")
		return i18n("Design of script \"%1\" has been modified.");
	if (englishMessage=="Object \"%1\" already exists.")
		return i18n("Script \"%1\" already exists.");
	return englishMessage;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_script, KGenericFactory<KexiScriptPart>("kexihandler_script") )

#include "kexiscriptpart.moc"
