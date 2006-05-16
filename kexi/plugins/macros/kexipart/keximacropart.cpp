/* This file is part of the KDE project
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "keximacropart.h"

#include "keximacroview.h"
#include "keximacrodesignview.h"
#include "keximacrotextview.h"

//#include "kexiviewbase.h"
//#include "keximainwindow.h"
//#include "kexiproject.h"

#include <qdom.h>
#include <kgenericfactory.h>
#include <kexipartitem.h>
//#include <kxmlguiclient.h>
//#include <kexidialogbase.h>
//#include <kconfig.h>
//#include <kdebug.h>

#include "../lib/manager.h"
#include "../lib/macro.h"
#include "../lib/action.h"

#include "../kexiactions/openaction.h"
#include "../kexiactions/executeaction.h"
#include "../kexiactions/navigateaction.h"
#include "../kexiactions/messageaction.h"

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroPart::Private
{
	public:
};

KexiMacroPart::KexiMacroPart(QObject *parent, const char *name, const QStringList &l)
	: KexiPart::Part(parent, name, l)
	, d( new Private() )
{
	//kdDebug() << "KexiMacroPart::KexiMacroPart() Ctor" << endl;

	//registered ID
	m_registeredPartID = (int)KexiPart::MacroObjectType;

	//name of the instance.
	m_names["instanceName"] 
		= i18n("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
		"Use '_' character instead of spaces. First character should be a..z character. "
		"If you cannot use latin characters in your language, use english word.", 
		"macro");

	//describing caption
	m_names["instanceCaption"] = i18n("Macro");

	//supported viewmodes
	m_supportedViewModes = Kexi::DesignViewMode | Kexi::TextViewMode;
}

KexiMacroPart::~KexiMacroPart()
{
	//kdDebug() << "KexiMacroPart::~KexiMacroPart() Dtor" << endl;
	delete d;
}

bool KexiMacroPart::execute(KexiPart::Item* item, QObject* sender)
{
	KexiDialogBase* dialog = new KexiDialogBase(m_mainWin);
	dialog->setId( item->identifier() );
	KexiMacroView* view = dynamic_cast<KexiMacroView*>( createView(dialog, dialog, *item, Kexi::DataViewMode) );
	if(! view) {
		kdWarning() << "KexiMacroPart::execute() Failed to create a view." << endl;
		return false;
	}

	if(! view->macro().data()) {
		kdWarning() << "KexiMacroPart::execute() No such item " << item->name() << endl;
		return false;
	}

	kdDebug() << "KexiMacroPart::execute() itemname=" << item->name() << endl;
	view->loadData();
	view->execute(sender);
	view->deleteLater();
	return true;
}

void KexiMacroPart::initPartActions()
{
	//kdDebug() << "KexiMacroPart::initPartActions()" << endl;

	::KoMacro::Manager::init(m_mainWin);
	new KexiMacro::OpenAction;
	new KexiMacro::ExecuteAction;
	new KexiMacro::NavigateAction;
	new KexiMacro::MessageAction;
}

void KexiMacroPart::initInstanceActions()
{
	//kdDebug() << "KexiMacroPart::initInstanceActions()" << endl;

	//createSharedAction(Kexi::DesignViewMode, i18n("Execute Macro"), "exec", 0, "data_execute");
}

KexiViewBase* KexiMacroPart::createView(QWidget* parent, KexiDialogBase* dialog, KexiPart::Item& item, int viewMode, QMap<QString,QString>*)
{
	const QString itemname = item.name();
	//kdDebug() << "KexiMacroPart::createView() itemname=" << itemname << endl;

	if(! itemname.isNull()) {
		KoMacro::Macro::Ptr macro = ::KoMacro::Manager::self()->getMacro(itemname);
		if(! macro) {
			// If we don't have a macro with that name yet, create one.
			macro = ::KoMacro::Manager::self()->createMacro(itemname);
			// and remember the new macro for later usage.
			::KoMacro::Manager::self()->addMacro(itemname, macro);
		}

		KexiMainWindow *win = dialog->mainWin();
		if(win && win->project() && win->project()->dbConnection()) {
			if(viewMode == Kexi::DesignViewMode) {
				return new KexiMacroDesignView(win, parent, macro);
			}
			if(viewMode == Kexi::TextViewMode) {
				return new KexiMacroTextView(win, parent, macro);
			}
			if(viewMode == Kexi::DataViewMode) {
				// Called if the macro should be executed.
				return new KexiMacroView(win, parent, macro);
			}
		}
	}

	//kdDebug() << "KexiMacroPart::createView() No view available." << endl;
	return 0;
}

QString KexiMacroPart::i18nMessage(const QCString& englishMessage) const
{
	if(englishMessage=="Design of object \"%1\" has been modified.") {
		return i18n("Design of macro \"%1\" has been modified.");
	}
	if(englishMessage=="Object \"%1\" already exists.") {
		return i18n("Macro \"%1\" already exists.");
	}
	return englishMessage;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_macro, KGenericFactory<KexiMacroPart>("kexihandler_script") )

#include "keximacropart.moc"
