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
				QStringList list;
				list << "first" << "previous" << "next" << "last" << "goto";
				this->children().append( KSharedPtr<KoMacro::Variable>( new KoMacro::Variable(list, "@list") ) );

				/*TODO should this actions belong to navigate? maybe it would be more wise to have
				such kind of functionality in an own e.g. "Modify" action to outline, that
				we are manipulating the database that way... */
				//"add" << "save" << "delete" << "query" << "execute" << "cancel" << "reload"

				this->setVariant( list[0] );
			}
	};

}

NavigateAction::NavigateAction()
	: KexiAction("navigate", i18n("Navigate"))
{
	KoMacro::Variable* navvar = new NavigateVariable<NavigateAction>(this);
	setVariable(KSharedPtr<KoMacro::Variable>( navvar ));

	KoMacro::Variable* rowvar = new KexiVariable<NavigateAction>(this, "rownr", i18n("Row"));
	rowvar->setVariant(0);
	setVariable(KSharedPtr<KoMacro::Variable>(rowvar));

	KoMacro::Variable* colvar = new KexiVariable<NavigateAction>(this, "colnr", i18n("Column"));
	colvar->setVariant(0);
	setVariable(KSharedPtr<KoMacro::Variable>(colvar));
}

NavigateAction::~NavigateAction() 
{
}

#if 0
bool NavigateAction::notifyUpdated(const QString& variablename, KoMacro::MacroItem* macroitem)
{
	Q_UNUSED(variablename);
	Q_UNUSED(variablemap);
	//kdDebug()<<"OpenObject::NavigateAction() name="<<variable->name()<<" value="<< variable->variant().toString() <<endl;
	KoMacro::Variable::List list;

	const QString record = variablemap.contains("record") ? variablemap["record"]->variant().toString() : QString::null;

	if(record == "goto") {
		if(variablename == "record") {
			KoMacro::Variable* rowvar = new KexiVariable<NavigateAction>(this, "rownr", i18n("Row"));
			const int rownr = variablemap.contains("rownr") ? variablemap["rownr"]->variant().toInt() : 0;
			rowvar->setVariant(rownr);
			list.append( KSharedPtr<KoMacro::Variable>(rowvar) );
			setVariable(KSharedPtr<KoMacro::Variable>( rowvar ));

			KoMacro::Variable* colvar = new KexiVariable<NavigateAction>(this, "colnr", i18n("Column"));
			const int colnr = variablemap.contains("colnr") ? variablemap["colnr"]->variant().toInt() : 0;
			colvar->setVariant(colnr);
			list.append( KSharedPtr<KoMacro::Variable>(colvar) );
			setVariable(KSharedPtr<KoMacro::Variable>( colvar ));
		}
		return list;
	}

	removeVariable("rownr");
	removeVariable("colnr");

	return list;
}
#endif

void NavigateAction::activate(KSharedPtr<KoMacro::Context> context)
{
	KexiDialogBase* dialog = dynamic_cast<KexiDialogBase*>( mainWin()->activeWindow() );
	if(! dialog) {
		throw KoMacro::Exception(i18n("No window active."));
	}

	KexiViewBase* view = dialog->selectedView();
	if(! view) {
		throw KoMacro::Exception(i18n("No view selected for \"%1\".").arg(dialog->caption()));
	}

	KexiDataAwareView* dbview = dynamic_cast<KexiDataAwareView*>( view );
	KexiDataAwareObjectInterface* dbobj = dbview ? dbview->dataAwareObject() : 0;
	if(! dbview) {
		throw KoMacro::Exception(i18n("The view for \"%1\" is not dataaware.").arg(dialog->caption()));
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
	else if(record == "goto") {
		int rownr = context->variable("rownr")->variant().toInt() - 1;
		int colnr = context->variable("colnr")->variant().toInt() - 1;
		dbobj->setCursorPosition(rownr >= 0 ? rownr : dbobj->currentRow(), colnr >= 0 ? colnr : dbobj->currentColumn());
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
		throw KoMacro::Exception(i18n("Unknown record \"%1\" in view for \"%2\".").arg(record).arg(dialog->caption()));
	}
}

//#include "navigateaction.moc"
