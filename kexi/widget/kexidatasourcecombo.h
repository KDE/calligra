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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDATASOURCECOMBO_H
#define KEXIDATASOURCECOMBO_H

#include <qpair.h>
#include <qmap.h>
#include <kexipartdatasource.h>
#include <kexipartitem.h>
#include <qcombobox.h>

class KexiMainWindow;


typedef QPair<KexiPart::DataSource*, KexiPart::Item> DSInfo;
typedef QMap<int, DSInfo> DSList;

/**
 * a combobox listing all availabe datasources with icons etc
 * see KexiPart::DataSource for more info
 */
class KEXIEXTWIDGETS_EXPORT KexiDataSourceCombo : public QComboBox
{
	Q_OBJECT

	public:
		/**
		 * constructs a KexiDataSourceCombo and populates it
		 */
		KexiDataSourceCombo(KexiMainWindow *win, QWidget *parent, const char *name=0);
		~KexiDataSourceCombo();

		/**
		 * @return the selected datasource
		 */
		DSInfo selectedSource();

	signals:
		/**
		 * this signal is emmited when the user selects another datasource
		 */
		void dataSourceChanged(KexiPart::DataSource *source, const KexiPart::Item &i);

	protected:
		/**
		 * adds data to the combo
		 */
		void populate();

	protected slots:
		void activated(int id);

	private:
		KexiMainWindow *m_win;
		DSList m_list;
};

#endif

