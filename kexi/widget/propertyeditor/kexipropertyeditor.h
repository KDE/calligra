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

#ifndef KEXIPROPERTYEDITOR_H
#define KEXIPROPERTYEDITOR_H

#include <qvariant.h>

#include <klistview.h>

class KexiProperty;
class KexiPropertyBuffer;
class KexiPropertyEditorItem;
class KexiPropertySubEditor;

class KFORMEDITOR_EXPORT KexiPropertyEditor : public KListView
{
	Q_OBJECT

	public:
		KexiPropertyEditor(QWidget *parent=0, bool returnToAccept = false, const char *name=0);
		~KexiPropertyEditor();

		void	reset(bool editorOnly = false);
		void	setBuffer(KexiPropertyBuffer *b) { m_buffer = b; fill(); }
		virtual QSize sizeHint() const;
		
		void	fill();

	public slots:
		void	slotClicked(QListViewItem *i);

	protected slots:
		void	slotEditorAccept(KexiPropertySubEditor *editor);
		void	slotEditorReject(KexiPropertySubEditor *editor);
		void	slotValueChanged(KexiPropertySubEditor *editor);

		void	slotColumnSizeChanged(int section, int oldS, int newS);

	protected:
		void	createEditor(KexiPropertyEditorItem *i, const QRect &geometry);

	private:
		KexiPropertySubEditor	*m_currentEditor;
		KexiPropertyEditorItem	*m_editItem;
		bool 			m_returnToAccept;
		KexiPropertyBuffer	*m_buffer;
};

#endif
