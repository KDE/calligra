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

#ifndef KEXIPRJTYPESELECTOR_H
#define KEXIPRJTYPESELECTOR_H

#include <kexi_export.h>
#include <QWidget>
#include "ui_kexiprjtypeselector.h"

//! @short A simple widget with radio buttons with "show file/server-based projects" options
class KEXIEXTWIDGETS_EXPORT KexiPrjTypeSelector
	: public QWidget, public Ui_KexiPrjTypeSelector
{
	Q_OBJECT

	public:
		KexiPrjTypeSelector( QWidget* parent = 0 );
		~KexiPrjTypeSelector();

	public slots:
		void slotSelectionChanged( int id );
};

#endif // KEXIPRJTYPESELECTOR_H
