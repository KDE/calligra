/* This file is part of the KDE project
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "keximdi.h"

#ifdef __GNUC__
 #warning "Implement MDI replacment"
#endif

/***************************************************************************
 * KexiMdiChildView
 */

KexiMdiChildView::KexiMdiChildView(KexiMdiMainFrm* parent, const QString &caption)
	: QWidget(parent)
{
}

KexiMdiChildView::~KexiMdiChildView()
{
}

KexiMdiMainFrm* KexiMdiChildView::mdiParent() const
{
	return static_cast<KexiMdiMainFrm*>(parent());
}

void KexiMdiChildView::setTabCaption(const QString &caption)
{
}

QWidget* KexiMdiChildView::focusedChildWidget()
{
	return 0;
}

void KexiMdiChildView::activate()
{
}

/***************************************************************************
 * KexiMdiMainFrm
 */

KexiMdiMainFrm::KexiMdiMainFrm(QWidget* parent)
	: KXmlGuiWindow(parent)
{
}

KexiMdiMainFrm::~KexiMdiMainFrm()
{
}

int KexiMdiMainFrm::captionHeight() const
{
	return 0;
}

QRect KexiMdiMainFrm::mdiAreaContentsRect() const
{
	return QRect();
}

KexiMdiMainFrm::MdiMode KexiMdiMainFrm::mdiMode()
{
	return ToplevelMode;
}

void KexiMdiMainFrm::addWindow(KexiMdiChildView* pView, MdiFlags flags)
{
}

void KexiMdiMainFrm::detachWindow(KexiMdiChildView *pWnd, bool bShow)
{
}

KexiMdiChildView* KexiMdiMainFrm::activeWindow()
{
	return 0;
}

KexiMdiMainFrm::MdiWindowState KexiMdiMainFrm::state () const
{
	return Normal;
}

#include "keximdi.moc"
