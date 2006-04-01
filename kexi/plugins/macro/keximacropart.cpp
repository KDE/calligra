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

#include "lib/manager.h"
//#include "kexiviewbase.h"
//#include "keximainwindow.h"
//#include "kexiproject.h"

#include <kgenericfactory.h>
#include <kexipartitem.h>
//#include <kxmlguiclient.h>
//#include <kexidialogbase.h>
//#include <kconfig.h>
//#include <kdebug.h>

/// \internal
class KexiMacroPart::Private
{
	public:

		/**
		* The @a KoMacro::Manager instace used to access the
		* KoMacro Framework.
		*/
		KoMacro::Manager* const manager;

		/**
		* Constructor.
		*/
		Private()
			: manager(0)
		{
		}

		/**
		* Destructor.
		*/
		~Private()
		{
			delete manager;
		}

};

KexiMacroPart::KexiMacroPart(QObject *parent, const char *name, const QStringList &l)
	: KexiPart::Part(parent, name, l)
	, d( new Private() )
{
	//registered ID
	m_registeredPartID = (int)KexiPart::MacroObjectType;

	m_names["instanceName"] 
		= i18n("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
		"Use '_' character instead of spaces. First character should be a..z character. "
		"If you cannot use latin characters in your language, use english word.", 
		"macro");
	m_names["instanceCaption"] = i18n("Macro");

	//supported viewmodes
	m_supportedViewModes = Kexi::DesignViewMode;
}

KexiMacroPart::~KexiMacroPart()
{
	delete d;
}

KAction* KexiMacroPart::action(const QString&, QObject*)
{
	///\todo
	return 0;
}

void KexiMacroPart::initPartActions()
{
    d->manager = new KoMacro::Manager(m_mainWin);
}

void KexiMacroPart::initInstanceActions()
{
	//createSharedAction(Kexi::DesignViewMode, i18n("Execute Macro"), "exec", 0, "script_execute");
}

KexiViewBase* KexiMacroPart::createView(QWidget* /*parent*/, KexiDialogBase* /*dialog*/, KexiPart::Item& /*item*/, int /*viewMode*/, QMap<QString,QString>*)
{
/*
	QString partname = item.name();
	if( ! partname.isNull() ) {
		KexiMainWindow *win = dialog->mainWin();
		if(!win || !win->project() || !win->project()->dbConnection())
			return 0;
		if(viewMode == Kexi::DesignViewMode)
			return new KexiScriptDesignView(win, parent, scriptaction);
	}
*/
	return 0;
}

QString KexiMacroPart::i18nMessage(const QCString& englishMessage) const
{
	if (englishMessage=="Design of object \"%1\" has been modified.")
		return i18n("Design of macro \"%1\" has been modified.");
	if (englishMessage=="Object \"%1\" already exists.")
		return i18n("Macro \"%1\" already exists.");
	return englishMessage;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_macro, KGenericFactory<KexiMacroPart>("kexihandler_script") )

#include "keximacropart.moc"
