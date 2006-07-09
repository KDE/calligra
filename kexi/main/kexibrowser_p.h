/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIBROWSER_P_H
#define KEXIBROWSER_P_H

#include <klistview.h>

/*! @internal */
class KexiBrowserListView : public KListView
{
	Q_OBJECT
	public:
		KexiBrowserListView(QWidget *parent);
		~KexiBrowserListView();
		
		virtual bool isExecuteArea( const QPoint& point );

		bool nameEndsWithAsterisk : 1;
		bool enableExecuteArea : 1; //!< used in isExecuteArea()
	public slots:
		virtual void rename(QListViewItem *item, int c);
	protected:
};

#endif
