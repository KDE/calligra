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

class KexiPropertyEditor;

class KEXIPROPERTYEDITOR_EXPORT KexiPropertyEditorView : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiPropertyEditorView(KexiMainWindow *mainWin, QWidget *parent, bool returnToAccept, bool AutoSync, const char *name);
		~KexiPropertyEditorView(){;}
	
	protected:
		KexiPropertyEditor   *m_editor;
};

#endif

