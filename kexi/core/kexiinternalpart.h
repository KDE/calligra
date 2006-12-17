/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIINTERNALPART_H
#define KEXIINTERNALPART_H

#include <qobject.h>
#include <qguardedptr.h>
#include <qvariant.h>

class KexiMainWindow;
class KexiDialogBase;
class KexiViewBase;

namespace KexiDB {
	class MessageHandler;
}

class QWidget;

/**
 * @short A prototype for Kexi Internal Parts (plugins) implementation.
 *
 * Internal Kexi parts are parts that are not available for users, but loaded
 * internally be application when needed. Example of such part is Relations Window.
 * The internal part instance is unique and has no explicitly stored data.
 * Parts may be able to create widgets or/and dialogs, depending on implementation
 * (createWidgetInstance(), createDialogInstance()).
 * Parts can have unique flag set for dialogs (true by default) 
 * - then a dialog created by createDialogInstance() is unique.
 */
class KEXICORE_EXPORT KexiInternalPart : public QObject
{
	Q_OBJECT

	public:
		KexiInternalPart(QObject *parent, const char *name, const QStringList &);
		virtual ~KexiInternalPart();
	
		KexiDialogBase *instance(KexiMainWindow *parent);

		/*! Creates a new widget instance using part \a partName.
		 \a widgetClass is a pseudo class used in case when the part offers more 
		 than one widget type.
		 \a msgHdr is a message handler for displaying error messages.
		 \a args is two-way optional argument: it can contain custom options used 
		 on widget's creation. Depending on implementation, the created widget can write its 
		 state (e.g. result or status information) back to this argument.
		 Created widget will have assigned \a parent widget and \a objName name. */
		static QWidget* createWidgetInstance(const char* partName, const char* widgetClass, 
			KexiDB::MessageHandler *msgHdr, KexiMainWindow* mainWin, 
			QWidget *parent, const char *objName = 0, QMap<QString,QString>* args = 0);

		/*! For convenience. */
		static QWidget* createWidgetInstance(const char* partName,
			KexiDB::MessageHandler *msgHdr, KexiMainWindow* mainWin, 
			QWidget *parent, const char *objName = 0, QMap<QString,QString>* args = 0)
		 { return createWidgetInstance(partName, 0, msgHdr, mainWin, parent, objName, args); }

		/*! Creates a new dialog instance. If such instance already exists, 
		 and is unique (see uniqueDialog()) it is just returned.
		 The part knows about destroying its dialog instance, (if it is uinque), 
		 so on another call the dialog will be created again. 
		 \a msgHdr is a message handler for displaying error messages.
		 The dialog is assigned to \a mainWin as its parent, 
		 and \a objName name is set. */
		static KexiDialogBase* createKexiDialogInstance(const char* partName, 
			KexiDB::MessageHandler *msgHdr, KexiMainWindow* mainWin, const char *objName = 0);

		/*! Creates a new modal dialog instance (QDialog or a subclass). 
		 If such instance already exists, and is unique (see uniqueDialog()) 
		 it is just returned.
		 \a dialogClass is a pseudo class used in case when the part offers more 
		 than one dialog type.
		 \a msgHdr is a message handler for displaying error messages.
		 \a args is two-way optional argument: it can contain custom options used 
		 on widget's creation. Depending on implementation, the created dialog can write its 
		 state (e.g. result or status information) back to this argument.
		 The part knows about destroying its dialog instance, (if it is uinque), 
		 so on another call the dialog will be created again. 
		 The dialog is assigned to \a mainWin as its parent, 
		 and \a objName name is set. */
		static QDialog* createModalDialogInstance(const char* partName, 
			const char* dialogClass, KexiDB::MessageHandler *msgHdr, KexiMainWindow* mainWin, 
			const char *objName = 0, QMap<QString,QString>* args = 0);

		/*! Adeded For convenience. */
		static QDialog* createModalDialogInstance(const char* partName, 
			KexiDB::MessageHandler *msgHdr, KexiMainWindow* mainWin, const char *objName = 0, 
			QMap<QString,QString>* args = 0)
		{ return createModalDialogInstance(partName, 0, msgHdr, mainWin, objName, args); }

		/*! Executes a command \a commandName (usually nonvisual) using part called \a partName.
		 The result can be put into the \a args. \return true on successful calling. */
		static bool executeCommand(const char* partName, 
			KexiMainWindow* mainWin, const char* commandName, QMap<QString,QString>* args = 0);

		/*! \return internal part of a name \a partName. Shouldn't be usable. */
		static const KexiInternalPart* part(KexiDB::MessageHandler *msgHdr, const char* partName);

		/*! \return true if the part can create only one (unique) dialog. */
		inline bool uniqueDialog() const { return m_uniqueDialog; }

		/*! \return true if the part creation has been cancelled (eg. by a user)
		 so it wasn't an error. Internal part's impelmentation should set it to true when needed. 
		 False by default. */
		inline bool cancelled() const { return m_cancelled; }
		
	protected:
		/*! Used internally */
		KexiDialogBase *findOrCreateKexiDialog(KexiMainWindow* mainWin, 
		 const char *objName);

		/*! Reimplement this if your internal part has to return widgets 
		 or QDialog objects. */
		virtual QWidget *createWidget(const char* widgetClass, KexiMainWindow* mainWin, 
			QWidget * parent, const char * objName = 0, QMap<QString,QString>* args = 0);
		
//		//! Reimplement this if your internal part has to return dialogs
//		virtual KexiDialogBase *createDialog(KexiMainWindow* /*mainWin*/, 
//		 const char * /*objName*/ =0)
//		 { return 0; }
		
		/*! Reimplement this if your internal part has to return a view object. */
		virtual KexiViewBase *createView(KexiMainWindow* mainWin, QWidget * parent,
			const char *objName = 0);

		/*! Reimplement this if your internal part has to execute a command \a commandName 
		 (usually nonvisual). Arguments are put into \a args and the result can be put into the \a args.
		 \return true on successful calling. */
		virtual bool executeCommand(KexiMainWindow* mainWin, const char* commandName, 
			QMap<QString,QString>* args = 0);

		//! Unique dialog - we're using guarded ptr for the dialog so can know if it has been closed
		QGuardedPtr<QWidget> m_uniqueWidget; 
		
		bool m_uniqueDialog : 1; //!< true if createDialogInstance() should return only one dialog

		bool m_cancelled : 1; //!< Used in cancelled()
};

#endif
