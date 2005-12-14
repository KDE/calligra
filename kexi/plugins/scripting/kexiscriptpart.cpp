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
#include "kexiscripttextview.h"

#include "kexiviewbase.h"
#include "keximainwindow.h"
#include "kexiproject.h"

#include <kross/main/manager.h>
#include <kross/main/scriptaction.h>
#include <kross/main/scriptguiclient.h>

#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kexipartitem.h>
#include <kxmlguiclient.h>
#include <kexidialogbase.h>
#include <kdebug.h>

/// \internal
class KexiScriptPart::Private
{
	public:
		Kross::Api::ScriptGUIClient* scriptguiclient;
};

KexiScriptPart::KexiScriptPart(QObject *parent, const char *name, const QStringList &l)
	: KexiPart::Part(parent, name, l)
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
	m_supportedViewModes = Kexi::DesignViewMode | Kexi::TextViewMode;
}

KexiScriptPart::~KexiScriptPart()
{
	delete d->scriptguiclient;
	delete d;
}

void KexiScriptPart::initPartActions()
{
	if(m_mainWin) {
		// At this stage the KexiPart::Part::m_mainWin should be defined, so
		// that we are able to use it's KXMLGUIClient.

		// Initialize the ScriptGUIClient.
		d->scriptguiclient = new Kross::Api::ScriptGUIClient( m_mainWin );

		// Set the configurationfile.
		QString file = KGlobal::dirs()->findResource("appdata", "kexiscripting.rc");
		d->scriptguiclient->setXMLFile(file, true);

		// Publish the KexiMainWindow singelton instance. At least the KexiApp 
		// scripting-plugin depends on this instance and loading the plugin will 
		// fail if it's not avaiable.
		if( Kross::Api::Manager::scriptManager()->addQObject(m_mainWin, "KexiMainWindow") ) {

			// Add the KAction's provided by the ScriptGUIClient to the
			// KexiMainWindow.
			//FIXME: fix+use createSharedPartAction() whyever it doesn't work as expected right now...
			QPopupMenu* popup = m_mainWin->findPopupMenu("tools");
			if(popup) {
				KAction* execscriptaction = d->scriptguiclient->action("executescriptfile");
				if(execscriptaction)
					execscriptaction->plug( popup );

				KAction* scriptmenuaction = d->scriptguiclient->action("scripts");
				if(scriptmenuaction)
					scriptmenuaction->plug( popup );
			}
		}
	}
}

void KexiScriptPart::initInstanceActions()
{
	//FIXME createSharedAction(Kexi::DesignViewMode, i18n("Execute Script"), "exec", 0, "script_execute");
	//FIXME createSharedAction(Kexi::DesignViewMode, i18n("Configure Editor..."), "configure", 0, "script_config_editor");
}

KexiViewBase* KexiScriptPart::createView(QWidget *parent, KexiDialogBase* dialog, KexiPart::Item &item, int viewMode)
{
	QString partname = dialog->partItem()->name();
	if(! partname.isNull()) {
		const char* name = partname.latin1();
		Kross::Api::ScriptAction* scriptaction = dynamic_cast< Kross::Api::ScriptAction* >( d->scriptguiclient->action(name) );
		if(! scriptaction) {
			scriptaction = new Kross::Api::ScriptAction(name, partname);
			d->scriptguiclient->actionCollection()->insert(scriptaction);
		}

		KexiMainWindow *win = dialog->mainWin();
		if(viewMode == Kexi::DesignViewMode) {
			if(!win || !win->project() || !win->project()->dbConnection())
				return 0;
			return new KexiScriptDesignView(win, parent, scriptaction);
		}
		if(viewMode == Kexi::TextViewMode) {
			if(!win || !win->project() || !win->project()->dbConnection())
				return 0;
			return new KexiScriptTextView(win, parent, scriptaction);
		}
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
