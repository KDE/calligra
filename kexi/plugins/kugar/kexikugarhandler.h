/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef KEXIKUGARHANDLER_H
#define KEXIKUGARHANDLER_H

class QPixmap;
class KexiTempDir;

#include "kexiprojecthandler.h"

class QDomDocument;
class QDomElement;

class KexiKugarHandler : public KexiProjectHandler
{
	Q_OBJECT

	public:
		KexiKugarHandler(QObject *project,const char *,const QStringList &);
		virtual ~KexiKugarHandler();

		virtual QString				name();
		virtual QString				groupName();
		virtual QString				mime();
		virtual bool				visible();


                virtual void hookIntoView(KexiView *view);

		virtual void loadXML(const QDomDocument &, const QDomElement&);
		virtual void saveXML(QDomDocument&);

                virtual void store (KoStore *);
                virtual void load  (KoStore *);


		virtual QPixmap				groupPixmap();
		virtual QPixmap				itemPixmap();

		void createReport(KexiView*);
		void editReport(KexiView*,const QString &identifier);
		void view(KexiView*,const QString &identifier);	

		QString tempPath();
	private:
		QString nextFreeID;
		QString nextID();
		KexiTempDir *m_tempDir;
	protected:
		friend class KexiKugarHandlerProxy;
		void				 getQueries();

	signals:
		void				itemListChanged(KexiProjectHandler*);
};

#endif
