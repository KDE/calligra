/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIFORMHANDLERITEM_H
#define KEXIFORMHANDLERITEM_H

#include "kexiprojecthandleritem.h"
#include "formeditor/propertybuffer.h"

class KexiFormHanlder;
class KexiPrjeectHandler;
class KoStore;

namespace KFormEditor
{
	class WidgetWatcher;
	class WidgetContainer;
	class EventBuffer;
}

class KexiFormHandlerItem : public KexiProjectHandlerItem
{
	Q_OBJECT

	public:
		KexiFormHandlerItem(KexiProjectHandler *parent, const QString &name, const QString &identifier);
		~KexiFormHandlerItem();

		PropertyBuffer	*propertyBuffer() { return m_propertyBuffer; }
		KFormEditor::WidgetWatcher	*widgetWatcher() { return m_widgetWatcher; }
		KFormEditor::WidgetContainer	*container() { return m_container; }
		KFormEditor::EventBuffer	*eventBuffer() { return m_eventBuffer; }

		void	load(KoStore *store);
		void	store(KoStore *store);
		void	setSource(const QString &src);

		void	setContainer(KFormEditor::WidgetContainer *c) { m_container = c; }

	private:
		PropertyBuffer			*m_propertyBuffer;
		KFormEditor::WidgetWatcher	*m_widgetWatcher;
		KFormEditor::WidgetContainer	*m_container;
		KFormEditor::EventBuffer	*m_eventBuffer;
};
#endif
