/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef EVENTEDITOREDITOR_H
#define EVENTEDITOREDITOR_H

#include <qvariant.h>
#include <qmap.h>
#include "propertyeditoreditor.h"

class PropertyEditorItem;
class EventEditorItem;
class EventEditor;
class EventDetail;

class KFORMEDITOR_EXPORT EventEditorEditor : public PropertyEditorEditor
{
	Q_OBJECT

	public:
		EventEditorEditor(QWidget *parent, PropertyEditorItem *i, const char *name=0);
		~EventEditorEditor();

		virtual QVariant	getValue();

	public slots:
		void			slotHandlerChanged(const QString &);

	protected slots:
		void			slotShowDetails();

	private:
		EventEditorItem		*m_item;
		EventEditor		*m_editor;
		EventDetail		*m_ed;
		QVariant		m_result;

		bool			m_faked;
};

#endif
