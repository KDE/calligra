/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIRELATION_H
#define KEXIRELATION_H

#include <qvaluelist.h>

#include "kexidialogbase.h"
#include "kexirelationview.h"

class QComboBox;
class KoStore;
class KexiDB;


typedef QValueList<SourceConnection> RelationList;

class KexiRelation : public KexiDialogBase
{
	Q_OBJECT

	public:
		KexiRelation(QWidget *parent, const char *name="relations", bool embedd=false);
		~KexiRelation();
	
		virtual KXMLGUIClient *guiClient(){return new KXMLGUIClient();}

		static RelationList *	projectRelations();
		static bool		storeRelations(RelationList relations, KoStore *store);

		KexiRelationView	*view() { return m_view; }
	public slots:
		void			slotAddTable();
		void			slotSave(KoStore *store);
	
	private:
		KexiDB			*m_db;
		QComboBox		*m_tableCombo;
		KexiRelationView	*m_view;
};

#endif
