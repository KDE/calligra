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

#ifndef KEXIPARTINFO_H
#define KEXIPARTINFO_H

#include "kexipartmanager.h"

namespace KexiPart
{

	class Manager;
	class Item;
	class Part;

/**
 * information about available modules
 */
class KEXICORE_EXPORT Info
{
	public:
		Info(KService::Ptr service); //js, Manager *manager);
		~Info();

		/**
		 * @return a i18n'ed group name e.g. "Tables"
		 */
		inline QString groupName() const { return m_groupName; }

		/**
		 * @return the internal mime of this part
		 */
		inline QString mime() const { return m_mime; }

		/**
		 * @return the icon for groups
		 */
		inline QString groupIcon() const { return m_groupIcon; }

		/**
		 * @return the icon for a item
		 */
		inline QString itemIcon() const { return m_itemIcon; }

		/**
		 * @return the object name associated with this part (e.g. "table")
		 */
		inline QString objectName() const { return m_objectName; }

		/**
		 * @return the project-part-id
		 */
		inline int projectPartID() const { return m_projectPartID; }

		/**
		 * @return the KService::Ptr associated with this part
		 */
		KService::Ptr ptr() const { return m_ptr; }

		/**
		 * @return true if loading was tried but failed
		 */
		bool broken() const { return m_broken; }

#if 0
		/**
		 * returns the instance of this @ref KexiProjectPart
		 * dlopens it if needed
		 */
		Part 			*instance();
#endif
	protected:
		friend class Manager;
		friend class KexiProject;

		/**
		 * @sets the project-part-id
		 */
		void 			setProjectPartID(int id) { m_projectPartID=id; }

		/**
		 * sets the broken flag
		 * most likely to be called by @ref KexiPartManager
		 */
		void 			setBroken(bool broken) { m_broken = broken; }

	private:
		KService::Ptr 		m_ptr;
		bool 			m_broken;
//		Part 			*m_instance;

		QString m_groupName;
		QString m_mime;
		QString m_groupIcon;
		QString m_itemIcon;
		QString m_objectName;
		int 			m_projectPartID;
//		Manager 		*m_manager;
};

}

#endif

