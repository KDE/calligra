/* This file is part of the KDE project
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

#ifndef KEXIPROPERTYEDITORVIEW_H
#define KEXIPROPERTYEDITORVIEW_H

#include "kexiviewbase.h"
#include "kexipropertyeditor.h"

/*! The container (acts as a dock window) for KexiPropertyEditor
*/
class KEXIPROPERTYEDITOR_EXPORT KexiPropertyEditorView : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiPropertyEditorView(KexiMainWindow *mainWin);
		~KexiPropertyEditorView();

		virtual QSize sizeHint() const;
		virtual QSize minimumSizeHint() const;

		KexiPropertyEditor *editor() const { return m_editor; }

	public slots:
		virtual void setGeometry( const QRect &r );
		virtual void resize ( int w, int h );

	protected:
		KexiPropertyEditor   *m_editor;
};

#endif

