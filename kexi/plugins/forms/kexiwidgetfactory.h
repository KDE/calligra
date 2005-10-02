/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIWIDGETFACTORY_H
#define KEXIWIDGETFACTORY_H

#include <widgetfactory.h>
#include <container.h>

class KexiWidgetFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		KexiWidgetFactory(QObject *parent, const char *name, const QStringList &args);
		virtual ~KexiWidgetFactory();

		virtual QString name();
		virtual KFormDesigner::WidgetList classes();
		virtual QWidget *create(const QCString &, QWidget *, const char *, KFormDesigner::Container *);
		virtual bool createMenuActions(const QCString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container);

	private:
		KFormDesigner::WidgetList m_classes;
};

#endif

