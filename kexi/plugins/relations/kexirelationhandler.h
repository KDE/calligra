/* This file is part of the KDE project
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIRELATIONHANDLER_H
#define KEXIRELATIONHANDLER_H

#include "kexiprojecthandler.h"

class QPixmap;
class QWidget;

class KEXI_HAND_RELAT_EXPORT KexiRelationHandler: public KexiProjectHandler
{
	Q_OBJECT

	public:
		KexiRelationHandler(QObject *project,const char *,const QStringList &);

		virtual QString				name();
		virtual QString				mime();
		virtual bool				visible();


		virtual void hookIntoView(KexiView *view);
		virtual QWidget* embeddReadOnly(QWidget *parent, KexiView *v);

		virtual void store(KoStore *);
		virtual void load(KoStore *);


		virtual QPixmap				groupPixmap();
		virtual QPixmap				itemPixmap();
	
	protected:
		friend class KexiRelationHandlerProxy;

	signals:
		void				itemListChanged(KexiProjectHandler*);
};

#endif
