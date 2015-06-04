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
#include "kexiscriptadaptor.h"

#include <KexiView.h>
#include <KexiWindow.h>
#include <KexiMainWindowIface.h>
#include <kexiproject.h>

#include <kross/core/manager.h>
#include <kross/core/action.h>
#include <kross/core/actioncollection.h>

#include <kexipart.h>
#include <kexipartitem.h>
#include <KexiIcon.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <KSharedConfig>


/// \internal
class KexiScriptPart::Private
{
public:
    explicit Private(KexiScriptPart* p)
            : p(p)
            , actioncollection(new Kross::ActionCollection("projectscripts"))
            , adaptor(0) {}
    ~Private() {
        delete actioncollection; delete adaptor;
    }

    KexiScriptPart* p;
    Kross::ActionCollection* actioncollection;
    KexiScriptAdaptor* adaptor;

    Kross::Action* action(const QString &partname) {
        Kross::Action *action = actioncollection->action(partname);
        if (! action) {
            if (! adaptor)
                adaptor = new KexiScriptAdaptor();
            action = new Kross::Action(p, partname);
            actioncollection->addAction(action);
            action->addObject(adaptor);
        }
        return action;
    }
};

KexiScriptPart::KexiScriptPart(QObject *parent, const QVariantList& l)
  : KexiPart::Part(parent,
        xi18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
              "Use '_' character instead of spaces. First character should be a..z character. "
              "If you cannot use latin characters in your language, use english word.",
              "script"),
        xi18nc("tooltip", "Create new script"),
        xi18nc("what's this", "Creates new script."),
        l)
  , d(new Private(this))
{
}

KexiScriptPart::~KexiScriptPart()
{
    delete d;
}

bool KexiScriptPart::execute(KexiPart::Item* item, QObject* sender)
{
    Q_UNUSED(sender);
    if (!item) {
        kWarning() << "Invalid item.";
        return false;
    }

#if 0
    KexiDialogBase* dialog = new KexiDialogBase(m_mainWin);
    dialog->setId(item->identifier());
    KexiScriptDesignView* view = dynamic_cast<KexiScriptDesignView*>(
                                     createView(dialog, dialog, *item, Kexi::DesignViewMode));
    if (! view) {
        kWarning() << "Failed to create a view.";
        return false;
    }

    Kross::Action* scriptaction = view->scriptAction();
    if (scriptaction) {

        const QString dontAskAgainName = "askExecuteScript";
        KSharedConfig::Ptr config = KSharedConfig::openConfig();
        QString dontask = config->readEntry(dontAskAgainName).toLower();

        bool exec = (dontask == "yes");
        if (!exec && dontask != "no") {
            exec = KMessageBox::warningContinueCancel(0,
                    futureI18n("Do you want to execute the script \"%1\"?\n\n"
                         "Scripts obtained from unknown sources can contain dangerous code.", scriptaction->text()),
                    futureI18n("Execute Script?"), KGuiItem(futureI18n("Execute"), koIconName("system-run")),
                    dontAskAgainName, KMessageBox::Notify | KMessageBox::Dangerous
                                                     ) == KMessageBox::Continue;
        }

        if (exec) {
            //QTimer::singleShot(10, scriptaction, SLOT(activate()));
            d->scriptguiclient->executeScriptAction(scriptaction);
        }
    }
    view->deleteLater(); // not needed any longer.
#else

    Kross::Action *action = d->action(item->name());
    Q_ASSERT(action);
    action->trigger();
#endif
    return true;
}

void KexiScriptPart::initPartActions()
{
    kDebug() << ".............";
#if 0
    if (m_mainWin) {
        // At this stage the KexiPart::Part::m_mainWin should be defined, so
        // that we are able to use it's KXMLGUIClient.

        // Initialize the ScriptGUIClient.
        d->scriptguiclient = new Kross::Api::ScriptGUIClient(m_mainWin);

        // Publish the KexiMainWindow singelton instance. At least the KexiApp
        // scripting-plugin depends on this instance and loading the plugin will
        // fail if it's not avaiable.
        if (! Kross::Api::Manager::scriptManager()->hasChild("KexiMainWindow")) {
            Kross::Api::Manager::scriptManager()->addQObject(m_mainWin, "KexiMainWindow");

            // Add the QAction's provided by the ScriptGUIClient to the
            // KexiMainWindow.
            //FIXME: fix+use createSharedPartAction() whyever it doesn't work as expected right now...
            Q3PopupMenu* popup = m_mainWin->findPopupMenu("tools");
            if (popup) {
                QAction* execscriptaction = d->scriptguiclient->action("executescriptfile");
                if (execscriptaction)
                    execscriptaction->plug(popup);
                QAction* configscriptaction = d->scriptguiclient->action("configurescripts");
                if (configscriptaction)
                    configscriptaction->plug(popup);
                QAction* scriptmenuaction = d->scriptguiclient->action("installedscripts");
                if (scriptmenuaction)
                    scriptmenuaction->plug(popup);
                /*
                QAction * execscriptmenuaction = d->scriptguiclient->action("executedscripts");
                if(execscriptmenuaction)
                    execscriptmenuaction->plug( popup );
                QAction * loadedscriptmenuaction = d->scriptguiclient->action("loadedscripts");
                if(loadedscriptmenuaction)
                    loadedscriptmenuaction->plug( popup );
                */
            }
        }
    }
#endif
}

void KexiScriptPart::initInstanceActions()
{
    kDebug();
    createSharedAction(Kexi::DesignViewMode, xi18n("Configure Editor..."),
                       koIconName("configure"), QKeySequence(), "script_config_editor");
}

KexiView* KexiScriptPart::createView(QWidget *parent,
                                     KexiWindow *window,
                                     KexiPart::Item &item,
                                     Kexi::ViewMode viewMode,
                                     QMap<QString, QVariant>* staticObjectArgs)
{
    Q_UNUSED(window);
    Q_UNUSED(staticObjectArgs);
    kDebug() << "............. createView";
    QString partname = item.name();
    if (! partname.isNull()) {
        Kross::Action *action = d->action(partname);
#if 0
        KexiMainWindow *win = dialog->mainWin();
        if (!win || !win->project() || !win->project()->dbConnection())
            return 0;
        Kross::Api::ScriptActionCollection* collection = d->scriptguiclient->getActionCollection("projectscripts");
        if (! collection) {
            collection = new Kross::Api::ScriptActionCollection(xi18n("Scripts"), d->scriptguiclient->actionCollection(), "projectscripts");
            d->scriptguiclient->addActionCollection("projectscripts", collection);
        }
        const char* name = partname.toLatin1();
        Kross::Api::ScriptAction::Ptr scriptaction = collection->action(name);
        if (! scriptaction) {
            scriptaction = new Kross::Api::ScriptAction(partname);
            collection->attach(scriptaction); //!< @todo remove again on unload!
        }
#endif
        if (viewMode == Kexi::DesignViewMode) {
            return new KexiScriptDesignView(parent, action);
        }
    }
    return 0;
}

KLocalizedString KexiScriptPart::i18nMessage(
    const QString& englishMessage, KexiWindow* window) const
{
    if (englishMessage == "Design of object <resource>%1</resource> has been modified.")
        return ki18n(I18N_NOOP("Design of script <resource>%1</resource> has been modified."));
    if (englishMessage == "Object <resource>%1</resource> already exists.")
        return ki18n(I18N_NOOP("Script <resource>%1</resource> already exists."));
    return Part::i18nMessage(englishMessage, window);
}

K_EXPORT_KEXIPART_PLUGIN( KexiScriptPart, script )

