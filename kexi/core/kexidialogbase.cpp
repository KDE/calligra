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

#include "keximainwindow.h"
#include "kexidialogbase.h"
#include "kexicontexthelp_p.h"
#include "kexipart.h"
#include "kexipartinfo.h"

#include <qwidgetstack.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kiconloader.h>

KexiDialogBase::KexiDialogBase(KexiMainWindow *parent, const QString &caption)
 : KMdiChildView(caption, parent, "KexiDialogBase")
 , KexiActionProxy(this, parent)
 , m_isRegistered(false)
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
	m_docID = -1;
	m_item = 0;
}

KexiDialogBase::~KexiDialogBase()
{
}

void KexiDialogBase::addView(QWidget *view)
{
	m_stack->addWidget(view, 0);
}

void KexiDialogBase::addView(QWidget *view, int viewMode)
{
	m_stack->addWidget(view, viewMode);
}

QSize KexiDialogBase::minimumSizeHint() const
{
	QWidget *v = m_stack->visibleWidget();
	if (!v)
		return KMdiChildView::minimumSizeHint();
	return v->minimumSizeHint();
}

QSize KexiDialogBase::sizeHint() const
{
	QWidget *v = m_stack->visibleWidget();
	if (!v)
		return KMdiChildView::sizeHint();
	return v->sizeHint();
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


void
KexiDialogBase::setDocID(int id)
{
	kdDebug() << "KexiDialogBase::setDocID(): id = " << id << endl;
	m_docID = id;
//	m_parentWindow->registerChild(this);
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
	emit closing();
	KMdiChildView::closeEvent(e);
}

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

bool KexiDialogBase::dirty()
{
	return false;
}

QString KexiDialogBase::itemIcon()
{
	if (!m_part || !m_part->info())
		return QString::null;
	return m_part->info()->itemIcon();
}

bool KexiDialogBase::switchToViewMode( int viewMode )
{
	if (m_currentViewMode == viewMode)
		return true;
	if (!supportsViewMode(viewMode))
		return false;

	QWidget *view = m_stack->widget(viewMode);
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
	m_currentViewMode = viewMode;
	return true;
}

#include "kexidialogbase.moc"

