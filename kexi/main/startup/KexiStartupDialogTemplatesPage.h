/* This file is part of the KDE project
   Copyright (C) 2007 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KexiStartupDialogTemplatesPage_h
#define KexiStartupDialogTemplatesPage_h

#include <k3listview.h>
#include <core/kexiprojectdata.h>

/*! Helper class for displaying templates set with description. */
class KEXIMAIN_EXPORT KexiStartupDialogTemplatesPage : public K3ListView
{
	Q_OBJECT
	
	public:
		KexiStartupDialogTemplatesPage( QWidget * parent = 0 );
		~KexiStartupDialogTemplatesPage(); 
//		void addItem(const QString& key, const QString& name, 
//			const QString& description, const QPixmap& icon);

		QString selectedFileName() const;

		const KexiProjectData::AutoOpenObjects& autoopenObjectsForSelectedTemplate() const;

		void populate();

	signals:
		void selected(const QString& filename);

	protected slots:
		void slotExecuted(Q3ListViewItem* item);

//		void itemClicked(QIconViewItem *item);
	
	private:
		class Private;
		Private* const d;
//		KIconView *templates;
//		KTextBrowser *info;
};

#endif
