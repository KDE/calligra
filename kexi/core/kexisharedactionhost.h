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

#ifndef KEXISHAREDACTIONHOST_H
#define KEXISHAREDACTIONHOST_H

#include <qpointer.h>
#include <q3asciidict.h>
#include <qobject.h>
#include <qpair.h>

#include <kstandardaction.h>
#include <kaction.h>

#include "../kexi_export.h"

class KShortcut;
class KGuiItem;
class KexiMainWindow;
class KexiActionProxy;
class KexiSharedActionHostPrivate;

namespace KexiPart {
	class Part;
}

//! Acts as application-wide host that offers shared actions.
/*!
 You can inherit this class together with KexiMainWindow.
 Call setAsDefaultHost() to make the host default for all shared actions that have 
 not explicitly specified host.

 For example how all this is done, see KexiMainWindow implementation.

 \sa KexiActionProxy, KexiMainWindow
*/

class KEXICORE_EXPORT KexiSharedActionHost
{
	public:

		/*! Constructs host for main window \a mainWin. */
		KexiSharedActionHost(KexiMainWindow* mainWin);

		virtual ~KexiSharedActionHost();

		/*! \return true if \a w can accept shared actions. 
		 This method is used by focusWindow() to look up widgets hierarchy 
		 for widget that can accept shared actions. 
		 Default implementation always returns false. 
		 You can reimplement it e.g. like in KexiMainWindowImpl::acceptsSharedActions():
		 \code
			return o->inherits("KexiDialogBase") || o->inherits("KexiViewBase");
		 \endcode
		 */
		virtual bool acceptsSharedActions(QObject *o);

		/*! \return window widget that is currently focused (using QWidget::focusWidget())
		 and matches acceptsSharedActions(). If focused widget does not match,
		 it's parent, grandparent, and so on is checked. If all this fails, 
		 or no widget has focus, NULL is returned. 
		 Also works if currently focused window is detached (as in KMDI).
		 */
		QWidget* focusWindow();

		/*! Sets this host as default shared actions host. */
		void setAsDefaultHost();

		/*! \return default shared actions host, used when no host 
		 is explicitly specified for shared actions.
		 There can be exactly one deault shared actions host. */
		static KexiSharedActionHost& defaultHost();

		/*! \return shared actions list. */
		QList<KAction*> sharedActions() const;

		/*! PROTOTYPE, DO NOT USE YET */
		void setActionVolatile( KAction *a, bool set );


	protected:

		/*! Invalidates all shared actions declared using createSharedAction().
		 Any shared action will be enabled if \a o (typically: a child window or a dock window)
		 has this action plugged _and_ it is available (i.e. enabled). 
		 Otherwise the action is disabled.

		 If \a o is not KexiDialogBase or its child,
		 actions are only invalidated if these come from mainwindow's KActionCollection
		 (thus part-actions are untouched when the focus is e.g. in the Property Editor.

		 Call this method when it is known that some actions need invalidation 
		 (e.g. when new window is activated). See how it is used in KexiMainWindowImpl.
		*/
		virtual void invalidateSharedActions(QObject *o);

		void setActionAvailable(const QString& action_name, bool avail);

		/*! Plugs shared actions proxy \a proxy for this host. */
		void plugActionProxy(KexiActionProxy *proxy);

		/*! Updates availability of action \a action_name for object \a obj.
		 Object is mainly the window. */
		void updateActionAvailable(const QString& action_name, bool avail, QObject *obj);

		/*! \return main window for which this host is defined. */
		KexiMainWindow* mainWindow() const;

		/*! Creates shared action using \a text, \a pix_name pixmap, shortcut \a cut,
		 optional \a name. You can pass your own action collection as \a col. 
		 If \a col action collection is null, main window's action will be used.
		 Pass desired KAction subclass with \a subclassName (e.g. "KToggleAction") to have
		 that subclass allocated instead just KAction (what is the default).
		 Created action's data is owned by the main window. */
		KAction* createSharedAction(const QString &text, const QString &pix_name, 
			const KShortcut &cut, const char *name, KActionCollection* col = 0,
			const char *subclassName = 0);

		/*! Like above - creates shared action, but from standard action identified by \a id. 
		 Action's data is owned by the main window. */
		KAction* createSharedAction( KStandardAction::StdAction id, const char *name, 
			KActionCollection* col = 0);

		/*! Creates shared action with name \a name and shortcut \a cut 
		 by copying all properties of \a guiItem. 
		 If \a col action collection is null, main window's action will be used. */
		KAction* createSharedAction(const KGuiItem& guiItem, const KShortcut &cut, const char *name,
			KActionCollection* col = 0);

		/*! \return action proxy for object \a o, or NULL if this object has 
		 no plugged shared actions. */
		KexiActionProxy* actionProxyFor(QObject *o) const;

		/*! Like actionProxyFor(), but takes the proxy from the host completely.
		 This is called by KExiActionProxy on its destruction. */
		KexiActionProxy* takeActionProxyFor(QObject *o);

	private:
		/*! Helper function for createSharedAction(). */
		KAction* createSharedActionInternal( KAction *action );

		KexiSharedActionHostPrivate *d;

	friend class KexiActionProxy;
	friend class KexiPart::Part;
	friend class KexiViewBase;
	friend class KexiDialogBase;
};

#endif

