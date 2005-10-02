/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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


#include "karbon_part_base.h"
#include "vtoolcontroller.h"

KarbonPartBase::KarbonPartBase( QWidget* parentWidget, const char* widgetName,
						QObject* parent, const char* name, bool singleViewMode )
		: KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
}

KarbonPartBase::~KarbonPartBase()
{
	delete m_toolController;
	m_toolController = 0;
}

VToolController *
KarbonPartBase::toolController()
{
	return m_toolController;
}

