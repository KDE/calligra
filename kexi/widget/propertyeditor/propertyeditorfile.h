/* This file is part of the KDE project
   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef PROPERTYEDITORFILE_H
#define PROPERTYEDITORFILE_H

#include "kexipropertysubeditor.h"
#include <kurl.h>

class KLineEdit;
class KPushButton;
class QResizeEvent;
class KexiProperty;

class KEXIPROPERTYEDITOR_EXPORT PropertyEditorFile : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorFile(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorFile() {;}

		virtual QVariant	getValue();
		
		void setFilter(QString filter, bool add);
		void resizeEvent(QResizeEvent *ev);

	protected slots:
		void valueChanged();
		void selectFile();

	protected:
		KLineEdit	*m_lineedit;
		KPushButton	*m_button;
		KURL		m_url;
		QString 	m_filter;
};


class KEXIPROPERTYEDITOR_EXPORT PropertyEditorPixmap : public PropertyEditorFile
{
	Q_OBJECT

	public:
		PropertyEditorPixmap(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorPixmap() {;}

		virtual QVariant	getValue();
	
	protected slots:
		void selectFile();

};


#endif

