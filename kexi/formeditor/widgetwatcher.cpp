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

#include "widgetwatcher.h"

namespace KFormEditor {

WidgetWatcher::WidgetWatcher(QObject *parent, const char *name)
 : QObject(parent, name),
   QMap<char *, QWidget *>()
{
}

QString
WidgetWatcher::genName(const QString &base)
{
//	if(m_nameCounter.contains(base))
//	{
		int count = m_nameCounter[base];
//		int count = m_nameCounter.find(base).data();
		m_nameCounter.insert(base, count + 1);
		return QString(base) + QString::number(count + 1);
/*
	}
	else
	{
		m_nameCounter.insert(base, 1);
		return QString(base) + "1";
	}
*/
}

QString
WidgetWatcher::genName(QObject *o)
{
	return genName(o->className());
}

WidgetWatcher::~WidgetWatcher()
{
}

}

#include "widgetwatcher.moc"

