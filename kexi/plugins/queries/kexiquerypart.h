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

#ifndef KEXIQUERYPART_H
#define KEXIQUERYPART_H

class QPixmap;

#include "kexiprojecthandler.h"
#include "kexidataprovider.h"

class KEXI_HAND_QUERY_EXPORT KexiQueryPart : public KexiProjectHandler, KexiDataProvider
{
	Q_OBJECT

	public:
		KexiQueryPart(QObject *project,const char *,const QStringList &);

		virtual QString				name();
		virtual QString				groupName();
		virtual QString				mime();
		virtual bool				visible();


                virtual void hookIntoView(KexiView *view);

//                virtual void saveXML(QDomDocument&);
//		virtual void loadXML(const QDomDocument&);
		virtual void store (KoStore *);
                virtual void load  (KoStore *);


		virtual QPixmap				groupPixmap();
		virtual QPixmap				itemPixmap();

		virtual QStringList datasets(QWidget*);
		virtual QStringList datasetNames(QWidget*);
		virtual KexiDB::Cursor *records(QWidget*,const QString& identifier,Parameters params);
		virtual QStringList fields(QWidget*,const QString& identifier);

		virtual KexiDataProvider	*provider() { return this; }

	protected:
		friend class KexiQueryPartProxy;

	signals:
		virtual	void	itemListChanged(KexiProjectHandler *);

};

#endif
