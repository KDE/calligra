/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

//#ifndef KEXIPARTINFO_H
//#define KEXIPARTINFO_H
#ifndef KEXIPROJECTPART_H
#define KEXIPROJECTPART_H

#include "kexipartmanager.h"

namespace KexiPart
{

	class Manager;
	class Item;
	class Part;

/**
 * information about available modules
 */
class Info
{
	public:
		Info(KService::Ptr service); //js, Manager *manager);
		~Info();

		/**
		 * @returns a i18n'ed group name e.g. "Tables"
		 */
		const QString 		groupName();

		/**
		 * @returns the internal mime of this part
		 */
		const QString		mime();

		/**
		 * @returns the icon for groups
		 */
		const QString 		groupIcon();

		/**
		 * @returns the icon for a item
		 */
		const QString 		itemIcon();

		/**
		 * @returns the name object name assoisated with this part (e.g. "tables")
		 */
		const QString 		objectName();

		/**
		 * @returns the project-part-id
		 */
		int 			projectPartID();

		/**
		 * @returns the KService::Ptr associated with this part
		 */
		KService::Ptr 		ptr() { return m_ptr; }

		/**
		 * @returns true if loading was tried but failed
		 */
		bool 			broken() { return m_broken; }

		/**
		 * returns the instance of this @ref KexiProjectPart
		 * dlopens it if needed
		 */
		Part 			*instance();

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
		Part 			*m_instance;
		int 			m_projectPartID;
//		Manager 		*m_manager;
};

}

#endif

