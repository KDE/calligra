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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexiviewbase.h"

#include "keximainwindow.h"
#include "kexidialogbase.h"
#include "kexipropertybuffer.h"
#include "kexiproject.h"
#include "kexi_utils.h"

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kdebug.h>

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

bool KexiViewBase::beforeSwitchTo(int /* mode */, bool & /*cancelled*/, bool & /*dontStore*/)
{
	return true;
}

bool KexiViewBase::afterSwitchFrom(int /* mode */, bool & /*cancelled*/)
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
	emit closing();
	QWidget::closeEvent(e);
}

KexiPropertyBuffer *KexiViewBase::propertyBuffer()
{
	return 0;
}

void KexiViewBase::propertyBufferSwitched()
{
	if (parentDialog())
		m_mainWin->propertyBufferSwitched( parentDialog(), false );
}

void KexiViewBase::propertyBufferReloaded()
{
	if (parentDialog())
		m_mainWin->propertyBufferSwitched( parentDialog(), true );
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
	m_dirty = set;
	m_dirty = dirty();
//	if (m_dirty!=set)//eventually didn't change
//		return;
	if (m_parentView) {
		m_parentView->setDirty(m_dirty);
	}
	else {
		if (m_dialog)
			m_dialog->dirtyChanged();
	}
}

/*bool KexiViewBase::saveData()
{
	//TODO....

	//finally:
	setDirty(false);
	return true;
}*/

KexiDB::SchemaData* KexiViewBase::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
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

bool KexiViewBase::storeData(bool &cancel)
{
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

	return effectiveID>0 && m_dialog->storeDataBlock_internal( dataString, effectiveID, dataID );
}

//bool KexiViewBase::loadDataBlock( QString &dataString, const QString &id, const QString& dataID )
bool KexiViewBase::loadDataBlock( QString &dataString, const QString& dataID )
{
	if (!m_dialog)
		return false;
	return m_dialog->loadDataBlock(dataString, dataID);
}


bool KexiViewBase::eventFilter( QObject *o, QEvent *e )
{
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {// && o->inherits("QWidget")) {
//		//hp==true if currently focused widget is a child of this table view
//		const bool hp = Kexi::hasParent( static_cast<QWidget*>(o), focusWidget());
		if (Kexi::hasParent( this, static_cast<QWidget*>(o))) {
			if (e->type()==QEvent::FocusOut && focusWidget() && !Kexi::hasParent( this, focusWidget())) {
				//focus out: when currently focused widget is not a parent of this view
				emit focus(false);
			} else if (e->type()==QEvent::FocusIn) {
				emit focus(true);
			}
			if (e->type()==QEvent::FocusOut) { // && focusWidget() && Kexi::hasParent( this, focusWidget())) { // && focusWidget()->inherits("KexiViewBase")) {
//				kdDebug() << focusWidget()->className() << " " << focusWidget()->name()<< endl;
				kdDebug() << o->className() << " " << o->name()<< endl;
				KexiViewBase *v = Kexi::findParent<KexiViewBase>(o, "KexiViewBase") ;
				if (v) {
					while (v->m_parentView)
						v = v->m_parentView;
					v->m_lastFocusedChildBeforeFocusOut = static_cast<QWidget*>(o); //focusWidget();
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

void KexiViewBase::setViewWidget(QWidget* w)
{
	if (m_viewWidget == w)
		return;
	if (m_viewWidget) {
		m_viewWidget->removeEventFilter(this);
	}
	m_viewWidget = w;
	if (m_viewWidget) {
		m_viewWidget->installEventFilter(this);
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
		kdDebug() << "FOCUS: " << m_lastFocusedChildBeforeFocusOut->className() << " " << m_lastFocusedChildBeforeFocusOut->name()<< endl;
		m_lastFocusedChildBeforeFocusOut->setFocus();
	}
	else
		QWidget::setFocus();
	m_mainWin->invalidateSharedActions(this);
}

KAction* KexiViewBase::sharedAction( const char *name ) const
{
	if (!part() || !part()->actionCollectionForMode( viewMode() ))
		return 0;
	return part()->actionCollectionForMode( viewMode() )->action( name );
}

void KexiViewBase::updateActions(bool activated)
{
	//do nothing here
	//do the same for children :)
	for (QPtrListIterator<KexiViewBase> it(m_children); it.current(); ++it) {
		it.current()->updateActions(activated);
	}
}

#include "kexiviewbase.moc"

