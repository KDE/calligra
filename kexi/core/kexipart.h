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

class KexiMainWindow;
class KexiDialogBase;

namespace KexiPart
{
	class Info;
	class Item;
	class GUIClient;

/**
 * The main class for kexi frontend parts like tables, queries, relations
 */
class KEXICORE_EXPORT Part : public QObject
{
	Q_OBJECT

	public:
		
		Part(QObject *parent, const char *name, const QStringList &);
		virtual ~Part();

		KexiDialogBase* execute(KexiMainWindow *win, const KexiPart::Item &item);

		virtual KexiDialogBase* createInstance(KexiMainWindow *win, const KexiPart::Item &item) = 0;

		//! Creates GUICLient for this part, attached to \a win
		//! This method is called from KexiMainWindow
		GUIClient* createGUIClient(KexiMainWindow *win);

		/*! i18n'd iunstance name usable for displaying in gui.
		 @todo move this to Info class when the name could be moved as localised property 
		 to service's .desktop file. */
		virtual QString instanceName() const = 0;
		
		Info		*info() { return m_info; }


	protected:
		friend class Manager;


		void		setInfo(Info *info) { m_info = info; }

	private:
		Info	*m_info;
};

}

#endif

