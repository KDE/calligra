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

KexiViewBase::KexiViewBase(KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : QWidget(parent, name)
 , KexiActionProxy(this, mainWin)
 , m_mainWin(mainWin)
 , m_dirty(false)
{
	QWidget *wi=this;
	while ((wi = wi->parentWidget()) && !wi->inherits("KexiDialogBase"))
		;
	m_dialog = (wi && wi->inherits("KexiDialogBase")) ? static_cast<KexiDialogBase*>(wi) : 0;
}

KexiViewBase::~KexiViewBase()
{
}

bool KexiViewBase::beforeSwitchTo(int /* mode */)
{
	return true;
}

bool KexiViewBase::afterSwitchFrom(int /* mode */)
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
	KexiDialogBase *dlg = parentDialog();

	if (dlg)
		m_mainWin->propertyBufferSwitched( dlg );
}

void KexiViewBase::setDirty(bool set)
{
	if (m_dirty == set)
		return;
	m_dirty = set;
	m_dirty = dirty();
	if (m_dirty!=set)//eventually didn't change
		return;
	if (m_dialog)
		m_dialog->dirtyChanged();
}

bool KexiViewBase::saveData()
{
	//TODO....

	//finally:
	setDirty(false);
	return true;
}

#include "kexiviewbase.moc"

