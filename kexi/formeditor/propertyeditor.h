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

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qvariant.h>

#include <klistview.h>

#include "propertyeditoreditor.h"
//PropertyEditorEditor;
class PropertyEditorItem;

class PropertyEditor : public KListView
{
	Q_OBJECT

	public:
		PropertyEditor(QWidget *parent=0, const char *name=0);
		~PropertyEditor();

		void	reset();

	public slots:
		void	setObject(QObject *o);
		void	slotClicked(QListViewItem *i);

	protected slots:
		void	slotEditorAccept(PropertyEditorEditor *editor);
		void	slotEditorReject(PropertyEditorEditor *editor);
		void	slotValueChanged(PropertyEditorEditor *editor);

		void	slotColumnSizeChanged(int section, int oldS, int newS);

	protected:
		void	createEditor(PropertyEditorItem *i, const QRect &geometry);

	private:
		PropertyEditorEditor	*m_currentEditor;
		PropertyEditorItem	*m_editItem;
};

#endif
