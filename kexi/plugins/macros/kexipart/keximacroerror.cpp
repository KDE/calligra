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

#include <qtimer.h>

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroError::Private
{
	public:
		KoMacro::Context::Ptr context;

		Private(KoMacro::Context* const c)
			: context(c)
		{
		}
};

KexiMacroError::KexiMacroError(QWidget* parent, KoMacro::Context::Ptr context)
	: KexiMacroErrorBase(parent, "KexiMacroError" , /*WFlags*/ Qt::WDestructiveClose)
	, d(new Private(context))
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
		QListViewItem* qlistviewitem = new QListViewItem (errorlist);
		qlistviewitem->setText(0,QString("%1").arg(i++));
		qlistviewitem->setText(1,"Action");
		KoMacro::MacroItem::Ptr macroitem = *mit;

		if (macroitem != 0 && macroitem->action() != 0)
		{
			qlistviewitem->setText(2,macroitem->action()->name());
		}

		if(macroitem == context->macroItem())
		{
			qlistviewitem->setOpen(true);
			qlistviewitem->setSelected(true);
			errorlist->ensureItemVisible(qlistviewitem);
		}
		
		KoMacro::Variable::Map variables = macroitem->variables();
		KoMacro::Variable::Map::ConstIterator vit;
		for ( vit = variables.begin(); vit != variables.end(); ++vit ) {
			QListViewItem* child = new QListViewItem (qlistviewitem);
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
	//TODO
}

void KexiMacroError::continuebtnClicked()
{	
	QTimer::singleShot(200, d->context, SLOT(activateNext()));
	close();
}
