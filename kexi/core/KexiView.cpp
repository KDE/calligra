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

#include <kdebug.h>
#include <kactioncollection.h>
#include <QEvent>
#include <QCloseEvent>

class KexiView::Private
{
	public:
		Private()
		: viewWidget(0)
		, parentView(0)
		, newlyAssignedID(-1)
		, viewMode(Kexi::NoViewMode) //unknown!
		, isDirty(false)
		{
		}
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

		bool isDirty : 1;
};

//----------------------------------------------------------

KexiView::KexiView(QWidget *parent)
 : QWidget(parent)
 , KexiActionProxy(this)
 , d( new Private() )
{
	QWidget *wi=this;
	while ((wi = wi->parentWidget()) && !wi->inherits("KexiWindow"))
		;
	d->window = (wi && wi->inherits("KexiWindow")) ? static_cast<KexiWindow*>(wi) : 0;
	if (d->window) {
		//init view mode number for this view (obtained from window where this view is created)
		if (d->window->supportsViewMode(d->window->creatingViewsMode()))
			d->viewMode = d->window->creatingViewsMode();
	}

	installEventFilter(this);
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
#warning KexiView::preferredSizeHint()
#if 0 //todo
	KexiWindow* w = d->window;
	if (dlg && dlg->mdiParent()) {
		QRect r = dlg->mdiParent()->mdiAreaContentsRect();
		return otherSize.boundedTo( QSize(
			r.width() - 10,
			r.height() - dlg->mdiParent()->captionHeight() - dlg->pos().y() - 10
		) );
	}
#endif
	return otherSize;
}

void KexiView::closeEvent( QCloseEvent * e )
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
		KexiMainWindowIface::global()->propertySetSwitched( window(), false );
}

void KexiView::propertySetReloaded(bool preservePrevSelection, 
	const QByteArray& propertyToSelect)
{
	if (window())
		KexiMainWindowIface::global()->propertySetSwitched( 
			window(), true, preservePrevSelection, propertyToSelect );
}

void KexiView::setDirty(bool set)
{
	const bool changed = (d->isDirty != set);
	d->isDirty = set;
	d->isDirty = isDirty();
	if (d->parentView) {
		d->parentView->setDirty(d->isDirty);
	}
	else {
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
			->storeObjectSchemaData( *new_schema, true ))
	{
		delete new_schema;
		new_schema=0;
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
			->storeObjectSchemaData( *d->window->schemaData(), false /*existing object*/ ))
	{
		return false;
	}
	setDirty(false);
	return true;
}

bool KexiView::loadDataBlock( QString &dataString, const QString& dataID, bool canBeEmpty )
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

bool KexiView::storeDataBlock( const QString &dataString, const QString &dataID )
{
	if (!d->window)
		return false;
	int effectiveID;
	if (d->newlyAssignedID>0) {//ID not yet stored within window, but we've got ID here
		effectiveID = d->newlyAssignedID;
		d->newlyAssignedID = -1;
	}
	else
		effectiveID = d->window->id();

	return effectiveID>0
		&& KexiMainWindowIface::global()->project()->dbConnection()->storeDataBlock(
			effectiveID, dataString, dataID);
}

bool KexiView::removeDataBlock( const QString& dataID )
{
	if (!d->window)
		return false;
	return KexiMainWindowIface::global()->project()->dbConnection()
		->removeDataBlock(d->window->id(), dataID);
}

bool KexiView::eventFilter( QObject *o, QEvent *e )
{
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {// && o->inherits("QWidget")) {
//		//hp==true if currently focused widget is a child of this table view
//		const bool hp = Kexi::hasParent( static_cast<QWidget*>(o), focusWidget());
//		kexidbg << "KexiView::eventFilter(): " << o->name() << " " << e->type() << endl;
		if (KexiUtils::hasParent( this, static_cast<QWidget*>(o))) {
			if (e->type()==QEvent::FocusOut && focusWidget() && !KexiUtils::hasParent( this, focusWidget())) {
				//focus out: when currently focused widget is not a parent of this view
				emit focus(false);
			} else if (e->type()==QEvent::FocusIn) {
				emit focus(true);
			}
			if (e->type()==QEvent::FocusOut) {
//				kDebug() << focusWidget()->className() << " " << focusWidget()->name()<< endl;
//				kDebug() << o->className() << " " << o->name()<< endl;
				KexiView *v = KexiUtils::findParent<KexiView*>(o);
				if (v) {
					while (v->d->parentView)
						v = v->d->parentView;
					if (KexiUtils::hasParent( this, static_cast<QWidget*>(v->focusWidget()) ))
						v->d->lastFocusedChildBeforeFocusOut = static_cast<QWidget*>(v->focusWidget());
				}
			}

			if (e->type()==QEvent::FocusIn && m_actionProxyParent) {
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
	}
	d->viewWidget = w;
	if (d->viewWidget) {
		d->viewWidget->installEventFilter(this);
		if (focusProxy)
			setFocusProxy(d->viewWidget); //js: ok?
	}
}

void KexiView::addChildView( KexiView* childView )
{
	d->children.append( childView );
	addActionProxyChild( childView );
	childView->d->parentView = this;
//	if (d->parentView)
//		childView->installEventFilter(d->parentView);
	childView->installEventFilter(this);

}

void KexiView::setFocus()
{
	if (!d->lastFocusedChildBeforeFocusOut.isNull()) {
//		kDebug() << "FOCUS: " << d->lastFocusedChildBeforeFocusOut->className() << " " << d->lastFocusedChildBeforeFocusOut->name()<< endl;
		QWidget *w = d->lastFocusedChildBeforeFocusOut;
		d->lastFocusedChildBeforeFocusOut = 0;
		w->setFocus();
	}
	else {
		if (hasFocus())
			setFocusInternal();
		else
			setFocusInternal();
	}
	KexiMainWindowIface::global()->invalidateSharedActions(this);
}

QAction* KexiView::sharedAction( const char *action_name )
{
	if (part()) {
		KActionCollection *ac;
		if ( (ac = part()->actionCollectionForMode( viewMode() )) ) {
			QAction* a = ac->action( action_name );
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
		if ( (ac = part()->actionCollectionForMode( viewMode() )) && (a = ac->action( action_name )) ) {
			a->setEnabled(set);
		}
	}
	KexiActionProxy::setAvailable(action_name, set);
}

void KexiView::updateActions(bool activated)
{
	//do nothing here
	//do the same for children :)
 	foreach (KexiView* view, d->children)
		view->updateActions(activated);
}

#include "KexiView.moc"

