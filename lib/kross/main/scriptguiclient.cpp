/***************************************************************************
 * scriptguiclient.cpp
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
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

#include "scriptguiclient.h"
#include "manager.h"
#include "../api/interpreter.h"
#include "wdgscriptsmanager.h"

#include <kapplication.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kmimetype.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kurl.h>
#include <kdebug.h>

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// @internal
    class ScriptGUIClientPrivate
    {
        public:
            KXMLGUIClient* guiclient;
            QWidget* parent;
            bool dirtyscriptlist;
            KActionMenu* scriptextensions;
            ScriptAction::List installedscriptactions;
            ScriptAction::List executedscriptactions;
    };

}}

ScriptGUIClient::ScriptGUIClient(KXMLGUIClient* guiclient, QWidget* parent)
    : QObject( parent )
    , KXMLGUIClient( guiclient )
    , d( new ScriptGUIClientPrivate() ) // initialize d-pointer class
{
    kdDebug() << QString("ScriptGUIClient::ScriptGUIClient() Ctor") << endl;

    d->guiclient = guiclient;
    d->parent = parent;
    d->dirtyscriptlist = true;

    setInstance( ScriptGUIClient::instance() );

//setXMLFile( guiclient->xmlFile() );
//setXMLFile( KGlobal::dirs()->findResource("appdata", "scripting.rc") );

    // action to execute a scriptfile.
    new KAction(i18n("Execute Script File..."), 0, 0, this, SLOT(executeScriptFile()), actionCollection(), "executescriptfile");

    // acion to show the ScriptManagerGUI dialog.
    new KAction(i18n("Script Manager..."), 0, 0, this, SLOT(showScriptManager()), actionCollection(), "configurescripts");

    // list of actions with each item beeing a scriptfile we are able to execute.
    d->scriptextensions = new KActionMenu(i18n("Scripts"), actionCollection(), "scripts");
    connect(d->scriptextensions->popupMenu(), SIGNAL( aboutToShow() ), SLOT( showScriptGUIClientsMenu() ));
}

ScriptGUIClient::~ScriptGUIClient()
{
    kdDebug() << QString("ScriptGUIClient::~ScriptGUIClient() Dtor") << endl;
    delete d;
}

ScriptAction::List ScriptGUIClient::getInstalledScriptActions()
{
    showScriptGUIClientsMenu(); // update the list if needed.
    return d->installedscriptactions;
}

ScriptAction::List ScriptGUIClient::getExecutedScriptActions()
{
    return d->executedscriptactions;
}

void ScriptGUIClient::setXMLFile(const QString &file, bool merge, bool setxmldoc)
{
    KXMLGUIClient::setXMLFile(file, merge, setxmldoc);
}

void ScriptGUIClient::setLocalXMLFile(const QString &file)
{
    KXMLGUIClient::setLocalXMLFile(file);
}

void ScriptGUIClient::setXML(const QString &document, bool merge)
{
    KXMLGUIClient::setXML(document, merge);
}

void ScriptGUIClient::setDOMDocument(const QDomDocument &document, bool merge)
{
    KXMLGUIClient::setDOMDocument(document, merge);
}

void ScriptGUIClient::executionFailed(const QString& errormessage, const QString& tracedetails)
{
    if(tracedetails.isEmpty())
        KMessageBox::error(0, errormessage);
    else
        KMessageBox::detailedError(0, errormessage, tracedetails);
}

void ScriptGUIClient::showScriptGUIClientsMenu()
{
    if(! d->dirtyscriptlist) {
        // if the d->scriptextensions KActionMenu isn't dirty we don't
        // need to update it.
        return;
    }

    // remove the dirty ScriptAction's we don't need any longer.
    for(ScriptAction::List::Iterator it = d->installedscriptactions.begin(); it != d->installedscriptactions.end(); ++it) {
        d->scriptextensions->remove(*it);
        (*it)->deleteLater();
    }
    d->installedscriptactions.clear();

    // time to parse the DOM-document
    QDomDocument domdoc = domDocument();
    kdDebug()<< QString("Kross::Api::ScriptGUIClient::showScriptGUIClientsMenu() XML-file: %1").arg(xmlFile()) << endl;
    //kdDebug()<< QString("Kross::Api::ScriptGUIClient::showScriptGUIClientsMenu() XML-DOM: %1").arg(domdoc.toString()) << endl;

    // walk throuh the list of Scripts-elements.
    QDomNodeList nodelist = domdoc.elementsByTagName("Scripts");
    uint nodelistcount = nodelist.count();
    for(uint i = 0; i < nodelistcount; i++) {
        QDomNode node = nodelist.item(i).firstChild();
        // walk through the list of children this Scripts-element has.
        while(! node.isNull()) {
            //kdDebug() << "subnode => " << node.nodeName() << endl;

            if(node.nodeName() == "ScriptAction") {
                ScriptAction* action = new ScriptAction( node.toElement() );
                d->installedscriptactions.append( action );
                d->scriptextensions->insert( action );
                connect(action, SIGNAL( failed(const QString&, const QString&) ),
                        this, SLOT( executionFailed(const QString&, const QString&) ));
            }

            node = node.nextSibling();
        }
    }

    d->dirtyscriptlist = false;
}

void ScriptGUIClient::dirtyInstalledScriptActions()
{
    d->dirtyscriptlist = true;
}

bool ScriptGUIClient::executeScriptFile()
{
    QStringList mimetypes;
    QMap<QString, InterpreterInfo*> infos = Manager::scriptManager()->getInterpreterInfos();
    for(QMap<QString, InterpreterInfo*>::Iterator it = infos.begin(); it != infos.end(); ++it)
        mimetypes.append( it.data()->getMimeTypes().join(" ").stripWhiteSpace() );

    KFileDialog* filedialog = new KFileDialog(
        QString::null, // startdir
        mimetypes.join(" "), // filter
        0, // parent widget
        "ScriptGUIClientFileDialog", // name
        true // modal
    );
    filedialog->setCaption( i18n("Execute Script File") );
    if( filedialog->exec() ) {
        KURL file = filedialog->selectedURL();
        return executeScriptFile( file.path() );
    }
    return false;
}

bool ScriptGUIClient::executeScriptFile(const QString& file)
{
    kdDebug() << QString("Kross::Api::ScriptGUIClient::executeScriptFile() file='%1'").arg(file) << endl;

    ScriptAction* action = new ScriptAction(file);
    QString errormessage, tracedetails;
    bool ok = action->activate(errormessage, tracedetails);
    if(ok) {
        action->finalize(); // execution is done.
        d->executedscriptactions.append(action);
    }
    else { // display an errormessage if execution failed.
        executionFailed(errormessage, tracedetails);
        action->deleteLater(); // we don't need the ScriptAction any longer.
    }
    return ok;
}

void ScriptGUIClient::showScriptManager()
{
    KDialogBase* kdb = new KDialogBase(d->parent, "", true, i18n("Script Manager"), KDialogBase::Close);
    WdgScriptsManager* wsm = new WdgScriptsManager(this, kdb);
    kdb->setMainWidget(wsm);
    kdb->show();
}

#include "scriptguiclient.moc"
