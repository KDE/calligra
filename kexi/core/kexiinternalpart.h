/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIINTERNALPART_H
#define KEXIINTERNALPART_H

#include <qobject.h>
#include <qguardedptr.h>

class KexiMainWindow;
class KexiDialogBase;
class QWidget;

/**
 * This is a virtual prototype for the internal Kexi part implementation
 * Internal Kexi parts are a parts that are not available for users, but loaded
 * internally be application when needed. Example of such part is Relations Window.
 * The internal part instance is unique and has no explicity stored data.
 * Parts can be able to create widgets or/and dialogs, depending on implementation
 * (createWidgetInstance(), createDialogInstance()).
 * Parts can have unique flag set for dialogs (true by default) 
 * - then dialog created by createDialogInstance() is unique.
 */
class KEXICORE_EXPORT KexiInternalPart : public QObject
{
	Q_OBJECT

	public:
		KexiInternalPart(QObject *parent, const char *name, const QStringList &);
		virtual ~KexiInternalPart();
	
		KexiDialogBase *instance(KexiMainWindow *parent);

		/*! Creates a new widget instance using pare \a partName.
		 \a widgetClass is a pseudo clas in used in case when the part offers more widgets.
		 Created widget will have assigned \a parent widget and \a objName name. */
		static QWidget* createWidgetInstance(const char* partName, const char* widgetClass, 
		 KexiMainWindow* mainWin, QWidget *parent, const char *objName = 0);

		/*! For convenience. */
		static QWidget* createWidgetInstance(const char* partName,
		 KexiMainWindow* mainWin, QWidget *parent, const char *objName = 0)
		 { return createWidgetInstance(partName, 0, mainWin, parent, objName); }

		/*! Creates a new dialog instance. If such instance already exists, 
		 it is just returned, if this part is unique (see m_unique).
		 The part know about destroying its dialog instance, (if it is uinque), 
		 so on another call the dialog will be created again. 
		 Dialog is assigned \a mainWin as its main window, and \a objName name. */
		static KexiDialogBase* createDialogInstance(const char* partName, 
		 KexiMainWindow* mainWin, const char *objName = 0);
		
		/*! \return internal part of a name \a partName. Shouldn't be usable. */
		static const KexiInternalPart* part(const char* partName);

		/*! \return true is the part can create only one (unique) dialog. */
		inline bool uniqueDialog() const { return m_uniqueDialog; }

		/*! Used internally */
		KexiDialogBase *findOrCreateDialog(KexiMainWindow* mainWin, 
		 const char *objName);
		
	protected:
		//! Reimplement this if your internal part has to return widgets
		virtual QWidget *createWidget(const char* /*widgetClass*/, KexiMainWindow* /*mainWin*/, 
		 QWidget * /*parent*/, const char * /*objName*/ =0) { return 0; }
		
		//! Reimplement this if your internal part has to return dialogs
		virtual KexiDialogBase *createDialog(KexiMainWindow* /*mainWin*/, 
		 const char * /*objName*/ =0)
		 { return 0; }
		
		
		//! Unique dialog - we're using guarded ptr for the dialog so can know if it has been closed
		QGuardedPtr<KexiDialogBase> m_dialog; 
		
		bool m_uniqueDialog : 1; //!< true if createDialogInstance() should return only one dialog
	
};

#endif

