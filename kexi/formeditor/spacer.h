/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef SPACER_H
#define SPACER_H

#include <qwidget.h>

class QDomElement;
class QDomDocument;

namespace KFormDesigner {

class ObjectTreeItem;

class KFORMEDITOR_EXPORT Spacer : public QWidget
{
	Q_OBJECT

	public:
		Spacer(QWidget *parent, const char *name);
		~Spacer() {;}

		static bool showProperty(const QString &name);
		static void saveSpacer(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc);

	private:
		void paintEvent(QPaintEvent *ev);
};

}

#endif


