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
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIMDI_H
#define KEXIMDI_H

#include <QRect>
#include <QWidget>
#include <kxmlguiwindow.h>
//#include <kxmlguiclient.h>

#include <kexi_export.h>

class KexiMdiMainFrm;

/// KMdiChildView replacment
class KEXICORE_EXPORT KexiMdiChildView : public QWidget
{
		Q_OBJECT
	public:
		KexiMdiChildView(KexiMdiMainFrm* parent, const QString &caption);
		virtual ~KexiMdiChildView();
		KexiMdiMainFrm* mdiParent() const;
		void setTabCaption(const QString &caption);
		QWidget* focusedChildWidget();
		void activate();
};

/// KMdiMainFrm replacment
class KEXICORE_EXPORT KexiMdiMainFrm : public KXmlGuiWindow
{
		Q_OBJECT
	public:
		KexiMdiMainFrm(QWidget* parent = 0);
		virtual ~KexiMdiMainFrm();

		int captionHeight() const;
		QRect mdiAreaContentsRect() const;

		enum MdiMode { ToplevelMode };
		MdiMode mdiMode();

		enum MdiFlags { StandardAdd, Detach /*, Hide, Minimize, ToolWindow, Maximize*/ };
		void addWindow(KexiMdiChildView* pView, MdiFlags flags);
		void detachWindow(KexiMdiChildView *pWnd, bool bShow=true);

		KexiMdiChildView* activeWindow();

		enum MdiWindowState { Normal, Maximized, Minimized };
		MdiWindowState state () const;
};

#endif
