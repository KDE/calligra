/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
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

#include "navigateaction.h"
//#include "objectvariable.h"

#include "../lib/macroitem.h"
#include "../lib/context.h"

#include "objectnamevariable.h"

#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>
#include <core/kexipart.h>
#include <core/keximainwindow.h>
#include <core/kexidialogbase.h>

#include <widget/kexidataawareview.h>
#include <widget/tableview/kexidataawareobjectiface.h>

#include <klocale.h>
#include <kdebug.h>

using namespace KexiMacro;

namespace KexiMacro {

	template<class ACTIONIMPL>
	class NavigateVariable : public KexiVariable<ACTIONIMPL>
	{
		public:
			NavigateVariable(ACTIONIMPL* actionimpl)
				: KexiVariable<ACTIONIMPL>(actionimpl, "record", i18n("Record"))
			{
				this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "first" )) );
				this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "previous" )) );
				this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "next" )) );
				this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "last" )) );
				this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "goto" )) );

				//this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "add" )) );
				//this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "save" )) );
				//this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "delete" )) );
				//this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "query" )) );
				//this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "execute" )) );
				//this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "cancel" )) );
				//this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable( "reload" )) );

				this->setVariant( this->children()[0]->variant() );
			}
	};

}

NavigateAction::NavigateAction()
	: KexiAction("navigate", i18n("Navigate"))
{
	KoMacro::Variable* navvar = new NavigateVariable<NavigateAction>(this);
	setVariable(KoMacro::Variable::Ptr( navvar ));

	/*TODO
	- We need to determinate on what object the navigate-action should be applied;
		- Form/Table/invoker/ActualSelectedForm/etc.
		- KexiDataAwareObjectInterface provides us navigation-functionality.

	- We need to be able to add/remove variables during runtime.
	*/
}

NavigateAction::~NavigateAction() 
{
}

KoMacro::Variable::List NavigateAction::notifyUpdated(const QString& variablename, KoMacro::Variable::Map variablemap)
{
	Q_UNUSED(variablename);
	Q_UNUSED(variablemap);
	//kdDebug()<<"OpenObject::NavigateAction() name="<<variable->name()<<" value="<< variable->variant().toString() <<endl;
	KoMacro::Variable::List list;

	if(variablename == "record") {
		const QString record = variablemap.contains("record") ? variablemap["record"]->variant().toString() : QString::null;
		if(record == "goto") {
			KoMacro::Variable* rowvar = new KexiVariable<NavigateAction>(this, "row", i18n("Row"));
			rowvar->setVariant(0);
			list.append( KoMacro::Variable::Ptr(rowvar) );
setVariable(KoMacro::Variable::Ptr( rowvar ));

			KoMacro::Variable* colvar = new KexiVariable<NavigateAction>(this, "col", i18n("Column"));
			colvar->setVariant(-1);
			list.append( KoMacro::Variable::Ptr(colvar) );
setVariable(KoMacro::Variable::Ptr( colvar ));
		}
	}

	return list;
}

void NavigateAction::activate(KoMacro::Context::Ptr context)
{
	KexiDialogBase* dialog = dynamic_cast<KexiDialogBase*>( mainWin()->activeWindow() );
	if(! dialog) {
		kdWarning() << "NavigateAction::activate() No window active." << endl;
		return;
	}

	KexiViewBase* view = dialog->selectedView();
	if(! view) {
		kdWarning() << "NavigateAction::activate() No view selected." << endl;
		return;
	}

	KexiDataAwareView* dbview = dynamic_cast<KexiDataAwareView*>( view );
	if(! dbview) {
		kdWarning() << "NavigateAction::activate() View is not dataaware." << endl;
		return;
	}

	KexiDataAwareObjectInterface* dbobj = dbview->dataAwareObject();
	if(! dbobj) {
		kdWarning() << "NavigateAction::activate() Dataaware view has no dbobject." << endl;
		return;
	}

	const QString record = context->variable("record")->variant().toString();
	if(record == "previous") {
		dbobj->selectPrevRow();
	}
	else if(record == "next") {
		dbobj->selectNextRow();
	}
	else if(record == "first") {
		dbobj->selectFirstRow();
	}
	else if(record == "last") {
		dbobj->selectLastRow();
	}
	else {
		/*
		virtual void selectNextPage(); //!< page down action
		virtual void selectPrevPage(); //!< page up action
		
		void deleteAllRows();
		void deleteCurrentRow();
		void deleteAndStartEditCurrentCell();
		void startEditOrToggleValue();
		bool acceptRowEdit();
		void cancelRowEdit();
		void sortAscending();
		void sortDescending();
		*/
		kdWarning() << QString("NavigateAction::activate() Unknown record \"%1\".").arg(record) << endl;
	}
}

#include "navigateaction.moc"
