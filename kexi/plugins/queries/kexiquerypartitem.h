/* This file is part of the KDE project
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIQUERYPARTITEM_H
#define KEXIQUERYPARTITEM_H

#include <kexiprojecthandleritem.h>
#include <kexidataprovider.h>
#include <qvaluelist.h>

class QString;
class KexiProjectHandler;

class KexiQueryPartItem: public KexiProjectHandlerItem 
{
	Q_OBJECT
public:

	class QueryEntry
	{
	public:
		QueryEntry(){;}
		QueryEntry(const QString& source_, const QString& field_, 
			bool show_, const QString& orC_, const QString& andC_): source(source_),
				field(field_),show(show_),orC(orC_),andC(andC_){;}
		QString source;
		QString field;
		bool show;
		QString orC;
		QString andC;
	};
	
	typedef QValueList<QueryEntry> QueryEntryList;
	
	KexiQueryPartItem(KexiProjectHandler *parent, const QString& name, const QString& mime, 
			const QString& identifier);
	virtual ~KexiQueryPartItem();
	const QueryEntryList &getQueryData();
	void setQueryData(const QueryEntryList&);
	void setParameterList(const KexiDataProvider::ParameterList& params);
	const KexiDataProvider::ParameterList parameters();

	void store(KoStore*);
	void load(KoStore*);

protected:
	friend class KexiQueryDesigner;
	friend class KexiQueryDesigerGuiEditor;

private:
	QueryEntryList m_queryEntryList;
	KexiDataProvider::ParameterList m_params;
};

#endif
