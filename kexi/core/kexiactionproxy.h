/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIACTIONPROXY_H
#define KEXIACTIONPROXY_H

#include <qpointer.h>
#include <qobject.h>
#include <qpair.h>

#include "kexiproject.h"
#include "kexisharedactionhost.h"

class Q3Signal;
class QAction;
class KAction;
class KXMLGUIClient;
class KAction_setEnabled_Helper;
class KexiActionProxy;

//! Abstract helper class used to connect shared actions from outside of shared-action-aware object.
/*! Methods like KexiActionProxy::plugSharedAction() are not public, but
 sometimes there's need for plugging an object that implements KexiActionProxy interface
 from outside.
 
 Reimplement KexiSharedActionConnector: do all needed connections in the constructor.

 For example, with KexiQueryDesignerSQLEditor class we're using KTextEdit
 (or KTextEditor::View) that's not shared-action-aware. So it's needed to conenct
 e.g. "edit_undo" shared action to undo() slot, and so on. It is impelmented in more
 generic way by implementing KTextEdit_SharedActionConnector class,
 so the conenction can be reused many times by just allocating KTextEdit_SharedActionConnector 
 object for any KTextEditor when required (not only within KexiQueryDesignerSQLEditor).
*/
//TODO add method for setAvailable()
class KEXICORE_EXPORT KexiSharedActionConnector
{
	public:
		/* Connects shared actions offered by \a proxy to \a obj. */
		KexiSharedActionConnector(KexiActionProxy* proxy, QObject *obj);
		~KexiSharedActionConnector();

	protected:
		void plugSharedAction(const QString& action_name, const char *slot);

		void plugSharedActionToExternalGUI(const QString& action_name, KXMLGUIClient *client);

		void plugSharedActionsToExternalGUI(QList<QString> action_names, KXMLGUIClient *client);

		KexiActionProxy* m_proxy;
		QObject *m_object;
};

//! An interface that acts as proxy for shared actions within the application.
/*!
 For example, edit->copy action can be reused to copy different types of items.
 Availability and meaning of given action depends on the context, while
 the context changes e.g. when another window is activated.
 This class is mostly used by subclassing in KexiWindow or KexiDockBase
 - you can subclass in a similar way.
*/

class KEXICORE_EXPORT KexiActionProxy
{
	public:
		/*! Constructs action proxy for object \a receiver, using \a host.
		 If \a host is NULL, KexiSharedActionHost::defaultHost() is used.
		 (you must be sure that it's true) -- it is casted to QObject and assigned as the receiver.*/
		KexiActionProxy(QObject *receiver , KexiSharedActionHost *host = 0 );
		virtual ~KexiActionProxy();

		/*! Activates  action named \a action_name for this proxy. If the action is executed
		 (accepted), true is returned. */
		bool activateSharedAction(const QString& action_name, bool alsoCheckInChildren = true);

		/*! Sets host to \a host; rerely used. */
		void setSharedActionHost(KexiSharedActionHost& host) { m_host = &host; }

		/*! \return true, if action named \a action_name is enabled within the proxy.
		 False is returned either if the action is not available or is not supported.
		 \ sa isSupported() */
		bool isAvailable(const QString& action_name, bool alsoCheckInChildren = true) const;

		/*! \return true, if action named \a action_name is supported by the proxy. */
		bool isSupported(const QString& action_name) const;

	protected:
		/*! Plugs shared action named \a action_name to slot \a slot in \a receiver.
		 \a Receiver is usually a child of _this_ widget. */
		void plugSharedAction(const QString& action_name, QObject* receiver, const char *slot);

		void unplugSharedAction(const QString& action_name);

		/*! Typical version of plugAction() method -- plugs action named \a action_name
		 to slot \a slot in _this_ widget. */
		inline void plugSharedAction(const QString& action_name, const char *slot) {
			plugSharedAction(action_name, m_receiver, slot);
		}

		/*! Plugs action named \a action_name to a widget \a w, so the action is visible on this widget 
		 as an item. \a w will typically be a menu, popup menu or a toolbar. 
		 Does nothing if no action found, so generally this is safer than just caling e.g.
		 <code> action("myaction")->plug(myPopup); </code> 
		 \sa action(), QWidget::addAction(QAction*) */
		void plugSharedAction(const QString& action_name, QWidget* w);

		void plugSharedActionToExternalGUI(const QString& action_name, KXMLGUIClient *client);

		void plugSharedActionsToExternalGUI(QList<QString> action_names, KXMLGUIClient *client);

		/*! Unplugs action named \a action_name from a widget \a w.
		 \sa plugSharedAction(const char *action_name, QWidget* w) */
		void unplugSharedAction(const QString& action_name, QWidget* w);

		/*! Like above, but creates alternative action as a copy of \a action_name,
		 with \a alternativeText set. When this action is activated, just original action
		 specified by \a action_name is activated. The aternative action has autmatically set name as:
		 action_name + "_alt". 
		 \return newly created action or 0 if \a action_name not found. */
		KAction* plugSharedAction(const QString& action_name, const QString& alternativeText, QWidget* w);

		/*! \return action named with \a name or NULL if there is no such action. */
		virtual QAction* sharedAction(const QString& action_name);

		inline QObject *receiver() const { return m_receiver; }

		virtual void setAvailable(const QString& action_name, bool set);

		/*! Adds \a child of this proxy. Children will receive activateSharedAction() event,
		 If activateSharedAction() "event" is not consumed by the main proxy,
		 we start to iterate over proxy children (in unspecified order) to and call 
		 activateSharedAction() on every child until one of them accept the "event".
		 
		 If proxy child is destroyed, it is automatically detached from its parent proxy. 
		 Parent proxy is 0 by default. This pointer is properly cleared when parent proxy is destroyed. */
		void addActionProxyChild( KexiActionProxy* child );

		void takeActionProxyChild( KexiActionProxy* child );

		KexiSharedActionHost *m_host;
		QPointer<QObject> m_receiver;
		QMap<QString, QPair<Q3Signal*,bool>* > m_signals;

		QList<KexiActionProxy*> m_sharedActionChildren;

		QList<KAction*> m_alternativeActions;

		KexiActionProxy* m_actionProxyParent;

		QObject m_signal_parent; //!< it's just to have common parent for owned signals

		//! For internal use by plugSharedActionToExternalGUI()
		KAction_setEnabled_Helper *m_KAction_setEnabled_helper;

	public:
		//! For internal use by addActionProxyChild(). \a parent can be 0.
		void setActionProxyParent_internal( KexiActionProxy* parent );

		//! @internal
		KexiActionProxy *m_focusedChild;

	friend class KexiSharedActionHost;
	friend class KAction_setEnabled_Helper;
	friend class KexiSharedActionConnector;
};

#endif

