/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIRELATION_H
#define KEXIRELATION_H

#include <qvaluelist.h>
#include <qvaluestack.h>
#include <kexidb/connection.h>


class KexiProject;
class KoStore;

class KEXICORE_EXPORT KexiRelation : public QObject
{
	Q_OBJECT

	public:
		KexiRelation(KexiProject *parent, const char *name="relationmanager");
		~KexiRelation();
#warning FIXME
/*
		RelationList projectRelations();
		void updateRelationList(QObject *who, RelationList relationList);
*/
		void storeRelations(KoStore *store);
		void loadRelations(KoStore *store);
		void incUsageCount();
		void decUsageCount();

	signals:
		void relationListUpdated(QObject *sender);
		
	private:
#warning FIXME
/*		typedef QValueStack<RelationList> UndoRedoStack;
		RelationList m_relationList;*/
		KexiProject *m_parent;
//		UndoRedoStack m_undoStack;
		int m_usageCount;
};

#endif
