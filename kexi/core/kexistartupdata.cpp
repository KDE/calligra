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

#include "kexistartupdata.h"
#include "kexi.h"

#include <kexidb/driver.h>
#include <kexidb/drivermanager.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kdeversion.h>

#include <qfileinfo.h>
#include <qcstring.h>


KexiStartupData::KexiStartupData()
	: m_projectData(0)
	, m_action(KexiStartupData::DoNothing)
	, m_forcedFinalMode(false)
	, m_forcedDesignMode(false)
	, m_createDB(false)
	, m_dropDB(false)
	, m_alsoOpenDB(false)
{
}

KexiStartupData::~KexiStartupData()
{
}

KexiProjectData *KexiStartupData::projectData() const
{
	return m_projectData;
}

KexiStartupData::Action KexiStartupData::action() const
{
	return m_action;
}

bool KexiStartupData::forcedFinalMode() const
{
	return m_forcedFinalMode;
}

bool KexiStartupData::forcedDesignMode() const
{
	return m_forcedDesignMode;
}

