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

#ifndef KEXIDBWIDGETCONTAINER_H
#define KEXIDBWIDGETCONTAINER_H

#include "formeditor/widgetcontainer.h"
#include "formeditor/container_frame.h"


#include <kexiDB/kexidbrecordset.h>

/**
 * this class aims to be a replacement for KFormEditor's default widget container
 * which handles db-records with a specified db-source and give some nice scripting ;) options.
 */

namespace KFormEditor
{
	class WidgetWatcher;
};

class KEXI_HAND_FORM_EXPORT KexiDBWidgetContainer : public KFormEditor::WidgetContainer
{
	Q_OBJECT

	//db-properties
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true SCRIPTABLE true)

	public:
		KexiDBWidgetContainer(QWidget *parent, const char *name=0, QString identifier=QString::null);
		~KexiDBWidgetContainer();

		/*
		 * db binding
		 */
		QString		dataSource() const;
		void		setDataSource(QString source);

		void		setRecord(KexiDBRecordSet *rec);
		void		setWidgetWatcher(KFormEditor::WidgetWatcher *ww) { m_ww = ww; }

	public slots:
		void		next();
		void		prev();

	protected:
		void		setupWidgets();

	private:
		QString		m_dataSource;
		KexiDBRecordSet	*m_rec;
		KFormEditor::WidgetWatcher	*m_ww;
};

#endif

