/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidialogbase.h"

#include "keximainwindow.h"
#include "kexiviewbase.h"
#include "kexicontexthelp_p.h"
#include "kexipart.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexipropertybuffer.h"
#include "kexiproject.h"
#include "kexi_utils.h"

#include <kexidb/connection.h>
#include <kexidb/utils.h>

#include <qwidgetstack.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kiconloader.h>

KexiDialogBase::KexiDialogBase(KexiMainWindow *parent, const QString &caption)
 : KMdiChildView(caption, parent, "KexiDialogBase")
 , KexiActionProxy(this, parent)
 , m_isRegistered(false)
 , m_origCaption(caption)
 , m_schemaData(0)
 , m_destroying(false)
 , m_disableDirtyChanged(false)
// , m_neverSaved(false)
{
	m_supportedViewModes = 0; //will be set by KexiPart
	m_openedViewModes = 0;
	m_currentViewMode = Kexi::NoViewMode; //no view available yet
	m_parentWindow = parent;
	m_newlySelectedView = 0;
	m_creatingViewsMode = -1;

	QVBoxLayout *lyr = new QVBoxLayout(this);
	m_stack = new QWidgetStack(this, "stack");
	lyr->addWidget(m_stack);

#ifdef KEXI_NO_CTXT_HELP
	m_contextHelpInfo=new KexiContextHelpInfo();
#endif
//	m_instance=parent->instance();
	m_id = -1;
	m_item = 0;

	hide(); //will be shown later
}

KexiDialogBase::~KexiDialogBase()
{
	m_destroying = true;
}

KexiViewBase *KexiDialogBase::selectedView() const
{
	if (m_destroying)
		return 0;
	return static_cast<KexiViewBase*>(m_stack->visibleWidget());
}

KexiViewBase *KexiDialogBase::viewForMode(int mode) const
{
	return static_cast<KexiViewBase*>( m_stack->widget(mode) );
}

void KexiDialogBase::addView(KexiViewBase *view)
{
	addView(view,0);
}

void KexiDialogBase::addView(KexiViewBase *view, int mode)
{
	m_stack->addWidget(view, mode);
//	addActionProxyChild( view );

	//set focus proxy inside this view
	QWidget *ch = static_cast<QWidget*>(view->child( 0, "QWidget", false ));
	if (ch)
		view->setFocusProxy(ch);

	m_openedViewModes |= mode;
}

QSize KexiDialogBase::minimumSizeHint() const
{
	KexiViewBase *v = selectedView();
	if (!v)
		return KMdiChildView::minimumSizeHint();
	return v->minimumSizeHint() + QSize(0, mdiParent() ? mdiParent()->captionHeight() : 0);
}

QSize KexiDialogBase::sizeHint() const
{
	KexiViewBase *v = selectedView();
	if (!v)
		return KMdiChildView::sizeHint();
	return v->preferredSizeHint( v->sizeHint() );
}

/*
KInstance *KexiDialogBase::instance() {
	return m_instance;
}*/

void KexiDialogBase::registerDialog() {
	m_parentWindow->registerChild(this);
	m_isRegistered=true;
	m_parentWindow->addWindow((KMdiChildView *)this);
//later	show();
//	m_parentWindow->activeWindowChanged(this);
}

bool KexiDialogBase::isRegistered(){
	return m_isRegistered;
}

void KexiDialogBase::attachToGUIClient() {
	if (!guiClient())
		return;

}

void KexiDialogBase::detachFromGUIClient() {
	if (!guiClient())
		return;
	//TODO
}

int KexiDialogBase::id() const 
{
	return (partItem() && partItem()->identifier()>0) ? partItem()->identifier() : m_id;
}

void KexiDialogBase::setContextHelp(const QString& caption, const QString& text, const QString& iconName) {
#ifdef KEXI_NO_CTXT_HELP
	m_contextHelpInfo->caption=caption;
	m_contextHelpInfo->text=text;
	m_contextHelpInfo->text=iconName;
	updateContextHelp();
#endif
}

void KexiDialogBase::closeEvent( QCloseEvent * e )
{
	//let any view send "closing" signal
	QObjectList *list = m_stack->queryList( "KexiViewBase", 0, false, false);
	KexiViewBase *view;
	QObjectListIt it( *list );
	for ( ;(view = static_cast<KexiViewBase*>(it.current()) ) != 0; ++it ) {
		emit view->closing();
	}
	delete list;

	emit closing();
	KMdiChildView::closeEvent(e);
}

#if 0
//js removed
bool KexiDialogBase::tryClose(bool dontSaveChanges)
{
	if (!dontSaveChanges && dirty()) {
/*TODO		if (KMessageBox::questionYesNo(this, "<b>"+i18n("Do you want save:")
		+"<p>"+typeName+" \""+ item->name() + "\"?</b>",
		0, KStdGuiItem::yes(), KStdGuiItem::no(), ???????)==KMessageBox::No)
		return false;*/
		//js TODO: save data using saveChanges()
	}
	close(true);
	return true;
}
#endif

bool KexiDialogBase::dirty() const
{
	//look for "dirty" flag
	int m = m_openedViewModes, mode = 1;
	while (m>0) {
		if (m & 1) {
			if (static_cast<KexiViewBase*>(m_stack->widget(mode))->dirty())
				return true;
		}
		m >>= 1;
		mode <<= 1;
	}
	return false;
/*	KexiViewBase *v = m_newlySelectedView ? m_newlySelectedView : selectedView();
	return v ? v->dirty() : false;*/
}

void KexiDialogBase::setDirty(bool dirty)
{
	m_disableDirtyChanged = true;
	int m = m_openedViewModes, mode = 1;
	while (m>0) {
		if (m & 1) {
			static_cast<KexiViewBase*>(m_stack->widget(mode))->setDirty(dirty);
		}
		m >>= 1;
		mode <<= 1;
	}
	m_disableDirtyChanged = false;
	dirtyChanged(); //update
}

QString KexiDialogBase::itemIcon()
{
	if (!m_part || !m_part->info()) {
		KexiViewBase *v = selectedView();
		if (v) {//m_stack->visibleWidget() && m_stack->visibleWidget()->inherits("KexiViewBase")) {
			return v->m_defaultIconName;
		}
		return QString::null;
	}
	return m_part->info()->itemIcon();
}

KexiPart::GUIClient* KexiDialogBase::guiClient() const
{
	if (!m_part || m_currentViewMode<1)
		return 0;
	return m_part->instanceGuiClient(m_currentViewMode);
}

KexiPart::GUIClient* KexiDialogBase::commonGUIClient() const
{
	if (!m_part)
		return 0;
	return m_part->instanceGuiClient(0);
}

bool KexiDialogBase::switchToViewMode( int newViewMode, bool &cancelled )
{
	kdDebug() << "KexiDialogBase::switchToViewMode()" << endl;
	cancelled = false;
	bool dontStore = false;
	KexiViewBase *view = selectedView();

	if (m_currentViewMode == newViewMode)
		return true;
	if (!supportsViewMode(newViewMode))
		return false;

	if (view) {
		bool success = view->beforeSwitchTo(newViewMode, cancelled, dontStore);
		if (cancelled)
			return true;
		if (!success)
			return false;
		if (!dontStore && view->dirty()) {
			if (!m_parentWindow->saveObject(this, cancelled, i18n("Design has been changed. You must save it before switching to other view.")))
				return false;
			if (cancelled)
				return true;
//			KMessageBox::questionYesNo(0, i18n("Design has been changed. You must save it before switching to other view."))
//				==KMessageBox::No
		}
	}

	//get view for viewMode
	KexiViewBase *newView = (m_stack->widget(newViewMode) && m_stack->widget(newViewMode)->inherits("KexiViewBase"))
		? static_cast<KexiViewBase*>(m_stack->widget(newViewMode)) : 0;
	if (!newView) {
		Kexi::setWaitCursor();
		//ask the part to create view for the new mode
		m_creatingViewsMode = newViewMode;
		newView = m_part->createView(m_stack, this, *m_item, newViewMode);
		Kexi::removeWaitCursor();
		if (!newView) {
			//js TODO error?
			kdDebug() << "Switching to mode " << newViewMode << " failed. Previous mode "
				<< m_currentViewMode << " restored." << endl;
			return false;
		}
		m_creatingViewsMode = -1;
		addView(newView, newViewMode);
	}
	if (!newView->beforeSwitchTo(newViewMode, cancelled, dontStore)) {
		kdDebug() << "Switching to mode " << newViewMode << " failed. Previous mode "
			<< m_currentViewMode << " restored." << endl;
		return false;
	}
	const int prevViewMode = m_currentViewMode;
	m_currentViewMode = newViewMode;
	m_newlySelectedView = newView;
	if (prevViewMode==Kexi::NoViewMode)
		m_newlySelectedView->setDirty(false);
	if (!newView->afterSwitchFrom(prevViewMode, cancelled)) {
		kdDebug() << "Switching to mode " << newViewMode << " failed. Previous mode "
			<< prevViewMode << " restored." << endl; 
		m_currentViewMode = prevViewMode;
		return false;
	}
	m_newlySelectedView = 0;
	if (cancelled) {
		m_currentViewMode = prevViewMode;
		return true;
	}
	if (view)
		takeActionProxyChild( view ); //take current proxy child
	addActionProxyChild( newView ); //new proxy child
	m_stack->raiseWidget( newView );
	newView->propertyBufferSwitched();
	m_parentWindow->invalidateSharedActions( newView );
//	setFocus();
	return true;
}

void KexiDialogBase::setFocus()
{
	if (m_stack->visibleWidget()) {
		if (m_stack->visibleWidget()->inherits("KexiViewBase"))
			static_cast<KexiViewBase*>( m_stack->visibleWidget() )->setFocus();
		else
			m_stack->visibleWidget()->setFocus();
	}
	else {
		KMdiChildView::setFocus();
	}
	activate();
}

KexiPropertyBuffer *KexiDialogBase::propertyBuffer()
{
	KexiViewBase *v = selectedView();
	if (!v)
		return 0;
	return v->propertyBuffer();
}

bool KexiDialogBase::eventFilter(QObject *obj, QEvent *e)
{
	if (KMdiChildView::eventFilter(obj, e))
		return true;
	if (m_stack->visibleWidget() && Kexi::hasParent(m_stack->visibleWidget(), obj)) {
		if ((e->type()==QEvent::FocusIn && m_parentWindow->activeWindow()==this)
			|| e->type()==QEvent::MouseButtonPress) 
		{
			//pass the activation
			activate();
		}
	}
	return false;
}

void KexiDialogBase::dirtyChanged()
{
	if (m_disableDirtyChanged)
		return;
/*	if (!dirty()) {
		if (caption()!=m_origCaption)
			KMdiChildView::setCaption(m_origCaption);
	}
	else {
		if (caption()!=(m_origCaption+"*"))
			KMdiChildView::setCaption(m_origCaption+"*");
	}*/
	updateCaption();
	emit dirtyChanged(this);
}

/*QString KexiDialogBase::caption() const
{
	return m_origCaption;
	if (dirty())
		return KMdiChildView::caption()+;

	return KMdiChildView::caption();
}*/

void KexiDialogBase::updateCaption()
{
	if (!m_item || !m_origCaption.isEmpty())
		return;
//	m_origCaption = c;
	QString capt = m_item->name();
	QString fullCapt = capt;
	if (m_part)
		fullCapt += (" : " + m_part->instanceName());
	if (dirty()) {
		KMdiChildView::setCaption(fullCapt+"*");
		KMdiChildView::setTabCaption(capt+"*");
	}
	else {
		KMdiChildView::setCaption(fullCapt);
		KMdiChildView::setTabCaption(capt);
	}
}

bool KexiDialogBase::neverSaved() const 
{
	return m_item ? m_item->neverSaved() : true;
}

bool KexiDialogBase::storeNewData(bool &cancel)
{
	cancel = false;
	if (!neverSaved())
		return false;
	KexiViewBase *v = selectedView();
	if (m_schemaData)
		return false; //schema must not exist
	if (!v)
		return false;
	//create schema object and assign information
	KexiDB::SchemaData sdata(m_part->info()->projectPartID());
	sdata.setName( m_item->name() );
	sdata.setCaption( m_item->caption() );
	sdata.setDescription( m_item->description() );

	m_schemaData = v->storeNewData(sdata, cancel);
	if (cancel)
		return true;
	if (!m_schemaData) {
		setStatus(i18n("Saving object's definition failed."),""); 
		return false;
	}
	/* Sets 'dirty' flag on every dialog's view. */
	setDirty(false);
//	v->setDirty(false);
	//new schema data has now ID updated to a unique value 
	//-assign that to item's identifier
	m_item->setIdentifier( m_schemaData->id() );
	m_item->setNeverSaved(false);
	return true;
}

bool KexiDialogBase::storeData(bool &cancel)
{
	cancel = false;
	if (neverSaved())
		return false;
	KexiViewBase *v = selectedView();
	if (!v)
		return false;
	const bool r = v->storeData(cancel);
	if (cancel)
		return true;
	if (!r) {
		setStatus(i18n("Saving object's data failed."),""); 
		return false;
	}
	/* Sets 'dirty' flag on every dialog's view. */
	setDirty(false);
//	v->setDirty(false);
	return true;
}

bool KexiDialogBase::storeDataBlock_internal( const QString &dataString, int o_id, const QString& dataID )
{
	if (o_id<=0)
		return false;
	KexiDB::Connection *conn = m_parentWindow->project()->dbConnection();
	KexiDB::Driver *drv = conn->driver();
	
	QString sql = "select kexi__objectdata.o_id from kexi__objectdata where o_id=" + QString::number(o_id);
	QString sql_sub = KexiDB::sqlWhere(drv, KexiDB::Field::Text, "o_sub_id", dataID);

	bool ok, exists;
	exists = conn->resultExists(sql + " and " + sql_sub, ok);
	if (!ok)
		return false;
	if (exists) {
		return conn->executeSQL( "update kexi__objectdata set o_data="
			+ drv->valueToSQL( KexiDB::Field::BLOB, dataString )
			+ " where o_id=" + QString::number(o_id) + " and " + sql_sub );
	}
	return conn->executeSQL( 
		"insert into kexi__objectdata (o_id, o_data, o_sub_id) values ("
		+ QString::number(o_id) +"," + drv->valueToSQL( KexiDB::Field::BLOB, dataString )
		+ "," + drv->valueToSQL( KexiDB::Field::Text, dataID ) + ")" );
}

bool KexiDialogBase::loadDataBlock( QString &dataString, const QString& dataID )
{
	KexiDB::Connection *conn = m_parentWindow->project()->dbConnection();
	if (!conn->querySingleString(
		QString("select o_data from kexi__objectdata where o_id=") + QString::number(id())
		+ " and " + KexiDB::sqlWhere(conn->driver(), KexiDB::Field::Text, "o_sub_id", dataID), 
		dataString ))
	{
		setStatus(conn->errorMsg(), ""); 
		return false;
	}
	return true;
}

bool KexiDialogBase::storeDataBlock( const QString &dataString, const QString& dataID )
{
	return storeDataBlock_internal(dataString, id(), dataID);
}

bool KexiDialogBase::removeDataBlock( QString &dataString, const QString& dataID)
{
	KexiDB::Connection *conn = m_parentWindow->project()->dbConnection();
	bool ok;
	if (dataID.isEmpty())
		ok = KexiDB::deleteRow(*conn, "kexi__objectdata", "o_id", QString::number(id()));
	else
		ok = KexiDB::deleteRow(*conn, "kexi__objectdata", 
		"o_id", KexiDB::Field::Integer, id(), "o_sub_id", KexiDB::Field::Text, dataID);
	if (!ok)
		setStatus(conn->errorMsg(), ""); 
	return ok;
}

void KexiDialogBase::activate()
{
	KMdiChildView::activate();
	KexiViewBase *v = selectedView();
	if (v)
		v->updateActions(true);
//js: not neeed??	m_parentWindow->invalidateSharedActions(this);
}

void KexiDialogBase::deactivate()
{
	KexiViewBase *v = selectedView();
	if (v)
		v->updateActions(false);
}

#include "kexidialogbase.moc"

