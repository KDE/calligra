/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIPART_H
#define KEXIPART_H

#include <qobject.h>
#include <qmap.h>

#include "kexi.h"

class KexiMainWindow;
class KActionCollection;
class KexiDialogBase;
class KexiViewBase;

namespace KexiPart
{
	class Info;
	class Item;
	class GUIClient;
	class DataSource;

/**
 * The main class for kexi frontend parts like tables, queries, relations
 */
class KEXICORE_EXPORT Part : public QObject
{
	Q_OBJECT

	public:
		
		Part(QObject *parent, const char *name, const QStringList &);
		virtual ~Part();

		/*! "Opens" an instance that the part provides, pointed by \a item in a mode \a viewMode. 
		 \a viewMode is one of Kexi::ViewMode enum. */
		KexiDialogBase* openInstance(KexiMainWindow *win, KexiPart::Item &item, 
			int viewMode = Kexi::DataViewMode);

		/*! "Removes" any stored data pointed by \a item (example: table is dropped for table part). 
		 From now this data is inaccesible, and \a item disappear.
		 For this, a database connection associated with kexi project owned by \a win can be used.
		*/
		virtual bool remove(KexiMainWindow *win, KexiPart::Item &item) = 0;

		/*! i18n'd instance name usable for displaying in gui.
		 @todo move this to Info class when the name could be moved as localized property 
		 to service's .desktop file. */
		inline QString instanceName() const { return m_names["instance"]; }
		
		inline Info *info() const { return m_info; }

		inline GUIClient *guiClient() const { return m_guiClient; }

		inline GUIClient *instanceGuiClient(int mode = 0) const { return m_instanceGuiClients[mode]; }

		virtual KexiViewBase* createView(QWidget *parent, KexiDialogBase* dialog, KexiPart::Item &item, int viewMode = Kexi::DataViewMode) = 0;

		/**
		 * @returns the datasource object of this part
		 * reeimplement it to make a part work as dataprovider ;)
		 */
		virtual DataSource *dataSource() { return 0; }

		
	signals: 
		void newObjectRequest( KexiPart::Info *info );

	protected slots:
		void slotCreate();

	protected:
//		virtual KexiDialogBase* createInstance(KexiMainWindow *win, const KexiPart::Item &item, int viewMode = Kexi::DataViewMode) = 0;

		//! Creates GUICLients for this part, attached to \a win
		//! This method is called from KexiMainWindow
		void createGUIClients(KexiMainWindow *win);

		virtual void initPartActions( KActionCollection * ) {};
//		virtual void initInstanceActions( KActionCollection * ) {};
		virtual void initInstanceActions( int mode, KActionCollection * ) {};

		inline void setInfo(Info *info) { m_info = info; }

		//! Set of i18n'd action names for, initialised on KexiPart::Part subclass ctor
		//! The names are useful because the same action can have other name for each part
		//! E.g. "New table" vs "New query" can have different forms for some languages...
		QMap<QString,QString> m_names;

		/*! Supported modes for dialogs created by this part.
		 \a modes are one of Kexi::ViewMode enum elements.
		 Set this member in your KexiPart subclass' ctor, if you need to override the default value
		 that equals Kexi::DataViewMode | Kexi::DesignViewMode.
		 Theis member is used to set supported view modes for every KexiDialogBase derived object
		 created by this KexiPart.
		 Default flag combination is Kexi::DataViewMode | Kexi::DesignViewMode. */
		int m_supportedViewModes;

	private:
		Info *m_info;
		GUIClient *m_guiClient;
//		GUIClient *m_instanceGuiClient;
		QIntDict<GUIClient> m_instanceGuiClients;

	friend class Manager;
	friend class KexiMainWindow;
	friend class KexiMainWindowImpl;
	friend class GUIClient;
};

}

#endif

