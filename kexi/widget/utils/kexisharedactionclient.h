/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISHAREDACTIONCLIENT_H
#define KEXISHAREDACTIONCLIENT_H

#include <qasciidict.h>

class KAction;
#include <kexi_export.h>

//! The KexiSharedActionClient is an interface using application-wide (shared) actions.
/** See KexiTableView and KexiFormScrollView for example usage. 
*/
class KEXIGUIUTILS_EXPORT KexiSharedActionClient
{
	public:
		KexiSharedActionClient();
		virtual ~KexiSharedActionClient();

		/*! Plugs action \a a for a widget. The action will be later looked up (by name) 
		 on key press event, to get proper shortcut. If found, we know that the action is already 
		 performed at main window's level, so we should give up. Otherwise - default shortcut 
		 will be used (example: Shift+Enter key for "data_save_row" action). \sa KexiTableView::shortCutPressed()
		*/
		void plugSharedAction(KAction* a);

	protected:
		//! Actions pluged for this widget using plugSharedAction(), available by name.
		QAsciiDict<KAction> m_sharedActions;
};

#endif
