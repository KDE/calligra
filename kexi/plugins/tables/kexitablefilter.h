/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXITABLEFILTER_H
#define KEXITABLEFILTER_H

/**
 * this is just a small abstrat class
 * provided for interaction between the manager and sublevel
 * abstracted filters ;)
 * goto 10
 */

#include <qobject.h>

class KexiTableFilter : public QObject
{
	Q_OBJECT

	public:
		KexiTableFilter(QObject *parent, const char *name=0);
		virtual ~KexiTableFilter() {; }
};

#endif
