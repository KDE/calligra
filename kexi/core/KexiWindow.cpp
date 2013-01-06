/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2012 Jarosław Staniek <staniek@kde.org>

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

#include "KexiWindow.h"
#include "KexiWindowData.h"
#include "KexiView.h"
#include "KexiMainWindowIface.h"

#include "kexipart.h"
#include "kexistaticpart.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexiproject.h"

#include <db/connection.h>
#include <db/utils.h>
#include <kexiutils/utils.h>
#include <kexiutils/SmallToolButton.h>
#include <kexiutils/FlowLayout.h>

#include <QStackedWidget>
#include <QTimer>
#include <QEvent>
#include <QCloseEvent>

#include <KDebug>
#include <KApplication>
#include <KToolBar>
#include <KActionCollection>
#include <KDialog>

//----------------------------------------------------------

//! @internal
class KexiWindow::Private
{
public:
    Private()
            : schemaData(0)
            , schemaDataOwned(false)
            , isRegistered(false)
            , dirtyChangedEnabled(true)
            , switchToViewModeEnabled(true) {
        supportedViewModes = Kexi::NoViewMode; //will be set by KexiPart
        openedViewModes = Kexi::NoViewMode;
        currentViewMode = Kexi::NoViewMode; //no view available yet
        creatingViewsMode = Kexi::NoViewMode;
        id = -1;
        item = 0;
    }

    ~Private() {
        if (schemaDataOwned)
            delete schemaData;
    }

    QVBoxLayout* mainLyr;
    QStackedWidget* stack;

    Kexi::ViewModes supportedViewModes;
    Kexi::ViewModes openedViewModes;
    Kexi::ViewMode currentViewMode;

#ifndef KEXI_NO_CTXT_HELP
    KexiContextHelpInfo *contextHelpInfo;
#endif
    int id;
    QPointer<KexiPart::Part> part;
    KexiPart::Item *item;
    KexiDB::SchemaData* schemaData;
    bool schemaDataOwned;
    QPointer<KexiView> newlySelectedView; //!< Used in isDirty(), temporary set in switchToViewMode()
    //!< during view setup, when a new view is not yet raised.
    //! Used in viewThatRecentlySetDirtyFlag(), modified in dirtyChanged().
    QPointer<KexiView> viewThatRecentlySetDirtyFlag;
    QPointer<KexiWindowData> data; //!< temporary data shared between views

    /*! Created view's mode - helper for switchToViewMode(),
     KexiView ctor uses that info. >0 values are useful. */
    Kexi::ViewMode creatingViewsMode;

    bool isRegistered;
    bool dirtyChangedEnabled; //!< used in setDirty(), affects dirtyChanged()
    bool switchToViewModeEnabled; //!< used internally switchToViewMode() to avoid infinite loop
    QMap<Kexi::ViewMode, KexiView*> views;
};

//----------------------------------------------------------

KexiWindow::KexiWindow(QWidget *parent, Kexi::ViewModes supportedViewModes,
                       KexiPart::Part& part, KexiPart::Item& item)
        : QWidget(parent)
        , KexiActionProxy(this, KexiMainWindowIface::global())
        , d(new Private())
        , m_destroying(false)
{
    d->part = &part;
    d->item = &item;
    d->supportedViewModes = supportedViewModes;
    createSubwidgets();
#ifndef KEXI_NO_CTXT_HELP
    d->contextHelpInfo = new KexiContextHelpInfo();
#endif
    updateCaption();
}

KexiWindow::KexiWindow()
        : QWidget(0)
        , KexiActionProxy(this, KexiMainWindowIface::global())
        , d(new Private())
        , m_destroying(false)
{
    createSubwidgets();
#ifndef KEXI_NO_CTXT_HELP
    d->contextHelpInfo = new KexiContextHelpInfo();
#endif
    updateCaption();
}

KexiWindow::~KexiWindow()
{
    m_destroying = true;
    delete d;
    d = 0;
}

void KexiWindow::createSubwidgets()
{
    d->mainLyr = new QVBoxLayout(this);
    d->mainLyr->setContentsMargins(0, KDialog::marginHint() / 2, 0, 0);
    d->stack = new QStackedWidget(this);
    d->mainLyr->addWidget(d->stack);
}

KexiView *KexiWindow::selectedView() const
{
    if (m_destroying)
        return 0;
    return static_cast<KexiView*>(d->stack->currentWidget());
}

KexiView *KexiWindow::viewForMode(Kexi::ViewMode mode) const
{
    return d->views.value(mode);
}

void KexiWindow::addView(KexiView *view)
{
    addView(view, Kexi::NoViewMode);
}

void KexiWindow::addView(KexiView *view, Kexi::ViewMode mode)
{
    d->stack->addWidget(view);
    d->views.insert(mode, view);

    //set focus proxy inside this view
    QWidget *ch = KexiUtils::findFirstChild<QWidget*>(view, "QWidget");
    if (ch)
        view->setFocusProxy(ch);

    d->openedViewModes |= mode;
}

void KexiWindow::removeView(Kexi::ViewMode mode)
{
    KexiView *view = viewForMode(mode);
    if (view) {
        d->stack->removeWidget(view);
        d->views.remove(mode);
    }
    d->openedViewModes |= mode;
    d->openedViewModes ^= mode;
}

QSize KexiWindow::minimumSizeHint() const
{
    KexiView *v = selectedView();
    if (!v)
        return QWidget::minimumSizeHint();
    return v->minimumSizeHint();
}

QSize KexiWindow::sizeHint() const
{
    KexiView *v = selectedView();
    if (!v)
        return QWidget::sizeHint();
    return v->preferredSizeHint(v->sizeHint());
}

void KexiWindow::setId(int id)
{
    d->id = id;
}

KexiPart::Part* KexiWindow::part() const
{
    return d->part;
}

KexiPart::Item *KexiWindow::partItem() const
{
    return d->item;
}

bool KexiWindow::supportsViewMode(Kexi::ViewMode mode) const
{
    return d->supportedViewModes & mode;
}

Kexi::ViewModes KexiWindow::supportedViewModes() const
{
    return d->supportedViewModes;
}

Kexi::ViewMode KexiWindow::currentViewMode() const
{
    return d->currentViewMode;
}

KexiView* KexiWindow::viewThatRecentlySetDirtyFlag() const
{
    return d->viewThatRecentlySetDirtyFlag;
}

void KexiWindow::registerWindow()
{
    if (d->isRegistered)
        return;
    KexiMainWindowIface::global()->registerChild(this);
    d->isRegistered = true;
}

bool KexiWindow::isRegistered() const
{
    return d->isRegistered;
}

int KexiWindow::id() const
{
    return (partItem() && partItem()->identifier() > 0)
           ? partItem()->identifier() : d->id;
}

void KexiWindow::setContextHelp(const QString& caption,
                                const QString& text, const QString& iconName)
{
#ifdef KEXI_NO_CTXT_HELP
    Q_UNUSED(caption);
    Q_UNUSED(text);
    Q_UNUSED(iconName);
#else
    d->contextHelpInfo->caption = caption;
    d->contextHelpInfo->text = text;
    d->contextHelpInfo->text = iconName;
    updateContextHelp();
#endif
}

void KexiWindow::closeEvent(QCloseEvent * e)
{
    KexiMainWindowIface::global()->acceptPropertySetEditing();

    //let any view send "closing" signal
    /* QObjectList list = queryList( "KexiView", 0, false, false);
      foreach(QObject* obj, list) {
        KexiView *view = static_cast<KexiView*>(obj);*/
    QList<KexiView *> list(findChildren<KexiView*>());
    foreach(KexiView * view, list) {
        bool cancel = false;
        emit view->closing(cancel);
        if (cancel) {
            e->ignore();
            return;
        }
    }
    emit closing();
    QWidget::closeEvent(e);
}

bool KexiWindow::isDirty() const
{
    //look for "dirty" flag
    int m = d->openedViewModes;
    int mode = 1;
    while (m > 0) {
        if (m & 1) {
            KexiView *view = viewForMode(static_cast<Kexi::ViewMode>(mode));
            if (view && view->isDirty()) {
                return true;
            }
        }
        m >>= 1;
        mode <<= 1;
    }
    return false;
}

void KexiWindow::setDirty(bool dirty)
{
    d->dirtyChangedEnabled = false;
    int m = d->openedViewModes;
    int mode = 1;
    while (m > 0) {
        if (m & 1) {
            KexiView *view = viewForMode(static_cast<Kexi::ViewMode>(mode));
            if (view) {
                view->setDirty(dirty);
            }
        }
        m >>= 1;
        mode <<= 1;
    }
    d->dirtyChangedEnabled = true;
    dirtyChanged(d->viewThatRecentlySetDirtyFlag); //update
}

QString KexiWindow::itemIconName()
{
    if (!d->part || !d->part->info()) {
        KexiView *v = selectedView();
        if (v) {
            return v->defaultIconName();
        }
        return QString();
    }
    return d->part->info()->itemIconName();
}

KexiPart::GUIClient* KexiWindow::guiClient() const
{
    if (!d->part || d->currentViewMode == 0)
        return 0;
    return d->part->instanceGuiClient(d->currentViewMode);
}

KexiPart::GUIClient* KexiWindow::commonGUIClient() const
{
    if (!d->part)
        return 0;
    return d->part->instanceGuiClient(Kexi::AllViewModes);
}

bool KexiWindow::isDesignModePreloadedForTextModeHackUsed(Kexi::ViewMode newViewMode) const
{
    return newViewMode == Kexi::TextViewMode
           && !viewForMode(Kexi::DesignViewMode)
           && supportsViewMode(Kexi::DesignViewMode);
}

tristate KexiWindow::switchToViewMode(
    Kexi::ViewMode newViewMode,
    QMap<QString, QVariant>* staticObjectArgs,
    bool& proposeOpeningInTextViewModeBecauseOfProblems)
{
    KexiMainWindowIface::global()->acceptPropertySetEditing();

    const bool designModePreloadedForTextModeHack = isDesignModePreloadedForTextModeHackUsed(newViewMode);
    tristate res = true;
    if (designModePreloadedForTextModeHack) {
        /* A HACK: open design BEFORE text mode: otherwise Query schema becames crazy */
        bool _proposeOpeningInTextViewModeBecauseOfProblems = false; // used because even if opening the view failed,
        // text view can be opened
        res = switchToViewMode(Kexi::DesignViewMode, staticObjectArgs, _proposeOpeningInTextViewModeBecauseOfProblems);
        if ((!res && !_proposeOpeningInTextViewModeBecauseOfProblems) || ~res)
            return res;
    }

    kDebug();
    bool dontStore = false;
    KexiView *view = selectedView();

    if (d->currentViewMode == newViewMode)
        return true;
    if (!supportsViewMode(newViewMode)) {
        kWarning() << "! KexiWindow::supportsViewMode(" << Kexi::nameForViewMode(newViewMode) << ")";
        return false;
    }

    if (view) {
        res = true;
        if (view->isDataEditingInProgress()) {
            KGuiItem saveItem(KStandardGuiItem::save());
            saveItem.setText(i18n("Save Changes"));
            KGuiItem dontSaveItem(KStandardGuiItem::dontSave());
            KGuiItem cancelItem(KStandardGuiItem::cancel());
            cancelItem.setText(i18n("Do Not Switch"));
            const int res = KMessageBox::questionYesNoCancel(
                selectedView(),
                i18n("There are unsaved changes in object \"%1\". "
                     "Do you want to save these changes before switching to other view?")
                    .arg(partItem()->captionOrName()),
                    i18n("Confirm Saving Changes"),
                    saveItem, dontSaveItem, cancelItem
            );
            if (res == KMessageBox::Yes) {
                if (true != view->saveDataChanges())
                    return cancelled;
            }
            else if (res == KMessageBox::No) {
                if (true != view->cancelDataChanges())
                    return cancelled;
            }
            else { // Cancel:
                return cancelled;
            }
        }
        if (!designModePreloadedForTextModeHack) {
            res = view->beforeSwitchTo(newViewMode, dontStore);
        }
        if (~res || !res)
            return res;
        if (!dontStore && view->isDirty()) {
            res = KexiMainWindowIface::global()->saveObject(this, i18n("Design has been changed. "
                    "You must save it before switching to other view."));
            if (~res || !res)
                return res;
//   KMessageBox::questionYesNo(0, i18n("Design has been changed. You must save it before switching to other view."))
//    ==KMessageBox::No
        }
    }

    //get view for viewMode
    KexiView *newView = viewForMode(newViewMode);
    if (newView && !newView->inherits("KexiView")) {
        newView = 0;
    }
    if (!newView) {
        KexiUtils::setWaitCursor();
        //ask the part to create view for the new mode
        d->creatingViewsMode = newViewMode;
        KexiPart::StaticPart *staticPart = dynamic_cast<KexiPart::StaticPart*>((KexiPart::Part*)d->part);
        if (staticPart)
            newView = staticPart->createView(this, this, *d->item, newViewMode, staticObjectArgs);
        else
            newView = d->part->createView(this, this, *d->item, newViewMode, staticObjectArgs);
        KexiUtils::removeWaitCursor();
        if (!newView) {
            //js TODO error?
            kWarning() << "Switching to mode " << newViewMode << " failed. Previous mode "
            << d->currentViewMode << " restored.";
            return false;
        }
        d->creatingViewsMode = Kexi::NoViewMode;
        newView->initViewActions();
        addView(newView, newViewMode);
    }
    const Kexi::ViewMode prevViewMode = d->currentViewMode;
    res = true;
    if (designModePreloadedForTextModeHack) {
        d->currentViewMode = Kexi::NoViewMode; //SAFE?
    }
    res = newView->beforeSwitchTo(newViewMode, dontStore);
    proposeOpeningInTextViewModeBecauseOfProblems
    = data()->proposeOpeningInTextViewModeBecauseOfProblems;
    if (!res) {
        removeView(newViewMode);
        delete newView;
        kWarning() << "Switching to mode " << newViewMode << " failed. Previous mode "
        << d->currentViewMode << " restored.";
        return false;
    }
    d->currentViewMode = newViewMode;
    d->newlySelectedView = newView;
    if (prevViewMode == Kexi::NoViewMode)
        d->newlySelectedView->setDirty(false);

    res = newView->afterSwitchFrom(
              designModePreloadedForTextModeHack ? Kexi::NoViewMode : prevViewMode);
    proposeOpeningInTextViewModeBecauseOfProblems
        = data()->proposeOpeningInTextViewModeBecauseOfProblems;
    if (!res) {
        removeView(newViewMode);
        delete newView;
        kWarning() << "Switching to mode " << newViewMode << " failed. Previous mode "
        << prevViewMode << " restored.";
        const Kexi::ObjectStatus status(*this);
        setStatus(KexiMainWindowIface::global()->project()->dbConnection(),
                  i18n("Switching to other view failed (%1).", Kexi::nameForViewMode(newViewMode)), "");
        append(status);
        d->currentViewMode = prevViewMode;
        return false;
    }
    d->newlySelectedView = 0;
    if (~res) {
        d->currentViewMode = prevViewMode;
        return cancelled;
    }
    if (view) {
        takeActionProxyChild(view);   //take current proxy child
        // views have distinct local toolbars, and user has switched the mode button so switch it back
        //view->toggleViewModeButtonBack();
    }
    addActionProxyChild(newView);   //new proxy child
    d->stack->setCurrentWidget(newView);
    newView->propertySetSwitched();
    KexiMainWindowIface::global()->invalidateSharedActions(newView);
    newView->setFocus();
    return true;
}

tristate KexiWindow::switchToViewModeInternal(Kexi::ViewMode newViewMode)
{
    return KexiMainWindowIface::global()->switchToViewMode(*this, newViewMode);
}

tristate KexiWindow::switchToViewMode(Kexi::ViewMode newViewMode)
{
    if (newViewMode == d->currentViewMode)
        return true;
    if (!d->switchToViewModeEnabled)
        return false;
    bool dummy;
    return switchToViewMode(newViewMode, 0, dummy);
}

void KexiWindow::setFocus()
{
    if (d->stack->currentWidget()) {
        if (d->stack->currentWidget()->inherits("KexiView"))
            static_cast<KexiView*>(d->stack->currentWidget())->setFocus();
        else
            d->stack->currentWidget()->setFocus();
    } else {
        QWidget::setFocus();
    }
    activate();
}

KoProperty::Set*
KexiWindow::propertySet()
{
    KexiView *v = selectedView();
    if (!v)
        return 0;
    return v->propertySet();
}

void KexiWindow::setSchemaData(KexiDB::SchemaData* schemaData)
{
    d->schemaData = schemaData;
}

KexiDB::SchemaData* KexiWindow::schemaData() const
{
    return d->schemaData;
}

void KexiWindow::setSchemaDataOwned(bool set)
{
    d->schemaDataOwned = set;
}

KexiWindowData *KexiWindow::data() const
{
    return d->data;
}

void KexiWindow::setData(KexiWindowData* data)
{
    if (data != d->data)
        delete(KexiWindowData*)d->data;
    d->data = data;
}

bool KexiWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (QWidget::eventFilter(obj, e))
        return true;
    /*if (e->type()==QEvent::FocusIn) {
      QWidget *w = m_parentWindow->activeWindow();
      w=0;
    }*/
    if ((e->type() == QEvent::FocusIn && KexiMainWindowIface::global()->currentWindow() == this)
            || e->type() == QEvent::MouseButtonPress) {
        if (d->stack->currentWidget() && KexiUtils::hasParent(d->stack->currentWidget(), obj)) {
            //pass the activation
            activate();
        }
    }
    return false;
}

void KexiWindow::dirtyChanged(KexiView* view)
{
    if (!d->dirtyChangedEnabled)
        return;
    d->viewThatRecentlySetDirtyFlag = isDirty() ? view : 0;
    updateCaption();
    emit dirtyChanged(this);
}

void KexiWindow::updateCaption()
{
    if (!d->item || !d->part)
        return;
    //! @todo use d->item->captionOrName() if defined in settings
    QString fullCapt(d->item->name());
    setWindowTitle(fullCapt + (isDirty() ? "*" : ""));
}

bool KexiWindow::neverSaved() const
{
    return d->item ? d->item->neverSaved() : true;
}

tristate KexiWindow::storeNewData(KexiView::StoreNewDataOptions options)
{
    if (!neverSaved())
        return false;
    KexiView *v = selectedView();
    if (d->schemaData)
        return false; //schema must not exist
    if (!v)
        return false;
    //create schema object and assign information
    KexiProject *project = KexiMainWindowIface::global()->project();
    KexiDB::SchemaData sdata(
        project->idForClass(d->part->info()->partClass()));
    sdata.setName(d->item->name());
    sdata.setCaption(d->item->caption());
    sdata.setDescription(d->item->description());

    KexiPart::Item* existingItem = project->item(part()->info(), sdata.name());
    if (existingItem && !(options & KexiView::OverwriteExistingObject)) {
        KMessageBox::information(this,
                                 i18n("Could not create new object.")
                                 + part()->i18nMessage("Object \"%1\" already exists.", this)
                                   .subs(sdata.name()).toString());
        return false;
    }

    bool cancel = false;
    d->schemaData = v->storeNewData(sdata, options, cancel);
    if (cancel)
        return cancelled;
    if (!d->schemaData) {
        setStatus(project->dbConnection(), i18n("Saving object's definition failed."), "");
        return false;
    }

    if (project->idForClass(part()->info()->partClass()) < 0) {
        if (!project->createIdForPart(*part()->info()))
            return false;
    }
    /* Sets 'dirty' flag on every dialog's view. */
    setDirty(false);
    //new schema data has now ID updated to a unique value
    //-assign that to item's identifier
    d->item->setIdentifier(d->schemaData->id());
    project->addStoredItem(part()->info(), d->item);

    return true;
}

tristate KexiWindow::storeData(bool dontAsk)
{
    if (neverSaved())
        return false;
    KexiView *v = selectedView();
    if (!v)
        return false;

#define storeData_ERR \
    setStatus(KexiMainWindowIface::global()->project()->dbConnection(), \
        i18n("Saving object's data failed."),"");

    //save changes using transaction
    KexiDB::Transaction transaction = KexiMainWindowIface::global()
                                      ->project()->dbConnection()->beginTransaction();
    if (transaction.isNull()) {
        storeData_ERR;
        return false;
    }
    KexiDB::TransactionGuard tg(transaction);

    const tristate res = v->storeData(dontAsk);
    if (~res) //trans. will be cancelled
        return res;
    if (!res) {
        storeData_ERR;
        return res;
    }
    if (!tg.commit()) {
        storeData_ERR;
        return false;
    }
    /* Sets 'dirty' flag on every dialog's view. */
    setDirty(false);
    return true;
}

void KexiWindow::activate()
{
    KexiView *v = selectedView();
    //kDebug() << "focusWidget(): " << focusWidget()->name();
#ifdef __GNUC__
#warning TODO KexiWindow::activate() OK instead of focusedChildWidget()?
#else
#pragma WARNING( TODO KexiWindow::activate() OK instead of focusedChildWidget()? )
#endif
    if (KexiUtils::hasParent(v, /*kde4*/ KexiMainWindowIface::global()->focusWidget()))   //QWidget::focusedChildWidget()))
#ifdef __GNUC__
#warning TODO  QWidget::activate();
#else
#pragma WARNING( TODO  QWidget::activate(); )
#endif
#if 0
        else
#endif
        {//ah, focused widget is not in this view, move focus:
            if (v)
                v->setFocus();
        }
    if (v)
        v->updateActions(true);
    //js: not neeed?? m_parentWindow->invalidateSharedActions(this);
}

void KexiWindow::deactivate()
{
    KexiView *v = selectedView();
    if (v)
        v->updateActions(false);
}

void KexiWindow::sendDetachedStateToCurrentView()
{
    KexiView *v = selectedView();
    if (v)
        v->windowDetached();
}

void KexiWindow::sendAttachedStateToCurrentView()
{
    KexiView *v = selectedView();
    if (v)
        v->windowAttached();
}

bool KexiWindow::saveSettings()
{
    bool result = true;
    for (int i = 0; i < d->stack->count(); ++i) {
        KexiView *view = qobject_cast<KexiView*>(d->stack->widget(i));
        if (!view->saveSettings()) {
            result = false;
        }
    }
    return result;
}

Kexi::ViewMode KexiWindow::creatingViewsMode() const
{
    return d->creatingViewsMode;
}

QVariant KexiWindow::internalPropertyValue(const QByteArray& name,
        const QVariant& defaultValue) const
{
    return d->part->internalPropertyValue(name, defaultValue);
}

#include "KexiWindow.moc"
