/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include <qobject.h>
#include <qtimer.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QEvent>
#include <QCloseEvent>

#include <kdebug.h>
#include <kapplication.h>
#include <kiconloader.h>

class KexiWindow::Private
{
	public:
		Private()
		: schemaData(0)
		, isRegistered(false)
		, disableDirtyChanged(false)
		{
			supportedViewModes = Kexi::NoViewMode; //will be set by KexiPart
			openedViewModes = Kexi::NoViewMode;
			currentViewMode = Kexi::NoViewMode; //no view available yet
			creatingViewsMode = Kexi::NoViewMode;
			id = -1;
			item = 0;
		}
		
		inline int indexForView( int mode ) const {
			return indicesForViews.contains(mode) ? indicesForViews.value(mode) : -1;
		}
		inline void setIndexForView( Kexi::ViewMode mode, int idx )
		{ indicesForViews.insert((int)mode, idx); }
		
		Kexi::ViewModes supportedViewModes;
		Kexi::ViewModes openedViewModes;
		Kexi::ViewMode currentViewMode;
		
//		KexiMainWindow *parentWindow;
#ifdef KEXI_NO_CTXT_HELP
		KexiContextHelpInfo *contextHelpInfo;
#endif
		int id;
		QPointer<KexiPart::Part> part;
		KexiPart::Item *item;
		QString origCaption; //!< helper
		KexiDB::SchemaData* schemaData;
		QPointer<KexiView> newlySelectedView; //!< Used in isDirty(), temporary set in switchToViewMode()
		                                   //!< during view setup, when a new view is not yet raised.
		//! Used in viewThatRecentlySetDirtyFlag(), modified in dirtyChanged().
		QPointer<KexiView> viewThatRecentlySetDirtyFlag; 
		QPointer<KexiWindowData> data; //!< temporary data shared between views

		/*! Created view's mode - helper for switchToViewMode(),
		 KexiView ctor uses that info. >0 values are useful. */
		Kexi::ViewMode creatingViewsMode;

		bool isRegistered : 1;
		bool disableDirtyChanged : 1; //!< used in setDirty(), affects dirtyChanged()
		
	protected:
		QHash<int, int> indicesForViews;
};

//----------------------------------------------------------

KexiWindow::KexiWindow(/*const QString &caption, */ QWidget *parent,
	Kexi::ViewModes supportedViewModes, KexiPart::Part& part,	KexiPart::Item& item)
// : KexiMdiChildView(parent, caption)
 : QStackedWidget(parent)
 , KexiActionProxy(this, KexiMainWindowIface::global())
 , d( new Private() )
 , m_destroying(false)
{
	d->part = &part;
	d->item = &item;
	d->supportedViewModes = supportedViewModes;
#ifdef __GNUC__
#warning todo KexiWindow: caption, parent?
#else
#pragma WARNING( todo KexiWindow: caption, parent? )
#endif
//kde4	Q3VBoxLayout *lyr = new Q3VBoxLayout(this);
//kde4	m_stack = new Q3WidgetStack(this, "stack");
//kde4	lyr->addWidget(m_stack);

#ifndef KEXI_NO_CTXT_HELP
	d->contextHelpInfo=new KexiContextHelpInfo();
#endif
//kde4 	hide(); //will be shown later

	updateCaption();
}

KexiWindow::KexiWindow()
// : KexiMdiChildView(parent, caption)
 : QStackedWidget(0)
 , KexiActionProxy(this, KexiMainWindowIface::global())
 , d( new Private() )
 , m_destroying(false)
{
#ifdef __GNUC__
#warning todo KexiWindow: caption, parent?
#else
#pragma WARNING( todo KexiWindow: caption, parent? )
#endif
//kde4	Q3VBoxLayout *lyr = new Q3VBoxLayout(this);
//kde4	m_stack = new Q3WidgetStack(this, "stack");
//kde4	lyr->addWidget(m_stack);

#ifndef KEXI_NO_CTXT_HELP
	d->contextHelpInfo=new KexiContextHelpInfo();
#endif
//kde4 	hide(); //will be shown later

	updateCaption();
}

KexiWindow::~KexiWindow()
{
	m_destroying = true;
	delete d;
	d = 0;
}

KexiView *KexiWindow::selectedView() const
{
	if (m_destroying)
		return 0;
	return static_cast<KexiView*>( currentWidget() );
}

KexiView *KexiWindow::viewForMode(Kexi::ViewMode mode) const
{
	return static_cast<KexiView*>( widget( d->indexForView(mode) ) );
}

void KexiWindow::addView(KexiView *view)
{
	addView(view, Kexi::NoViewMode);
}

void KexiWindow::addView(KexiView *view, Kexi::ViewMode mode)
{
	const int idx = addWidget(view);
	d->setIndexForView( mode, idx );

	//set focus proxy inside this view
	QWidget *ch = KexiUtils::findFirstChild<QWidget*>(view, "QWidget");
	if (ch)
		view->setFocusProxy(ch);

	d->openedViewModes |= mode;
}

void KexiWindow::removeView(Kexi::ViewMode mode)
{
	KexiView *view = viewForMode(mode);
	if (view)
		removeWidget(view);

	d->openedViewModes |= mode;
	d->openedViewModes ^= mode;
}

QSize KexiWindow::minimumSizeHint() const
{
	KexiView *v = selectedView();
	if (!v)
		return QStackedWidget::minimumSizeHint();
	return v->minimumSizeHint() 
		/*+ QSize(0, mdiParent() ? mdiParent()->captionHeight() : 0)*/;
}

QSize KexiWindow::sizeHint() const
{
	KexiView *v = selectedView();
	if (!v)
		return QStackedWidget::sizeHint();
	return v->preferredSizeHint( v->sizeHint() );
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

bool KexiWindow::supportsViewMode( Kexi::ViewMode mode ) const
{
kexidbg<<d->supportedViewModes << endl;
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
	d->isRegistered=true;
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
//kde4 todo		KexiMainWindo::global()->addWindow(this, KexiMdiMainFrm::StandardAdd);
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
	return (partItem() && partItem()->identifier()>0) 
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
	d->contextHelpInfo->caption=caption;
	d->contextHelpInfo->text=text;
	d->contextHelpInfo->text=iconName;
	updateContextHelp();
#endif
}

void KexiWindow::closeEvent( QCloseEvent * e )
{
	KexiMainWindowIface::global()->acceptPropertySetEditing();

	//let any view send "closing" signal
/*	QObjectList list = queryList( "KexiView", 0, false, false);
	foreach(QObject* obj, list) {
		KexiView *view = static_cast<KexiView*>(obj);*/
	QList<KexiView *> list( findChildren<KexiView*>() );
	foreach (KexiView * view, list) {
		bool cancel = false;
		emit view->closing(cancel);
		if (cancel) {
			e->ignore();
			return;
		}
	}
	emit closing();
	QStackedWidget::closeEvent(e);
}

#if 0
//js removed
bool KexiWindow::tryClose(bool dontSaveChanges)
{
	if (!dontSaveChanges && dirty()) {
/*TODO		if (KMessageBox::questionYesNo(this, "<b>"+i18n("Do you want save:")
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
	while (m>0) {
		if (m & 1) {
			if (static_cast<KexiView*>(widget( d->indexForView(mode) ))->isDirty())
				return true;
		}
		m >>= 1;
		mode <<= 1;
	}
	return false;
}

void KexiWindow::setDirty(bool dirty)
{
	d->disableDirtyChanged = true;
	int m = d->openedViewModes, mode = 1;
	while (m>0) {
		if (m & 1) {
			static_cast<KexiView*>(widget( d->indexForView(mode) ))->setDirty(dirty);
		}
		m >>= 1;
		mode <<= 1;
	}
	d->disableDirtyChanged = false;
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
	return newViewMode==Kexi::TextViewMode 
		&& !viewForMode(Kexi::DesignViewMode) 
		&& supportsViewMode(Kexi::DesignViewMode);
}

tristate KexiWindow::switchToViewMode(
	Kexi::ViewMode newViewMode, 
	QMap<QString,QString>* staticObjectArgs,
	bool& proposeOpeningInTextViewModeBecauseOfProblems)
{
	KexiMainWindowIface::global()->acceptPropertySetEditing();

	const bool designModePreloadedForTextModeHack = isDesignModePreloadedForTextModeHackUsed(newViewMode);
	tristate res = true;
	if (designModePreloadedForTextModeHack) {
		/* A HACK: open design BEFORE text mode: otherwise Query schema becames crazy */
		bool _proposeOpeningInTextViewModeBecauseOfProblems = false; // used because even if opening the view failed,
		                                                             // text view can be opened
		res = switchToViewMode( Kexi::DesignViewMode, staticObjectArgs, _proposeOpeningInTextViewModeBecauseOfProblems);
		if ((!res && !_proposeOpeningInTextViewModeBecauseOfProblems) || ~res)
			return res;
	}

	kexidbg << "KexiWindow::switchToViewMode()" << endl;
	bool dontStore = false;
	KexiView *view = selectedView();

	if (d->currentViewMode == newViewMode)
		return true;
	if (!supportsViewMode(newViewMode)) {
		kexiwarn << "! KexiWindow::supportsViewMode("<<Kexi::nameForViewMode(newViewMode)<<")"<<endl;
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
//			KMessageBox::questionYesNo(0, i18n("Design has been changed. You must save it before switching to other view."))
//				==KMessageBox::No
		}
	}

	//get view for viewMode
	KexiView *newView 
		= (widget( d->indexForView(newViewMode) ) 
			&& widget( d->indexForView(newViewMode) )->inherits("KexiView"))
		? static_cast<KexiView*>(widget( d->indexForView(newViewMode) )) : 0;
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
			kexiwarn << "Switching to mode " << newViewMode << " failed. Previous mode "
				<< d->currentViewMode << " restored." << endl;
			return false;
		}
		d->creatingViewsMode = Kexi::NoViewMode;
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
		kexiwarn << "Switching to mode " << newViewMode << " failed. Previous mode "
			<< d->currentViewMode << " restored." << endl;
		return false;
	}
	d->currentViewMode = newViewMode;
	d->newlySelectedView = newView;
	if (prevViewMode==Kexi::NoViewMode)
		d->newlySelectedView->setDirty(false);

	res = newView->afterSwitchFrom(
			designModePreloadedForTextModeHack ? Kexi::NoViewMode : prevViewMode);
	proposeOpeningInTextViewModeBecauseOfProblems
		= data()->proposeOpeningInTextViewModeBecauseOfProblems;
	if (!res) {
		removeView(newViewMode);
		delete newView;
		kexiwarn << "Switching to mode " << newViewMode << " failed. Previous mode "
			<< prevViewMode << " restored." << endl;
		const Kexi::ObjectStatus status(*this);
		setStatus(KexiMainWindowIface::global()->project()->dbConnection(), 
			i18n("Switching to other view failed (%1).", Kexi::nameForViewMode(newViewMode)),"");
		append( status );
		d->currentViewMode = prevViewMode;
		return false;
	}
	d->newlySelectedView = 0;
	if (~res) {
		d->currentViewMode = prevViewMode;
		return cancelled;
	}
	if (view)
		takeActionProxyChild( view ); //take current proxy child
	addActionProxyChild( newView ); //new proxy child
	setCurrentWidget( newView ); //d->indexForView(newViewMode) );
	newView->propertySetSwitched();
	KexiMainWindowIface::global()->invalidateSharedActions( newView );
	QTimer::singleShot(10, newView, SLOT(setFocus())); //newView->setFocus(); //js ok?
//	setFocus();
	return true;
}

tristate KexiWindow::switchToViewMode(Kexi::ViewMode newViewMode)
{
	bool dummy;
	return switchToViewMode( newViewMode, 0, dummy );
}

void KexiWindow::setFocus()
{
	if (currentWidget()) {
		if (currentWidget()->inherits("KexiView"))
			static_cast<KexiView*>( currentWidget() )->setFocus();
		else
			currentWidget()->setFocus();
	}
	else {
		QStackedWidget::setFocus();
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
	if (data!=d->data)
		delete (KexiWindowData*)d->data;
	d->data = data;
}

bool KexiWindow::eventFilter(QObject *obj, QEvent *e)
{
	if (QStackedWidget::eventFilter(obj, e))
		return true;
	/*if (e->type()==QEvent::FocusIn) {
		QWidget *w = m_parentWindow->activeWindow();
		w=0;
	}*/
	if ((e->type()==QEvent::FocusIn && KexiMainWindowIface::global()->currentWindow()==this)
		|| e->type()==QEvent::MouseButtonPress) {
		if (currentWidget() && KexiUtils::hasParent(currentWidget(), obj)) {
			//pass the activation
			activate();
		}
	}
	return false;
}

void KexiWindow::dirtyChanged(KexiView* view)
{
	if (d->disableDirtyChanged)
		return;
	d->viewThatRecentlySetDirtyFlag = isDirty() ? view : 0;
	updateCaption();
	emit dirtyChanged(this);
}

void KexiWindow::updateCaption()
{
	if (!d->item || !d->part || !d->origCaption.isEmpty())
		return;
	QString capt = d->item->name();
	QString fullCapt = capt;
	if (d->part)
		fullCapt += (" : " + d->part->instanceCaption());
#ifdef __GNUC__
#warning TODO KexiWindow::updateCaption()
#else
#pragma WARNING( TODO KexiWindow::updateCaption() )
#endif
	if (isDirty()) {
		setWindowTitle(fullCapt+"*");
	}
	else {
		setWindowTitle(fullCapt);
	}
#if 0 //TODO
	if (isDirty()) {
		QStackedWidget::setCaption(fullCapt+"*");
		QStackedWidget::setTabCaption(capt+"*");
	}
	else {
		QStackedWidget::setCaption(fullCapt);
		QStackedWidget::setTabCaption(capt);
	}
#endif
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
	KexiDB::SchemaData sdata(d->part->info()->projectPartID());
	sdata.setName( d->item->name() );
	sdata.setCaption( d->item->caption() );
	sdata.setDescription( d->item->description() );

	bool cancel = false;
	d->schemaData = v->storeNewData(sdata, cancel);
	if (cancel)
		return cancelled;
	if (!d->schemaData) {
		setStatus(KexiMainWindowIface::global()->project()->dbConnection(), i18n("Saving object's definition failed."),"");
		return false;
	}

	if (!part()->info()->isIdStoredInPartDatabase()) {
		//this part's ID is not stored within kexi__parts:
		KexiDB::TableSchema *ts =
			KexiMainWindowIface::global()->project()->dbConnection()->tableSchema("kexi__parts");
		kexidbg << "KexiWindow::storeNewData(): schema: " << ts << endl;
		if (!ts)
			return false;

		//temp. hack: avoid problems with autonumber
		// see http://bugs.kde.org/show_bug.cgi?id=89381
		int p_id = part()->info()->projectPartID();

		if (p_id<0) {
			// Find first available custom part ID by taking the greatest
			// existing custom ID (if it exists) and adding 1.
			p_id = (int)KexiPart::UserObjectType;
			tristate success = KexiMainWindowIface::global()->project()->dbConnection()
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
		kexidbg << "KexiWindow::storeNewData(): fieldlist: " 
			<< (fl ? fl->debugString() : QString()) << endl;
		if (!fl)
			return false;

		kexidbg << part()->info()->ptr()->untranslatedGenericName() << endl;
//		QStringList sl = part()->info()->ptr()->propertyNames();
//		for (QStringList::ConstIterator it=sl.constBegin();it!=sl.constEnd();++it)
//			kexidbg << *it << " " << part()->info()->ptr()->property(*it).toString() <<  endl;
		if (!KexiMainWindowIface::global()->project()->dbConnection()
			->insertRecord(
				*fl,
				QVariant(p_id),
				QVariant(part()->info()->ptr()->untranslatedGenericName()),
				QVariant(part()->info()->mimeType()),
				QVariant("http://www.koffice.org/kexi/" /*always ok?*/)))
			return false;

		kexidbg << "KexiWindow::storeNewData(): insert success!" << endl;
		part()->info()->setProjectPartID( p_id );
			//(int) project()->dbConnection()->lastInsertedAutoIncValue("p_id", "kexi__parts"));
		kexidbg << "KexiWindow::storeNewData(): new id is: " 
			<< part()->info()->projectPartID()  << endl;

		part()->info()->setIdStoredInPartDatabase(true);
	}

	/* Sets 'dirty' flag on every dialog's view. */
	setDirty(false);
//	v->setDirty(false);
	//new schema data has now ID updated to a unique value
	//-assign that to item's identifier
	d->item->setIdentifier( d->schemaData->id() );
	KexiMainWindowIface::global()->project()->addStoredItem( part()->info(), d->item );

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
	setStatus(KexiMainWindowIface::global()->project()->dbConnection(), i18n("Saving object's data failed."),"");

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
//	v->setDirty(false);
	return true;
}

void KexiWindow::activate()
{
	KexiView *v = selectedView();
	//kexidbg << "focusWidget(): " << focusWidget()->name() << endl;
#ifdef __GNUC__
#warning TODO KexiWindow::activate() OK instead of focusedChildWidget()?
#else
#pragma WARNING( TODO KexiWindow::activate() OK instead of focusedChildWidget()? )
#endif
	if (KexiUtils::hasParent( v, /*kde4*/ KexiMainWindowIface::global()->focusWidget() )) //QStackedWidget::focusedChildWidget()))
#ifdef __GNUC__
#warning TODO		QStackedWidget::activate();
#else
#pragma WARNING( TODO		QStackedWidget::activate(); )
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
	//js: not neeed??	m_parentWindow->invalidateSharedActions(this);
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

#include "KexiWindow.moc"
