/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXI_DATASOURCE_COMBO_H_
#define KEXI_DATASOURCE_COMBO_H_

#include <kcombobox.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qpixmap.h>
#include <kdebug.h>

class KexiProject;


class KexiDataSourceComboBox: public KComboBox {
	Q_OBJECT
public:
	class Item {
	public:
		Item(){}
		Item(const QString &name_, const QString &globID_,
			const QString &mime_, const QString &identifier_,const QPixmap& pixm) {

			name=name_;
			globalIdentifier=globID_;
			mime=mime_;
			identifier=identifier_;
			pixmap=pixm;
		}

		QString name;
		QString globalIdentifier;
		QString mime;
		QString identifier;
		QPixmap pixmap;
	};
	typedef QValueList<Item> ItemList;
	KexiDataSourceComboBox(QWidget *parent, const char* name, KexiProject *proj);
	KexiDataSourceComboBox(QWidget *parent, const char* name, const ItemList &list);
	virtual ~KexiDataSourceComboBox();
	QString globalIdentifier() const;
	QString mime() const ;
	QString identifier() const;
	void selectGlobalIdentifier(const QString& gid);
	static void fillList(KexiProject*,ItemList&);
private:
	ItemList m_list;
	KexiProject *m_project;
	void init();
};

#endif
