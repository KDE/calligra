/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiformmanager.h"
#include "kexidbform.h"
#include "kexiformscrollview.h"
#include "kexiformview.h"

#include <formmanager.h>
#include <form.h>
#include <widgetlibrary.h>

KexiFormManager::KexiFormManager(KexiPart::Part *parent, const QStringList& supportedFactoryGroups,
	const char* name)
 : KFormDesigner::FormManager(parent, supportedFactoryGroups, 
	KFormDesigner::FormManager::HideEventsInPopupMenu
	| KFormDesigner::FormManager::SkipFileActions, name)
 , m_part(parent)
{
	lib()->setAdvancedPropertiesVisible(false);
}

KexiFormManager::~KexiFormManager()
{
}

KAction* KexiFormManager::action( const char* name )
{
	KActionCollection *col = m_part->actionCollectionForMode(Kexi::DesignViewMode);
	if (!col)
		return 0;
	QCString n( translateName( name ).latin1() );
	KAction *a = col->action(n);
	if (a)
		return a;
	KexiDBForm *dbform;
	if (!activeForm() || !activeForm()->designMode()
		|| !(dbform = dynamic_cast<KexiDBForm*>(activeForm()->formWidget())))
		return 0;
	KexiFormScrollView *scrollViewWidget = dynamic_cast<KexiFormScrollView*>(dbform->dataAwareObject());
	if (!scrollViewWidget)
		return 0;
	KexiFormView* formViewWidget = dynamic_cast<KexiFormView*>(scrollViewWidget->parent());
	if (!formViewWidget)
		return 0;
	return formViewWidget->parentDialog()->mainWin()->actionCollection()->action(n);
}

void KexiFormManager::enableAction( const char* name, bool enable )
{
	KexiDBForm *dbform;
	if (!activeForm() || !activeForm()->designMode()
		|| !(dbform = dynamic_cast<KexiDBForm*>(activeForm()->formWidget())))
		return;
	KexiFormScrollView *scrollViewWidget = dynamic_cast<KexiFormScrollView*>(dbform->dataAwareObject());
	if (!scrollViewWidget)
		return;
	KexiFormView* formViewWidget = dynamic_cast<KexiFormView*>(scrollViewWidget->parent());
	if (!formViewWidget)
		return;
	if (QString(name)=="layout_menu")
		kdDebug() << "!!!!!!!!!!! " << enable << endl;
	formViewWidget->setAvailable(translateName( name ).latin1(), enable);
}

