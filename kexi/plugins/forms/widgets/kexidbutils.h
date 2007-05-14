/* This file is part of the KDE project
   Copyright (C) 2006-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KDBWIDGETS_UTILS_H
#define KDBWIDGETS_UTILS_H

#include <qpopupmenu.h>
#include <kexidataiteminterface.h>

QColor lighterGrayBackgroundColor(const QPalette& palette);

//! @short Used for extending editor widgets' context menu. 
/*! @internal This is performed by adding a title and disabling editing 
 actions when "read only" flag is true. */
class KexiDBWidgetContextMenuExtender : public QObject
{
	public:
		KexiDBWidgetContextMenuExtender( QObject* parent, KexiDataItemInterface* iface );
		~KexiDBWidgetContextMenuExtender();

		//! Creates title for context menu \a menu
		void createTitle(QPopupMenu *menu);

		//! Enables or disables context menu actions that can modify the value.
		//! The menu has to be previously provided by createTitle().
		void updatePopupMenuActions();

		/*! Updates title for context menu based on data item \a iface caption or name
		 Used in createTitle(QPopupMenu *menu) and KexiDBImageBox.
		 \return true is the title has been added. */
		static bool updateContextMenuTitleForDataItem(QPopupMenu *menu, KexiDataItemInterface* iface, 
			const QString& icon = QString());

	protected:
		KexiDataItemInterface* m_iface;
		QGuardedPtr<QPopupMenu> m_contextMenu;
		bool m_contextMenuHasTitle; //!< true if KPopupTitle has been added to the context menu.
};

class KexiDBAutoField;

//! An interface allowing to define custom behaviour for subwidget of the KexiDBAutoField
class KexiSubwidgetInterface
{
	public:
		KexiSubwidgetInterface();
		virtual ~KexiSubwidgetInterface();

		virtual bool appendStretchRequired(KexiDBAutoField* autoField) const
			{ Q_UNUSED(autoField); return false; }
		virtual bool subwidgetStretchRequired(KexiDBAutoField* autoField) const
			{ Q_UNUSED(autoField); return false; }
};

#endif
