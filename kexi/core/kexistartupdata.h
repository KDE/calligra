/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_STARTUPDATA_H
#define KEXI_STARTUPDATA_H

#include <qstring.h>

class KexiProjectData;

class KEXICORE_EXPORT KexiStartupData
{
	public:
		typedef enum Action {
			DoNothing = 0,
			CreateBlankProject = 1,
			UseTemplate = 2,
			OpenProject = 3,
			Exit = 4
		};
		
		KexiStartupData();
		virtual ~KexiStartupData();
		
		virtual bool init() { return true; };

		Action action() const;

		KexiProjectData *projectData() const;

		/*! \return true is the Design Mode is forced for this project. 
			Used on startup (by --design-mode comman line switch). */
		bool forcedDesignMode() const;
	
		/*! \return true is the Final Mode is forced for this project. 
			Used on startup (by --final-mode comman line switch). 
			By default this is false. */
		bool forcedFinalMode() const;
		
	protected:
		KexiProjectData *m_projectData;
		Action m_action;
		bool m_forcedFinalMode : 1;
		bool m_forcedDesignMode : 1;
		bool m_createDB : 1;
		bool m_dropDB : 1;
		bool m_alsoOpenDB : 1;
};

#endif

