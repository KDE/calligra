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

#ifndef KEXIMAINWINDOW_H
#define KEXIMAINWINDOW_H

#include "kexisharedactionhost.h"
#include "kexi.h"

#include <kmdimainfrm.h>

#include <qintdict.h>

class KexiDialogBase;
class KexiProject;
namespace KexiPart {
	class Item;
}

/**
 * @short Kexi's main window interface
 * This interface is implemented by KexiMainWindowImpl class.
 * KexiMainWindow offers simple features what lowers cross-dependency (and also avoids
 * circular dependencies between Kexi modules).
 */
class KEXICORE_EXPORT KexiMainWindow : public KMdiMainFrm, public KexiSharedActionHost
{
	Q_OBJECT
	public:
		KexiMainWindow();
		virtual ~KexiMainWindow();

		//! Project data of currently opened project or NULL if no project here yet.
		virtual KexiProject *project() = 0;
		
		/**
		 * registers a dialog for watching and adds it to the view
		 */
		virtual void registerChild(KexiDialogBase *dlg) = 0;

		virtual QPopupMenu* findPopupMenu(const char *popupName) = 0;

		/*! Generates ID for private "document" like Relations window.
		 Private IDs are negative numbers (while ID regular part instance's IDs are >0)
		 Private means that the object is not stored as-is in the project but is somewhat 
		 generated and in most cases there is at most one unique instance document of such type (part).
		 To generate this ID, just app-wide internal counter is used. */
		virtual int generatePrivateDocID() = 0;
	
	public slots:
		//! Opens object pointed by \a item in a view \a viewMode
		virtual KexiDialogBase * openObject(KexiPart::Item *item, int viewMode = Kexi::DataViewMode) = 0;
		//! For convenience
		virtual KexiDialogBase * openObject(const QCString& mime, const QString& name, int viewMode = Kexi::DataViewMode) = 0;

		/*! Received information from active view that \a dlg has switched its property buffer,
		 so property editor contents should be reloaded. */
		virtual void propertyBufferSwitched(KexiDialogBase *dlg) = 0;
};


#endif

