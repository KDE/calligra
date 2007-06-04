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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiscriptpart.h"
#include "kexiscriptdesignview.h"

#include <KexiView.h"
#include <KexiMainWindowIface.h>
#include <kexiproject.h>

#include <kross/main/manager.h>
#include <kross/main/scriptaction.h>
#include <kross/main/scriptguiclient.h>

#include <kgenericfactory.h>
#include <kexipartitem.h>
#include <kxmlguiclient.h>
#include <kexidialogbase.h>
#include <kconfig.h>
#include <kdebug.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <Q3CString>

/// \internal
class KexiScriptPart::Private
{
	public:
		Kross::Api::ScriptGUIClient* scriptguiclient;
};

KexiScriptPart::KexiScriptPart(QObject *parent, const QStringList &l)
	: KexiPart::Part(parent, l)
	, d( new Private() )
{
	d->scriptguiclient = 0;

	// REGISTERED ID:
	m_registeredPartID = (int)KexiPart::ScriptObjectType;

	m_names["instanceName"] 
		= i18n("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
		"Use '_' character instead of spaces. First character should be a..z character. "
		"If you cannot use latin characters in your language, use english word.", 
		"script");
	m_names["instanceCaption"] = i18n("Script");
	m_supportedViewModes = Kexi::DesignViewMode;
}

KexiScriptPart::~KexiScriptPart()
{
	delete d->scriptguiclient;
	delete d;
}

bool KexiScriptPart::execute(KexiPart::Item* item, QObject* sender)
{
	Q_UNUSED(sender);

	if(! item) {
		kWarning() << "KexiScriptPart::execute: Invalid item." << endl;
		return false;
	}

	KexiDialogBase* dialog = new KexiDialogBase(m_mainWin);
	dialog->setId( item->identifier() );
	KexiScriptDesignView* view = dynamic_cast<KexiScriptDesignView*>( createView(dialog, dialog, *item, Kexi::DesignViewMode) );
	if(! view) {
		kWarning() << "KexiScriptPart::execute: Failed to create a view." << endl;
		return false;
	}

	Kross::Api::ScriptAction* scriptaction = view->scriptAction();
	if(scriptaction) {

		const QString dontAskAgainName = "askExecuteScript";
		KSharedConfig::Ptr config = KGlobal::config();
		QString dontask = config->readEntry(dontAskAgainName).toLower();

		bool exec = (dontask == "yes");
		if( !exec && dontask != "no" ) {
			exec = KMessageBox::warningContinueCancel(0,
				i18n("Do you want to execute the script \"%1\"?\n\nScripts obtained from unknown sources can contain dangerous code.").arg(scriptaction->text()),
				i18n("Execute Script?"), KGuiItem(i18n("Execute"), "exec"),
				dontAskAgainName, KMessageBox::Notify | KMessageBox::Dangerous
			) == KMessageBox::Continue;
		}

		if(exec) {
			//QTimer::singleShot(10, scriptaction, SLOT(activate()));
			d->scriptguiclient->executeScriptAction( scriptaction );
		}
	}

	view->deleteLater(); // not needed any longer.
	return true;
}

void KexiScriptPart::initPartActions()
{
	if(m_mainWin) {
		// At this stage the KexiPart::Part::m_mainWin should be defined, so
		// that we are able to use it's KXMLGUIClient.

		// Initialize the ScriptGUIClient.
		d->scriptguiclient = new Kross::Api::ScriptGUIClient( m_mainWin );

		// Publish the KexiMainWindow singelton instance. At least the KexiApp 
		// scripting-plugin depends on this instance and loading the plugin will 
		// fail if it's not avaiable.
		if(! Kross::Api::Manager::scriptManager()->hasChild("KexiMainWindow")) {
			Kross::Api::Manager::scriptManager()->addQObject(m_mainWin, "KexiMainWindow");

			// Add the KAction's provided by the ScriptGUIClient to the
			// KexiMainWindow.
			//FIXME: fix+use createSharedPartAction() whyever it doesn't work as expected right now...
			Q3PopupMenu* popup = m_mainWin->findPopupMenu("tools");
			if(popup) {
				QAction* execscriptaction = d->scriptguiclient->action("executescriptfile");
				if(execscriptaction)
					execscriptaction->plug( popup );
				QAction* configscriptaction = d->scriptguiclient->action("configurescripts");
				if(configscriptaction)
					configscriptaction->plug( popup );
				QAction* scriptmenuaction = d->scriptguiclient->action("installedscripts");
				if(scriptmenuaction)
					scriptmenuaction->plug( popup );
				/*
				KAction* execscriptmenuaction = d->scriptguiclient->action("executedscripts");
				if(execscriptmenuaction)
				    execscriptmenuaction->plug( popup );
				KAction* loadedscriptmenuaction = d->scriptguiclient->action("loadedscripts");
				if(loadedscriptmenuaction)
				    loadedscriptmenuaction->plug( popup );
				*/
			}
		}
	}
}

void KexiScriptPart::initInstanceActions()
{
	//createSharedAction(Kexi::DesignViewMode, i18n("Execute Script"), "media-playback-start", 0, "data_execute");
	createSharedAction(Kexi::DesignViewMode, i18n("Configure Editor..."), "configure", 0, "script_config_editor");
}

KexiView* KexiScriptPart::createView(QWidget *parent, KexiDialogBase* dialog, KexiPart::Item& item, int viewMode, QMap<QString,QString>*)
{
	QString partname = item.name();
	if( ! partname.isNull() ) {
		KexiMainWindow *win = dialog->mainWin();
		if(!win || !win->project() || !win->project()->dbConnection())
			return 0;

		Kross::Api::ScriptActionCollection* collection = d->scriptguiclient->getActionCollection("projectscripts");
		if(! collection) {
			collection = new Kross::Api::ScriptActionCollection( i18n("Scripts"), d->scriptguiclient->actionCollection(), "projectscripts" );
			d->scriptguiclient->addActionCollection("projectscripts", collection);
		}

		const char* name = partname.toLatin1();
		Kross::Api::ScriptAction::Ptr scriptaction = collection->action(name);
		if(! scriptaction) {
			scriptaction = new Kross::Api::ScriptAction(partname);
			collection->attach(scriptaction); //TODO remove again on unload!
		}

		if(viewMode == Kexi::DesignViewMode) {
			return new KexiScriptDesignView(win, parent, scriptaction);
		}
	}
	return 0;
}

KLocalizedString KexiScriptPart::i18nMessage(
	const QString& englishMessage, KexiWindow* window) const
{
	if (englishMessage=="Design of object \"%1\" has been modified.")
		return ki18n(I18N_NOOP("Design of script \"%1\" has been modified."));
	if (englishMessage=="Object \"%1\" already exists.")
		return ki18n(I18N_NOOP("Script \"%1\" already exists."));
	return Part::i18nMessage(englishMessage, window);
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_script, KGenericFactory<KexiScriptPart>("kexihandler_script") )

#include "kexiscriptpart.moc"
