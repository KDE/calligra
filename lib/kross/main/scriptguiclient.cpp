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

            QMap<QString, ScriptActionCollection*> collections;
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

    setInstance( ScriptGUIClient::instance() );

//setXMLFile( guiclient->xmlFile() );
//setXMLFile( KGlobal::dirs()->findResource("appdata", "scripting.rc") );

    // action to execute a scriptfile.
    new KAction(i18n("Execute Script File..."), 0, 0, this, SLOT(executeScriptFile()), actionCollection(), "executescriptfile");

    // acion to show the ScriptManagerGUI dialog.
    new KAction(i18n("Script Manager..."), 0, 0, this, SLOT(showScriptManager()), actionCollection(), "configurescripts");

    // The predefined ScriptActionCollection's this ScriptGUIClient provides.
    d->collections.replace("installedscripts",
        new ScriptActionCollection(i18n("Scripts"), actionCollection(), "installedscripts") );
    d->collections.replace("loadedscripts",
        new ScriptActionCollection(i18n("Loaded"), actionCollection(), "loadedscripts") );
    d->collections.replace("executedscripts",
        new ScriptActionCollection(i18n("History"), actionCollection(), "executedscripts") );
}

ScriptGUIClient::~ScriptGUIClient()
{
    kdDebug() << QString("ScriptGUIClient::~ScriptGUIClient() Dtor") << endl;
    for(QMap<QString, ScriptActionCollection*>::Iterator it = d->collections.begin(); it != d->collections.end(); ++it)
        delete it.data();
    delete d;
}

bool ScriptGUIClient::hasActionCollection(const QString& name)
{
    return d->collections.contains(name);
}

ScriptActionCollection* ScriptGUIClient::getActionCollection(const QString& name)
{
    return d->collections[name];
}

QMap<QString, ScriptActionCollection*> ScriptGUIClient::getActionCollections()
{
    return d->collections;
}

void ScriptGUIClient::addActionCollection(const QString& name, ScriptActionCollection* collection)
{
    removeActionCollection(name);
    d->collections.replace(name, collection);
}

bool ScriptGUIClient::removeActionCollection(const QString& name)
{
    if(d->collections.contains(name)) {
        ScriptActionCollection* c = d->collections[name];
        d->collections.remove(name);
        delete c;
        return true;
    }
    return false;
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

void ScriptGUIClient::setDOMDocument(const QDomDocument &document, bool /*merge*/)
{
    ScriptActionCollection* installedcollection = d->collections["installedscripts"];
    if(installedcollection)
        installedcollection->clear();

    // time to parse the DOM-document
    //QDomDocument domdoc = domDocument();
    QDomDocument domdoc = document;
    kdDebug()<< QString("Kross::Api::ScriptGUIClient::setDOMDocument() XML-file: %1").arg(xmlFile()) << endl;

    // walk throuh the list of Scripts-elements.
    QDomNodeList nodelist = domdoc.elementsByTagName("Scripts");
    uint nodelistcount = nodelist.count();
    for(uint i = 0; i < nodelistcount; i++) {
        QDomNode node = nodelist.item(i).firstChild();
        // walk through the list of children this Scripts-element has.
        while(! node.isNull()) {
            //kdDebug() << "subnode => " << node.nodeName() << endl;

            if(node.nodeName() == "ScriptAction") {
                ScriptAction::Ptr action = new ScriptAction( node.toElement() );

                if(installedcollection)
                    installedcollection->attach( action );

                connect(action.data(), SIGNAL( failed(const QString&, const QString&) ),
                        this, SLOT( executionFailed(const QString&, const QString&) ));
                connect(action.data(), SIGNAL( success() ),
                        this, SLOT( successfullyExecuted() ));
            }

            node = node.nextSibling();
        }
    }

    emit collectionChanged(installedcollection);
}

void ScriptGUIClient::successfullyExecuted()
{
    const ScriptAction* action = dynamic_cast< const ScriptAction* >( QObject::sender() );
    if(action) {
        ScriptActionCollection* executedcollection = d->collections["executedscripts"];
        if(executedcollection) {
            ScriptAction* actionptr = const_cast< ScriptAction* >( action );
            executedcollection->detach(actionptr);
            executedcollection->attach(actionptr);

            emit collectionChanged(executedcollection);
        }
    }
}

void ScriptGUIClient::executionFailed(const QString& errormessage, const QString& tracedetails)
{
    if(tracedetails.isEmpty())
        KMessageBox::error(0, errormessage);
    else
        KMessageBox::detailedError(0, errormessage, tracedetails);
}

KURL ScriptGUIClient::openScriptFile(const QString& caption)
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
    if(! caption.isNull())
        filedialog->setCaption(caption);
    if( filedialog->exec() )
        return filedialog->selectedURL();
    return KURL();
}

bool ScriptGUIClient::loadScriptFile()
{
    KURL url = openScriptFile( i18n("Load Script File") );
    if(url.isValid()) {
        ScriptActionCollection* loadedcollection = d->collections["loadedscripts"];
        if(loadedcollection) {
            ScriptAction::Ptr action = new ScriptAction( url.path() );
            connect(action.data(), SIGNAL( failed(const QString&, const QString&) ),
                    this, SLOT( executionFailed(const QString&, const QString&) ));
            connect(action.data(), SIGNAL( success() ),
                    this, SLOT( successfullyExecuted() ));

            loadedcollection->detach(action);
            loadedcollection->attach(action);
            return true;
        }
    }
    return false;
}

bool ScriptGUIClient::executeScriptFile()
{
    KURL url = openScriptFile( i18n("Execute Script File") );
    if(url.isValid())
        return executeScriptFile( url.path() );
    return false;
}

bool ScriptGUIClient::executeScriptFile(const QString& file)
{
    kdDebug() << QString("Kross::Api::ScriptGUIClient::executeScriptFile() file='%1'").arg(file) << endl;

    ScriptAction::Ptr action = new ScriptAction(file);
    return executeScriptAction(action.data());
}

bool ScriptGUIClient::executeScriptAction(ScriptAction* action)
{
    connect(action, SIGNAL( failed(const QString&, const QString&) ),
            this, SLOT( executionFailed(const QString&, const QString&) ));
    connect(action, SIGNAL( success() ),
            this, SLOT( successfullyExecuted() ));

    action->activate();
    bool ok = action->hadException();
    action->finalize(); // execution is done.
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
