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

#ifndef KEXISHAREDACTIONHOST_H
#define KEXISHAREDACTIONHOST_H

#include <qguardedptr.h>
#include <qasciidict.h>
#include <qobject.h>
#include <qpair.h>

#include <kstdaction.h>

class KActionCollection;
class KAction;
class KShortcut;
class KMainWindow;
class KexiActionProxy;
class KexiSharedActionHostPrivate;

//! Acts as application-wide host that offers shared actions.
/*!
 You can inherit this class together with KMainWindow (or any KMainWindow).
 Call setAsDefaultHost() to make the host default for all shared actions that have 
 not explicity specified host.

 For example how all this is done, see KexiMainWindow implementation.

 \sa KexiActionProxy, KexiMainWindow
*/

class KEXICORE_EXPORT KexiSharedActionHost
{
	public:

		/*! Constructs host for main window \a mainWin. */
		KexiSharedActionHost(KMainWindow* mainWin);

		~KexiSharedActionHost();

		/*! \return true if \a w can accept shared actions. 
		 This method is used by focusWindow() to look up widgets hierarchy 
		 for widget that can accept shared actions. 
		 Default implementation always returns false. 
		 You can reimplement it e.g. like in KexiMainWindow::acceptsSharedActions():
		 <code>
			return w->inherits("KexiDialogBase") || w->inherits("KexiDockBase");
		 </code>
		 */
		virtual bool acceptsSharedActions(QObject *o);

		/*! \return widget that is currently focused (using QWidget::focusWidget())
		 and matches acceptsSharedActions(). If focused widget does not match,
		 it's parent, grandparent, and so on is checked. If all this fails, 
		 or no widget has focus, NULL is returned. */
		QWidget* focusWindow();

		/*! Sets this host as default shared actions host. */
		void setAsDefaultHost();

		/*! \return default shared actions host, used when no host is explicity specified for shared actions.
		 There can be exactly one deault shared actions host. */
		static KexiSharedActionHost& defaultHost();

	protected:

		/*! Invalidates all shared actions declared using createSharedAction().
		 Any shared action will be enabled if \a o (typically: a child window or a dock window)
		 has this action plugged _and_ it is available (i.e. enabled). Otherwise the action is disabled.

		 Call this method when it is known that some actions need invalidation 
		 (e.g. when new window is activated). See how it is used in KexiMainWindow.
		*/
		void invalidateSharedActions(QObject *o);

		void setActionAvailable(const char *action_name, bool avail);

		/*! Plugs shared actions proxy \a proxy for this host. */
		void plugActionProxy(KexiActionProxy *proxy);

		/*! Updates availability of action \a action_name for object \a obj.
		 Object is mainly the window. */
		void updateActionAvailable(const char *action_name, bool avail, QObject *obj);

		/*! \return main window that acts for which this host is defined. */
		KMainWindow* mainWindow() const;

		/*! Creates shared action. Action's data is owned by the main window. */
		KAction* createSharedAction(const QString &text, const QString &pix_name, 
			const KShortcut &cut, const char *name);

		/*! Like above - creates shared action, but from standard action identified by \a id. 
		 Action's data is owned by the main window. */
		KAction* createSharedAction( KStdAction::StdAction id, const char *name);

		/*! \return action proxy for object \a o, or NULL if this object has no plugged shared actions. */
		KexiActionProxy* actionProxyFor(QObject *o) const;

		/*! Like actionProxyFor(), but takes the proxy from the host completely.
		 This is called by KExiActionProxy on its destruction. */
		KexiActionProxy* takeActionProxyFor(QObject *o);

	private:
		/*! Helper function for createSharedAction(). */
		KAction* createSharedActionInternal( KAction *action );

		KexiSharedActionHostPrivate *d;

	friend class KexiActionProxy;
};

#endif

