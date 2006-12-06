/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiviewbase.h"

#include "keximainwindow.h"
#include "kexidialogbase.h"
#include "kexiproject.h"
#include <koproperty/set.h>

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexiutils/utils.h>

#include <kdebug.h>
#include <kactioncollection.h>
//Added by qt3to4:
#include <QEvent>
#include <QCloseEvent>
//Added by qt3to4:
#include <Q3CString>

KexiViewBase::KexiViewBase(KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : QWidget(parent, name)
 , KexiActionProxy(this, mainWin)
 , m_mainWin(mainWin)
 , m_viewWidget(0)
 , m_parentView(0)
 , m_newlyAssignedID(-1)
 , m_viewMode(0) //unknown!
 , m_dirty(false)
{
	QWidget *wi=this;
	while ((wi = wi->parentWidget()) && !wi->inherits("KexiDialogBase"))
		;
	m_dialog = (wi && wi->inherits("KexiDialogBase")) ? static_cast<KexiDialogBase*>(wi) : 0;
	if (m_dialog) {
		//init view mode number for this view (obtained from dialog where this view is created)
		if (m_dialog->supportsViewMode(m_dialog->m_creatingViewsMode))
			m_viewMode = m_dialog->m_creatingViewsMode;
	}

	installEventFilter(this);
}

KexiViewBase::~KexiViewBase()
{
}

KexiPart::Part* KexiViewBase::part() const
{
	return m_dialog ? m_dialog->part() : 0;
}

tristate KexiViewBase::beforeSwitchTo(int /* mode */, bool & /*dontStore*/)
{
	return true;
}

tristate KexiViewBase::afterSwitchFrom(int /* mode */)
{
	return true;
}

QSize KexiViewBase::preferredSizeHint(const QSize& otherSize)
{
	KexiDialogBase* dlg = parentDialog();
	if (dlg && dlg->mdiParent()) {
		QRect r = dlg->mdiParent()->mdiAreaContentsRect();
		return otherSize.boundedTo( QSize(
			r.width() - 10,
			r.height() - dlg->mdiParent()->captionHeight() - dlg->pos().y() - 10
		) );
	}
	return otherSize;
}

void KexiViewBase::closeEvent( QCloseEvent * e )
{
	bool cancel = false;
	emit closing(cancel);
	if (cancel) {
		e->ignore();
		return;
	}
	QWidget::closeEvent(e);
}

KoProperty::Set *KexiViewBase::propertySet()
{
	return 0;
}

void KexiViewBase::propertySetSwitched()
{
	if (parentDialog())
		m_mainWin->propertySetSwitched( parentDialog(), false );
}

void KexiViewBase::propertySetReloaded(bool preservePrevSelection, const Q3CString& propertyToSelect)
{
	if (parentDialog())
		m_mainWin->propertySetSwitched( parentDialog(), true, preservePrevSelection, propertyToSelect );
}

void KexiViewBase::setDirty(bool set)
{
/*	if (m_dirty == set) {//no change here
		if (m_dialog) {
			// however, it's a change from dialog perspective
			if (m_dialog->dirty()!=set)
				m_dialog->dirtyChanged();
		}
		return;
	}*/
	const bool changed = (m_dirty != set);
	m_dirty = set;
	m_dirty = dirty();
//	if (m_dirty!=set)//eventually didn't change
//		return;
	if (m_parentView) {
		m_parentView->setDirty(m_dirty);
	}
	else {
		if (changed && m_dialog)
			m_dialog->dirtyChanged(this);
	}
}

/*bool KexiViewBase::saveData()
{
	//TODO....

	//finally:
	setDirty(false);
	return true;
}*/

KexiDB::SchemaData* KexiViewBase::storeNewData(const KexiDB::SchemaData& sdata, bool & /*cancel*/)
{
	KexiDB::SchemaData *new_schema = new KexiDB::SchemaData();
	*new_schema = sdata;

	if (!m_mainWin->project()->dbConnection()
			->storeObjectSchemaData( *new_schema, true ))
	{
		delete new_schema;
		new_schema=0;
	}
	m_newlyAssignedID = new_schema->id();
	return new_schema;
}

tristate KexiViewBase::storeData(bool dontAsk)
{
	Q_UNUSED(dontAsk);
	if (!m_dialog || !m_dialog->schemaData())
		return false;
	if (!m_mainWin->project()->dbConnection()
			->storeObjectSchemaData( *m_dialog->schemaData(), false /*existing object*/ ))
	{
		return false;
	}
	setDirty(false);
	return true;
}

bool KexiViewBase::loadDataBlock( QString &dataString, const QString& dataID, bool canBeEmpty )
{
	if (!m_dialog)
		return false;
	const tristate res = m_mainWin->project()->dbConnection()->loadDataBlock(m_dialog->id(), dataString, dataID);
	if (canBeEmpty && ~res) {
		dataString.clear();
		return true;
	}
	return res == true;
}

bool KexiViewBase::storeDataBlock( const QString &dataString, const QString &dataID )
{
	if (!m_dialog)
		return false;
	int effectiveID;
	if (m_newlyAssignedID>0) {//ID not yet stored within dialog, but we've got ID here
		effectiveID = m_newlyAssignedID;
		m_newlyAssignedID = -1;
	}
	else
		effectiveID = m_dialog->id();

	return effectiveID>0
		&& m_mainWin->project()->dbConnection()->storeDataBlock(effectiveID, dataString, dataID);
}

bool KexiViewBase::removeDataBlock( const QString& dataID )
{
	if (!m_dialog)
		return false;
	return m_mainWin->project()->dbConnection()->removeDataBlock(m_dialog->id(), dataID);
}

bool KexiViewBase::eventFilter( QObject *o, QEvent *e )
{
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {// && o->inherits("QWidget")) {
//		//hp==true if currently focused widget is a child of this table view
//		const bool hp = Kexi::hasParent( static_cast<QWidget*>(o), focusWidget());
//		kexidbg << "KexiViewBase::eventFilter(): " << o->name() << " " << e->type() << endl;
		if (KexiUtils::hasParent( this, static_cast<QWidget*>(o))) {
			if (e->type()==QEvent::FocusOut && focusWidget() && !KexiUtils::hasParent( this, focusWidget())) {
				//focus out: when currently focused widget is not a parent of this view
				emit focus(false);
			} else if (e->type()==QEvent::FocusIn) {
				emit focus(true);
			}
			if (e->type()==QEvent::FocusOut) { // && focusWidget() && Kexi::hasParent( this, focusWidget())) { // && focusWidget()->inherits("KexiViewBase")) {
//				kDebug() << focusWidget()->className() << " " << focusWidget()->name()<< endl;
//				kDebug() << o->className() << " " << o->name()<< endl;
				KexiViewBase *v = KexiUtils::findParent<KexiViewBase>(o);
//				QWidget *www=v->focusWidget();
				if (v) {
					while (v->m_parentView)
						v = v->m_parentView;
					if (KexiUtils::hasParent( this, static_cast<QWidget*>(v->focusWidget()) ))
						v->m_lastFocusedChildBeforeFocusOut = static_cast<QWidget*>(v->focusWidget());
//					v->m_lastFocusedChildBeforeFocusOut = static_cast<QWidget*>(o); //focusWidget();
				}
			}

			if (e->type()==QEvent::FocusIn && m_actionProxyParent) {
				m_actionProxyParent->m_focusedChild = this;
			}
//			m_mainWin->invalidateSharedActions(this);
		}
	}
	return false;
}

void KexiViewBase::setViewWidget(QWidget* w, bool focusProxy)
{
	if (m_viewWidget == w)
		return;
	if (m_viewWidget) {
		m_viewWidget->removeEventFilter(this);
	}
	m_viewWidget = w;
	if (m_viewWidget) {
		m_viewWidget->installEventFilter(this);
		if (focusProxy)
			setFocusProxy(m_viewWidget); //js: ok?
	}
}

void KexiViewBase::addChildView( KexiViewBase* childView )
{
	m_children.append( childView );
	addActionProxyChild( childView );
	childView->m_parentView = this;
//	if (m_parentView)
//		childView->installEventFilter(m_parentView);
	childView->installEventFilter(this);

}

void KexiViewBase::setFocus()
{
	if (!m_lastFocusedChildBeforeFocusOut.isNull()) {
//		kDebug() << "FOCUS: " << m_lastFocusedChildBeforeFocusOut->className() << " " << m_lastFocusedChildBeforeFocusOut->name()<< endl;
		QWidget *w = m_lastFocusedChildBeforeFocusOut;
		m_lastFocusedChildBeforeFocusOut = 0;
		w->setFocus();
	}
	else {
		if (hasFocus())
			setFocusInternal();
		else
			setFocusInternal();
	}
	m_mainWin->invalidateSharedActions(this);
}

QAction* KexiViewBase::sharedAction( const char *action_name )
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

void KexiViewBase::setAvailable(const char* action_name, bool set)
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

void KexiViewBase::updateActions(bool activated)
{
	//do nothing here
	//do the same for children :)
 	for (Q3PtrListIterator<KexiViewBase> it(m_children); it.current(); ++it) {
		it.current()->updateActions(activated);
	}
}

#include "kexiviewbase.moc"

