/* This file is part of the KDE project
   Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIQUERYPARTITEM_H
#define KEXIQUERYPARTITEM_H

#include <qvaluelist.h>

#include "kexiprojecthandleritem.h"
#include "kexidataprovider.h"

class QString;
class KexiProjectHandler;
class KexiQueryDesigner;
class KexiTableList;
class KexiQueryDesigner;

class KEXI_HAND_QUERY_EXPORT KexiQueryPartItem: public KexiProjectHandlerItem
{
	Q_OBJECT

public:

	KexiQueryPartItem(KexiProjectHandler *handler, const QString& ident, 
	 const QString& mime, const QString& title);
	virtual ~KexiQueryPartItem();

	void setParameters(const KexiDataProvider::ParameterList& params);
	const KexiDataProvider::ParameterList parameters();

	KexiTableList	*designData() { return m_designData; }
	QString		sql() { return m_sql; }
	void		setSQL(const QString &sql) { m_sql = sql; }
	void		setClient(KexiQueryDesigner *c) { m_client = c; }

	void store(KoStore*);
	void load(KoStore*);

	Cursor *records(QWidget*,KexiDataProvider::Parameters params=KexiDataProvider::Parameters());


protected:
	friend class KexiQueryDesigner;
	friend class KexiQueryDesigerGuiEditor;

private:
	friend class KexiQueryPart;
	KexiQueryDesigner		*m_client;
	KexiTableList			*m_designData;
	QString				m_sql;
	KexiDataProvider::ParameterList	m_params;
	QStringList			m_fields;
};

#endif
