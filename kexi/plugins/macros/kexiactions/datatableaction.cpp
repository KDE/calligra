/***************************************************************************
 * This file is part of the KDE project
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

#include "datatableaction.h"
//#include "objectvariable.h"

#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>
#include <core/kexipart.h>
#include <core/keximainwindow.h>
#include <core/kexiinternalpart.h>

#include <klocale.h>

using namespace KexiMacro;

namespace KexiMacro {

	//static const QString OBJECT = "method";
	//static const QString OBJECT = "type";
	//static const QString OBJECT = "partitem";

	template<class ACTIONIMPL>
	class MethodVariable : public KexiVariable<ACTIONIMPL>
	{
		public:
			MethodVariable(ACTIONIMPL* actionimpl)
				: KexiVariable<ACTIONIMPL>(actionimpl, "method", i18n("Method"))
			{
				QStringList list;
				list << "import" << "export";
				this->appendChild( KSharedPtr<KoMacro::Variable>( new KoMacro::Variable(list, "@list") ) );

				this->setVariant( list[0] );
			}
	};

	template<class ACTIONIMPL>
	class TypeVariable : public KexiVariable<ACTIONIMPL>
	{
		public:
			TypeVariable(ACTIONIMPL* actionimpl)
				: KexiVariable<ACTIONIMPL>(actionimpl, "type", i18n("Type"))
			{
				QStringList list;
				list << "file" << "clipboard";
				this->appendChild( KSharedPtr<KoMacro::Variable>( new KoMacro::Variable(list, "@list") ) );

				this->setVariant( list[0] );
			}
	};

	template<class ACTIONIMPL>
	class PartItemVariable : public KexiVariable<ACTIONIMPL>
	{
		public:
			PartItemVariable(ACTIONIMPL* actionimpl, const QString& partitem = QString::null)
				: KexiVariable<ACTIONIMPL>(actionimpl, "partitem", i18n("Item"))
			{
				QStringList namelist;
				if(actionimpl->mainWin()->project()) {
					KexiPart::PartInfoList* parts = Kexi::partManager().partInfoList();
					for(KexiPart::PartInfoListIterator it(*parts); it.current(); ++it) {
						KexiPart::Info* info = it.current();
						if(! info->isDataExportSupported())
							continue;
						KexiPart::ItemDict* items = actionimpl->mainWin()->project()->items(info);
						if(items)
							for(KexiPart::ItemDictIterator item_it = *items; item_it.current(); ++item_it)
								namelist << info->objectName() + "." + item_it.current()->name();
					}
					for(QStringList::Iterator it = namelist.begin(); it != namelist.end(); ++it)
						this->appendChild( KSharedPtr<KoMacro::Variable>(new KoMacro::Variable(*it)) );

					//const QString name = info->objectName(); //info->groupName();
					//this->appendChild( KSharedPtr<KoMacro::Variable>(new KoMacro::Variable(name)) );
				}
				const QString n =
					namelist.contains(partitem)
						? partitem
						: namelist.count() > 0 ? namelist[0] : "";
				this->setVariant(n);
				kdDebug()<<"##################### KexiActions::ObjectVariable() variant="<<this->variant()<<endl;
			}
	};

}

DataTableAction::DataTableAction()
	: KexiAction("datatable", i18n("Data Table"))
{
	setVariable(KSharedPtr<KoMacro::Variable>( new MethodVariable<DataTableAction>(this) ));
	setVariable(KSharedPtr<KoMacro::Variable>( new TypeVariable<DataTableAction>(this) ));
	setVariable(KSharedPtr<KoMacro::Variable>( new PartItemVariable<DataTableAction>(this) ));
}

DataTableAction::~DataTableAction() 
{
}

bool DataTableAction::notifyUpdated(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name)
{
	kdDebug()<<"DataTableAction::notifyUpdated() name="<<name<<" macroitem.action="<<(macroitem->action() ? macroitem->action()->name() : "NOACTION")<<endl;
	/*
	KSharedPtr<KoMacro::Variable> variable = macroitem->variable(name, false);
	if(! variable) {
		kdWarning()<<"DataTableAction::notifyUpdated() No such variable="<<name<<" in macroitem."<<endl;
		return false;
	}
	variable->clearChildren();
	if(name == "method") {
		const int partitem = macroitem->variant(OBJECT, true).toString();
		macroitem->variable(OBJECT, true)->setChildren(
			KoMacro::Variable::List() << KSharedPtr<KoMacro::Variable>(new ObjectVariable<ExecuteAction>(this, partitem)) );
	}
	*/
	return true;
}

void DataTableAction::activate(KSharedPtr<KoMacro::Context> context)
{
	if(! mainWin()->project()) {
		kdWarning() << "ExecuteAction::activate(KSharedPtr<KoMacro::Context>) Invalid project" << endl;
		return;
	}

	const QString method = context->variable("method")->variant().toString();
	const QString type = context->variable("type")->variant().toString();

	const QString partitem = context->variable("partitem")->variant().toString();
	QString identifier;
	if(! partitem.isEmpty()) {
		QStringList parts = QStringList::split(".", partitem);
		KexiPart::Part* part = Kexi::partManager().partForMimeType( QString("kexi/%1").arg(parts[0]) );
		KexiPart::Item* item = part ? mainWin()->project()->item(part->info(), parts[1]) : 0;
		if(! item)
			throw KoMacro::Exception(i18n("No such item \"%1\"").arg(partitem));
		identifier = QString::number(item->identifier());
	}

	QMap<QString,QString> args;
	if(! identifier.isNull())
		args.insert("itemId", identifier);

	if(method == "import") {
		args.insert("sourceType", type);
		QDialog *dlg = KexiInternalPart::createModalDialogInstance(
			"csv_importexport", "KexiCSVImportDialog", 0, mainWin(), 0, &args);
		if (!dlg)
			return; //error msg has been shown by KexiInternalPart
		dlg->exec();
		delete dlg;
	}
	else if(method == "export") {
		args.insert("destinationType", type);
		QDialog *dlg = KexiInternalPart::createModalDialogInstance(
			"csv_importexport", "KexiCSVExportWizard", 0, mainWin(), 0, &args);
		if (!dlg)
			return; //error msg has been shown by KexiInternalPart
		dlg->exec();
		delete dlg;
	}
	else {
		throw KoMacro::Exception(i18n("No such method \"%1\"").arg(method));
	}
}

//#include "executeaction.moc"
