/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Tobi Krebs (tobi.krebs@gmail.com)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "keximacroerror.h"

#include <core/kexiproject.h>
#include <core/keximainwindow.h>

#include <qtimer.h>

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroError::Private
{
	public:
		KexiMainWindow* const mainwin;
		KoMacro::Context::Ptr context;

		Private(KexiMainWindow* const m, KoMacro::Context* const c)
			: mainwin(m)
			, context(c)
		{
		}
};

KexiMacroError::KexiMacroError(KexiMainWindow* mainwin, KoMacro::Context::Ptr context)
	: KexiMacroErrorBase(mainwin, "KexiMacroError" , /*WFlags*/ Qt::WDestructiveClose)
	, d(new Private(mainwin, context))
{
	//setText(i18n("Execution failed")); //caption
	//errortext, errorlist, continuebtn,cancelbtn, designerbtn
	
	KoMacro::Exception* exception = context->exception();

	iconlbl->setPixmap(KGlobal::instance()->iconLoader()->loadIcon("messagebox_critical", KIcon::Small, 32));
	errorlbl->setText(i18n("<qt>Failed to execute the macro \"%1\".<br>%2</qt>").arg( context->macro()->name() ).arg( exception->errorMessage() ));

	int i = 1;
	KoMacro::MacroItem::List items = context->macro()->items();
	for (KoMacro::MacroItem::List::ConstIterator mit = items.begin(); mit != items.end(); mit++)
	{
		KListViewItem* listviewitem = new KListViewItem(errorlist);
		listviewitem->setText(0,QString("%1").arg(i++));
		listviewitem->setText(1,i18n("Action"));
		KoMacro::MacroItem::Ptr macroitem = *mit;

		if (macroitem != 0 && macroitem->action() != 0)
		{
			listviewitem->setText(2,macroitem->action()->name());
		}

		if(macroitem == context->macroItem())
		{
			listviewitem->setOpen(true);
			listviewitem->setSelected(true);
			errorlist->setSelected(listviewitem, true);
			errorlist->ensureItemVisible(listviewitem);
		}
		
		KoMacro::Variable::Map variables = macroitem->variables();
		KoMacro::Variable::Map::ConstIterator vit;
		for ( vit = variables.begin(); vit != variables.end(); ++vit ) {
			KListViewItem* child = new KListViewItem(listviewitem);
			child->setText(1,vit.key());
			child->setText(2,vit.data()->toString());
		}
	}
	
	connect(designerbtn, SIGNAL(clicked()), this, SLOT(designbtnClicked()));
	connect(continuebtn, SIGNAL(clicked()), this, SLOT(continuebtnClicked()));
}

KexiMacroError::~KexiMacroError()
{
	delete d;
}

void KexiMacroError::designbtnClicked()
{
	if(! d->mainwin->project()) {
		kdWarning() << QString("KexiMacroError::designbtnClicked(): No project open.") << endl;
		return;
	}

	// We need to determinate the KexiPart::Item which should be opened.
	KoMacro::Macro::Ptr macro = d->context->macro();
	const QString name = macro->name();
	KexiPart::Item* item = d->mainwin->project()->itemForMimeType("kexi/macro", name);
	if(! item) {
		kdWarning() << QString("KexiMacroError::designbtnClicked(): No such macro \"%1\"").arg(name) << endl;
		return;
	}

	// Try to open the KexiPart::Item now.
	bool openingCancelled;
	if(! d->mainwin->openObject(item, Kexi::DesignViewMode, openingCancelled)) {
		if(! openingCancelled) {
			kdWarning() << QString("KexiMacroError::designbtnClicked(): Open macro \"%1\" in designview failed.").arg(name) << endl;
			return;
		}
	}

	close();
}

void KexiMacroError::continuebtnClicked()
{	
	QTimer::singleShot(200, d->context, SLOT(activateNext()));
	close();
}
