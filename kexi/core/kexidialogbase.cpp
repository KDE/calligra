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
#include "kexi_utils.h"

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
// , m_neverSaved(false)
{
	m_supportedViewModes = 0; //will be set by KexiPart
	m_currentViewMode = 0; //override this!
	m_parentWindow=parent;

	QVBoxLayout *lyr = new QVBoxLayout(this);
	m_stack = new QWidgetStack(this, "stack");
	lyr->addWidget(m_stack);

#ifdef KEXI_NO_CTXT_HELP
	m_contextHelpInfo=new KexiContextHelpInfo();
#endif
//	m_instance=parent->instance();
	m_id = -1;
	m_item = 0;
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

void KexiDialogBase::addView(KexiViewBase *view)
{
	addView(view,0);
}

void KexiDialogBase::addView(KexiViewBase *view, int mode)
{
	m_stack->addWidget(view, mode);
	addActionProxyChild( view );

	//set focus proxy inside this view
	QWidget *ch = static_cast<QWidget*>(view->child( 0, "QWidget", false ));
	if (ch)
		view->setFocusProxy(ch);
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
	show();
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
	return partItem() ? partItem()->identifier() : m_id;
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
	KexiViewBase *v = selectedView();
	return v ? v->dirty() : false;
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

bool KexiDialogBase::switchToViewMode( int viewMode )
{
	kdDebug() << "KexiDialogBase::switchToViewMode()" << endl;
	KexiViewBase *view = selectedView();

	if (m_currentViewMode == viewMode)
		return true;
	if (!supportsViewMode(viewMode))
		return false;

	if (view)
		view->beforeSwitchTo(viewMode);

	//get view for viewMode
	view = (m_stack->widget(viewMode) && m_stack->widget(viewMode)->inherits("KexiViewBase"))
		? static_cast<KexiViewBase*>(m_stack->widget(viewMode)) : 0;
	if (!view) {
		//ask the part to create view for the new mode
		view = m_part->createView(m_stack, this, *m_item, viewMode);
		if (!view) {
			//js TODO error?
			return false;
		}
		addView(view, viewMode);
	}
	m_stack->raiseWidget(view);
	view->afterSwitchFrom(m_currentViewMode);
	m_currentViewMode = viewMode;

	view->propertyBufferSwitched();
	return true;
}

void KexiDialogBase::setFocus()
{
	if (m_stack->visibleWidget()) {
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
		if (e->type()==QEvent::FocusIn || e->type()==QEvent::MouseButtonPress) {
			//pass the activation
			activate();
		}
	}
	return false;
}

void KexiDialogBase::dirtyChanged()
{
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

bool KexiDialogBase::storeNewData()
{
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

	m_schemaData = v->storeNewData(sdata);
	if (!m_schemaData)
		return false;
	//new schema data has now ID updated to a unique value 
	//-assign that to item's identifier
	m_item->setIdentifier( m_schemaData->id() );
	m_item->setNeverSaved(false);
	return true;
}

bool KexiDialogBase::storeData()
{
	if (neverSaved())
		return false;
	KexiViewBase *v = selectedView();
	if (!v)
		return false;
	return v->storeData() != 0;
}

#include "kexidialogbase.moc"

