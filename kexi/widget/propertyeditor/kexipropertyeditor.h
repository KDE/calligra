/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
class KPushButton;

class KEXIPROPERTYEDITOR_EXPORT KexiPropertyEditor : public KListView
{
	Q_OBJECT

	public:
		KexiPropertyEditor(QWidget *parent=0, bool returnToAccept = false, bool AutoSync=false, const char *name=0);
		~KexiPropertyEditor();

		void	reset(bool editorOnly = false);
		void	setBuffer(KexiPropertyBuffer *b);
		virtual QSize sizeHint() const;
		
	signals:
		void	valueChanged(const QString &propname, QVariant value);

	public slots:
		void	slotClicked(QListViewItem *i);
		void    resetItem();
		void    moveEditor();
		void	fill();

	protected slots:
		void	slotEditorAccept(KexiPropertySubEditor *editor);
		void	slotEditorReject(KexiPropertySubEditor *editor);
		void	slotValueChanged(KexiPropertySubEditor *editor);

		void	slotColumnSizeChanged(int section, int oldS, int newS);
		void	slotColumnSizeChanged(int section);

	protected:
		void	createEditor(KexiPropertyEditorItem *i, const QRect &geometry);
		void 	resizeEvent(QResizeEvent *ev);

	private:
		KexiPropertySubEditor	*m_currentEditor;
		KexiPropertyEditorItem	*m_editItem;
		KexiPropertyEditorItem	*m_topItem;
		bool 			m_returnToAccept;
		KexiPropertyBuffer	*m_buffer;
		KPushButton		*m_defaults; // "Revert to defaults" button
		bool			m_sync;
};

#endif
