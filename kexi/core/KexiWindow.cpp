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

#include "KexiWindow.h"
#include "KexiWindowData.h"
#include "KexiView.h"
#include "KexiMainWindowIface.h"

#include "kexicontexthelp_p.h"
#include "kexipart.h"
#include "kexistaticpart.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexiproject.h"

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexiutils/utils.h>
#include <kexiutils/SmallToolButton.h>
#include <kexiutils/FlowLayout.h>

#include <QStackedWidget>
#include <QTimer>
#include <QEvent>
#include <QCloseEvent>

#include <KDebug>
#include <KApplication>
#include <KIconLoader>
#include <KToolBar>
#include <KActionCollection>
#include <KDialog>

//----------------------------------------------------------

//! @internal
class KexiWindow::Private
{
public:
    Private()
//  : viewModeGroup(0)
            : schemaData(0)
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
//   delete d->stack;
//   d->stack = 0;
//   qDeleteAll(sharedViewActions);
    }

    inline int indexForView(int mode) const {
        return indicesForViews.contains(mode) ? indicesForViews.value(mode) : -1;
    }

    inline void setIndexForView(Kexi::ViewMode mode, int idx) {
        indicesForViews.insert((int)mode, idx);
    }

    QVBoxLayout* mainLyr;
    QStackedWidget* stack;
//  KToolBar* topBar;
//  KexiFlowLayout *topBarLyr;
//  QActionGroup* viewModeGroup;
//  QHash<Kexi::ViewMode, QAction*> toggleViewModeActions;

    /*! View-level actions, owned by the window. */
//  QHash<QByteArray, KAction*> sharedViewActions;

    Kexi::ViewModes supportedViewModes;
    Kexi::ViewModes openedViewModes;
    Kexi::ViewMode currentViewMode;

//  KexiMainWindow *parentWindow;
#ifdef KEXI_NO_CTXT_HELP
    KexiContextHelpInfo *contextHelpInfo;
#endif
    int id;
    QPointer<KexiPart::Part> part;
    KexiPart::Item *item;
//  QString origCaption; //!< helper
    KexiDB::SchemaData* schemaData;
    QPointer<KexiView> newlySelectedView; //!< Used in isDirty(), temporary set in switchToViewMode()
    //!< during view setup, when a new view is not yet raised.
    //! Used in viewThatRecentlySetDirtyFlag(), modified in dirtyChanged().
    QPointer<KexiView> viewThatRecentlySetDirtyFlag;
    QPointer<KexiWindowData> data; //!< temporary data shared between views

    /*! Created view's mode - helper for switchToViewMode(),
     KexiView ctor uses that info. >0 values are useful. */
    Kexi::ViewMode creatingViewsMode;

    KexiSmallToolButton* saveDesignButton;
    KexiToolBarSeparator* saveDesignButtonSeparator;

    bool isRegistered : 1;
    bool dirtyChangedEnabled : 1; //!< used in setDirty(), affects dirtyChanged()
    bool switchToViewModeEnabled : 1; //!< used internally switchToViewMode() to avoid infinite loop

protected:
    QHash<int, int> indicesForViews;
};

//----------------------------------------------------------

KexiWindow::KexiWindow(/*const QString &caption, */ QWidget *parent,
        Kexi::ViewModes supportedViewModes, KexiPart::Part& part, KexiPart::Item& item)
// : KexiMdiChildView(parent, caption)
        : QWidget(parent)
        , KexiActionProxy(this, KexiMainWindowIface::global())
        , d(new Private())
        , m_destroying(false)
{
    d->part = &part;
    d->item = &item;
    d->supportedViewModes = supportedViewModes;
    createSubwidgets();
#ifdef __GNUC__
#warning todo KexiWindow: caption, parent?
#else
#pragma WARNING( todo KexiWindow: caption; parent? )
#endif
//kde4 Q3VBoxLayout *lyr = new Q3VBoxLayout(this);
//kde4 m_stack = new Q3WidgetStack(this, "stack");
//kde4 lyr->addWidget(m_stack);

#ifndef KEXI_NO_CTXT_HELP
    d->contextHelpInfo = new KexiContextHelpInfo();
#endif
//kde4  hide(); //will be shown later

    updateCaption();
}

KexiWindow::KexiWindow()
// : KexiMdiChildView(parent, caption)
        : QWidget(0)
        , KexiActionProxy(this, KexiMainWindowIface::global())
        , d(new Private())
        , m_destroying(false)
{
    createSubwidgets();
#ifdef __GNUC__
#warning todo KexiWindow: caption, parent?
#else
#pragma WARNING( todo KexiWindow: caption; parent? )
#endif
//kde4 Q3VBoxLayout *lyr = new Q3VBoxLayout(this);
//kde4 m_stack = new Q3WidgetStack(this, "stack");
//kde4 lyr->addWidget(m_stack);

#ifndef KEXI_NO_CTXT_HELP
    d->contextHelpInfo = new KexiContextHelpInfo();
#endif
//kde4  hide(); //will be shown later

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
// QHBoxLayout *topBarHLyr = new QHBoxLayout(d->mainLyr); //needed unless KexiFlowLayout properly handles contents margins
// topBarHLyr->addSpacing(KDialog::marginHint()/2);
// d->topBarLyr = new KexiFlowLayout(topBarHLyr, KDialog::marginHint()/2, 2);

    //const bool userMode = KexiMainWindowIface::global()->userMode(); // unused

    /* if (userMode
        || d->supportedViewModes == Kexi::DataViewMode
        || d->supportedViewModes == Kexi::DesignViewMode
        || d->supportedViewModes == Kexi::TextViewMode)
      {
        // nothing to do: only single view mode supported
      }
      else {
        createViewModeToggleButtons();
        d->topBarLyr->addWidget( new KexiToolBarSeparator(this));
      }*/

    // Data actions
    //QAction * a; //unused
    //KActionCollection *ac = KexiMainWindowIface::global()->actionCollection(); //unused
    //KexiSmallToolButton *btn; // unused
    /*
      a = sharedAction("project_save");
      d->saveDesignButton = new KexiSmallToolButton(a, this);
      d->saveDesignButton->setText(i18n("Save Design"));
      d->saveDesignButton->setToolTip(i18n("Save current design"));
      d->saveDesignButton->setWhatsThis(i18n("Saves changes made to the current design."));
      d->topBarLyr->add(d->saveDesignButton);
      d->topBarLyr->addWidget( d->saveDesignButtonSeparator = new KexiToolBarSeparator(this));

      a = sharedAction("data_save_row");
      btn = new KexiSmallToolButton(a, this);
      btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
      d->topBarLyr->add(btn);

      a = sharedAction("data_cancel_row_changes");
      btn = new KexiSmallToolButton(a, this);
      btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
      d->topBarLyr->add(btn);

      d->topBarLyr->addWidget( new KexiToolBarSeparator(this));
    */
    /* moved...

      a = sharedAction("data_sort_az");
      btn = new KexiSmallToolButton(a, this);
      btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
      d->topBarLyr->add(btn);

      a = sharedAction("data_sort_za");
      btn = new KexiSmallToolButton(a, this);
      btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
      d->topBarLyr->add(btn);

      d->topBarLyr->addWidget( new KexiToolBarSeparator(this));
      a = ac->action("edit_find");
      btn = new KexiSmallToolButton(a, this);
      btn->setToolButtonStyle( Qt::ToolButtonIconOnly );
      d->topBarLyr->add(btn);
    */
    /*TODO
    // "data_execute"
      a = new KAction(KIcon("media-playback-start"), i18n("&Execute"), this);
      //a->setToolTip(i18n("")); //TODO
      //a->setWhatsThis(i18n("")); //TODO */

    d->stack = new QStackedWidget(this);
    d->mainLyr->addWidget(d->stack);
}

/*
void KexiWindow::createViewModeToggleButtons()
{
  KAction * a;
  KexiSmallToolButton *btn;
  d->viewModeGroup = new QActionGroup(this);
  if (supportsViewMode(Kexi::DataViewMode)) {
    a = new KexiToggleViewModeAction(Kexi::DataViewMode, d->viewModeGroup,
      this, SLOT(switchToViewModeInternal(Kexi::ViewMode)));
    d->toggleViewModeActions.insert( Kexi::DataViewMode, a );
    d->viewModeGroup->addAction(a);
    btn = new KexiSmallToolButton(a, this);
    d->topBarLyr->add(btn);
  }
  if (supportsViewMode(Kexi::DesignViewMode)) {
    a = new KexiToggleViewModeAction(Kexi::DesignViewMode, d->viewModeGroup,
      this, SLOT(switchToViewModeInternal(Kexi::ViewMode)));
    d->toggleViewModeActions.insert( Kexi::DesignViewMode, a );
    d->viewModeGroup->addAction(a);
    btn = new KexiSmallToolButton(a, this);
    d->topBarLyr->add(btn);
  }
  if (supportsViewMode(Kexi::TextViewMode)) {
    a = new KexiToggleViewModeAction(Kexi::TextViewMode, d->viewModeGroup,
      this, SLOT(switchToViewModeInternal(Kexi::ViewMode)));
    QString customTextViewModeCaption( d->part->internalPropertyValue("textViewModeCaption").toString() );
    if (!customTextViewModeCaption.isEmpty())
      a->setText( customTextViewModeCaption );
    d->toggleViewModeActions.insert( Kexi::TextViewMode, a );
    d->viewModeGroup->addAction(a);
    btn = new KexiSmallToolButton(a, this);
    d->topBarLyr->add(btn);
  }
}*/

KexiView *KexiWindow::selectedView() const
{
    if (m_destroying)
        return 0;
    return static_cast<KexiView*>(d->stack->currentWidget());
}

KexiView *KexiWindow::viewForMode(Kexi::ViewMode mode) const
{
    return static_cast<KexiView*>(d->stack->widget(d->indexForView(mode)));
}

void KexiWindow::addView(KexiView *view)
{
    addView(view, Kexi::NoViewMode);
}

void KexiWindow::addView(KexiView *view, Kexi::ViewMode mode)
{
    const int idx = d->stack->addWidget(view);
    d->setIndexForView(mode, idx);

    //set focus proxy inside this view
    QWidget *ch = KexiUtils::findFirstChild<QWidget*>(view, "QWidget");
    if (ch)
        view->setFocusProxy(ch);

    d->openedViewModes |= mode;
}

/*
void KexiWindow::initViewActions(KexiView* view, Kexi::ViewMode mode)
{
  QList<QAction*> viewActionsList( view->viewActions() );
  foreach(QAction* action, viewActionsList) {
    if (action->isSeparator()) {
      d->topBarLyr->addWidget( new KexiToolBarSeparator(this));
    }
    else {
      KexiSmallToolButton *btn = new KexiSmallToolButton(action, this);
      btn->setText(action->text());
      btn->setToolTip(action->toolTip());
      btn->setWhatsThis(action->whatsThis());
      d->topBarLyr->add(btn);
    }
  }
}*/

void KexiWindow::removeView(Kexi::ViewMode mode)
{
    KexiView *view = viewForMode(mode);
    if (view)
        d->stack->removeWidget(view);

    d->openedViewModes |= mode;
    d->openedViewModes ^= mode;
}

QSize KexiWindow::minimumSizeHint() const
{
    KexiView *v = selectedView();
    if (!v)
        return QWidget::minimumSizeHint();
    return v->minimumSizeHint()
           /*+ QSize(0, mdiParent() ? mdiParent()->captionHeight() : 0)*/;
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
    /* kde4
      if ( m_parentWindow->mdiMode() == KexiMdiMainFrm::ToplevelMode ) {
        m_parentWindow->addWindow(this, KexiMdiMainFrm::Detach);
        m_parentWindow->detachWindow(this, true);
      }
      else */
#ifdef __GNUC__
#warning KexiWindow::registerWindow()
#else
#pragma WARNING( KexiWindow::registerWindow() )
#endif
//kde4 todo  KexiMainWindo::global()->addWindow(this, KexiMdiMainFrm::StandardAdd);
}

bool KexiWindow::isRegistered() const
{
    return d->isRegistered;
}

void KexiWindow::attachToGUIClient()
{
    if (!guiClient())
        return;
}

void KexiWindow::detachFromGUIClient()
{
    if (!guiClient())
        return;
    //TODO
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

#if 0
//js removed
bool KexiWindow::tryClose(bool dontSaveChanges)
{
    if (!dontSaveChanges && dirty()) {
        /*TODO  if (KMessageBox::questionYesNo(this, "<b>"+i18n("Do you want save:")
            +"<p>"+typeName+" \""+ item->name() + "\"?</b>",
            0, KStandardGuiItem::yes(), KStandardGuiItem::no(), ???????)==KMessageBox::No)
            return false;*/
        //js TODO: save data using saveChanges()
    }
    close(true);
    return true;
}
#endif

bool KexiWindow::isDirty() const
{
    //look for "dirty" flag
    int m = d->openedViewModes, mode = 1;
    while (m > 0) {
        if (m & 1) {
            if (static_cast<KexiView*>(d->stack->widget(d->indexForView(mode)))->isDirty())
                return true;
        }
        m >>= 1;
        mode <<= 1;
    }
    return false;
}

void KexiWindow::setDirty(bool dirty)
{
    d->dirtyChangedEnabled = false;
    int m = d->openedViewModes, mode = 1;
    while (m > 0) {
        if (m & 1) {
            static_cast<KexiView*>(d->stack->widget(d->indexForView(mode)))->setDirty(dirty);
        }
        m >>= 1;
        mode <<= 1;
    }
    d->dirtyChangedEnabled = true;
    dirtyChanged(d->viewThatRecentlySetDirtyFlag); //update
}

QString KexiWindow::itemIcon()
{
    if (!d->part || !d->part->info()) {
        KexiView *v = selectedView();
        if (v) {//m_stack->visibleWidget() && m_stack->visibleWidget()->inherits("KexiView")) {
            return v->m_defaultIconName;
        }
        return QString();
    }
    return d->part->info()->itemIcon();
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
    KexiView *newView
    = (d->stack->widget(d->indexForView(newViewMode))
       && d->stack->widget(d->indexForView(newViewMode))->inherits("KexiView"))
      ? static_cast<KexiView*>(d->stack->widget(d->indexForView(newViewMode))) : 0;
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
    d->stack->setCurrentWidget(newView);   //d->indexForView(newViewMode) );
    newView->propertySetSwitched();
    KexiMainWindowIface::global()->invalidateSharedActions(newView);
    newView->setFocus();
//2.0    QTimer::singleShot(10, newView, SLOT(setFocus())); //newView->setFocus(); //js ok?
// setFocus();
    /*TODO move
      QAction* toggleAction = d->toggleViewModeActions.value(newViewMode);
      if (toggleAction) {
        d->switchToViewModeEnabled = false;
        toggleAction->setChecked(true);
        d->switchToViewModeEnabled = true;
      }*/
//! @todo sometimes save button is needed for data view mode as well (for queries, forms...)
//moved showSaveDesignButton( newViewMode == Kexi::DesignViewMode || newViewMode == Kexi::TextViewMode );

    /*TODO

      const QList<QWidget*> childWidgets( d->topBarLyr->findChildren<QWidget*>() );
      foreach(QWidget* childWidget, childWidgets) {
        if (dynamic_cast<KexiToolBarSeparator*>(childWidget)) {

        }
        else if (dynamic_cast<KexiSmallToolButton*>(childWidget)) {
          QAction *a = dynamic_cast<KexiSmallToolButton*>(childWidget)->action();
          if (a && a != newView->viewAction(a->objectName().toLatin1())) {
            childWidget->hide();
            kDebug() << "Hide " << a->objectName();
          }
          else {
            childWidget->show();
            kDebug() << "Show " << a->objectName();
          }
        }
      }*/
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
//moved d->saveDesignButton->setEnabled( isDirty() );
    emit dirtyChanged(this);
}

void KexiWindow::updateCaption()
{
    if (!d->item || !d->part)
        return;
    QString fullCapt(KexiPart::fullCaptionForItem(*d->item, d->part));
    setWindowTitle(fullCapt + (isDirty() ? "*" : ""));
}

bool KexiWindow::neverSaved() const
{
    return d->item ? d->item->neverSaved() : true;
}

tristate KexiWindow::storeNewData()
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
//    KexiDB::SchemaData sdata(d->part->info()->projectPartID());
    sdata.setName(d->item->name());
    sdata.setCaption(d->item->caption());
    sdata.setDescription(d->item->description());

    bool cancel = false;
    d->schemaData = v->storeNewData(sdata, cancel);
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
#if 0 // moved to KexiProject
    if (!part()->info()->isIdStoredInPartDatabase()) {
        //this part's ID is not stored within kexi__parts:
        KexiDB::TableSchema *ts =
            project->dbConnection()->tableSchema("kexi__parts");
        kDebug() << "schema: " << ts;
        if (!ts)
            return false;

        //temp. hack: avoid problems with autonumber
        // see http://bugs.kde.org/show_bug.cgi?id=89381
        int p_id = project->idForClass(part()->info()->partClass());

        if (p_id < 0) {
            // Find first available custom part ID by taking the greatest
            // existing custom ID (if it exists) and adding 1.
            p_id = (int)KexiPart::UserObjectType;
            tristate success = project->dbConnection()
                                ->querySingleNumber("SELECT max(p_id) FROM kexi__parts", d->id);
            if (!success) {
                // Couldn't read part id's from the kexi__parts table
                return false;
            } else {
                // Got a maximum part ID, or there were no parts
                p_id = p_id + 1;
                p_id = qMax(p_id, (int)KexiPart::UserObjectType);
            }
        }

        KexiDB::FieldList *fl = ts->subList("p_id", "p_name", "p_mime", "p_url");
        kDebug() << "fieldlist: "
        << (fl ? fl->debugString() : QString());
        if (!fl)
            return false;

        kDebug() << part()->info()->ptr()->untranslatedGenericName();
//  QStringList sl = part()->info()->ptr()->propertyNames();
//  for (QStringList::ConstIterator it=sl.constBegin();it!=sl.constEnd();++it)
//   kDebug() << *it << " " << part()->info()->ptr()->property(*it).toString();
        if (!project->dbConnection()->insertRecord(
                    *fl,
                    QVariant(p_id),
                    QVariant(part()->info()->ptr()->untranslatedGenericName()),
                    QVariant(QString::fromLatin1("kexi/") + part()->info()->objectName()/*ok?*/),
                    QVariant(part()->info()->partClass() /*always ok?*/)))
            return false;

        kDebug() << "insert success!";
        part()->info()->setProjectPartID(p_id);
        //(int) project()->dbConnection()->lastInsertedAutoIncValue("p_id", "kexi__parts"));
        kDebug() << "new id is: " << part()->info()->projectPartID();

        part()->info()->setIdStoredInPartDatabase(true);
    }
#endif
    /* Sets 'dirty' flag on every dialog's view. */
    setDirty(false);
// v->setDirty(false);
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
// v->setDirty(false);
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

Kexi::ViewMode KexiWindow::creatingViewsMode() const
{
    return d->creatingViewsMode;
}

QVariant KexiWindow::internalPropertyValue(const QByteArray& name,
        const QVariant& defaultValue) const
{
    return d->part->internalPropertyValue(name, defaultValue);
}

/*
void KexiWindow::showSaveDesignButton(bool show)
{
  d->saveDesignButton->setVisible(show);
  d->saveDesignButtonSeparator->setVisible(show);
}*/

/*KAction* KexiWindow::sharedViewAction(const char* name)
{
  return d->sharedViewActions.value(name);
}*/

#include "KexiWindow.moc"
