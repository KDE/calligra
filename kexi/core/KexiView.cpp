/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiView.h"
#include "KexiMainWindowIface.h"

#include "KexiWindow.h"
#include "kexiproject.h"
#include <koproperty/set.h>

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexiutils/utils.h>
#include <kexiutils/SmallToolButton.h>
#include <kexiutils/FlowLayout.h>

#include <KDebug>
#include <KDialog>
#include <KActionCollection>
#include <QEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>

/*class KexiToggleViewModeAction::Private
{
  public:
    Private()
    {
    }
  Kexi::ViewMode mode;
};*/

KexiToggleViewModeAction::KexiToggleViewModeAction(
    Kexi::ViewMode mode, QObject* parent)//, QObject* receiver, const char* slot)
        : KAction(
            KIcon(Kexi::iconNameForViewMode(mode)),
            Kexi::nameForViewMode(mode, true/*withAmpersand*/),
            parent)
// , d( new Private )
{
// d->mode = mode;
// connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
// if (receiver && slot)
//  connect(this, SIGNAL(switchedTo(Kexi::ViewMode)), receiver, slot);
    setCheckable(true);
    if (mode == Kexi::DataViewMode) {
        setObjectName("view_data_mode");
        setToolTip(i18n("Switch to data view"));
        setWhatsThis(i18n("Switches to data view."));
    } else if (mode == Kexi::DesignViewMode) {
        setObjectName("view_design_mode");
        setToolTip(i18n("Switch to design view"));
        setWhatsThis(i18n("Switches to design view."));
    } else if (mode == Kexi::TextViewMode) {
        setObjectName("view_text_mode");
        setToolTip(i18n("Switch to text view"));
        setWhatsThis(i18n("Switches to text view."));
    } else {
        kexiwarn << "KexiToggleViewModeAction: invalid mode " << mode << endl;
    }
}

/*
void KexiToggleViewModeAction::slotToggled(bool checked)
{
  if (!checked)
    return;
  emit switchedTo(d->mode);
}*/

//-------------------------

class KexiView::Private
{
public:
    Private()
    //: viewModeGroup(0)
            : viewWidget(0)
            , parentView(0)
            , newlyAssignedID(-1)
            , viewMode(Kexi::NoViewMode) //unknown!
            , isDirty(false)
            , slotSwitchToViewModeInternalEnabled(true)
            , recentResultOfSwitchToViewModeInternal(true) {
    }

    ~Private() {
        /*   qDeleteAll(viewActions); */
        /*   foreach(KAction* action, viewActions) {
                //only delete action if it is not shared (otherwise, window itself will delete it)
                if (!window->sharedViewAction(action->name()))
                  delete action;
              }*/
    }

    void toggleViewModeButtonBack(Kexi::ViewMode mode) {
        QAction *a = toggleViewModeActions.value(mode);
        if (a) {
            slotSwitchToViewModeInternalEnabled = false;
            toggleViewModeActions.value(mode)->blockSignals(true);
            toggleViewModeButtons.value(mode)->blockSignals(true);
            toggleViewModeButtons.value(mode)->setChecked(viewMode == mode);
            toggleViewModeActions.value(mode)->blockSignals(false);
            toggleViewModeButtons.value(mode)->blockSignals(false);
            //a->setChecked(viewMode == mode);
            slotSwitchToViewModeInternalEnabled = true;
        }
    }

    QVBoxLayout* mainLyr;
    QWidget *topBarHWidget;
    KexiFlowLayout *topBarLyr;
    //QActionGroup* viewModeGroup;
    QHash<Kexi::ViewMode, QAction*> toggleViewModeActions;
    QHash<Kexi::ViewMode, KexiSmallToolButton*> toggleViewModeButtons;

    KexiSmallToolButton* saveDesignButton;
//  KexiToolBarSeparator* saveDesignButtonSeparator;

    QString defaultIconName;
    KexiWindow *window;
    QWidget *viewWidget;
    KexiView *parentView;

    QPointer<QWidget> lastFocusedChildBeforeFocusOut;

    /*! Member set to newly assigned object's ID in storeNewData()
     and used in storeDataBlock(). This is needed because usually,
     storeDataBlock() can be called from storeNewData() and in this case
     window has not yet assigned valid identifier (it has just negative temp. number).
     \sa KexiWindow::id()
     */
    int newlyAssignedID;

    /*! Mode for this view. Initialized by KexiWindow::switchToViewMode().
     Can be useful when single class is used for more than one view (e.g. KexiDBForm). */
    Kexi::ViewMode viewMode;

    QList<KexiView*> children;

    /*! View-level actions (not shared), owned by the view. */
    QList<QAction*> viewActions;
    QHash<QByteArray, QAction*> viewActionsHash;

bool isDirty : 1;
    //! Used in slotSwitchToViewModeInternal() to disabling it
bool slotSwitchToViewModeInternalEnabled : 1;
    //! Used in slotSwitchToViewModeInternal() to disabling d->window->switchToViewModeInternal(mode) call.
    //! Needed because there is another slotSwitchToViewModeInternal() calls if d->window->switchToViewModeInternal(mode)
    //! did not succeed, so for the second time we block this call.
    tristate recentResultOfSwitchToViewModeInternal;
};

//----------------------------------------------------------

KexiView::KexiView(QWidget *parent)
        : QWidget(parent)
        , KexiActionProxy(this)
        , d(new Private())
{
    QWidget *wi = this;
    while ((wi = wi->parentWidget()) && !wi->inherits("KexiWindow"))
        ;
    d->window = (wi && wi->inherits("KexiWindow")) ? static_cast<KexiWindow*>(wi) : 0;
    if (d->window) {
        //init view mode number for this view (obtained from window where this view is created)
        if (d->window->supportsViewMode(d->window->creatingViewsMode()))
            d->viewMode = d->window->creatingViewsMode();
    }
    setObjectName(
        QString("%1_for_%2_object")
        .arg(Kexi::nameForViewMode(d->viewMode).replace(" ", "_"))
        .arg(d->window ? d->window->partItem()->name() : QString("??")));

    installEventFilter(this);

    QLayout *l = layout();
    d->mainLyr = new QVBoxLayout(this);
    d->mainLyr->setContentsMargins(0, KDialog::marginHint() / 3, 0, 0);

    if (parentWidget()->inherits("KexiWindow")) {
        d->topBarHWidget = new QWidget(this);
        d->topBarHWidget->setFont(KexiUtils::smallFont());
        d->mainLyr->addWidget(d->topBarHWidget);
        QHBoxLayout *topBarHLyr = new QHBoxLayout(d->topBarHWidget); //needed unless KexiFlowLayout properly handles contents margins
        topBarHLyr->setContentsMargins(0, 0, 0, 0);
        topBarHLyr->addSpacing(KDialog::marginHint() / 2);
        d->topBarLyr = new KexiFlowLayout(topBarHLyr, KDialog::marginHint() / 2, 2);

        const bool userMode = KexiMainWindowIface::global()->userMode();

        if (userMode
                || d->window->supportedViewModes() == Kexi::DataViewMode
                || d->window->supportedViewModes() == Kexi::DesignViewMode
                || d->window->supportedViewModes() == Kexi::TextViewMode) {
            // nothing to do: only single view mode supported
        } else {
            if (parentWidget()->inherits("KexiWindow")) {
                createViewModeToggleButtons();
            }
        }

        QAction * a;
        if (d->viewMode == Kexi::DesignViewMode || d->viewMode == Kexi::TextViewMode) {
            d->topBarLyr->addWidget(new KexiToolBarSeparator(d->topBarHWidget));

            a = sharedAction("project_save");
            d->saveDesignButton = new KexiSmallToolButton(a, d->topBarHWidget);
            d->saveDesignButton->setText(i18n("Save Design"));
            d->saveDesignButton->setToolTip(i18n("Save current design"));
            d->saveDesignButton->setWhatsThis(i18n("Saves changes made to the current design."));
            d->topBarLyr->addWidget(d->saveDesignButton);
//   d->topBarLyr->addWidget( d->saveDesignButtonSeparator = new KexiToolBarSeparator(d->topBarHWidget));
        } else {
            d->saveDesignButton = 0;
        }
    } else {
        // no toolbar
        d->saveDesignButton = 0;
        d->topBarHWidget = 0;
        d->topBarLyr = 0;
    }
}

KexiView::~KexiView()
{
    delete d;
}

KexiWindow* KexiView::window() const
{
    return d->window;
}

bool KexiView::isDirty() const
{
    return d->isDirty;
}

Kexi::ViewMode KexiView::viewMode() const
{
    return d->viewMode;
}

KexiPart::Part* KexiView::part() const
{
    return d->window ? d->window->part() : 0;
}

tristate KexiView::beforeSwitchTo(Kexi::ViewMode mode, bool & dontStore)
{
    Q_UNUSED(mode);
    Q_UNUSED(dontStore);
    return true;
}

tristate KexiView::afterSwitchFrom(Kexi::ViewMode mode)
{
    Q_UNUSED(mode);
    return true;
}

QSize KexiView::preferredSizeHint(const QSize& otherSize)
{
#ifdef __GNUC__
#warning KexiView::preferredSizeHint()
#else
#pragma WARNING( KexiView::preferredSizeHint() )
#endif
#if 0 //todo
    KexiWindow* w = d->window;
    if (dlg && dlg->mdiParent()) {
        QRect r = dlg->mdiParent()->mdiAreaContentsRect();
        return otherSize.boundedTo(QSize(
                                       r.width() - 10,
                                       r.height() - dlg->mdiParent()->captionHeight() - dlg->pos().y() - 10
                                   ));
    }
#endif
    return otherSize;
}

void KexiView::closeEvent(QCloseEvent * e)
{
    bool cancel = false;
    emit closing(cancel);
    if (cancel) {
        e->ignore();
        return;
    }
    QWidget::closeEvent(e);
}

KoProperty::Set *KexiView::propertySet()
{
    return 0;
}

void KexiView::propertySetSwitched()
{
    if (window())
        KexiMainWindowIface::global()->propertySetSwitched(window(), false);
}

void KexiView::propertySetReloaded(bool preservePrevSelection,
                                   const QByteArray& propertyToSelect)
{
    if (window())
        KexiMainWindowIface::global()->propertySetSwitched(
            window(), true, preservePrevSelection, propertyToSelect);
}

void KexiView::setDirty(bool set)
{
    const bool changed = (d->isDirty != set);
    d->isDirty = set;
    d->isDirty = isDirty();
    if (d->saveDesignButton)
        d->saveDesignButton->setEnabled(d->isDirty);
    if (d->parentView) {
        d->parentView->setDirty(d->isDirty);
    } else {
        if (changed && d->window)
            d->window->dirtyChanged(this);
    }
}

/*bool KexiView::saveData()
{
  //TODO....

  //finally:
  setDirty(false);
  return true;
}*/

KexiDB::SchemaData* KexiView::storeNewData(const KexiDB::SchemaData& sdata, bool & /*cancel*/)
{
    KexiDB::SchemaData *new_schema = new KexiDB::SchemaData();
    *new_schema = sdata;

    if (!KexiMainWindowIface::global()->project()->dbConnection()
            ->storeObjectSchemaData(*new_schema, true)) {
        delete new_schema;
        new_schema = 0;
    }
    d->newlyAssignedID = new_schema->id();
    return new_schema;
}

tristate KexiView::storeData(bool dontAsk)
{
    Q_UNUSED(dontAsk);
    if (!d->window || !d->window->schemaData())
        return false;
    if (!KexiMainWindowIface::global()->project()->dbConnection()
            ->storeObjectSchemaData(*d->window->schemaData(), false /*existing object*/)) {
        return false;
    }
    setDirty(false);
    return true;
}

bool KexiView::loadDataBlock(QString &dataString, const QString& dataID, bool canBeEmpty)
{
    if (!d->window)
        return false;
    const tristate res = KexiMainWindowIface::global()->project()->dbConnection()
                         ->loadDataBlock(d->window->id(), dataString, dataID);
    if (canBeEmpty && ~res) {
        dataString.clear();
        return true;
    }
    return res == true;
}

bool KexiView::storeDataBlock(const QString &dataString, const QString &dataID)
{
    if (!d->window)
        return false;
    int effectiveID;
    if (d->newlyAssignedID > 0) {//ID not yet stored within window, but we've got ID here
        effectiveID = d->newlyAssignedID;
        d->newlyAssignedID = -1;
    } else
        effectiveID = d->window->id();

    return effectiveID > 0
           && KexiMainWindowIface::global()->project()->dbConnection()->storeDataBlock(
               effectiveID, dataString, dataID);
}

bool KexiView::removeDataBlock(const QString& dataID)
{
    if (!d->window)
        return false;
    return KexiMainWindowIface::global()->project()->dbConnection()
           ->removeDataBlock(d->window->id(), dataID);
}

bool KexiView::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut) {// && o->inherits("QWidget")) {
//  //hp==true if currently focused widget is a child of this table view
//  const bool hp = Kexi::hasParent( static_cast<QWidget*>(o), focusWidget());
        kexidbg << "KexiView::eventFilter(): this=[" << o->metaObject()->className()
        << " " << objectName() << "] o=[" << o->metaObject()->className() << " " << o->objectName()
        << "] focusWidget=[" << (qApp->focusWidget() ? qApp->focusWidget()->metaObject()->className() : QString()) << " "
        << (qApp->focusWidget() ? qApp->focusWidget()->objectName() : QString()) << "] ev.type=" << e->type() << endl;
        if (KexiUtils::hasParent(this, o)) {
            if (e->type() == QEvent::FocusOut && qApp->focusWidget()
                    && !KexiUtils::hasParent(this, qApp->focusWidget())) {
                //focus out: when currently focused widget is not a parent of this view
                emit focus(false);
            } else if (e->type() == QEvent::FocusIn) {
                emit focus(true);
            }
            if (e->type() == QEvent::FocusOut) {
//    kDebug() << focusWidget()->className() << " " << focusWidget()->name()<< endl;
//    kDebug() << o->className() << " " << o->name()<< endl;
                KexiView *v = KexiUtils::findParent<KexiView*>(o);
                if (v) {
                    while (v->d->parentView)
                        v = v->d->parentView;
                    if (KexiUtils::hasParent(this, static_cast<QWidget*>(v->focusWidget())))
                        v->d->lastFocusedChildBeforeFocusOut = static_cast<QWidget*>(v->focusWidget());
                }
            }

            if (e->type() == QEvent::FocusIn && m_actionProxyParent) {
                m_actionProxyParent->m_focusedChild = this;
            }
        }
    }
    return false;
}

void KexiView::setViewWidget(QWidget* w, bool focusProxy)
{
    if (d->viewWidget == w)
        return;
    if (d->viewWidget) {
        d->viewWidget->removeEventFilter(this);
        d->mainLyr->removeWidget(d->viewWidget);
    }
    d->viewWidget = w;
    if (d->viewWidget) {
        //if (!d->children.contains(dynamic_cast<KexiView*>(d->viewWidget))) {
        d->viewWidget->setParent(this);
        d->mainLyr->addWidget(d->viewWidget);
        d->viewWidget->installEventFilter(this);
        //}
        if (focusProxy)
            setFocusProxy(d->viewWidget); //js: ok?
    }
}

void KexiView::addChildView(KexiView* childView)
{
    d->children.append(childView);
    addActionProxyChild(childView);
    childView->d->parentView = this;

    //childView->setParent(this);
    //d->mainLyr->addWidget(childView);

// if (d->parentView)
//  childView->installEventFilter(d->parentView);
    childView->installEventFilter(this);
}

void KexiView::setFocus()
{
    if (!d->lastFocusedChildBeforeFocusOut.isNull()) {
//  kDebug() << "FOCUS: " << d->lastFocusedChildBeforeFocusOut->className() << " " << d->lastFocusedChildBeforeFocusOut->name()<< endl;
        QWidget *w = d->lastFocusedChildBeforeFocusOut;
        d->lastFocusedChildBeforeFocusOut = 0;
        w->setFocus();
    } else {
        if (hasFocus())
            setFocusInternal();
        else
            setFocusInternal();
    }
    KexiMainWindowIface::global()->invalidateSharedActions(this);
}

QAction* KexiView::sharedAction(const char *action_name)
{
    if (part()) {
        KActionCollection *ac;
        if ((ac = part()->actionCollectionForMode(viewMode()))) {
            QAction* a = ac->action(action_name);
            if (a)
                return a;
        }
    }
    return KexiActionProxy::sharedAction(action_name);
}

void KexiView::setAvailable(const char* action_name, bool set)
{
    if (part()) {
        KActionCollection *ac;
        QAction* a;
        if ((ac = part()->actionCollectionForMode(viewMode())) && (a = ac->action(action_name))) {
            a->setEnabled(set);
        }
    }
    KexiActionProxy::setAvailable(action_name, set);
}

void KexiView::updateActions(bool activated)
{
    //do nothing here
    //do the same for children :)
    foreach(KexiView* view, d->children)
    view->updateActions(activated);
}

void KexiView::setViewActions(const QList<QAction*>& actions)
{
    d->viewActions = actions;
    d->viewActionsHash.clear();
    foreach(QAction* action, d->viewActions)
    d->viewActionsHash.insert(action->objectName().toLatin1(), action);
}

QAction* KexiView::viewAction(const char* name) const
{
    return d->viewActionsHash.value(name);
}

QList<QAction*> KexiView::viewActions() const
{
    return d->viewActions;
}

void KexiView::toggleViewModeButtonBack()
{
// d->switchToViewModeInternalEnabled = false;
    d->toggleViewModeButtonBack(Kexi::DataViewMode);
    d->toggleViewModeButtonBack(Kexi::DesignViewMode);
    d->toggleViewModeButtonBack(Kexi::TextViewMode);
// d->switchToViewModeInternalEnabled = true;
}

void KexiView::createViewModeToggleButtons()
{
    KAction * a;
    KexiSmallToolButton *btn;
// d->viewModeGroup = new QActionGroup(this);
    QLabel *showLabel = new QLabel(i18n("Show:"), d->topBarHWidget);
    d->topBarLyr->addWidget(showLabel);
    d->topBarLyr->setAlignment(showLabel, Qt::AlignVCenter | Qt::AlignLeft);
    if (d->window->supportsViewMode(Kexi::DataViewMode)) {
        a = new KexiToggleViewModeAction(Kexi::DataViewMode, this);
//   this, SLOT(slotSwitchToViewModeInternal(Kexi::ViewMode)));
        d->toggleViewModeActions.insert(Kexi::DataViewMode, a);
//  d->viewModeGroup->addAction(a);
        btn = new KexiSmallToolButton(a, d->topBarHWidget);
        connect(btn, SIGNAL(toggled(bool)), this, SLOT(slotSwitchToDataViewModeInternal(bool)));
        d->toggleViewModeButtons.insert(Kexi::DataViewMode, btn);
        btn->setText(i18n("Data"));
        d->topBarLyr->addWidget(btn);
    }
    if (d->window->supportsViewMode(Kexi::DesignViewMode)) {
        a = new KexiToggleViewModeAction(Kexi::DesignViewMode, this);
//   this, SLOT(slotSwitchToViewModeInternal(Kexi::ViewMode)));
        d->toggleViewModeActions.insert(Kexi::DesignViewMode, a);
//  d->viewModeGroup->addAction(a);
        btn = new KexiSmallToolButton(a, d->topBarHWidget);
        connect(btn, SIGNAL(toggled(bool)), this, SLOT(slotSwitchToDesignViewModeInternal(bool)));
        d->toggleViewModeButtons.insert(Kexi::DesignViewMode, btn);
        btn->setText(i18n("Design"));
        d->topBarLyr->addWidget(btn);
    }
    if (d->window->supportsViewMode(Kexi::TextViewMode)) {
        a = new KexiToggleViewModeAction(Kexi::TextViewMode, this);
//   this, SLOT(slotSwitchToViewModeInternal(Kexi::ViewMode)));
        QString customTextViewModeCaption(d->window->internalPropertyValue("textViewModeCaption").toString());
        if (!customTextViewModeCaption.isEmpty())
            a->setText(customTextViewModeCaption);
        d->toggleViewModeActions.insert(Kexi::TextViewMode, a);
//  d->viewModeGroup->addAction(a);
        btn = new KexiSmallToolButton(a, d->topBarHWidget);
        connect(btn, SIGNAL(toggled(bool)), this, SLOT(slotSwitchToTextViewModeInternal(bool)));
        d->toggleViewModeButtons.insert(Kexi::TextViewMode, btn);
        d->topBarLyr->addWidget(btn);
    }
    toggleViewModeButtonBack();
}

void KexiView::slotSwitchToDataViewModeInternal(bool)
{
    slotSwitchToViewModeInternal(Kexi::DataViewMode);
}

void KexiView::slotSwitchToDesignViewModeInternal(bool)
{
    slotSwitchToViewModeInternal(Kexi::DesignViewMode);
}

void KexiView::slotSwitchToTextViewModeInternal(bool)
{
    slotSwitchToViewModeInternal(Kexi::TextViewMode);
}

void KexiView::slotSwitchToViewModeInternal(Kexi::ViewMode mode)
{
    if (!d->slotSwitchToViewModeInternalEnabled)
        return;
    if (d->recentResultOfSwitchToViewModeInternal != true)
        d->recentResultOfSwitchToViewModeInternal = true;
    else
        d->recentResultOfSwitchToViewModeInternal = d->window->switchToViewModeInternal(mode);

    if (d->viewMode != mode) {
        //switch back visually
        KexiSmallToolButton *b = d->toggleViewModeButtons.value(mode);
        d->slotSwitchToViewModeInternalEnabled = false;
        b->setChecked(false);
        d->slotSwitchToViewModeInternalEnabled = true;
    }
}

void KexiView::initViewActions()
{
    if (!d->topBarLyr)
        return;
    if (!d->viewActions.isEmpty())
        d->topBarLyr->addWidget(new KexiToolBarSeparator(d->topBarHWidget));
    foreach(QAction* action, d->viewActions) {
        if (action->isSeparator()) {
            d->topBarLyr->addWidget(new KexiToolBarSeparator(d->topBarHWidget));
        } else {
            KexiSmallToolButton *btn = new KexiSmallToolButton(action, d->topBarHWidget);
            btn->setText(action->text());
            btn->setToolTip(action->toolTip());
            btn->setWhatsThis(action->whatsThis());
            d->topBarLyr->addWidget(btn);
        }
    }
}

#include "KexiView.moc"
