/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartitem.h"
#include "kexistaticpart.h"
#include "KexiWindow.h"
#include "KexiWindowData.h"
#include "KexiView.h"

#include "kexipartguiclient.h"
#include "KexiMainWindowIface.h"
#include "kexi.h"

#include <kexidb/connection.h>
#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>

#include <kactioncollection.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kxmlguifactory.h>
#include <kstandarddirs.h>

namespace KexiPart
{
//! @internal
class Part::Private
{
public:
    Private()
            : guiClient(0)
//            , registeredPartID(-1) //no registered ID by default
            , newObjectsAreDirty(false)
            , instanceActionsInitialized(false) {
    }

    //! Helper, used in Part::openInstance()
    tristate askForOpeningInTextMode(KexiWindow *window, KexiPart::Item &item,
                                     Kexi::ViewModes supportedViewModes, Kexi::ViewMode viewMode) {
        if (viewMode != Kexi::TextViewMode
                && supportedViewModes & Kexi::TextViewMode
                && window->data()->proposeOpeningInTextViewModeBecauseOfProblems) {
            //ask
            KexiUtils::WaitCursorRemover remover;
            //! @todo use message handler for this to enable non-gui apps
            QString singleStatusString(window->singleStatusString());
            if (!singleStatusString.isEmpty())
                singleStatusString.prepend(QString("\n\n") + i18n("Details:") + " ");
            if (KMessageBox::No == KMessageBox::questionYesNo(0,
                    ((viewMode == Kexi::DesignViewMode)
                     ? i18n("Object \"%1\" could not be opened in Design View.", item.name())
                     : i18n("Object could not be opened in Data View.")) + "\n"
                    + i18n("Do you want to open it in Text View?") + singleStatusString, 0,
                    KStandardGuiItem::open(), KStandardGuiItem::cancel())) {
                return false;
            }
            return true;
        }
        return cancelled;
    }

    /*! Supported modes for dialogs created by this part.
    @see supportedViewModes() */
    Kexi::ViewModes supportedViewModes;

    Kexi::ViewModes supportedUserViewModes;

    Info *info;
    GUIClient *guiClient;
    QMap<int, GUIClient*> instanceGuiClients;
    Kexi::ObjectStatus status;

#if 0 // unused now: we use class names
    /*! If you're implementing a new part, set this to value >0 in your ctor
    if you have well known (ie registered ID) for your part.
    So far, table, query, form, report and script part have defined their IDs
    (see KexiPart::ObjectTypes). */
    int registeredPartID;
#endif

bool newObjectsAreDirty : 1;

bool instanceActionsInitialized : 1;
};
}

//----------------------------------------------------------------

using namespace KexiPart;

Part::Part(QObject *parent, const QStringList &)
        : QObject(parent)
        , d(new Private())
{
    d->info = 0;
//    d->registeredPartID = partID;
    d->supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
    d->supportedUserViewModes = Kexi::DataViewMode;
}

Part::Part(QObject* parent, StaticInfo *info)
        : QObject(parent)
        , d(new Private())
{
    setObjectName("StaticPart");
    d->info = info;
    d->supportedViewModes = Kexi::DesignViewMode;
    d->supportedUserViewModes = 0;
}

Part::~Part()
{
    delete d;
}

void Part::createGUIClients()//KexiMainWindow *win)
{
// d->mainWin = win;
    if (!d->guiClient) {
        //create part's gui client
        d->guiClient = new GUIClient(this, false, "part");

        //default actions for part's gui client:
        KAction *act = new KAction(
            KIcon(info()->createItemIcon()),
            instanceCaption() + "...",
            KexiMainWindowIface::global()->actionCollection()
        );
        act->setObjectName(KexiPart::nameForCreateAction(*info()));
        QString defaultToolTip(i18n("Create new object of type \"%1\"", instanceCaption().toLower()));
        act->setToolTip(internalPropertyValue("instanceToolTip", defaultToolTip).toString());
        QString defaultWhatsThis(i18n("Creates new object of type \"%1\"", instanceCaption().toLower()));
        act->setWhatsThis(internalPropertyValue("instanceWhatsThis", defaultWhatsThis).toString());
        connect(act, SIGNAL(triggered()), this, SLOT(slotCreate()));
        KexiMainWindowIface::global()->actionCollection()->addAction(act->objectName(), act);
#ifdef __GNUC__
#warning TODO  KexiMainWindowIface::global()->guiFactory()->addClient(d->guiClient); //this client is added permanently
#else
#pragma WARNING( TODO  KexiMainWindowIface::global()->guiFactory()->addClient(d->guiClient); )
#endif

        //default actions for part instance's gui client:
        //NONE
        //let init specific actions for part instances
        for (int mode = 1; mode <= 0x01000; mode <<= 1) {
            if (d->supportedViewModes & (Kexi::ViewMode)mode) {
                GUIClient *instanceGuiClient = new GUIClient(
                    this, true, Kexi::nameForViewMode((Kexi::ViewMode)mode).toLatin1());
                d->instanceGuiClients.insert((Kexi::ViewMode)mode, instanceGuiClient);
            }
        }
        // also add an instance common for all modes (mode==0)
        GUIClient *instanceGuiClient = new GUIClient(this, true, "allViews");
        d->instanceGuiClients.insert(Kexi::AllViewModes, instanceGuiClient);

//todo
        initPartActions();
//  initActions();
    }
}

KActionCollection* Part::actionCollectionForMode(Kexi::ViewMode viewMode) const
{
    KXMLGUIClient *cli = d->instanceGuiClients.value((int)viewMode);
    return cli ? cli->actionCollection() : 0;
}

KAction* Part::createSharedAction(Kexi::ViewMode mode, const QString &text,
                                  const QString &pix_name, const KShortcut &cut, const char *name,
                                  const char *subclassName)
{
    GUIClient *instanceGuiClient = d->instanceGuiClients.value((int)mode);
    if (!instanceGuiClient) {
        kDebug() << "no gui client for mode " << mode << "!";
        return 0;
    }
    return KexiMainWindowIface::global()->createSharedAction(text, pix_name, cut, name,
            instanceGuiClient->actionCollection(), subclassName);
}

KAction* Part::createSharedPartAction(const QString &text,
                                      const QString &pix_name, const KShortcut &cut, const char *name,
                                      const char *subclassName)
{
    if (!d->guiClient)
        return 0;
    return KexiMainWindowIface::global()->createSharedAction(text, pix_name, cut, name,
            d->guiClient->actionCollection(), subclassName);
}

KAction* Part::createSharedToggleAction(Kexi::ViewMode mode, const QString &text,
                                        const QString &pix_name, const KShortcut &cut, const char *name)
{
    return createSharedAction(mode, text, pix_name, cut, name, "KToggleAction");
}

KAction* Part::createSharedPartToggleAction(const QString &text,
        const QString &pix_name, const KShortcut &cut, const char *name)
{
    return createSharedPartAction(text, pix_name, cut, name, "KToggleAction");
}

/*KAction* Part::sharedAction(int mode, const char* name, const char *classname)
{
  GUIClient *instanceGuiClient = d->instanceGuiClients[mode];
  if (!instanceGuiClient) {
    kDebug() << "no gui client for mode " << mode << "!";
    return 0;
  }
  return instanceGuiClient->actionCollection()->action(name, classname);
}

KAction* Part::sharedPartAction(int mode, const char* name, const char *classname)
{
  if (!d->guiClient)
    return 0;
  return d->guiClient->actionCollection()->action(name, classname);
}*/

void Part::setActionAvailable(const char *action_name, bool avail)
{
    for (QMap<int, GUIClient*>::Iterator it = d->instanceGuiClients.begin(); it != d->instanceGuiClients.end(); ++it) {
        QAction *act = it.value()->actionCollection()->action(action_name);
        if (act) {
            act->setEnabled(avail);
            return;
        }
    }
    KexiMainWindowIface::global()->setActionAvailable(action_name, avail);
}

KexiWindow* Part::openInstance(QWidget* parent, KexiPart::Item &item, Kexi::ViewMode viewMode,
                               QMap<QString, QVariant>* staticObjectArgs)
{
    //now it's the time for creating instance actions
    if (!d->instanceActionsInitialized) {
        initInstanceActions();
        d->instanceActionsInitialized = true;
    }

    d->status.clearStatus();
    KexiWindow *window = new KexiWindow(parent,
                                        d->supportedViewModes, *this, item);

    KexiProject *project = KexiMainWindowIface::global()->project();
    KexiDB::SchemaData sdata(project->idForClass(d->info->partClass())); // d->info->projectPartID());
    sdata.setName(item.name());
    sdata.setCaption(item.caption());
    sdata.setDescription(item.description());

    /*! @todo js: apply settings for caption displaying method; there can be option for
     - displaying item.caption() as caption, if not empty, without instanceName
     - displaying the same as above in tabCaption (or not) */
    window->setId(item.identifier()); //not needed, but we did it
    window->setWindowIcon(SmallIcon(window->itemIcon()));
#ifdef __GNUC__
#warning todo
#else
#pragma WARNING( todo )
#endif
#if 0
    if (window->mdiParent())
        window->mdiParent()->setIcon(*window->icon());
#endif
// window->setWindowIcon( *window->icon() );
    window->setData(createWindowData(window));

    if (!item.neverSaved()) {
        //we have to load schema data for this dialog
        window->setSchemaData(loadSchemaData(window, sdata, viewMode));
        if (!window->schemaData()) {
            //last chance:
            if (false == d->askForOpeningInTextMode(
                        window, item, window->supportedViewModes(), viewMode)) {
                delete window;
                return 0;
            }
            viewMode = Kexi::TextViewMode;
            window->setSchemaData(loadSchemaData(window, sdata, viewMode));
        }
        if (!window->schemaData()) {
            if (!d->status.error())
                d->status = Kexi::ObjectStatus(KexiMainWindowIface::global()->project()->dbConnection(),
                                               i18n("Could not load object's definition."), i18n("Object design may be corrupted."));
            d->status.append(
                Kexi::ObjectStatus(i18n("You can delete \"%1\" object and create it again.",
                                        item.name()), QString()));

            window->close();
            delete window;
            return 0;
        }
    }

    bool switchingFailed = false;
    bool dummy;
    tristate res = window->switchToViewMode(viewMode, staticObjectArgs, dummy);
    if (!res) {
        tristate askForOpeningInTextModeRes
        = d->askForOpeningInTextMode(window, item, window->supportedViewModes(), viewMode);
        if (true == askForOpeningInTextModeRes) {
            delete window->schemaData(); //old one
            window->close();
            delete window;
            //try in text mode
            return openInstance(parent, item, Kexi::TextViewMode, staticObjectArgs);
        } else if (false == askForOpeningInTextModeRes) {
            delete window->schemaData(); //old one
            window->close();
            delete window;
            kexiwarn << "Part::openInstance() !window, cannot switch to a view mode " <<
            Kexi::nameForViewMode(viewMode);
            return 0;
        }
        //the window has an error info
        switchingFailed = true;
    }
    if (~res)
        switchingFailed = true;

    if (switchingFailed) {
        d->status = window->status();
        window->close();
        delete window;
        kexiwarn << "Part::openInstance() !window, switching to view mode failed, " <<
        Kexi::nameForViewMode(viewMode);
        return 0;
    }
    window->registerWindow(); //ok?
    window->show();

#ifdef __GNUC__
#warning Part::openInstance(): resize window->resize(window->sizeHint()) for standalone windows
#else
#pragma WARNING( Part::openInstance(): resize window->resize(window->sizeHint()) for standalone windows )
#endif
#if 0
    if (window->mdiParent() && window->mdiParent()->state() == KexiMdiMainFrm::Normal) //only resize the window if it is in normal state
        window->resize(window->sizeHint());
#endif
    window->setMinimumSize(window->minimumSizeHint().width(), window->minimumSizeHint().height());

    //dirty only if it's a new object
    if (window->selectedView())
        window->selectedView()->setDirty(
            internalPropertyValue("newObjectsAreDirty", false).toBool() ? item.neverSaved() : false);

    kDebug() << "window returned.";
    return window;
}

void Part::slotCreate()
{
    emit newObjectRequest(d->info);
}

KexiDB::SchemaData* Part::loadSchemaData(KexiWindow *window, const KexiDB::SchemaData& sdata,
        Kexi::ViewMode viewMode)
{
    Q_UNUSED(window);
    Q_UNUSED(viewMode);
    KexiDB::SchemaData *new_schema = new KexiDB::SchemaData();
    *new_schema = sdata;
    return new_schema;
}

bool Part::loadDataBlock(KexiWindow *window, QString &dataString, const QString& dataID)
{
    if (!KexiMainWindowIface::global()->project()->dbConnection()->loadDataBlock(
                window->id(), dataString, dataID)) {
        d->status = Kexi::ObjectStatus(KexiMainWindowIface::global()->project()->dbConnection(),
                                       i18n("Could not load object's data."), i18n("Data identifier: \"%1\".", dataID));
        d->status.append(*window);
        return false;
    }
    return true;
}

void Part::initPartActions()
{
}

void Part::initInstanceActions()
{
}

bool Part::remove(KexiPart::Item &item)
{
    KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    if (!conn)
        return false;
    return conn->removeObject(item.identifier());
}

KexiWindowData* Part::createWindowData(KexiWindow* window)
{
    return new KexiWindowData(window);
}

KLocalizedString Part::i18nMessage(const QString& englishMessage, KexiWindow* window) const
{
    Q_UNUSED(window);
    if (QString(englishMessage).startsWith(":"))
        return KLocalizedString();
    return ki18n(englishMessage.toLatin1());
}

void Part::setupCustomPropertyPanelTabs(KTabWidget *)
{
}

QString Part::instanceName() const
{
    // "instanceName" should be already valid identifier but we're using
    // KexiUtils::string2Identifier() to be sure translators did it right.
    return KexiUtils::string2Identifier(
               internalPropertyValue("instanceName",
                                     i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
                                           "Use '_' character instead of spaces. First character should be a..z character. "
                                           "If you cannot use latin characters in your language, use english word.",
                                           "object")).toString()).toLower();
}

QString Part::instanceCaption() const
{
    return internalPropertyValue("instanceCaption", i18n("Object")).toString();
}

tristate Part::rename(KexiPart::Item &item, const QString& newName)
{
    Q_UNUSED(item);
    Q_UNUSED(newName);
    return true;
}

GUIClient* Part::instanceGuiClient(Kexi::ViewMode mode) const
{
    return d->instanceGuiClients.value((int)mode);
}

Kexi::ViewModes Part::supportedViewModes() const
{
    return d->supportedViewModes;
}

Kexi::ViewModes Part::supportedUserViewModes() const
{
    return d->supportedUserViewModes;
}

Info* Part::info() const
{
    return d->info;
}

GUIClient* Part::guiClient() const
{
    return d->guiClient;
}

const Kexi::ObjectStatus& Part::lastOperationStatus() const
{
    return d->status;
}

void Part::setInfo(Info *info)
{
    d->info = info;
}

/*
int Part::registeredPartID() const
{
    return d->registeredPartID;
}*/

void Part::setSupportedViewModes(Kexi::ViewModes modes)
{
    d->supportedViewModes = modes;
}

void Part::setSupportedUserViewModes(Kexi::ViewModes modes)
{
    d->supportedUserViewModes = modes;
}

KEXICORE_EXPORT QString KexiPart::fullCaptionForItem(KexiPart::Item& item, KexiPart::Part *part)
{
    if (part)
        return item.name() + " : " + part->instanceCaption();
    return item.name();
}

//-------------------------------------------------------------------------

GUIClient::GUIClient(Part* part, bool partInstanceClient, const char* nameSuffix)
        : QObject(part)
#ifdef __GNUC__
#warning TODO , KXMLGUIClient(*KexiMainWindowIface::global()->guiClient())
#else
#pragma WARNING( TODO: KXMLGUIClient(*KexiMainWindowIface::global()->guiClient()) )
#endif
{
    setObjectName(
        part->info()->objectName()
        + (nameSuffix ? QString(":%1").arg(nameSuffix) : QString()));

    if (!KexiMainWindowIface::global()->project()->data()->userMode()) {
        const QString file( QString::fromLatin1("kexi") + part->info()->objectName()
                     + "part" + (partInstanceClient ? "inst" : "") + "ui.rc" );
        const QString filter = componentData().componentName() + '/' + file;
        const QStringList allFiles = componentData().dirs()->findAllResources("data", filter) +
                                     componentData().dirs()->findAllResources("data", file);
        if (!allFiles.isEmpty()) {
            QString doc;
            if (!findMostRecentXMLFile(allFiles, doc).isEmpty())
              setXMLFile(file);
        }
    }

// new KAction(part->d->names["new"], part->info()->itemIcon(), 0, this,
//  SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").toLatin1());

// new KAction(i18nInstanceName+"...", part->info()->itemIcon(), 0, this,
//  SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").toLatin1());

// win->guiFactory()->addClient(this);
}

#include "kexipart.moc"
